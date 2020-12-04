#include <cstdint>

#include "ADC_SPI.h"
#include "HyTech_FlexCAN.h"
#include "HyTech_CAN.h"
#include "kinetis_flexcan.h"
#include "Metro.h"

#include "MCU_rev10_dfs.h"

// set to true or false for debugging
#define DEBUG false

/*
 * CAN messages
 */
MCU_status mcu_status{};
MCU_pedal_readings mcu_pedal_readings{};
MCU_wheel_speed mcu_wheel_speed{};
BMS_status bms_status{};
BMS_temperatures bms_temperatures{};
BMS_voltages bms_voltages{};
BMS_coulomb_counts bms_coulomb_counts{};
Dashboard_status dashboard_status{};
MC_motor_position_information mc_motor_position_information{};
MC_current_information mc_current_informtarion{};
MC_voltage_information mc_voltage_information{};
MC_internal_states mc_internal_states{};

//Timers
#if DEBUG
Metro timer_bms_imd_print_fault = Metro(500);
Metro timer_debug = Metro(200);
Metro timer_debug_raw_torque = Metro(200);
Metro timer_debug_torque = Metro(200);
Metro timer_bms_print_fault = Metro(500);
Metro timer_imd_print_fault = Metro(500);
#endif
Metro timer_inverter_enable = Metro(2000); // Timeout failed inverter enable
Metro timer_motor_controller_send = Metro(50);
Metro timer_ready_sound = Metro(2000); // Time to play RTD sound
Metro timer_can_update = Metro(100);
Metro timer_pedal_can_update = Metro(5);
Metro timer_restart_inverter = Metro(500, 1); // Allow the MCU to restart the inverter
Metro timer_status_send = Metro(100);

/*
 * Variables to store filtered values from ADC channels
 */
float filtered_accel1_reading{};
float filtered_accel2_reading{};
float filtered_brake1_reading{};
float filtered_brake2_reading{};

bool imd_faulting = false;
bool inverter_restart = false; // True when restarting the inverter

uint8_t torque_mode = 0;

uint16_t MAX_TORQUE = 600; // Torque in Nm * 10

uint32_t total_charge_amount = 0;
uint32_t total_discharge_amount = 0;

static CAN_message_t rx_msg;
static CAN_message_t tx_msg;
ADC_SPI ADC(ADC_CS, ADC_SPI_SPEED);
FlexCAN CAN(500000);

/*
 * Wheel speed stuff
 */
volatile uint8_t cur_state_front_left{};
volatile uint8_t cur_state_front_right{};
volatile uint8_t prev_state_front_left{};
volatile uint8_t prev_state_front_right{};
int cur_time_front_left{};
int cur_time_front_right{};
int prev_time_front_left{};
int prev_time_front_right{};
int total_ticks_front_left{};
int total_ticks_front_right{};
float rpm_front_left{};
float rpm_front_right{};

volatile uint8_t cur_state_back_left{};
volatile uint8_t cur_state_back_right{};
volatile uint8_t prev_state_back_left{};
volatile uint8_t prev_state_back_right{};
int cur_time_back_left{};
int cur_time_back_right{};
int prev_time_back_left{};
int prev_time_back_right{};
int total_ticks_back_left{};
int total_ticks_back_right{};
float rpm_back_left{};
float rpm_back_right{};

float total_revs{};

constexpr int num_teeth = 24; //CHANGE THIS FOR #OF TEETH PER REVOLUTION
constexpr float wheel_circumference = 1.300619; //CIRCUMFERENCE OF WHEEL IN METERS

void setup() {
    pinMode(BRAKE_LIGHT_CTRL,OUTPUT);
    pinMode(WS3_READ,INPUT);
    pinMode(WS4_READ,INPUT);
    pinMode(WS1_READ,INPUT);
    pinMode(WS2_READ,INPUT);
    pinMode(INVERTER_CTRL,OUTPUT);

    pinMode(FAN_1, OUTPUT);
    pinMode(FAN_2, OUTPUT);

    pinMode(WATCHDOG_INPUT, OUTPUT);
    pinMode(TEENSY_OK, OUTPUT);

    #if DEBUG
    Serial.begin(115200);
    #endif
    CAN.begin();

    /* Configure CAN rx interrupt */
    interrupts();
    NVIC_ENABLE_IRQ(IRQ_CAN_MESSAGE);
    attachInterruptVector(IRQ_CAN_MESSAGE,parse_can_message);
    FLEXCAN0_IMASK1 = FLEXCAN_IMASK1_BUF5M;
    /* Configure CAN rx interrupt */

    delay(500);

    #if DEBUG
    Serial.println("CAN system and serial communication initialized");
    #endif

    set_state(MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE);
    // should the inveter be powered on start up?
    digitalWrite(INVERTER_CTRL, HIGH);
    analogWrite(FAN_1, FAN_1_DUTY_CYCLE);
    analogWrite(FAN_2, FAN_2_DUTY_CYCLE);
    // these are false by default
    // mcu_status.set_bms_ok_high(false);
    // mcu_status.set_imd_okhs_high(false);
    mcu_status.set_inverter_powered(true);
}

void loop() {
    read_pedal_values();
    read_dashboard_buttons();

    set_dashboard_leds();

    /*
     * Send state over CAN
     */
    if (timer_can_update.check()) {

        // Update the status values
        read_status_values();

        // Send Main Control Unit status message
        mcu_status.write(tx_msg.buf);
        tx_msg.id = ID_MCU_STATUS;
        tx_msg.len = sizeof(CAN_message_mcu_status_t);
        CAN.write(tx_msg);

        // // Update the pedal readings to send over CAN
        // mcu_pedal_readings.set_accelerator_pedal_raw_1(filtered_accel1_reading);
        // mcu_pedal_readings.set_accelerator_pedal_raw_2(filtered_accel2_reading);
        // mcu_pedal_readings.set_brake_pedal_raw(filtered_brake_reading);

        // // Send Main Control Unit pedal reading message
        // mcu_pedal_readings.write(tx_msg.buf);
        // tx_msg.id = ID_MCU_PEDAL_READINGS;
        // tx_msg.len = sizeof(CAN_message_mcu_pedal_readings_t);
        // CAN.write(tx_msg);

        // Send couloumb counting information
        bms_coulomb_counts.set_total_charge(total_charge_amount);
        bms_coulomb_counts.set_total_discharge(total_discharge_amount);
        tx_msg.id = ID_BMS_COULOMB_COUNTS;
        tx_msg.len = sizeof(CAN_message_bms_coulomb_counts_t);
        CAN.write(tx_msg);
    }

    if (timer_pedal_can_update.check()) {
        // Update the pedal readings to send over CAN
        mcu_pedal_readings.set_accelerator_pedal_raw_1(filtered_accel1_reading);
        mcu_pedal_readings.set_accelerator_pedal_raw_2(filtered_accel2_reading);
        mcu_pedal_readings.set_brake_pedal_raw(filtered_brake_reading);
        mcu_pedal_readings.set_torque_map_mode(MAX_TORQUE);

        // Send Main Control Unit pedal reading message
        mcu_pedal_readings.write(tx_msg.buf);
        tx_msg.id = ID_MCU_PEDAL_READINGS;
        tx_msg.len = sizeof(CAN_message_mcu_pedal_readings_t);
        CAN.write(tx_msg);
    }

    /*
     * Finish restarting the inverter when timer expires
     */
    if (timer_restart_inverter.check() && inverter_restart) {
        inverter_restart = false;
        digitalWrite(SSR_INVERTER, HIGH);
        mcu_status.set_inverter_powered(true);
    }

    /*
     * State machine
     */
    switch (mcu_status.get_state()) {
        case MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE:
        break;

        case MCU_STATE_TRACTIVE_SYSTEM_ACTIVE:
        // if start button has been pressed and brake pedal is held down, transition to the next state
        if (btn_start_pressed) {
            if (mcu_pedal_readings.get_brake_pedal_active()) {
                //set_state(MCU_STATE_ENABLING_INVERTER);
                set_state(MCU_STATE_WAITING_READY_TO_DRIVE_SOUND);
            }
        }
        break;

        case MCU_STATE_WAITING_READY_TO_DRIVE_SOUND:

        EXPANDER.digitalWrite(EXPANDER_READY_SOUND, HIGH);

        if (timer_ready_sound.check()) {
            //EXPANDER.digitalWrite(EXPANDER_READY_SOUND, LOW);
            set_state(MCU_STATE_ENABLING_INVERTER);
        }
        break;

        case MCU_STATE_ENABLING_INVERTER:
        if (timer_inverter_enable.check()) {
            set_state(MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE);
        }
        break;

        // case MCU_STATE_WAITING_READY_TO_DRIVE_SOUND:

        // EXPANDER.digitalWrite(EXPANDER_READY_SOUND, HIGH);

        // if (timer_ready_sound.check()) {
        //     set_state(MCU_STATE_READY_TO_DRIVE);
        // }
        // break;

        case MCU_STATE_READY_TO_DRIVE:
        if (timer_motor_controller_send.check()) {
            MC_command_message mc_command_message = MC_command_message(0, 0, 1, 1, 0, 0);
            //read_pedal_values();

            // Check for accelerator implausibility FSAE EV2.3.10
            mcu_pedal_readings.set_accelerator_implausibility(false);
            if (mcu_pedal_readings.get_accelerator_pedal_raw_1() < MIN_ACCELERATOR_PEDAL_1 || mcu_pedal_readings.get_accelerator_pedal_raw_1() > MAX_ACCELERATOR_PEDAL_1) {
                mcu_pedal_readings.set_accelerator_implausibility(true);
            }
            if (mcu_pedal_readings.get_accelerator_pedal_raw_2() > MIN_ACCELERATOR_PEDAL_2 || mcu_pedal_readings.get_accelerator_pedal_raw_2() < MAX_ACCELERATOR_PEDAL_2) {
                mcu_pedal_readings.set_accelerator_implausibility(true);
            }

            int calculated_torque = calculate_torque();

            // FSAE EV2.5 APPS / Brake Pedal Plausibility Check
            if (mcu_pedal_readings.get_brake_implausibility() && calculated_torque < (MAX_TORQUE / 20)) {
                mcu_pedal_readings.set_brake_implausibility(false); // Clear implausibility
            }
            // if (mcu_pedal_readings.get_brake_pedal_active() && calculated_torque > (MAX_TORQUE / 4)) {
            //     mcu_pedal_readings.set_brake_implausibility(true);
            // }

            if (mcu_pedal_readings.get_brake_implausibility() || mcu_pedal_readings.get_accelerator_implausibility()) {
                // Implausibility exists, command 0 torque
                calculated_torque = 0;
            }

            // FSAE FMEA specifications // if BMS or IMD are faulting, set torque to 0
            if (!mcu_status.get_bms_ok_high() ) {
                calculated_torque = 0;
            }

            if (!mcu_status.get_imd_okhs_high()) {
                calculated_torque = 0;
            }

            if (debug && timer_debug_torque.check()) {
                Serial.print("MCU REQUESTED TORQUE: ");
                Serial.println(calculated_torque);
                Serial.print("MCU IMPLAUS ACCEL: ");
                Serial.println(mcu_pedal_readings.get_accelerator_implausibility());
                Serial.print("MCU IMPLAUS BRAKE: ");
                Serial.println(mcu_pedal_readings.get_brake_implausibility());
            }

            // Serial.print("RPM: ");
            // Serial.println(mc_motor_position_information.get_motor_speed());
            // Serial.println(calculated_torque);

            mc_command_message.set_torque_command(calculated_torque);

            mc_command_message.write(tx_msg.buf);
            tx_msg.id = ID_MC_COMMAND_MESSAGE;
            tx_msg.len = 8;
            CAN.write(tx_msg);
        }
        break;

    }

    /*
     * Send a message to the Motor Controller over CAN when vehicle is not ready to drive
     */
    if (mcu_status.get_state() < MCU_STATE_READY_TO_DRIVE && timer_motor_controller_send.check()) {
        MC_command_message mc_command_message = MC_command_message(0, 0, 1, 0, 0, 0);

        // if (mcu_status.get_state() >= MCU_STATE_ENABLING_INVERTER) {
        //      mc_command_message.set_inverter_enable(true);
        // }

        mc_command_message.write(tx_msg.buf);
        tx_msg.id = ID_MC_COMMAND_MESSAGE;
        tx_msg.len = 8;
        CAN.write(tx_msg);
    }
}

/*
 * Parse incoming CAN messages
 */
void parse_can_message() {
    while (CAN.read(rx_msg)) {
        switch (rx_msg.id) {
            case ID_MC_VOLTAGE_INFORMATION:
                mc_voltage_information.load(rx_msg.buf);
                if (mc_voltage_information.get_dc_bus_voltage() >= MIN_HV_VOLTAGE && mcu_status.get_state() == MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE) {
                    set_state(MCU_STATE::TRACTIVE_SYSTEM_ACTIVE);
                }
                if (mc_voltage_information.get_dc_bus_voltage() < MIN_HV_VOLTAGE && static_cast<uint8_t>(mcu_status.get_state()) > static_cast<uint8_t>(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE)) {
                    set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);
                }
                break;
            case ID_MC_INTERNAL_STATES:
                mc_internal_states.load(rx_msg.buf);
                if (mc_internal_states.get_inverter_enable_state() && mcu_status.get_state() == MCU_STATE::ENABLING_INVERTER) {
                    //set_state(MCU_STATE_WAITING_READY_TO_DRIVE_SOUND);
                    set_state(MCU_STATE::READY_TO_DRIVE);
                }
                if (!mc_internal_states.get_inverter_enable_state() && mcu_status.get_state() == MCU_STATE::READY_TO_DRIVE) {
                    set_state(MCU_STATE::TRACTIVE_SYSTEM_ACTIVE);
                }
                break;
            case ID_MC_MOTOR_POSITION_INFORMATION:
                mc_motor_position_information.load(rx_msg.buf);
                break;
            case ID_BMS_STATUS:
                bms_status.load(rx_msg.buf);
                break;
            case ID_BMS_TEMPERATURES:
                bms_temperatures.load(rx_msg.buf);
                break;
            case ID_BMS_VOLTAGES:
                bms_voltages.load(rx_msg.buf);
                break;
            case ID_MC_CURRENT_INFORMATION:
                if (mcu_status.get_state() == MCU_STATE::READY_TO_DRIVE) {
                    mc_current_informtarion.load(rx_msg.buf);
                    update_couloumb_count();
                }
                break;
        }
    }

}

void reset_inverter() {
    inverter_restart = true;
    digitalWrite(INVERTER_CTRL, LOW);
    timer_restart_inverter.reset();
    mcu_status.set_inverter_powered(false);
    #if DEBUG
    Serial.println("INVERTER RESET");
    #endif
}

/*
 * Read values of sensors
 */
void read_pedal_values() {

    /*
     * Filter ADC readings
     */
    filtered_accel1_reading = ALPHA * filtered_accel1_reading + (1 - ALPHA) * ADC.read_adc(ADC_ACCEL_1_CHANNEL);
    filtered_accel2_reading = ALPHA * filtered_accel2_reading + (1 - ALPHA) * ADC.read_adc(ADC_ACCEL_2_CHANNEL);
    filtered_brake1_reading = ALPHA * filtered_brake1_reading + (1 - ALPHA) * ADC.read_adc(ADC_BRAKE_1_CHANNEL);
    filtered_brake2_reading = ALPHA * filtered_brake2_reading + (1 - ALPHA) * ADC.read_adc(ADC_BRAKE_2_CHANNEL);
    #if DEBUG
    Serial.print("ACCEL 1: "); Serial.println(filtered_accel1_reading);
    Serial.print("ACCEL 2: "); Serial.println(filtered_accel2_reading);
    Serial.print("BRAKE 1: "); Serial.println(filtered_brake1_reading);
    Serial.print("BRAKE 2: "); Serial.println(filtered_brake2_reading);
    #endif

    // set the brake pedal active flag if the median reading is above the threshold
    mcu_pedal_readings.set_brake_pedal_active((filtered_brake1_reading + filtered_brake2_reading) >= BRAKE_ACTIVE);
    digitalWrite(BRAKE_LIGHT_CTRL, mcu_pedal_readings.get_brake_pedal_active());

    /*
     * Print values for debugging
     */
    #if DEBUG
    if (timer_debug.check()) {
        Serial.print("MCU PEDAL ACCEL 1: ");
        Serial.println(mcu_pedal_readings.get_accelerator_pedal_raw_1());
        Serial.print("MCU PEDAL ACCEL 2: ");
        Serial.println(mcu_pedal_readings.get_accelerator_pedal_raw_2());
        Serial.print("MCU PEDAL BRAKE: ");
        Serial.println(mcu_pedal_readings.get_brake_pedal_raw());
        Serial.print("MCU BRAKE ACT: ");
        Serial.println(mcu_pedal_readings.get_brake_pedal_active());
        Serial.print("MCU STATE: ");
        Serial.println(mcu_status.get_state());
    }
    #endif
}

void read_status_values() {
    /*
     * Check for BMS fault
     */
    uint16_t BMS_fault = analogRead(BMS_OK_READ);
    // the most likely state is high latched so optimze for that

    if (BMS_fault >= RELAY_INPUT_HIGH_LATCHED)
        // adding the comparison should be faster than adding another if block
        mcu_status.set_bms_ok_state(static_cast<SHUTDOWN_INPUTS>(2 + BMS_fault >= RELAY_INPUT_HIGH_UNLATCHED));
    else
        mcu_status.set_bms_ok_state(static_cast<SHUTDOWN_INPUTS>(BMS_fault >= RELAY_INPUT_LOW_FAULT));

    // if(BMS_fault < RELAY_INPUT_LOW_FAULT)
    //     mcu_status.set_bms_ok_state(SHUTDOWN_INPUTS::LOW);
    // else if (BMS_fault < RELAY_INPUT_HIGH_LATCHED)
    //     mcu_status.set_bms_ok_state(SHUTDOWN_INPUTS::UNKNOWN_ERROR);
    // else if (BMS_fault < RELAY_INPUT_HIGH_UNLATCHED)
    //     mcu_status.set_bms_ok_state(SHUTDOWN_INPUTS::HIGH_LATCHED);
    // else
    //     mcu_status.set_bms_ok_state(SHUTDOWN_INPUTS::HIGH_UNLATCHED);

    #if DEBUG
    if (timer_bms_print_fault.check() && mcu_status.get_bms_ok_state() < SHUTDOWN_INPUTS::RELAY_INPUT_HIGH_LATCHED) {
        Serial.println("BMS fault detected");
    }
    #endif
    /*
     * Check for IMD fault
     */
    uint16_t IMD_fault = analogRead(IMD_OK_READ);

    if (IMD_fault >= RELAY_INPUT_HIGH_LATCHED)
        // adding the comparison should be faster than adding another if block
        mcu_status.set_imd_ok_state(static_cast<SHUTDOWN_INPUTS>(2 + IMD_fault >= RELAY_INPUT_HIGH_UNLATCHED));
    else
        mcu_status.set_imd_ok_state(static_cast<SHUTDOWN_INPUTS>(IMD_fault >= RELAY_INPUT_LOW_FAULT));

    // if(IMD_fault < RELAY_INPUT_LOW_FAULT)
    //     mcu_status.set_imd_ok_state(SHUTDOWN_INPUTS::LOW);
    // else if (IMD_fault < RELAY_INPUT_HIGH_LATCHED)
    //     mcu_status.set_imd_ok_state(SHUTDOWN_INPUTS::UNKNOWN_ERROR);
    // else if (IMD_fault < RELAY_INPUT_HIGH_UNLATCHED)
    //     mcu_status.set_imd_ok_state(SHUTDOWN_INPUTS::HIGH_LATCHED);
    // else
    //     mcu_status.set_imd_ok_state(SHUTDOWN_INPUTS::HIGH_UNLATCHED);

    /*
     * Check for BSPD fault
     */
    uint16_t BSPD_fault = analogRead(BSPD_OK_READ);
    if (BSPD_fault >= RELAY_INPUT_HIGH_LATCHED)
        // adding the comparison should be faster than adding another if block
        mcu_status.set_bspd_ok_state(static_cast<SHUTDOWN_INPUTS>(2 + BSPD_fault >= RELAY_INPUT_HIGH_UNLATCHED));
    else
        mcu_status.set_bspd_ok_state(static_cast<SHUTDOWN_INPUTS>(BSPD_fault >= RELAY_INPUT_LOW_FAULT));

    // if(BSPD_fault < RELAY_INPUT_LOW_FAULT)
    //     mcu_status.set_bspd_ok_state(SHUTDOWN_INPUTS::LOW);
    // else if (BSPD_fault < RELAY_INPUT_HIGH_LATCHED)
    //     mcu_status.set_bspd_ok_state(SHUTDOWN_INPUTS::UNKNOWN_ERROR);
    // else if (BSPD_fault < RELAY_INPUT_HIGH_UNLATCHED)
    //     mcu_status.set_bspd_ok_state(SHUTDOWN_INPUTS::HIGH_LATCHED);
    // else
    //     mcu_status.set_bspd_ok_state(SHUTDOWN_INPUTS::HIGH_UNLATCHED);

    /*
     * Check for software shutdown fault
     */
    uint16_t SOFTWARE_fault = analogRead(SOFTWARE_OK_READ);
    if (SOFTWARE_fault >= RELAY_INPUT_HIGH_LATCHED)
        // adding the comparison should be faster than adding another if block
        mcu_status.set_software_ok_state(static_cast<SHUTDOWN_INPUTS>(2 + SOFTWARE_fault >= RELAY_INPUT_HIGH_UNLATCHED));
    else
        mcu_status.set_software_ok_state(static_cast<SHUTDOWN_INPUTS>(SOFTWARE_fault >= RELAY_INPUT_LOW_FAULT));

    // if(SOFTWARE_fault < RELAY_INPUT_LOW_FAULT)
    //     mcu_status.set_software_ok_state(SHUTDOWN_INPUTS::LOW);
    // else if (SOFTWARE_fault < RELAY_INPUT_HIGH_LATCHED)
    //     mcu_status.set_software_ok_state(SHUTDOWN_INPUTS::UNKNOWN_ERROR);
    // else if (SOFTWARE_fault < RELAY_INPUT_HIGH_UNLATCHED)
    //     mcu_status.set_software_ok_state(SHUTDOWN_INPUTS::HIGH_LATCHED);
    // else
    //     mcu_status.set_software_ok_state(SHUTDOWN_INPUTS::HIGH_UNLATCHED);

    /*
     * Measure shutdown circuits' voltages
     */
     mcu_status.set_shutdown_b_above_threshold(analogRead(SHUTDOWN_B_READ) > SHUTDOWOWN_X_HIGH);
     mcu_status.set_shutdown_c_above_threshold(analogRead(SHUTDOWN_C_READ) > SHUTDOWOWN_X_HIGH);
     mcu_status.set_shutdown_d_above_threshold(analogRead(SHUTDOWN_D_READ) > SHUTDOWOWN_X_HIGH);
     mcu_status.set_shutdown_e_above_threshold(analogRead(SHUTDOWN_E_READ) > SHUTDOWOWN_X_HIGH);

}

/*
 * Handle changes in state
 */
void set_state(uint8_t new_state) {
    if (mcu_status.get_state() == new_state) {
        return;
    }
    mcu_status.set_state(new_state);
    if (new_state == MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE) {
        set_start_led(0);
    }
    if (new_state == MCU_STATE_TRACTIVE_SYSTEM_ACTIVE) {
        set_start_led(2);
    }
    if (new_state == MCU_STATE_ENABLING_INVERTER) {
        EXPANDER.digitalWrite(EXPANDER_READY_SOUND, LOW);
        // states are switched to fix the RTDS not being loud enough
        Serial.println("RTDS deactivated");

        set_start_led(1);
        Serial.println("MCU Enabling inverter");
        MC_command_message mc_command_message = MC_command_message(0, 0, 1, 1, 0, 0);
        tx_msg.id = 0xC0;
        tx_msg.len = 8;

        for(int i = 0; i < 10; i++) {
            mc_command_message.write(tx_msg.buf); // many enable commands
            CAN.write(tx_msg);
        }

        mc_command_message.set_inverter_enable(false);
        mc_command_message.write(tx_msg.buf); // disable command
        CAN.write(tx_msg);

        for(int i = 0; i < 10; i++) {
            mc_command_message.set_inverter_enable(true);
            mc_command_message.write(tx_msg.buf); // many more enable commands
            CAN.write(tx_msg);
        }

        Serial.println("MCU Sent enable command");
        timer_inverter_enable.reset();
    }
    if (new_state == MCU_STATE_WAITING_READY_TO_DRIVE_SOUND) {
        timer_ready_sound.reset();
        //Serial.println("Inverter enabled");
        Serial.println("RTDS enabled");
    }
    if (new_state == MCU_STATE_READY_TO_DRIVE) {
        //EXPANDER.digitalWrite(EXPANDER_READY_SOUND, LOW);

        Serial.println("Inverter enabled");
        //Serial.println("RTDS deactivated");
        Serial.println("Ready to drive");
    }
}

int calculate_torque() {
    int calculated_torque = 0;

    //if (!mcu_pedal_readings.get_accelerator_implausibility()) {
        int torque1 = map(round(filtered_accel1_reading), START_ACCELERATOR_PEDAL_1, END_ACCELERATOR_PEDAL_1, 0, MAX_TORQUE);
        int torque2 = map(round(filtered_accel2_reading), START_ACCELERATOR_PEDAL_2, END_ACCELERATOR_PEDAL_2, 0, MAX_TORQUE);

        // torque values are greater than the max possible value, set them to max
        if (torque1 > MAX_TORQUE) {
            torque1 = MAX_TORQUE;
        }
        if (torque2 > MAX_TORQUE) {
            torque2 = MAX_TORQUE;
        }
        // compare torques to check for accelerator implausibility
        if (abs(torque1 - torque2) * 100 / MAX_TORQUE > 10) {
            mcu_pedal_readings.set_accelerator_implausibility(true);
            Serial.println("ACCEL IMPLAUSIBILITY: COMPARISON FAILED");
        } else {
            calculated_torque = (torque1 + torque2) / 2; //min(torque1, torque2);

            if (debug && timer_debug_raw_torque.check()) {
                Serial.print("TORQUE REQUEST DELTA PERCENT: "); // Print the % difference between the 2 accelerator sensor requests
                Serial.println(abs(torque1 - torque2) / (double) MAX_TORQUE * 100);
                Serial.print("MCU RAW TORQUE: ");
                Serial.println(calculated_torque);
                Serial.print("TORQUE 1: ");
                Serial.println(torque1);
                Serial.print("TORQUE 2: ");
                Serial.print(torque2);
            }
            if (calculated_torque > MAX_TORQUE) {
                calculated_torque = MAX_TORQUE;
            }
            if (calculated_torque < 0) {
                calculated_torque = 0;
            }
        }
    //}

    return calculated_torque;
}

void update_couloumb_count() {
    int new_current = mc_current_informtarion.get_dc_bus_current() * 10; // get current in Amps * 100
    if (new_current > 0) {
        total_discharge_amount += new_current; //
    } else {
        total_charge_amount -= new_current;
    }
}

/*
    if (btn_mode_debouncing && timer_btn_mode.check()) {                        // debounce period finishes
        btn_mode_pressed = !btn_mode_pressed;
        if (btn_mode_pressed) {
            torque_mode = (torque_mode + 1) % 3;
            if (torque_mode == 0) {
                set_mode_led(0); // no light
                MAX_TORQUE = 600;
                //MAX_ACCEL_REGEN = 0;
                //MAX_BRAKE_REGEN = 0;
            } else if (torque_mode == 1) {
                MAX_TORQUE = 1000;
                set_mode_led(1); // blink
                //MAX_ACCEL_REGEN = 0;
                //MAX_BRAKE_REGEN = -400;
            } else if (torque_mode == 2) {
                MAX_TORQUE = 1200;
                set_mode_led(2); // solid
                //MAX_ACCEL_REGEN = -100;
                //MAX_BRAKE_REGEN = -400;
            } //else if (torque_mode == 3) {
            //     set_mode_led(3);
            //     MAX_ACCEL_REGEN = -400;
            //     MAX_BRAKE_REGEN = 0;
            // }
        }
    }
*/