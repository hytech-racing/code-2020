/*
 * MC_voltage_information.cpp - CAN message parser: RMS Motor Controller voltage information message
 * Created by Nathan Cheek, November 20, 2016.
 */

#include "HyTech_CAN.h"

MC_voltage_information::MC_voltage_information() : Abstract_CAN_Container() {};
MC_voltage_information::MC_voltage_information(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t MC_voltage_information::get_dc_bus_voltage() {
    return message.dc_bus_voltage;
}

int16_t MC_voltage_information::get_output_voltage() {
    return message.output_voltage;
}

int16_t MC_voltage_information::get_phase_ab_voltage() {
    return message.phase_ab_voltage;
}

int16_t MC_voltage_information::get_phase_bc_voltage() {
    return message.phase_bc_voltage;
}

#ifdef HYTECH_LOGGING_EN
    void MC_voltage_information::print(Stream& serial) {
        serial.print("DC BUS VOLTAGE: ");
        serial.println(get_dc_bus_voltage() / (double) 10, 1);
        serial.print("OUTPUT VOLTAGE: ");
        serial.println(get_output_voltage() / (double) 10, 1);
        serial.print("PHASE AB VOLTAGE: ");
        serial.println(get_phase_ab_voltage() / (double) 10, 1);
        serial.print("PHASE BC VOLTAGE: ");
        serial.println(get_phase_bc_voltage() / (double) 10, 1);
    }
#endif