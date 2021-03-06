#pragma once
#include <Metro.h>

enum class BLINK_MODES { OFF = 0, ON = 1, FAST = 2, SLOW = 3 };
const int BLINK_RATES[4] = { 0, 0, 150, 400 }; // OFF, ON, FAST, SLOW

typedef struct VariableLED {
    Metro blinker;
    int pin;
    BLINK_MODES mode;
    bool led_value = false;

    VariableLED(int p, bool metro_should_autoreset = true) : 
        blinker(0, metro_should_autoreset),
        pin(p) {};

    void setMode(BLINK_MODES m) {
        if (mode == m)
            return;
        mode = m;
        if (BLINK_RATES[(int)m]) {
            blinker.interval(BLINK_RATES[(int)m]);
            blinker.reset();
        }
    }

    void update() {
        if (mode == BLINK_MODES::OFF)
            digitalWrite(pin, led_value = LOW);
        else if (mode == BLINK_MODES::ON)
            digitalWrite(pin, led_value = HIGH);
        else if (blinker.check()) // blinker mode
            digitalWrite(pin, led_value = !led_value);
    }
} VariableLED;