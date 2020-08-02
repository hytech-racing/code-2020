/*
 * MC_motor_position_information.cpp - CAN message parser: RMS Motor Controller motor position information message
 * Created by Nathan Cheek, November 22, 2016.
 */

#include "HyTech_CAN.h"

MC_motor_position_information::MC_motor_position_information() : Abstract_CAN_Container() {};
MC_motor_position_information::MC_motor_position_information(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t MC_motor_position_information::get_motor_angle() {
    return message.motor_angle;
}

int16_t MC_motor_position_information::get_motor_speed() {
    return message.motor_speed;
}

int16_t MC_motor_position_information::get_electrical_output_frequency() {
    return message.electrical_output_frequency;
}

int16_t MC_motor_position_information::get_delta_resolver_filtered() {
    return message.delta_resolver_filtered;
}

#ifdef HYTECH_LOGGING_EN
    void MC_motor_position_information::print(Stream& serial) {
            serial.print("MOTOR ANGLE: ");
            serial.println(get_motor_angle());
            serial.print("MOTOR SPEED: ");
            serial.println(get_motor_speed());
            serial.print("ELEC OUTPUT FREQ: ");
            serial.println(get_electrical_output_frequency());
            serial.print("DELTA RESOLVER FILT: ");
            serial.println(get_delta_resolver_filtered());
    }
#endif