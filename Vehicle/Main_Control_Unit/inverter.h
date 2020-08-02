#pragma once
#include <HyTech_CAN.h>
#include <Metro.h>
#include <HT_CAN_Util.h>

extern MCU_status mcu_status;
extern HT_CAN_Util CAN;

bool restarting = false; // True when restarting the inverter
Metro timer_restart_inverter = Metro(500, 1); // Allow the FCU to restart the inverter

namespace Inverter {
    void reset();
    void enable();
    bool finished_resetting();
}

void Inverter::reset() {
    restarting = true;
    digitalWrite(SSR_INVERTER, LOW);
    timer_restart_inverter.reset();
    mcu_status.set_inverter_powered(false);
    Serial.println("INVERTER RESET");
}

void Inverter::enable() {
    MC_command_message mc_command_message = MC_command_message(0, 0, 1, 1, 0, 0);
    CAN.write(ID_MC_COMMAND_MESSAGE, &mc_command_message, 10);  // many enable commands

    mc_command_message.set_inverter_enable(false); 
    CAN.write(ID_MC_COMMAND_MESSAGE, &mc_command_message); // disable command

    mc_command_message.set_inverter_enable(true);
    CAN.write(ID_MC_COMMAND_MESSAGE, &mc_command_message, 10); // many more enable commands

    Serial.println("MCU Sent enable command");
}

bool Inverter::finished_resetting() {
    if (restarting && timer_restart_inverter.check()) {
        restarting = false;
        return true;
    }
    return false;
}