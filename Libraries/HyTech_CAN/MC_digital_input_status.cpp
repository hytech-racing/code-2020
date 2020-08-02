/*
 * MC_digital_input_status.cpp - CAN message parser: RMS Motor Controller digital input status message
 * Created by Ryan Gallaway, December 1, 2016.
 */

#include "HyTech_CAN.h"

MC_digital_input_status::MC_digital_input_status() : Abstract_CAN_Container() {};
MC_digital_input_status::MC_digital_input_status(uint8_t buf []) : Abstract_CAN_Container(buf) {};

bool MC_digital_input_status::get_digital_input_1() {
    return message.digital_input_1;
}

bool MC_digital_input_status::get_digital_input_2() {
    return message.digital_input_2;
}

bool MC_digital_input_status::get_digital_input_3() {
    return message.digital_input_3;
}

bool MC_digital_input_status::get_digital_input_4() {
    return message.digital_input_4;
}

bool MC_digital_input_status::get_digital_input_5() {
    return message.digital_input_5;
}

bool MC_digital_input_status::get_digital_input_6() {
    return message.digital_input_6;
}

bool MC_digital_input_status::get_digital_input_7() {
    return message.digital_input_7;
}

bool MC_digital_input_status::get_digital_input_8() {
    return message.digital_input_8;
}