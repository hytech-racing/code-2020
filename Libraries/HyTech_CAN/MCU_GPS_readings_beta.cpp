/*
 * MCU_GPS_readings_beta.cpp - CAN message parser: MCU_GPS Beta message
 * Created by Nathan Cheek, May 19, 2019.
 */

#include "HyTech_CAN.h"

MCU_GPS_readings_beta::MCU_GPS_readings_beta() : Abstract_CAN_Container() {};
MCU_GPS_readings_beta::MCU_GPS_readings_beta(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int32_t MCU_GPS_readings_beta::get_altitude() {
    return message.altitude;
}

int32_t MCU_GPS_readings_beta::get_speed() {
    return message.speed;
}

void MCU_GPS_readings_beta::set_altitude(int32_t altitude) {
    message.altitude = altitude;
}

void MCU_GPS_readings_beta::set_speed(int32_t speed) {
    message.speed = speed;
}

#ifdef HYTECH_LOGGING_EN
    void MCU_GPS_readings_beta::print(Stream& serial) {
        serial.print("Altitude (x10000): ");
        serial.println(get_altitude());
        serial.print("Speed (x10000): ");
        serial.println(get_speed());
    }
#endif