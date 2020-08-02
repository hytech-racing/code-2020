/*
 * MCU_pedal_readings.cpp - CAN message parser: Main Control Unit pedal readings message
 * Created Feb, 2019.
 */

#include "HyTech_CAN.h"

/* Constructor for MCU_pedal_readings
 *
 * Used to copy data from msg variable in
 * microcontroller code to instance variable
 *
 * Param (uint16_t) - Accelerator Pedal Raw Value 1
 *     - Raw voltage readings from accelerator pedal sensor 1
 * Param (uint16_t) - Accelerator Pedal Raw Value 2
 *     - Raw voltage readings from accelerator pedal sensor 2
 * Param (uint16_t) - Brake Pedal Raw Value
 *     - Raw voltage readings from brake pedal sensor
 * Param (uint8_t) - Pedal Flags
 *     - Indicators for accelerator/brake implausibility and active brake pedal
 * Param (uint8_t) - Torque Map Mode
 *     - Torque map mode selection
 */

MCU_pedal_readings::MCU_pedal_readings() : Abstract_CAN_Container() {};
MCU_pedal_readings::MCU_pedal_readings(uint8_t buf []) : Abstract_CAN_Container(buf) {};

MCU_pedal_readings::MCU_pedal_readings(uint16_t accelerator_pedal_raw_1, uint16_t accelerator_pedal_raw_2, uint16_t brake_pedal_raw, uint8_t pedal_flags, uint8_t torque_map_mode) : Abstract_CAN_Container() {
    set_accelerator_pedal_raw_1(accelerator_pedal_raw_1);
    set_accelerator_pedal_raw_2(accelerator_pedal_raw_2);
    set_brake_pedal_raw(brake_pedal_raw);
    set_pedal_flags(pedal_flags);
    set_torque_map_mode(torque_map_mode);
}

/* Get functions
 *
 * Used to retrieve values stored in this CAN_message_t
 */

uint16_t MCU_pedal_readings::get_accelerator_pedal_raw_1() {
    return message.accelerator_pedal_raw_1;
}

uint16_t MCU_pedal_readings::get_accelerator_pedal_raw_2() {
    return message.accelerator_pedal_raw_2;
}

uint16_t MCU_pedal_readings::get_brake_pedal_raw() {
    return message.brake_pedal_raw;
}

uint8_t MCU_pedal_readings::get_pedal_flags() {
    return message.pedal_flags;
}

bool MCU_pedal_readings::get_accelerator_implausibility() {
    return message.pedal_flags & 0x01;
}

bool MCU_pedal_readings::get_brake_implausibility() {
    return (message.pedal_flags & 0x02) >> 1;
}

bool MCU_pedal_readings::get_brake_pedal_active() {
    return (message.pedal_flags & 0x04) >> 2;
}

uint8_t MCU_pedal_readings::get_torque_map_mode() {
    return message.torque_map_mode;
}

/* Set functions
 *
 * Used to replace values in this CAN_message_t
 *
 * Param (uint16_t) - Variable to replace old data
 */

void MCU_pedal_readings::set_accelerator_pedal_raw_1(uint16_t accelerator_pedal_raw_1) {
    message.accelerator_pedal_raw_1 = accelerator_pedal_raw_1;
}

void MCU_pedal_readings::set_accelerator_pedal_raw_2(uint16_t accelerator_pedal_raw_2) {
    message.accelerator_pedal_raw_2 = accelerator_pedal_raw_2;
}

void MCU_pedal_readings::set_brake_pedal_raw(uint16_t brake_pedal_raw) {
    message.brake_pedal_raw = brake_pedal_raw;
}

void MCU_pedal_readings::set_pedal_flags(uint8_t pedal_flags) {
    message.pedal_flags = pedal_flags;
}

void MCU_pedal_readings::set_accelerator_implausibility(bool accelerator_implausibility) {
    message.pedal_flags = (message.pedal_flags & 0xFE) | (accelerator_implausibility & 0x1);
}

void MCU_pedal_readings::set_brake_implausibility(bool brake_implausibility) {
    message.pedal_flags = (message.pedal_flags & 0xFD) | ((brake_implausibility & 0x1) << 1);
}

void MCU_pedal_readings::set_brake_pedal_active(bool brake_pedal_active) {
    message.pedal_flags = (message.pedal_flags & 0xFB) | ((brake_pedal_active & 0x1) << 2);
}

void MCU_pedal_readings::set_torque_map_mode(uint8_t torque_map_mode) {
    message.torque_map_mode = torque_map_mode;
}

#ifdef HYTECH_LOGGING_EN
    void MCU_pedal_readings::print(Stream& serial) {
        serial.print("MCU PEDAL ACCEL 1: ");
        serial.println(get_accelerator_pedal_raw_1());
        serial.print("MCU PEDAL ACCEL 2: ");
        serial.println(get_accelerator_pedal_raw_2());
        serial.print("MCU PEDAL BRAKE: ");
        serial.println(get_brake_pedal_raw());
        serial.print("MCU BRAKE ACT: ");
        serial.println(get_brake_pedal_active());
        serial.print("MCU STATE: ");
        serial.println(get_state());
    }
#endif