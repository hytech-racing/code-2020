/*
 * MC_temperatures_2.cpp - CAN message parser: RMS Motor Controller temperatures 2 message
 * Created by Nathan Cheek, November 22, 2016.
 */

#include "HyTech_CAN.h"

MC_temperatures_2::MC_temperatures_2() : Abstract_CAN_Container() {};
MC_temperatures_2::MC_temperatures_2(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t MC_temperatures_2::get_control_board_temperature() {
    return message.control_board_temperature;
}

int16_t MC_temperatures_2::get_rtd_1_temperature() {
    return message.rtd_1_temperature;
}

int16_t MC_temperatures_2::get_rtd_2_temperature() {
    return message.rtd_2_temperature;
}

int16_t MC_temperatures_2::get_rtd_3_temperature() {
    return message.rtd_3_temperature;
}
