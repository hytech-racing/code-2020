/*
 * MC_analog_input_voltages.cpp - CAN message parser: RMS Motor Controller analog input voltages message
 * Created by Ryan Gallaway, December 1, 2016.
 */

#include "HyTech_CAN.h"

MC_analog_input_voltages::MC_analog_input_voltages() : Abstract_CAN_Container() {};
MC_analog_input_voltages::MC_analog_input_voltages(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t MC_analog_input_voltages::get_analog_input_1() {
    return message.analog_input_1;
}

int16_t MC_analog_input_voltages::get_analog_input_2() {
    return message.analog_input_2;
}

int16_t MC_analog_input_voltages::get_analog_input_3() {
    return message.analog_input_3;
}

int16_t MC_analog_input_voltages::get_analog_input_4() {
    return message.analog_input_4;
}
