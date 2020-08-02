/*
 * MC_command_message.cpp - CAN message parser: RMS Motor Controller command message
 * Created by Nathan Cheek, November 20, 2016.
 */

#include "HyTech_CAN.h"

MC_command_message::MC_command_message() : Abstract_CAN_Container() {};
MC_command_message::MC_command_message(uint8_t buf []) : Abstract_CAN_Container(buf) {};

MC_command_message::MC_command_message(int16_t torque_command, int16_t angular_velocity, bool direction, bool inverter_enable, bool discharge_enable, int16_t commanded_torque_limit) : Abstract_CAN_Container() {
    set_torque_command(torque_command);
    set_angular_velocity(angular_velocity);
    set_direction(direction);
    set_inverter_enable(inverter_enable);
    set_discharge_enable(discharge_enable);
    set_commanded_torque_limit(commanded_torque_limit);
}


int16_t MC_command_message::get_torque_command() {
    return message.torque_command;
}

int16_t MC_command_message::get_angular_velocity() {
    return message.angular_velocity;
}

bool MC_command_message::get_direction() {
    return message.direction;
}

bool MC_command_message::get_inverter_enable() {
    return message.inverter_enable_discharge_enable & 0x1;
}

bool MC_command_message::get_discharge_enable() {
    return (message.inverter_enable_discharge_enable & 0x2) >> 1;
}

int16_t MC_command_message::get_commanded_torque_limit() {
    return message.commanded_torque_limit;
}

void MC_command_message::set_torque_command(int16_t torque_command) {
    message.torque_command = torque_command;
}

void MC_command_message::set_angular_velocity(int16_t angular_velocity) {
    message.angular_velocity = angular_velocity;
}

void MC_command_message::set_direction(bool direction) {
    message.direction = direction & 0x1;
}

void MC_command_message::set_inverter_enable(bool inverter_enable) {
    message.inverter_enable_discharge_enable = (message.inverter_enable_discharge_enable & 0xFE) | (inverter_enable & 0x1);
}

void MC_command_message::set_discharge_enable(bool discharge_enable) {
    message.inverter_enable_discharge_enable = (message.inverter_enable_discharge_enable & 0xFD) | ((discharge_enable & 0x1) << 1);
}

void MC_command_message::set_commanded_torque_limit(int16_t commanded_torque_limit) {
    message.commanded_torque_limit = commanded_torque_limit;
}

#ifdef HYTECH_LOGGING_EN
    void MC_command_message::print(Stream& serial) {
        serial.print("CMD_MSG TORQUE COMMAND: ");
        serial.println(get_torque_command() / (double) 10, 1);
        serial.print("CMD_MSG ANGULAR VELOCITY: ");
        serial.println(get_angular_velocity());
        serial.print("CMD_MSG DIRECTION: ");
        serial.println(get_direction());
        serial.print("CMD_MSG INVERTER ENABLE: ");
        serial.println(get_inverter_enable());
        serial.print("CMD_MSG DISCHARGE ENABLE: ");
        serial.println(get_discharge_enable());
        serial.print("CMD_MSG COMMANDED TORQUE LIMIT: ");
        serial.println(get_commanded_torque_limit() / (double) 10, 1);
    }
#endif