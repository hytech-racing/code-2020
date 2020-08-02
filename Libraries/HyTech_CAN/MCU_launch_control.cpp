/*
 * MCU_launch_control.cpp - CAN message parser: Launch Control information message
 * Created by Peter Wagstaff, March 1, 2020.
 */

#include "HyTech_CAN.h"

MCU_launch_control::MCU_launch_control() : Abstract_CAN_Container() {};
MCU_launch_control::MCU_launch_control(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t MCU_launch_control::get_slip_ratio() {
    return message.slip_ratio;
}

int16_t MCU_launch_control::get_slip_limiting_factor() {
    return message.slip_limiting_factor;
}

void MCU_launch_control::set_slip_ratio(uint16_t value) {
    message.slip_ratio = value;
}

void MCU_launch_control::set_slip_limiting_factor(uint16_t value) {
    message.slip_limiting_factor = value;
}
