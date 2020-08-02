/*
 * BMS_detailed_temperatures.cpp - CAN message parser: Battery Management System detailed temperatures message
 * Created by Nathan Cheek, December 17, 2017.
 */

#include "HyTech_CAN.h"

BMS_detailed_temperatures::BMS_detailed_temperatures() : Abstract_CAN_Container() {};
BMS_detailed_temperatures::BMS_detailed_temperatures(uint8_t buf []) : Abstract_CAN_Container(buf) {};

BMS_detailed_temperatures::BMS_detailed_temperatures(uint8_t ic_id, int16_t temperature_0, int16_t temperature_1, int16_t temperature_2) : Abstract_CAN_Container() {
    set_ic_id(ic_id);
    set_temperature_0(temperature_0);
    set_temperature_1(temperature_1);
    set_temperature_2(temperature_2);
}

uint8_t BMS_detailed_temperatures::get_ic_id() {
    return message.ic_id;
}

int16_t BMS_detailed_temperatures::get_temperature_0() {
    return message.temperature_0;
}

int16_t BMS_detailed_temperatures::get_temperature_1() {
    return message.temperature_1;
}

int16_t BMS_detailed_temperatures::get_temperature_2() {
    return message.temperature_2;
}

int16_t BMS_detailed_temperatures::get_temperature(uint8_t temperature_id) {
    if (temperature_id == 0)
        return message.temperature_0;
    else if (temperature_id == 1)
        return message.temperature_1;
    else if (temperature_id == 2)
        return message.temperature_2;
    return 0;
}

void BMS_detailed_temperatures::set_ic_id(uint8_t ic_id) {
    message.ic_id = ic_id;
}

void BMS_detailed_temperatures::set_temperature_0(int16_t temperature_0) {
    message.temperature_0 = temperature_0;
}

void BMS_detailed_temperatures::set_temperature_1(int16_t temperature_1) {
    message.temperature_1 = temperature_1;
}

void BMS_detailed_temperatures::set_temperature_2(int16_t temperature_2) {
    message.temperature_2 = temperature_2;
}

void BMS_detailed_temperatures::set_temperature(uint8_t temperature_id, int16_t temperature) {
    if (temperature_id == 0)
        message.temperature_0 = temperature;
    else if (temperature_id == 1)
        message.temperature_1 = temperature;
    else if (temperature_id == 2)
        message.temperature_2 = temperature;
}

uint8_t BMS_detailed_temperatures::getIndex() {
    return get_ic_id();
}

void BMS_detailed_temperatures::setIndex(uint8_t index) {
    set_ic_id(index);
}