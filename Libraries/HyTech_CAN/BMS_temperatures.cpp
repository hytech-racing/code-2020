/*
 * BMS_temperatures.cpp - CAN message parser: Battery Management System temperatures message
 * Created by Shrivathsav Seshan, February 7, 2017.
 */

#include "HyTech_CAN.h"

BMS_temperatures::BMS_temperatures() : Abstract_CAN_Container() {};
BMS_temperatures::BMS_temperatures(uint8_t buf []) : Abstract_CAN_Container(buf) {};

BMS_temperatures::BMS_temperatures(int16_t average_temperature, int16_t low_temperature, int16_t high_temperature) : Abstract_CAN_Container() {
    set_average_temperature(average_temperature);
    set_low_temperature(low_temperature);
    set_high_temperature(high_temperature);
}

int16_t BMS_temperatures::get_average_temperature() {
    return message.average_temperature;
}

int16_t BMS_temperatures::get_low_temperature() {
    return message.low_temperature;
}

int16_t BMS_temperatures::get_high_temperature() {
    return message.high_temperature;
}

void BMS_temperatures::set_average_temperature(int16_t average_temperature) {
    message.average_temperature = average_temperature;
}

void BMS_temperatures::set_low_temperature(int16_t low_temperature) {
    message.low_temperature = low_temperature;
}

void BMS_temperatures::set_high_temperature(int16_t high_temperature) {
    message.high_temperature = high_temperature;
}

#ifdef HYTECH_LOGGING_EN
    void BMS_temperatures::print(Stream& serial) {
        serial.print("BMS AVERAGE TEMPERATURE: ");
        serial.println(get_average_temperature() / (double) 100, 2);
        serial.print("BMS LOW TEMPERATURE: ");
        serial.println(get_low_temperature() / (double) 100, 2);
        serial.print("BMS HIGH TEMPERATURE: ");
        serial.println(get_high_temperature() / (double) 100, 2);
    }
#endif
