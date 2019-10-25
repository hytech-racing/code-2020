/*
 * BMS_coulomb_counts - CAN message parser: Battery Management System coulomb count message
 * Created by Shaan Dhawan, March 26, 2019.
 */

#include "HyTech_CAN.h"

BMS_coulomb_counts::BMS_coulomb_counts() {
    message = {};
}

BMS_coulomb_counts::BMS_coulomb_counts(uint8_t buf[]) {
    load(buf);
}

BMS_coulomb_counts::BMS_coulomb_counts(uint16_t total_charge, uint16_t total_discharge, uint8_t soc_percentage) {
    set_total_charge(total_charge);
    set_total_discharge(total_discharge);
    set_SOC_Percentage(soc_percentage);
}

void BMS_coulomb_counts::load(uint8_t buf[]) {
    message = {};
    memcpy(&(message.total_charge), &buf[0], sizeof(uint16_t));
    memcpy(&(message.total_discharge), &buf[2], sizeof(uint16_t));
    memcpy(&(message.soc_percentage), &buf[4], sizeof(uint8_t));
}

void BMS_coulomb_counts::write(uint8_t buf[]) {
    memcpy(&buf[0], &(message.total_charge), sizeof(uint16_t));
    memcpy(&buf[2], &(message.total_discharge), sizeof(uint16_t));
    memcpy(&buf[4], &(message.soc_percentage), sizeof(uint8_t));
}

uint16_t BMS_coulomb_counts::get_total_charge() {
    return message.total_charge;
}

uint16_t BMS_coulomb_counts::get_total_discharge() {
    return message.total_discharge;
}

uint8_t BMS_coulomb_counts::get_SOC_Percentage() {
    return message.soc_percentage;
}

void BMS_coulomb_counts::set_total_charge(uint16_t total_charge) {
    message.total_charge = total_charge;
}

void BMS_coulomb_counts::set_total_discharge(uint16_t total_discharge) {
    message.total_discharge = total_discharge;
}

void BMS_coulomb_counts::set_SOC_Percentage(uint8_t percentage) {
    message.soc_percentage = percentage; //Chenchen change log
}

