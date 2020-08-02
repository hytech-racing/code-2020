/*
 * CCU_status.cpp - CAN message parser: Charger Control Unit status message
 * Created by Nathan Cheek, December 17, 2017.
 */

#include "HyTech_CAN.h"

CCU_status::CCU_status() : Abstract_CAN_Container() {};
CCU_status::CCU_status(uint8_t buf []) : Abstract_CAN_Container(buf) {};

bool CCU_status::get_charger_enabled() {
    return message.charger_enabled & 0x1;
}

void CCU_status::set_charger_enabled(bool charger_enabled) {
    message.charger_enabled = charger_enabled & 0x1;;
}
