/*
 * MC_current_information.cpp - CAN message parser: RMS Motor Controller voltage information message
 * Created by Nathan Cheek, November 20, 2016.
 */

#include "HyTech_CAN.h"

MC_current_information::MC_current_information() : Abstract_CAN_Container() {};
MC_current_information::MC_current_information(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t MC_current_information::get_phase_a_current() {
    return message.phase_a_current;
}

int16_t MC_current_information::get_phase_b_current() {
    return message.phase_b_current;
}

int16_t MC_current_information::get_phase_c_current() {
    return message.phase_c_current;
}

int16_t MC_current_information::get_dc_bus_current() {
    return message.dc_bus_current;
}

#ifdef HYTECH_LOGGING_EN
    void MC_current_information::print(Stream& serial) {
        serial.print("PHASE A CURRENT: ");
        serial.println(get_phase_a_current() / (double) 10, 1);
        serial.print("PHASE B CURRENT: ");
        serial.println(get_phase_b_current() / (double) 10, 1);
        serial.print("PHASE C CURRENT: ");
        serial.println(get_phase_c_current() / (double) 10, 1);
        serial.print("DC BUS CURRENT: ");
        serial.println(get_dc_bus_current() / (double) 10, 1);*/
    }
#endif