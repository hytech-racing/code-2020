/*
 * MC_temperatures_1.cpp - CAN message parser: RMS Motor Controller temperatures 1 message
 * Created by Nathan Cheek, November 22, 2016.
 */

#include "HyTech_CAN.h"

MC_temperatures_1::MC_temperatures_1() : Abstract_CAN_Container() {};
MC_temperatures_1::MC_temperatures_1(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t MC_temperatures_1::get_module_a_temperature() {
    return message.module_a_temperature;
}

int16_t MC_temperatures_1::get_module_b_temperature() {
    return message.module_b_temperature;
}

int16_t MC_temperatures_1::get_module_c_temperature() {
    return message.module_c_temperature;
}

int16_t MC_temperatures_1::get_gate_driver_board_temperature() {
    return message.gate_driver_board_temperature;
}

#ifdef HYTECH_LOGGING_EN
    void MC_temperatures_1::print(Stream& serial) {
        serial.print("MODULE A TEMP: ");
        serial.println(get_module_a_temperature() / (double) 10, 1);
        serial.print("MODULE B TEMP: ");
        serial.println(get_module_b_temperature() / (double) 10, 1);
        serial.print("MODULE C TEMP: ");
        serial.println(get_module_c_temperature() / (double) 10, 1);
        serial.print("GATE DRIVER BOARD TEMP: ");
        serial.println(get_gate_driver_board_temperature() / (double) 10, 1);
    }
#endif