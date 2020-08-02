/*
 * MCU_GPS_readings_alpha.cpp - CAN message parser: MCU_GPS Alpha message
 * Created by Nathan Cheek, May 19, 2019.
 */

#include "HyTech_CAN.h"

MCU_GPS_readings_alpha::MCU_GPS_readings_alpha() : Abstract_CAN_Container() {};
MCU_GPS_readings_alpha::MCU_GPS_readings_alpha(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int32_t MCU_GPS_readings_alpha::get_latitude() {
    return message.latitude;
}

int32_t MCU_GPS_readings_alpha::get_longitude() {
    return message.longitude;
}

void MCU_GPS_readings_alpha::set_latitude(int32_t latitude) {
    message.latitude = latitude;
}

void MCU_GPS_readings_alpha::set_longitude(int32_t longitude) {
    message.longitude = longitude;
}

#ifdef HYTECH_LOGGING_EN
    void MCU_GPS_readings_alpha::print(Stream& serial) {
        serial.print("Latitude (x10000): ");
        serial.println(get_latitude());
        serial.print("Longitude (x10000): ");
        serial.println(get_longitude());
    }
#endif