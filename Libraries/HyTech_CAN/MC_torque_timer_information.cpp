/*
 * MC_torque_timer_information.cpp - CAN message parser: RMS Motor Controller torque timer information message
 * Created by Nathan Cheek, November 22, 2016.
 */

#include "HyTech_CAN.h"

MC_torque_timer_information::MC_torque_timer_information() : Abstract_CAN_Container() {};
MC_torque_timer_information::MC_torque_timer_information(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t MC_torque_timer_information::get_commanded_torque() {
    return message.commanded_torque;
}

int16_t MC_torque_timer_information::get_torque_feedback() {
    return message.torque_feedback;
}

uint32_t MC_torque_timer_information::get_power_on_timer() {
    return message.power_on_timer;
}

#ifdef HYTECH_LOGGING_EN
    void MC_torque_timer_information::print(Stream& serial) {
        serial.print("COMMANDED TORQUE: ");
        serial.println(get_commanded_torque() / (double) 10, 1);
        serial.print("TORQUE FEEDBACK: ");
        serial.println(get_torque_feedback());
        serial.print("RMS UPTIME: ");
        serial.println(get_power_on_timer() * .003, 0);
    }
#endif