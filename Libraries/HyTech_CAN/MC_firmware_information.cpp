/*
 * MC_firmware_information.cpp - CAN message parser: RMS Motor Controller firmware information message
 * Created by Nathan Cheek, November 23, 2016.
 */

#include "HyTech_CAN.h"

MC_firmware_information::MC_firmware_information() : Abstract_CAN_Container() {};
MC_firmware_information::MC_firmware_information(uint8_t buf []) : Abstract_CAN_Container(buf) {};

uint16_t MC_firmware_information::get_eeprom_version_project_code() {
    return message.eeprom_version_project_code;
}

uint16_t MC_firmware_information::get_software_version() {
    return message.software_version;
}

uint16_t MC_firmware_information::get_date_code_mmdd() {
    return message.date_code_mmdd;
}

uint16_t MC_firmware_information::get_date_code_yyyy() {
    return message.date_code_yyyy;
}
