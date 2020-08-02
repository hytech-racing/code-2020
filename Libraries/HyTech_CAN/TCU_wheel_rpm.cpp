/*
 * TCU_wheel_rpm.cpp - CAN message parser: Wheel rpm sensor information message
 * Created by Nathan Cheek, November 22, 2016.
 */

#include "HyTech_CAN.h"

TCU_wheel_rpm::TCU_wheel_rpm() : Abstract_CAN_Container() {};
TCU_wheel_rpm::TCU_wheel_rpm(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t TCU_wheel_rpm::get_wheel_rpm_left() {
    return message.wheel_rpm_left;
}

int16_t TCU_wheel_rpm::get_wheel_rpm_right() {
    return message.wheel_rpm_right;
}

float TCU_wheel_rpm::get_wheel_rpm() {
    return (message.wheel_rpm_left + message.wheel_rpm_right) / 200.;
}

void TCU_wheel_rpm::set_wheel_rpm_left(uint16_t value) {
    message.wheel_rpm_left = value;
}

void TCU_wheel_rpm::set_wheel_rpm_right(uint16_t value) {
    message.wheel_rpm_right = value;
}