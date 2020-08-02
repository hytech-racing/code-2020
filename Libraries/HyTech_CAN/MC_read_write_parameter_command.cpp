/*
 * MC_read_write_parameter_command.cpp - CAN message parser: RMS Motor Controller read / write parameter command message - sent to PM
 * Created by Nathan Cheek, November 22, 2016.
 */

#include "HyTech_CAN.h"

MC_read_write_parameter_command::MC_read_write_parameter_command() : Abstract_CAN_Container() {};
MC_read_write_parameter_command::MC_read_write_parameter_command(uint8_t buf []) : Abstract_CAN_Container(buf) {};

MC_read_write_parameter_command::MC_read_write_parameter_command(uint16_t parameter_address, bool rw_command, uint32_t data) : Abstract_CAN_Container() {
    set_parameter_address(parameter_address);
    set_rw_command(rw_command);
    set_data(data);
}

uint16_t MC_read_write_parameter_command::get_parameter_address() {
    return message.parameter_address;
}

bool MC_read_write_parameter_command::get_rw_command() {
    return message.rw_command;
}

uint32_t MC_read_write_parameter_command::get_data() {
    return message.data;
}

void MC_read_write_parameter_command::set_parameter_address(uint16_t parameter_address) {
    message.parameter_address = parameter_address;
}
void MC_read_write_parameter_command::set_rw_command(bool rw_command) {
    message.rw_command = rw_command;
}
void MC_read_write_parameter_command::set_data(uint32_t data) {
    message.data = data;
}
