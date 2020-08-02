/*
 * MC_temperatures_3.cpp - CAN message parser: RMS Motor Controller temperatures 3 message
 * Created by Nathan Cheek, November 22, 2016.
 */

#include "HyTech_CAN.h"

MC_temperatures_3::MC_temperatures_3() : Abstract_CAN_Container() {};
MC_temperatures_3::MC_temperatures_3(uint8_t buf []) : Abstract_CAN_Container(buf) {};

int16_t MC_temperatures_3::get_rtd_4_temperature() {
    return message.rtd_4_temperature;
}

int16_t MC_temperatures_3::get_rtd_5_temperature() {
    return message.rtd_5_temperature;
}

int16_t MC_temperatures_3::get_motor_temperature() {
    return message.motor_temperature;
}

int16_t MC_temperatures_3::get_torque_shudder() {
    return message.torque_shudder;
}

#ifdef HYTECH_LOGGING_EN
    void MC_temperatures_3::print(Stream& serial) {
        serial.print("RTD 4 TEMP: "); // These aren't needed since we aren't using RTDs
        serial.println(get_rtd_4_temperature());
        serial.print("RTD 5 TEMP: ");
        serial.println(get_rtd_5_temperature());
        serial.print("MOTOR TEMP: ");
        serial.println(get_motor_temperature() / (double) 10, 1);
        serial.print("TORQUE SHUDDER: ");
        serial.println(get_torque_shudder() / (double) 10, 1);*/
    }
#endif