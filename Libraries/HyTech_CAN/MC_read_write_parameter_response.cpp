/*
 * MC_read_write_parameter_response.cpp - CAN message parser: RMS Motor Controller read / write parameter response message - response from PM
 * Created by Nathan Cheek, November 22, 2016.
 */

#include "HyTech_CAN.h"

MC_read_write_parameter_response::MC_read_write_parameter_response() : Abstract_CAN_Container() {};
MC_read_write_parameter_response::MC_read_write_parameter_response(uint8_t buf []) : Abstract_CAN_Container(buf) {};

uint16_t MC_read_write_parameter_response::get_parameter_address() {
    return message.parameter_address;
}

bool MC_read_write_parameter_response::get_write_success() {
    return message.write_success;
}

uint32_t MC_read_write_parameter_response::get_data() {
    return message.data;
}
