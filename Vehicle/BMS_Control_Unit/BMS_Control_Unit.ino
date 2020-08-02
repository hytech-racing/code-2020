#include <ADC_SPI.h>
#include <Arduino.h>
// #include <EEPROM.h> TODO add EEPROM functionality so we can configure parameters over CAN
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <kinetis_flexcan.h>
#include <LT_SPI.h>
#include <LTC68042.h>
#include <Metro.h>

#include "Constants.h"
#include "HT_CAN_Util.h"
#include "Telemetry.h"
#include "adc.h"
#include "ltc.h"
#include "balancer.h"
#include "temperatures.h"
#include "printers.h"

// Timers
Metro timer_can_update_slow = Metro(1000);
Metro timer_process_cells_slow = Metro(1000);
Metro timer_watchdog_timer = Metro(250);
Metro timer_charge_timeout = Metro(1000);

// Interrupt timers
//IntervalTimer current_timer;

uint8_t total_count_cells = CELLS_PER_IC * TOTAL_IC; // Number of non-ignored cells (used for calculating averages)
uint8_t total_count_cell_thermistors = THERMISTORS_PER_IC * TOTAL_IC; // Number of non-ignored cell thermistors (used for calculating averages)
uint8_t total_count_pcb_thermistors = PCB_THERM_PER_IC * TOTAL_IC; // Number of non-ignored PCB thermistors (used for calculating averages)

uint16_t cell_voltages[TOTAL_IC][12]; // contains 12 battery cell voltages. Numbers are stored in 0.1 mV units.

int8_t ignore_cell[TOTAL_IC][CELLS_PER_IC]; // Cells to be ignored for under/overvoltage and balancing
int8_t ignore_pcb_therm[TOTAL_IC][PCB_THERM_PER_IC]; // PCB thermistors to be ignored
int8_t ignore_cell_therm[TOTAL_IC][THERMISTORS_PER_IC]; // Cell thermistors to be ignored


uint8_t tx_cfg[TOTAL_IC][6]; // data defining how data will be written to daisy chain ICs.

uint8_t rx_cfg[TOTAL_IC][8];

// CAN Variables
HT_CAN_Util CAN(500000);
static CAN_message_t rx_msg;

// BMS State Variables
BMS_detailed_voltages bms_detailed_voltages[TOTAL_IC][3];
BMS_status bms_status;
extern BMS_temperatures bms_temperatures;
extern BMS_onboard_temperatures bms_onboard_temperatures;
extern BMS_onboard_detailed_temperatures bms_onboard_detailed_temperatures[TOTAL_IC];
BMS_voltages bms_voltages;
extern BMS_detailed_temperatures bms_detailed_temperatures[TOTAL_IC];
extern BMS_balancing_status bms_balancing_status[(TOTAL_IC + 3) / 4]; // Round up TOTAL_IC / 4 since data from 4 ICs can fit in a single message
BMS_coulomb_counts bms_coulomb_counts;

Telemetry telemetry;
HT_ADC adc;

FaultCounter overvoltage_fc(IGNORE_FAULT_THRESHOLD), undervoltage_fc(IGNORE_FAULT_THRESHOLD), total_voltage_fc(IGNORE_FAULT_THRESHOLD);

bool fh_watchdog_test = false; // Initialize test mode to false - if set to true the BMS stops sending pulse to the watchdog timer in order to test its functionality
bool watchdog_high = true; // Initialize watchdog signal - this alternates every loop

void setup() {
    setupTelemetry();

    adc.init();
    pinMode(BMS_OK, OUTPUT);
    pinMode(LED_STATUS, OUTPUT);
    pinMode(LTC6820_CS, OUTPUT);
    pinMode(WATCHDOG, OUTPUT);
    digitalWrite(BMS_OK, HIGH);
    digitalWrite(WATCHDOG, watchdog_high);

    Serial.begin(115200); // Init serial for PC communication
    CAN.begin(); // Init CAN for vehicle communication
    
    const CAN_filter_t can_filter_ccu_status = {0, 0, ID_CCU_STATUS}; // Note: If this is passed into CAN.begin() it will be treated as a mask. Instead, pass it into CAN.setFilter(), making sure to fill all slots 0-7 with duplicate filters as necessary
    for (int i = 0; i < 8; i++)  // Fill all filter slots with Charger Control Unit message filter (CAN controller requires filling all slots)
        CAN.setFilter(can_filter_ccu_status, i);

    delay(100);
    Serial.println("CAN system and serial communication initialized");

    if (MODE_CHARGE_OVERRIDE) { // Configure Charge Override Mode if enabled
        bms_status.set_state(BMS_STATE_CHARGING);
        digitalWrite(LED_STATUS, HIGH);
    }
    else
        bms_status.set_state(BMS_STATE_DISCHARGING);

    /* Ignore cells or thermistors for bench testing */
    // DEBUG Code for testing cell packs | Example:
    // for (int i=0; i<4; i++) {
    //     for (int j=0; j<9; j++) {
    //         ignore_cell[i][j] = true; // Ignore ICs 0-3
    //         total_count_cells--; // Decrement cell count (used for calculating averages)
    //     }
    // }
    // DEBUG insert cell thermistors to ignore here | Example:
    // ignore_cell_therm[5][1] = true; // Ignore IC 5 cell thermistor 1
    // total_count_cell_thermistors--; // Decrement cell thermistor count (used for calculating averages)
    // DEBUG insert PCB thermistors to ignore here | Example:
    // ignore_pcb_therm[2][0] = true; // Ignore IC 2 pcb thermistor 0
    // total_count_pcb_thermistors--; // Decrement pcb thermistor count (used for calculating averages)

    /* Ignore cells or thermistors in 2018 accumulator */
    #ifdef ACCUMULATOR_VERSION_HYTECH_2018_ACCUMULATOR
    ignore_cell_therm[6][2] = true; // Ignore IC 6 cell thermistor 2 due to faulty connector
    total_count_cell_thermistors -= 1;
    #endif

    /* Ignore cells or thermistors in 2019 accumulator */
    #ifdef ACCUMULATOR_VERSION_HYTECH_2019_ACCUMULATOR
    #endif

    /* Set up isoSPI */
    LTC::init(); // Initialize and write configuration registers to LTC6804 chips

    /* Bench test mode: check which ICs are online at startup and ignore cells from disconnected ICs */
    if (MODE_BENCH_TEST) {
        Serial.println("\nBench Test Mode: Ignoring all ICs which do not respond at startup");
        LTC6804_rdcfg(TOTAL_IC, rx_cfg); // Read back configuration registers that we just initialized
        for (int i=0; i < TOTAL_IC; i++) { // Check whether checksum is valid
            int calculated_pec = pec15_calc(6, &rx_cfg[i][0]);
            int received_pec = (rx_cfg[i][6] << 8) | rx_cfg[i][7];
            if (calculated_pec != received_pec) { // IC did not respond properly - ignore cells and thermistors
                Serial.print("Ignoring IC ");
                Serial.println(i);
                for (int j = 0; j < CELLS_PER_IC; j++)
                    ignore_cell[i][j] = true;
                total_count_cells -= CELLS_PER_IC; // Adjust cell count (used for calculating averages)
                for (int j = 0; j < THERMISTORS_PER_IC; j++)
                    ignore_cell_therm[i][j] = true;
                total_count_cell_thermistors -= THERMISTORS_PER_IC; // Adjust cell thermistor count (used for calculating averages)
                for (int j = 0; j < PCB_THERM_PER_IC; j++)
                    ignore_pcb_therm[i][j] = true;
                total_count_pcb_thermistors -= PCB_THERM_PER_IC; // Adjust cell pcb thermistor count (used for calculating averages)
            }
        }
        Serial.println();
    }
    
    Serial.println("Setup Complete!");
}

// TODO Implement Coulomb counting to track state of charge of battery.

// Main BMS Control Loop
void loop() {
    parse_can_message();

    if (timer_charge_timeout.check() && bms_status.get_state() > BMS_STATE_DISCHARGING && !MODE_CHARGE_OVERRIDE) { // 1 second timeout - if timeout is reached, disable charging
        Serial.println("Disabling charge mode - CCU timeout");
        bms_status.set_state(BMS_STATE_DISCHARGING);
        digitalWrite(LED_STATUS, LOW);
    }

    if (timer_process_cells_slow.check()) {
        process_voltages(); // Poll controllers, process values, populate bms_voltages
        Balancer::balance_cells(); // Check local cell voltage data and balance individual cells as necessary
        Temperatures::process(); // Poll controllers, process values, populate populate bms_temperatures, bms_detailed_temperatures, bms_onboard_temperatures, and bms_onboard_detailed_temperatures
        adc.process(); // Poll ADC, process values, populate bms_status
        
        #ifdef HYTECH_LOGGING_EN
            printers::print_all();
        #endif
    }

    CAN.set_timeout(4);
    telemetry.write([](uint32_t id, Interface_CAN_Container* can) { CAN.write(id, can); });
    CAN.set_timeout(0);

    if (timer_watchdog_timer.check() && !fh_watchdog_test) { // Send alternating keepalive signal to watchdog timer
        watchdog_high = !watchdog_high;
        digitalWrite(WATCHDOG, watchdog_high);
    }
}

void process_voltages() {
    poll_cell_voltages(); // Poll controller and store data in cell_voltages[] array
    uint32_t totalVolts = 0; // stored in 10 mV units
    uint16_t maxVolt = 0; // stored in 0.1 mV units
    uint16_t minVolt = 65535; // stored in 0.1 mV units
    #ifdef HYTECH_LOGGING_EN
        int maxIC = 0;
        int maxCell = 0;
        int minIC = 0;
        int minCell = 0;
    #endif
    for (int ic = 0; ic < TOTAL_IC; ic++) {
        for (int cell = 0; cell < CELLS_PER_IC; cell++) {
            bms_detailed_voltages[ic][cell / 3].set_voltage(cell % 3, cell_voltages[ic][cell]); // Populate CAN message struct
            if (!ignore_cell[ic][cell]) {
                uint16_t currentCell = cell_voltages[ic][cell];
                if (currentCell > maxVolt) {
                    maxVolt = currentCell;
                    // maxIC = ic;
                    // maxCell = cell;
                }
                if (currentCell < minVolt) {
                    minVolt = currentCell;
                    // minIC = ic;
                    // minCell = cell;
                }
                totalVolts += currentCell;
            }
        }
    }
    bms_voltages.set_average(totalVolts / total_count_cells); // stored in 0.1 mV units
    bms_voltages.set_low(minVolt);
    bms_voltages.set_high(maxVolt);
    bms_voltages.set_total(totalVolts /= 100); // convert 0.1mV units down to 10mV units

    bms_status.set_overvoltage(overvoltage_fc.update(maxVolt > VOLTAGE_CUTOFF_HIGH));
    bms_status.set_undervoltage(undervoltage_fc.update(minVolt < VOLTAGE_CUTOFF_LOW));
    bms_status.set_total_voltage_high(total_voltage_fc.update(totalVolts > TOTAL_VOLTAGE_CUTOFF));
        
    #ifdef HYTECH_LOGGING_EN
        if (overvoltage_fc.fault()) {
            Serial.println("VOLTAGE FAULT too high!!!!!!!!!!!!!!!!!!!");
            Serial.print("max IC: "); Serial.println(maxIC);
            Serial.print("max Cell: "); Serial.println(maxCell); Serial.println();
        }

        if (undervoltage_fc.fault()) {
            Serial.println("VOLTAGE FAULT too low!!!!!!!!!!!!!!!!!!!");
            Serial.print("min IC: "); Serial.println(minIC);
            Serial.print("min Cell: "); Serial.println(minCell); Serial.println();
        }
    
        if (total_voltage_fc.fault())
            Serial.println("VOLTAGE FAULT!!!!!!!!!!!!!!!!!!!");
    #endif
}

void parse_can_message() {
    while (CAN.read(rx_msg)) {
        if (rx_msg.id == ID_CCU_STATUS) { // Enter charging mode if CCU status message is received
            timer_charge_timeout.reset();
            if (bms_status.get_state() == BMS_STATE_DISCHARGING && Balancer::can_start_charging()) {
                bms_status.set_state(BMS_STATE_CHARGING);
                digitalWrite(LED_STATUS, HIGH);
            }
        }

        if (rx_msg.id == ID_FH_WATCHDOG_TEST) // Stop sending pulse to watchdog timer in order to test its functionality
            fh_watchdog_test = true;
    }
}

void setupTelemetry() {
    telemetry.insert(ID_BMS_STATUS, &bms_status, new Metro(100));
    telemetry.insert(ID_BMS_VOLTAGES, &bms_voltages, &timer_can_update_slow);
    telemetry.insert(ID_BMS_TEMPERATURES, &bms_temperatures, &timer_can_update_slow);
    telemetry.insert(ID_BMS_ONBOARD_TEMPERATURES, &bms_onboard_temperatures, &timer_can_update_slow);
    telemetry.insert(ID_BMS_DETAILED_VOLTAGES, new Abstract_CAN_Set<BMS_detailed_voltages>(&bms_detailed_voltages[0][0], 3*TOTAL_IC), &timer_can_update_slow);
    telemetry.insert(ID_BMS_DETAILED_TEMPERATURES, new Abstract_CAN_Set<BMS_detailed_temperatures>(&bms_detailed_temperatures[0], TOTAL_IC), &timer_can_update_slow);
    telemetry.insert(ID_BMS_ONBOARD_DETAILED_TEMPERATURES, new Abstract_CAN_Set<BMS_onboard_detailed_temperatures>(&bms_onboard_detailed_temperatures[0], TOTAL_IC), &timer_can_update_slow);
    telemetry.insert(ID_BMS_BALANCING_STATUS, new Abstract_CAN_Set<BMS_balancing_status>(&bms_balancing_status[0], (TOTAL_IC + 3) / 4), &timer_can_update_slow);
}