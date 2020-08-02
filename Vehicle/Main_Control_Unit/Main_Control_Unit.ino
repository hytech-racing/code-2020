#include <ADC_SPI.h>
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <kinetis_flexcan.h>
#include <Metro.h>
#include <Wire.h>
#include "constants.h"
#include "dashboard.h"
#include "finite_state_machine.h"
#include "torque.h"
#include "HT_CAN_Util.h"

// Global variables
MCU_status mcu_status;
MCU_pedal_readings mcu_pedal_readings;
MC_motor_position_information mc_motor_position_information;
MCU_launch_control mcu_launch_control;

// Timers
// Metro timer_debug = Metro(200);
// Metro timer_debug_raw_torque = Metro(200);
// Metro timer_debug_torque = Metro(200);
Metro timer_motor_controller_send = Metro(50);
Metro timer_can_update = Metro(100);
Metro timer_bms_print_fault = Metro(500);
Metro timer_imd_print_fault = Metro(500);

// Variables to store filtered values from ADC channels
float filtered_accel1_reading = 0;
float filtered_accel2_reading = 0;
float filtered_brake_reading = 0;

#define LAUNCH_CONTROL_ACTIVE true
float rear_rpm = 0;
float front_rpm = 0;

int16_t mc_motor_speed = 0;

uint16_t MAX_TORQUE = MAX_POSSIBLE_TORQUE; // Torque in Nm * 10
int16_t MAX_ACCEL_REGEN = 0;
int16_t MAX_BRAKE_REGEN = 0;

static CAN_message_t rx_msg;
ADC_SPI ADC(ADC_CS, ADC_SPI_SPEED);
HT_CAN_Util CAN(500000);

void setup() {    
    pinMode(SOFTWARE_SHUTDOWN_RELAY, OUTPUT);
    pinMode(SSR_INVERTER, OUTPUT);
    pinMode(SSR_BRAKE_LIGHT, OUTPUT);
    pinMode(PUMP_CTRL, OUTPUT);
    pinMode(FAN_1, OUTPUT);
    pinMode(FAN_2, OUTPUT);
    pinMode(SENSE_SHUTDOWN_B, INPUT);
    pinMode(SENSE_SHUTDOWN_C, INPUT);
    pinMode(SENSE_SHUTDOWN_E, INPUT);

    Serial.begin(115200);
    CAN.begin();

    /* Configure CAN rx interrupt */
    interrupts();
    NVIC_ENABLE_IRQ(IRQ_CAN_MESSAGE);
    attachInterruptVector(IRQ_CAN_MESSAGE, parse_can_message);
    FLEXCAN0_IMASK1 = FLEXCAN_IMASK1_BUF5M;

    delay(500);

    Serial.println("CAN system and serial communication initialized");

    FSM::set_state(MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE);
    digitalWrite(SOFTWARE_SHUTDOWN_RELAY, HIGH);
    digitalWrite(SSR_INVERTER, HIGH);
    digitalWrite(PUMP_CTRL, HIGH);
    //analogWrite(FAN_1, FAN_1_DUTY_CYCLE);
    analogWrite(FAN_2, FAN_2_DUTY_CYCLE);
    mcu_status.set_bms_ok_high(true);
    mcu_status.set_imd_okhs_high(true);
    mcu_status.set_inverter_powered(true);
}

void loop() {
    read_pedal_values();
    Dashboard::update();
    #if LAUNCH_CONTROL_ACTIVE 
        Torque::update_slip_limiting_factor(front_rpm, rear_rpm); //use the pid to update the slip limiting factor based on current slip ratio
    #endif

    // Send state over CAN
    if (timer_can_update.check()) {
        read_status_values();

        // Update the pedal readings and send over CAN
        mcu_pedal_readings.set_accelerator_pedal_raw_1(filtered_accel1_reading);
        mcu_pedal_readings.set_accelerator_pedal_raw_2(filtered_accel2_reading);
        mcu_pedal_readings.set_brake_pedal_raw(filtered_brake_reading);

        CAN.write(ID_MCU_STATUS, &mcu_status);
        CAN.write(ID_MCU_PEDAL_READINGS, &mcu_pedal_readings);
        CAN.write(ID_MCU_LAUNCH_CONTROL, &mcu_launch_control);
    }

    // Finish restarting the inverter when timer expires
    if (Inverter::finished_resetting()) {
        digitalWrite(SSR_INVERTER, HIGH);
        mcu_status.set_inverter_powered(true);
    }

    // State machine
    if (mcu_status.get_state() != MCU_STATE_READY_TO_DRIVE)
        FSM::update();

    if (!timer_motor_controller_send.check())
        return;

    // Send a message to the Motor Controller over CAN when vehicle is not ready to drive
    if (mcu_status.get_state() < MCU_STATE_READY_TO_DRIVE)
        CAN.write(ID_MC_COMMAND_MESSAGE, &MC_command_message(0, 0, 1, 0, 0, 0));

    else if (mcu_status.get_state() == MCU_STATE_READY_TO_DRIVE) {
        // Check for accelerator implausibility FSAE EV2.3.10
        mcu_pedal_readings.set_accelerator_implausibility(
            mcu_pedal_readings.get_accelerator_pedal_raw_1() < MIN_ACCELERATOR_PEDAL_1 
            || mcu_pedal_readings.get_accelerator_pedal_raw_1() > MAX_ACCELERATOR_PEDAL_1
            || mcu_pedal_readings.get_accelerator_pedal_raw_2() > MIN_ACCELERATOR_PEDAL_2 
            || mcu_pedal_readings.get_accelerator_pedal_raw_2() < MAX_ACCELERATOR_PEDAL_2);

        int calculated_torque = Torque::calculate_torque();

        // FSAE EV2.5 APPS / Brake Pedal Plausibility Check
        if (mcu_pedal_readings.get_brake_implausibility() && calculated_torque < (MAX_TORQUE / 20))
            mcu_pedal_readings.set_brake_implausibility(false); // Clear implausibility

        // if (mcu_pedal_readings.get_brake_pedal_active() && calculated_torque > (MAX_TORQUE / 4)) {
        //     mcu_pedal_readings.set_brake_implausibility(true);
        // }

        if (mcu_pedal_readings.get_brake_implausibility() || mcu_pedal_readings.get_accelerator_implausibility()) // Implausibility exists, command 0 torque
                calculated_torque = 0;

        // FSAE FMEA specifications - if BMS or IMD are faulting, set torque to 0
        if (!mcu_status.get_bms_ok_high() || !mcu_status.get_imd_okhs_high())
            calculated_torque = 0;

        // if (timer_debug_torque.check()) {
        //     Serial.print("MCU REQUESTED TORQUE: ");
        //     Serial.println(calculated_torque);
        //     Serial.print("MCU IMPLAUS ACCEL: ");
        //     Serial.println(mcu_pedal_readings.get_accelerator_implausibility());
        //     Serial.print("MCU IMPLAUS BRAKE: ");
        //     Serial.println(mcu_pedal_readings.get_brake_implausibility());
        // }
        // Serial.print("RPM: ");
        // Serial.println(mc_motor_position_information.get_motor_speed());
        // Serial.println(calculated_torque);

        MC_command_message mc_command_message = MC_command_message(0, 0, 1, 1, 0, 0);
        mc_command_message.set_torque_command(calculated_torque);
        CAN.write(ID_MC_COMMAND_MESSAGE, &mc_command_message);
    }
}

// Parse incoming CAN messages
void parse_can_message() {
    while (CAN.read(rx_msg)) {
        if (rx_msg.id == ID_MC_VOLTAGE_INFORMATION) {
            MC_voltage_information mc_voltage_information = MC_voltage_information(rx_msg.buf);
            if (mc_voltage_information.get_dc_bus_voltage() >= MIN_HV_VOLTAGE && mcu_status.get_state() == MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE)
                FSM::set_state(MCU_STATE_TRACTIVE_SYSTEM_ACTIVE);
            if (mc_voltage_information.get_dc_bus_voltage() < MIN_HV_VOLTAGE && mcu_status.get_state() > MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE)
                FSM::set_state(MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE);
        }

        else if (rx_msg.id == ID_MC_INTERNAL_STATES) {
            MC_internal_states mc_internal_states = MC_internal_states(rx_msg.buf);
            if (mc_internal_states.get_inverter_enable_state() && mcu_status.get_state() == MCU_STATE_ENABLING_INVERTER)
                FSM::set_state(MCU_STATE_READY_TO_DRIVE);
            if (!mc_internal_states.get_inverter_enable_state() && mcu_status.get_state() == MCU_STATE_READY_TO_DRIVE)
                FSM::set_state(MCU_STATE_TRACTIVE_SYSTEM_ACTIVE);
        }

        else if (rx_msg.id == ID_MC_MOTOR_POSITION_INFORMATION) 
            mc_motor_speed = mc_motor_speed;
        else if (rx_msg.id == ID_TCU_WHEEL_RPM_REAR) 
            rear_rpm = TCU_wheel_rpm(rx_msg.buf).get_wheel_rpm();
        else if (rx_msg.id == ID_TCU_WHEEL_RPM_FRONT) 
            front_rpm = TCU_wheel_rpm(rx_msg.buf).get_wheel_rpm() * 2; // Multiplying by 2 because currently only one sensor is installed
    }

}

// Read values of sensors
void read_pedal_values() {
    // Filter ADC readings
    filtered_accel1_reading = ALPHA * filtered_accel1_reading + (1 - ALPHA) * ADC.read_adc(ADC_ACCEL_1_CHANNEL);
    filtered_accel2_reading = ALPHA * filtered_accel2_reading + (1 - ALPHA) * ADC.read_adc(ADC_ACCEL_2_CHANNEL);
    filtered_brake_reading  = ALPHA * filtered_brake_reading  + (1 - ALPHA) * ADC.read_adc(ADC_BRAKE_CHANNEL);

    // set the brake pedal active flag if the median reading is above the threshold
    mcu_pedal_readings.set_brake_pedal_active(filtered_brake_reading >= BRAKE_ACTIVE);
    digitalWrite(SSR_BRAKE_LIGHT, mcu_pedal_readings.get_brake_pedal_active());

    // Print values for debugging
    #ifdef HYTECH_LOGGING_EN
        if (timer_debug.check()) 
            mcu_pedal_readings.print(Serial);
    #endif
}

void read_status_values() {
    // Filter ADC readings of GLV voltage
    mcu_status.set_glv_battery_voltage(ADC.read_adc(ADC_12V_SUPPLY_CHANNEL) * GLV_VOLTAGE_MULTIPLIER); // convert GLV voltage and to send it over CAN

    // Check for BMS fault
    bool bms_fault = ADC.read_adc(ADC_BMS_OK_CHANNEL) > BMS_HIGH;
    mcu_status.set_bms_ok_high(bms_fault);
    if (bms_fault && timer_bms_print_fault.check())
        Serial.println("BMS fault detected");

    // Check for IMD fault
    bool imd_fault = ADC.read_adc(ADC_OKHS_CHANNEL) > IMD_HIGH;
    mcu_status.set_imd_okhs_high(imd_fault);
    if (imd_fault && timer_imd_print_fault.check())
        Serial.println("IMD fault detected");

    // Measure shutdown circuits' voltages
     mcu_status.set_shutdown_b_above_threshold(analogRead(SENSE_SHUTDOWN_B) > SHUTDOWN_B_HIGH);
     mcu_status.set_shutdown_c_above_threshold(analogRead(SENSE_SHUTDOWN_C) > SHUTDOWN_C_HIGH);
     mcu_status.set_shutdown_d_above_threshold(ADC.read_adc(ADC_SHUTDOWN_D_READ_CHANNEL) > SHUTDOWN_D_HIGH);
     mcu_status.set_shutdown_e_above_threshold(analogRead(SENSE_SHUTDOWN_E) > SHUTDOWN_E_HIGH);
     mcu_status.set_shutdown_f_above_threshold(analogRead(SENSE_SHUTDOWN_F) > SHUTDOWN_F_HIGH);

     // Measure the temperature from on-board thermistors
     mcu_status.set_temperature(ADC.read_adc(ADC_TEMPSENSE_CHANNEL) * 100); // send temperatures in 0.01 C
}