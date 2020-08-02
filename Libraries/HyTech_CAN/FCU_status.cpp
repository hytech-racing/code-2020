/*
 * FCU_status.cpp - CAN message parser: Front Control Unit status message
 * Created by Nathan Cheek, December 22, 2017.
 */

#include "HyTech_CAN.h"

FCU_status::FCU_status() : Abstract_CAN_Container() {};
FCU_status::FCU_status(uint8_t buf []) : Abstract_CAN_Container(buf) {};

FCU_status::FCU_status(uint8_t state, uint8_t flags, uint8_t start_button_press_id) : Abstract_CAN_Container() {
    set_state(state);
    set_flags(flags);
    set_start_button_press_id(start_button_press_id);
}

uint8_t FCU_status::get_state() {
    return message.state;
}

uint8_t FCU_status::get_flags() {
    return message.flags;
}

bool FCU_status::get_accelerator_implausibility() {
    return message.flags & 0x01;
}

bool FCU_status::get_accelerator_boost_mode() {
    return (message.flags & 0x02) >> 1;
}

bool FCU_status::get_brake_implausibility() {
    return (message.flags & 0x04) >> 2;
}

bool FCU_status::get_brake_pedal_active() {
    return (message.flags & 0x08) >> 3;
}

uint8_t FCU_status::get_start_button_press_id() {
    return message.start_button_press_id;
}

void FCU_status::set_state(uint8_t state) {
    message.state = state;
}

void FCU_status::set_flags(uint8_t flags) {
    message.flags = flags;
}

void FCU_status::set_accelerator_implausibility(bool accelerator_implausibility) {
    message.flags = (message.flags & 0xFE) | (accelerator_implausibility & 0x1);
}

void FCU_status::set_accelerator_boost_mode(bool accelerator_boost_mode) {
    message.flags = (message.flags & 0xFD) | ((accelerator_boost_mode & 0x1) << 1);
}

void FCU_status::set_brake_implausibility(bool brake_implausibility) {
    message.flags = (message.flags & 0xFB) | ((brake_implausibility & 0x1) << 2);
}

void FCU_status::set_brake_pedal_active(bool brake_pedal_active) {
    message.flags = (message.flags & 0xF7) | ((brake_pedal_active & 0x1) << 3);
}

void FCU_status::set_start_button_press_id(uint8_t start_button_press_id) {
    message.start_button_press_id = start_button_press_id;
}