/*
 * BMS_voltages.cpp - CAN message parser: Battery Management System voltages message
 * Created by Shrivathsav Seshan, January 10, 2017.
 */

#include "HyTech_CAN.h"

BMS_voltages::BMS_voltages() : Abstract_CAN_Container() {};
BMS_voltages::BMS_voltages(uint8_t buf []) : Abstract_CAN_Container(buf) {};

BMS_voltages::BMS_voltages(uint16_t average_voltage, uint16_t low_voltage, uint16_t high_voltage, uint16_t total_voltage) : Abstract_CAN_Container() {
    message.average_voltage = average_voltage;
    message.low_voltage = low_voltage;
    message.high_voltage = high_voltage;
    message.total_voltage = total_voltage;
}

uint16_t BMS_voltages::get_average() {
    return message.average_voltage;
}

uint16_t BMS_voltages::get_low() {
    return message.low_voltage;
}

uint16_t BMS_voltages::get_high() {
    return message.high_voltage;
}

uint16_t BMS_voltages::get_total() {
    return message.total_voltage;
}

void BMS_voltages::set_average(uint16_t average_voltage) {
    message.average_voltage = average_voltage;
}

void BMS_voltages::set_low(uint16_t low_voltage) {
    message.low_voltage = low_voltage;
}

void BMS_voltages::set_high(uint16_t high_voltage) {
    message.high_voltage = high_voltage;
}

void BMS_voltages::set_total(uint16_t total_voltage) {
    message.total_voltage = total_voltage;
}

#ifdef HYTECH_LOGGING_EN
    void BMS_voltages::print(Stream& serial) {
        serial.print("BMS VOLTAGE AVERAGE: ");
        serial.println(bms_voltages.get_average() / (double) 10000, 4);
        serial.print("BMS VOLTAGE LOW: ");
        serial.println(bms_voltages.get_low() / (double) 10000, 4);
        serial.print("BMS VOLTAGE HIGH: ");
        serial.println(bms_voltages.get_high() / (double) 10000, 4);
        serial.print("BMS VOLTAGE TOTAL: ");
        serial.println(bms_voltages.get_total() / (double) 100, 2);*/
    }
#endif