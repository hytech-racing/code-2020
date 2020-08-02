#pragma once

#include <MCP23S17.h>
#include <HyTech_CAN.h>
#include <DebouncedButton.h>  
#include <PackedValues.h>  
#include <VariableLED.h>  
#include "inverter.h"

// Expander pin definitions
#define EXPANDER_CS 10
#define EXPANDER_SPI_SPEED 9000000      // max SPI clock frequency for MCP23S17 is 10MHz in ideal conditions

#define EXPANDER_BTN_RESTART_INVERTER 0
#define EXPANDER_BTN_START 1
#define EXPANDER_BTN_MODE 15
#define EXPANDER_READY_SOUND 8
#define EXPANDER_LED_START 9
#define EXPANDER_LED_BMS 10
#define EXPANDER_LED_IMD 11
#define EXPANDER_LED_MC_ERR 12
#define EXPANDER_LED_MODE 13

extern MCU_status mcu_status;
extern uint16_t MAX_TORQUE;
extern int16_t MAX_ACCEL_REGEN;
extern int16_t MAX_BRAKE_REGEN;

namespace Dashboard {
    namespace {
        DebouncedButton btn_start(100);
        DebouncedButton btn_mode(100);
        DebouncedButton btn_restart_inverter(100);

        VariableLED led_start(EXPANDER_LED_START, false);
        VariableLED led_mode(EXPANDER_LED_MODE, true);

        uint8_t torque_mode = 0;

        PackedValues dash_values;

        MCP23S17 EXPANDER(0, EXPANDER_CS, EXPANDER_SPI_SPEED);
    }

    void init();
    void update();
    void set_sound(bool state);
    void set_start_led(int mode);
    bool is_start_pressed();
    void read_buttons();
    void set_leds();
};

void Dashboard::init() {
    EXPANDER.begin();

    EXPANDER.pinMode(EXPANDER_BTN_RESTART_INVERTER, INPUT);
    EXPANDER.pullupMode(EXPANDER_BTN_RESTART_INVERTER, HIGH);
    EXPANDER.pinMode(EXPANDER_BTN_MODE, INPUT);
    EXPANDER.pullupMode(EXPANDER_BTN_MODE, HIGH);
    EXPANDER.pinMode(EXPANDER_BTN_START, INPUT);
    EXPANDER.pullupMode(EXPANDER_BTN_START, HIGH);
    EXPANDER.pinMode(EXPANDER_LED_BMS, OUTPUT);
    EXPANDER.pinMode(EXPANDER_LED_IMD, OUTPUT);
    EXPANDER.pinMode(EXPANDER_LED_MODE, OUTPUT);
    EXPANDER.pinMode(EXPANDER_LED_MC_ERR, OUTPUT);
    EXPANDER.pinMode(EXPANDER_LED_START, OUTPUT);
    EXPANDER.pinMode(EXPANDER_READY_SOUND, OUTPUT);
}

void Dashboard::update() {
    read_buttons();
    set_leds();
}

void Dashboard::set_sound(bool state) { EXPANDER.digitalWrite(EXPANDER_READY_SOUND, state); }

void Dashboard::set_start_led(int mode) { led_start.setMode(mode); }

bool Dashboard::is_start_pressed() { return btn_start.pressed; }

void Dashboard::read_buttons() {
    PackedValues dash_reading = { EXPANDER.digitalRead() };
    btn_start.update(dash_reading[EXPANDER_BTN_START]);
    if (btn_restart_inverter.update(dash_reading[EXPANDER_BTN_RESTART_INVERTER]))
        Inverter::reset();

    if (btn_mode.update(dash_reading[EXPANDER_BTN_MODE])) {
        torque_mode = (torque_mode + 1) % 3;
        led_mode.setMode(torque_mode);
        if (torque_mode == 0) {
            // 40
            MAX_TORQUE = 1000;
            MAX_ACCEL_REGEN = 0;
            MAX_BRAKE_REGEN = 0;
        } else if (torque_mode == 1) {
            // blink 80
            MAX_TORQUE = 1300;
            MAX_ACCEL_REGEN = 0;
            MAX_BRAKE_REGEN = -400;
        } else if (torque_mode == 2) {
            // solid 160
            MAX_TORQUE = 1600;
            MAX_ACCEL_REGEN = -100;
            MAX_BRAKE_REGEN = -400;
        }
    }
}

void Dashboard::set_leds() {
    led_mode.update(dash_values);
    led_start.update(dash_values);

    dash_values.update(EXPANDER_LED_BMS, mcu_status.get_bms_ok_high());
    dash_values.update(EXPANDER_LED_IMD, mcu_status.get_imd_okhs_high());

    EXPANDER.digitalWrite(dash_values.data);
}