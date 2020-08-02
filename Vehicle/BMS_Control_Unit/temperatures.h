#include "poll_voltages.h"


extern uint8_t total_count_cells;
extern uint8_t total_count_cell_thermistors;
extern uint8_t total_count_pcb_thermistors;

extern int8_t ignore_pcb_therm[TOTAL_IC][PCB_THERM_PER_IC]; // PCB thermistors to be ignored
extern int8_t ignore_cell_therm[TOTAL_IC][THERMISTORS_PER_IC]; // Cell thermistors to be ignored

BMS_temperatures bms_temperatures;
BMS_onboard_temperatures bms_onboard_temperatures;
BMS_detailed_temperatures bms_detailed_temperatures[TOTAL_IC];
BMS_onboard_detailed_temperatures bms_onboard_detailed_temperatures[TOTAL_IC];


namespace Temperatures {
    namespace { FaultCounter thermistor_fc(IGNORE_FAULT_THRESHOLD); }
    void process();
    void process_cell_temps();
    void process_onboard_temps();
}

void Temperatures::process() {
    poll_aux_voltages(); // Poll controllers and store data in aux_voltages[] array
    process_cell_temps(); // Process values, populate populate bms_temperatures and bms_detailed_temperatures
    process_onboard_temps(); // Process values, populate bms_onboard_temperatures and bms_onboard_detailed_temperatures
}

/* aux_voltage = (R/(10k+R))*v_ref
 * R = 10k * aux_voltage / (v_ref - aux_voltage)
 * 
 * Temperature equation (in Kelvin) based on resistance is the following:
 * 1/T = 1/T0 + (1/B) * ln(R/R0)      (R = thermistor resistance)
 * T = 1/(1/T0 + (1/B) * ln(R/R0))
 */
int16_t calculate_temp(double aux_voltage, double v_ref, double thermistor_b) {
    double thermistor_resistance = aux_voltage / (v_ref - aux_voltage) * 1e4;
    double temperature = 1 / ((1 / THERMISTOR_T0) + (1 / thermistor_b) * log(thermistor_resistance / THERMISTOR_R0)) - 273.15;
    return (int16_t)(temperature * 100);
}

typedef struct {
    double totalTemp;
    int16_t lowTemp, highTemp;
} temp_data;

template<class T, size_t therm_per_ic, uint8_t offset = 0>
temp_data poll_temperatures(T* temperature_arr, int8_t (&ignore_therm)[TOTAL_IC][therm_per_ic], int16_t thermistor_b_const) {
    double totalTemp = 0;
    int16_t lowTemp = 9999; // Alternatively use first value from array, but that could be an ignored thermistor
    int16_t highTemp = -9999;
    
    for (int ic = 0; ic < TOTAL_IC; ic++)
        for (uint8_t j = 0; j < therm_per_ic; j++) {
            if (!ignore_therm[ic][j]) {
                int16_t thermTemp = calculate_temp(aux_voltages[ic][j+offset], aux_voltages[ic][5], thermistor_b_const); // aux_voltages[ic][5] stores the reference voltage
                if (thermTemp < lowTemp)
                    lowTemp = thermTemp;
                if (thermTemp > highTemp)
                    highTemp = thermTemp;
                temperature_arr[ic].set_temperature(j, thermTemp); // Populate CAN message struct
                totalTemp += thermTemp;
            }
        }
    
    return { totalTemp, lowTemp, highTemp };
}

void Temperatures::process_cell_temps() { // Note: For up-to-date information you must poll the LTC6820s with poll_aux_voltages() before calling this function
    temp_data td = poll_temperatures<BMS_detailed_temperatures, THERMISTORS_PER_IC>(bms_detailed_temperatures, ignore_cell_therm, THERMISTOR_CELL_B);
    int16_t lowTemp = td.lowTemp;
    int16_t highTemp = td.highTemp;
    double totalTemp = td.totalTemp;
    
    bms_temperatures.set_low_temperature(lowTemp);
    bms_temperatures.set_high_temperature(highTemp);
    bms_temperatures.set_average_temperature((int16_t) (totalTemp / total_count_cell_thermistors));

    bms_status.set_discharge_overtemp(false);
    bms_status.set_charge_overtemp(false);

    if (bms_status.get_state() == BMS_STATE_DISCHARGING) // Check temperatures if discharging
        bms_status.set_discharge_overtemp(thermistor_fc.update(highTemp > DISCHARGE_TEMP_CELL_CRITICAL_HIGH));
    else if (bms_status.get_state() >= BMS_STATE_CHARGING) // Check temperatures if charging
        bms_status.set_charge_overtemp(thermistor_fc.update(highTemp > CHARGE_TEMP_CELL_CRITICAL_HIGH));
    else
        thermistor_fc.update(false);
    
    #ifdef HYTECH_LOGGING_EN
        if (thermistor_fc.fault())
            Serial.println("TEMPERATURE FAULT!!!!!!!!!!!!!!!!!!!");
    #endif
}

void Temperatures::process_onboard_temps() { // Note: For up-to-date information you must poll the LTC6820s with poll_aux_voltages() before calling this function
    temp_data td = poll_temperatures<BMS_onboard_detailed_temperatures, PCB_THERM_PER_IC, 3>(bms_onboard_detailed_temperatures, ignore_pcb_therm, THERMISTOR_ONBOARD_B);
    int16_t lowTemp = td.lowTemp;
    int16_t highTemp = td.highTemp;
    double totalTemp = td.totalTemp;

    bms_onboard_temperatures.set_low_temperature((int16_t) lowTemp);
    bms_onboard_temperatures.set_high_temperature((int16_t) highTemp);
    bms_onboard_temperatures.set_average_temperature((int16_t) (totalTemp / total_count_pcb_thermistors));

    bms_status.set_onboard_overtemp(thermistor_fc.update(highTemp > ONBOARD_TEMP_CRITICAL_HIGH)); // RESET this value, then check below if they should be set

    if (bms_status.get_state() == BMS_STATE_BALANCING && highTemp >= ONBOARD_TEMP_BALANCE_DISABLE) {
        bms_status.set_state(BMS_STATE_BALANCING_OVERHEATED);
        Serial.println("WARNING: Onboard temperature too high; disabling balancing");
    } else if (bms_status.get_state() == BMS_STATE_BALANCING_OVERHEATED && bms_onboard_temperatures.get_high_temperature() < ONBOARD_TEMP_BALANCE_REENABLE) {
        bms_status.set_state(BMS_STATE_BALANCING);
        Serial.println("CLEARED: Onboard temperature OK; reenabling balancing");
    }
}
