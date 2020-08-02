#include "HyTech_CAN.h"

//Constructors

GLV_current_readings::GLV_current_readings() : Abstract_CAN_Container() {};
GLV_current_readings::GLV_current_readings(uint8_t buf []) : Abstract_CAN_Container(buf) {};

GLV_current_readings::GLV_current_readings(uint16_t ecu_current_value, uint16_t cooling_current_value) : Abstract_CAN_Container() {
    set_ecu_current_value(ecu_current_value);
    set_cooling_current_value(cooling_current_value);
}

//Get Functions

uint16_t GLV_current_readings::get_ecu_current_value() {
    return message.ecu_current_value;
}

uint16_t GLV_current_readings::get_cooling_current_value() {
    return message.cooling_current_value;
}

// Set functions

void GLV_current_readings::set_ecu_current_value(uint16_t ecu_current_value) {
    message.ecu_current_value = ecu_current_value;
}

void GLV_current_readings::set_cooling_current_value(uint16_t cooling_current_value) {
    message.cooling_current_value = cooling_current_value;
}
