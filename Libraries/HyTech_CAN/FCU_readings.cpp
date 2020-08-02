/*
 * FCU_readings.cpp - CAN message parser: Front Control Unit readings message
 * Created by Charith (Karvin) Dassanayake, March 10, 2017.
 */

#include "HyTech_CAN.h"

/* Constructor for FCU_readings
 *
 * Used to copy data from msg variable in
 * microcontroller code to instance variable
 *
 * Param (uint16_t) - Accelerator Pedal Raw Value 1
 *     - Raw voltage readings from accelerator pedal sensor 1
 * Param (uint16_t) - Accelerator Pedal Raw Value 2
 *     - Raw voltage readings from accelerator pedal sensor 2
 * Param (uint16_t) - Brake Pedal Raw Value
 *     - Raw voltage readings from brake pedal sensor
 * Param (int16_t) - Temperature reading from temperature sensor
 *     - Temperature calculated in Celsius * 0.1
 */

FCU_readings::FCU_readings() : Abstract_CAN_Container() {};
FCU_readings::FCU_readings(uint8_t buf []) : Abstract_CAN_Container(buf) {};

FCU_readings::FCU_readings(uint16_t accelerator_pedal_raw_1, uint16_t accelerator_pedal_raw_2, uint16_t brake_pedal_raw, int16_t temperature) : Abstract_CAN_Container() {
    set_accelerator_pedal_raw_1(accelerator_pedal_raw_1);
    set_accelerator_pedal_raw_2(accelerator_pedal_raw_2);
    set_brake_pedal_raw(brake_pedal_raw);
    set_temperature(temperature);
}

/* Get functions
 *
 * Used to retrieve values stored in this CAN_message_t
 */

uint16_t FCU_readings::get_accelerator_pedal_raw_1() {
    return message.accelerator_pedal_raw_1;
}

uint16_t FCU_readings::get_accelerator_pedal_raw_2() {
    return message.accelerator_pedal_raw_2;
}

uint16_t FCU_readings::get_brake_pedal_raw() {
    return message.brake_pedal_raw;
}

int16_t FCU_readings::get_temperature() {
    return message.temperature;
}

/* Set functions
 *
 * Used to replace values in this CAN_message_t
 *
 * Param (uint16_t) - Variable to replace old data
 */

void FCU_readings::set_accelerator_pedal_raw_1(uint16_t accelerator_pedal_raw_1) {
    message.accelerator_pedal_raw_1 = accelerator_pedal_raw_1;
}

void FCU_readings::set_accelerator_pedal_raw_2(uint16_t accelerator_pedal_raw_2) {
    message.accelerator_pedal_raw_2 = accelerator_pedal_raw_2;
}

void FCU_readings::set_brake_pedal_raw(uint16_t brake_pedal_raw) {
    message.brake_pedal_raw = brake_pedal_raw;
}

void FCU_readings::set_temperature(int16_t temperature) {
    message.temperature = temperature;
}
