/*
 * MCU_status.cpp - CAN message parser: Main Control Unit status message
 * Created Feb, 2019.
 */

#include "HyTech_CAN.h"

/* Constructor for MCU_status
 *
 * Used to copy data from msg variable in
 * microcontroller code to instance variable
 *
 * Param (uint8_t) - Board State
 *     - Board state value
 * Param (uint8_t) - Board Flags
 *     - Indicators for BMS OK High, IMD OKHS High, and shutdown circuit B/C/D voltage above threshold
 * Param (int16_t) - Temperature
 *     - Value from the board thermistor (in C) times 100
 * Param (uint16_t) - GLV Battery Voltage
 *     - Battery voltage reading (in Volts) times 1000
 */

MCU_status::MCU_status() : Abstract_CAN_Container() {};
MCU_status::MCU_status(uint8_t buf []) : Abstract_CAN_Container(buf) {};

MCU_status::MCU_status(uint8_t state, uint8_t flags, int16_t temperature, uint16_t glv_battery_voltage) : Abstract_CAN_Container() {
    set_state(state);
    set_flags(flags);
    set_temperature(temperature);
    set_glv_battery_voltage(glv_battery_voltage);
}

/* Get functions
 *
 * Used to retrieve values stored in this CAN_message_t
 */


uint8_t MCU_status::get_state() {
    return message.state;
}

uint8_t MCU_status::get_flags() {
    return message.flags;
}

bool MCU_status::get_bms_ok_high() {
    return message.flags & 0x1;
}

bool MCU_status::get_imd_okhs_high() {
    return (message.flags & 0x2) >> 1;
}

bool MCU_status::get_inverter_powered() {
    return (message.flags & 0x4) >> 2;
}

bool MCU_status::get_shutdown_b_above_threshold() {
    return (message.flags & 0x8) >> 3;
}

bool MCU_status::get_shutdown_c_above_threshold() {
    return (message.flags & 0x10) >> 4;
}

bool MCU_status::get_shutdown_d_above_threshold() {
    return (message.flags & 0x20) >> 5;
}

bool MCU_status::get_shutdown_e_above_threshold() {
    return (message.flags & 0x40) >> 6;
}

bool MCU_status::get_shutdown_f_above_threshold() {
    return (message.flags & 0x80) >> 7;
}

int16_t MCU_status::get_temperature() {
    return message.temperature;
}

uint16_t MCU_status::get_glv_battery_voltage() {
    return message.glv_battery_voltage;
}

/* Set functions
 *
 * Used to replace values in this CAN_message_t
 *
 * Param (uint16_t) - Variable to replace old data
 */

void MCU_status::set_state(uint8_t state) {
    message.state = state;
}

void MCU_status::set_flags(uint8_t flags) {
    message.flags = flags;
}

void MCU_status::set_bms_ok_high(bool bms_ok_high) {
    message.flags = (message.flags & 0xFE) | (bms_ok_high & 0x1);
}

void MCU_status::set_imd_okhs_high(bool imd_okhs_high) {
    message.flags = (message.flags & 0xFD) | ((imd_okhs_high & 0x1) << 1);
}

void MCU_status::set_inverter_powered(bool inverter_powered) {
    message.flags = (message.flags & 0xFB) | ((inverter_powered & 0x1) << 2);
}

void MCU_status::set_shutdown_b_above_threshold(bool shutdown_b_above_threshold) {
    message.flags = (message.flags & 0xF7) | ((shutdown_b_above_threshold & 0x1) << 3);
}

void MCU_status::set_shutdown_c_above_threshold(bool shutdown_c_above_threshold) {
    message.flags = (message.flags & 0xEF) | ((shutdown_c_above_threshold & 0x1) << 4);
}

void MCU_status::set_shutdown_d_above_threshold(bool shutdown_d_above_threshold) {
    message.flags = (message.flags & 0xDF) | ((shutdown_d_above_threshold & 0x1) << 5);
}

void MCU_status::set_shutdown_e_above_threshold(bool shutdown_e_above_threshold) {
    message.flags = (message.flags & 0xBF) | ((shutdown_e_above_threshold & 0x1) << 6);
}

void MCU_status::set_shutdown_f_above_threshold(bool shutdown_f_above_threshold) {
    message.flags = (message.flags & 0x7F) | ((shutdown_f_above_threshold & 0x1) << 7);
}

void MCU_status::set_temperature(int16_t temperature) {
    message.temperature = temperature;
}

void MCU_status::set_glv_battery_voltage(uint16_t glv_battery_voltage) {
    message.glv_battery_voltage = glv_battery_voltage;
}