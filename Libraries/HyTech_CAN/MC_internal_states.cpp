/*
 * MC_internal_states.cpp - CAN message parser: RMS Motor Controller internal states message
 * Created by Nathan Cheek, November 20, 2016.
 */

#include "HyTech_CAN.h"

MC_internal_states::MC_internal_states() : Abstract_CAN_Container() {};
MC_internal_states::MC_internal_states(uint8_t buf []) : Abstract_CAN_Container(buf) {};

uint8_t MC_internal_states::get_vsm_state() {
    return message.vsm_state;
}

uint8_t MC_internal_states::get_inverter_state() {
    return message.inverter_state;
}

bool MC_internal_states::get_relay_active_1() {
    return message.relay_state & 0x01;
}

bool MC_internal_states::get_relay_active_2() {
    return (message.relay_state & 0x02) >> 1;
}

bool MC_internal_states::get_relay_active_3() {
    return (message.relay_state & 0x04) >> 2;
}

bool MC_internal_states::get_relay_active_4() {
    return (message.relay_state & 0x08) >> 3;
}

bool MC_internal_states::get_relay_active_5() {
    return (message.relay_state & 0x10) >> 4;
}

bool MC_internal_states::get_relay_active_6() {
    return (message.relay_state & 0x20) >> 5;
}

bool MC_internal_states::get_inverter_run_mode() {
    return message.inverter_run_mode_discharge_state & 0x01;
}

uint8_t MC_internal_states::get_inverter_active_discharge_state() {
    return (message.inverter_run_mode_discharge_state & 0xE0) >> 5;
}

bool MC_internal_states::get_inverter_command_mode() {
    return message.inverter_command_mode;
}

bool MC_internal_states::get_inverter_enable_state() {
    return message.inverter_enable & 0x01;
}

bool MC_internal_states::get_inverter_enable_lockout() {
    return (message.inverter_enable & 0x80) >> 7;
}

bool MC_internal_states::get_direction_command() {
    return message.direction_command;
}

#ifdef HYTECH_LOGGING_EN
    MC_internal_states::print(Stream& seral) {
        serial.print("VSM STATE: ");
        serial.println(get_vsm_state());
        serial.print("INVERTER STATE: ");
        serial.println(get_inverter_state());
        serial.print("INVERTER RUN MODE: ");
        serial.println(get_inverter_run_mode());
        serial.print("INVERTER ACTIVE DISCHARGE STATE: ");
        serial.println(get_inverter_active_discharge_state());
        serial.print("INVERTER COMMAND MODE: ");
        serial.println(get_inverter_command_mode());
        serial.print("INVERTER ENABLE: ");
        serial.println(get_inverter_enable_state());
        serial.print("INVERTER LOCKOUT: ");
        serial.println(get_inverter_enable_lockout());
        serial.print("DIRECTION COMMAND: ");
        serial.println(get_direction_command());
    }
#endif
