/*
 * TCU_distance_traveled.cpp - CAN message parser: Total distance traveled
 * Created by Peter Wagstaff, March 1, 2020.
 */

#include "HyTech_CAN.h"

TCU_distance_traveled::TCU_distance_traveled() : Abstract_CAN_Container() {};
TCU_distance_traveled::TCU_distance_traveled(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t TCU_distance_traveled::get_distance_traveled() {
    return message.distance_traveled;
}

void TCU_distance_traveled::set_distance_traveled(uint16_t value) {
    message.distance_traveled = value;
}
