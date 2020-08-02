/*
 *BMS_coulomb_counts - CAN message parser: Battery Management System coulomb count message
 * Created by Shaan Dhawan, March 26, 2019.
 */

#include "HyTech_CAN.h"

BMS_coulomb_counts::BMS_coulomb_counts() : Abstract_CAN_Container() {};
BMS_coulomb_counts::BMS_coulomb_counts(uint8_t buf []) : Abstract_CAN_Container(buf) {};

BMS_coulomb_counts::BMS_coulomb_counts(uint32_t total_charge, uint32_t total_discharge) : Abstract_CAN_Container() {
    set_total_charge(total_charge);
    set_total_discharge(total_discharge);
}

uint32_t BMS_coulomb_counts::get_total_charge() {
    return message.total_charge;
}

uint32_t BMS_coulomb_counts::get_total_discharge() {
    return message.total_discharge;
}

void BMS_coulomb_counts::set_total_charge(uint32_t total_charge) {
    message.total_charge = total_charge;
}

void BMS_coulomb_counts::set_total_discharge(uint32_t total_discharge) {
    message.total_discharge = total_discharge;
}

