#include <HyTech_CAN.h>
#include "dashboard.h"
#include "constants.h"
#include "inverter.h"

extern DebouncedButton btn_start;
extern MCU_pedal_readings mcu_pedal_readings;
extern MCU_status mcu_status;

Metro timer_ready_sound = Metro(2000); // Time to play RTD sound
Metro timer_inverter_enable = Metro(2000); // Timeout failed inverter enable

namespace FSM {
    void update();
    void set_state(uint8_t new_state);
}

void FSM::update() {
    switch (mcu_status.get_state()) {
    
    case MCU_STATE_TRACTIVE_SYSTEM_ACTIVE:
        if (Dashboard::is_start_pressed() && mcu_pedal_readings.get_brake_pedal_active())
            set_state(MCU_STATE_WAITING_READY_TO_DRIVE_SOUND);
        break;

    case MCU_STATE_WAITING_READY_TO_DRIVE_SOUND:
        Dashboard::set_sound(HIGH);

        if (timer_ready_sound.check())
            set_state(MCU_STATE_ENABLING_INVERTER);
        break;

    case MCU_STATE_ENABLING_INVERTER:
        if (timer_inverter_enable.check()) 
            set_state(MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE);
        break;
    }
}

// Handle changes in state
void FSM::set_state(uint8_t new_state) {
    if (mcu_status.get_state() == new_state)
        return;
    mcu_status.set_state(new_state);
    if (new_state == MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE)
        Dashboard::set_start_led(OFF);
    else if (new_state == MCU_STATE_TRACTIVE_SYSTEM_ACTIVE)
        Dashboard::set_start_led(ON);
    else if (new_state == MCU_STATE_ENABLING_INVERTER) {
        Dashboard::set_sound(LOW);
        Dashboard::set_start_led(ON);

        Serial.println("RTDS deactivated\nMCU Enabling inverter"); // states are switched to fix the RTDS not being loud enough
        Inverter::enable();
        timer_inverter_enable.reset();
    }
    else if (new_state == MCU_STATE_WAITING_READY_TO_DRIVE_SOUND) {
        timer_ready_sound.reset();
        Serial.println("RTDS enabled");
    }
    else if (new_state == MCU_STATE_READY_TO_DRIVE)
        Serial.println("Inverter enabled\nReady to drive");
}