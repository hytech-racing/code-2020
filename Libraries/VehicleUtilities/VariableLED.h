#pragma once
#include <Metro.h>
#include <PackedValues.h>

enum BLINK_MODES { OFF = 0, ON = 1, FAST = 2, SLOW = 3 };
const int BLINK_RATES [4] = { 0, 0, 150, 400 }; // OFF, ON, FAST, SLOW

typedef struct _VariableLED {
    uint8_t expander_bit;
    Metro blinker;
    int mode;

    _VariableLED(uint8_t eb, bool metro_should_autoreset) : blinker(0, metro_should_autoreset) { expander_bit = eb; }

    void setMode(int m) {
        if (mode == m)
            return;
        mode = m;
        if (BLINK_RATES[m]) {
            blinker.interval(BLINK_RATES[m]);
            blinker.reset();
        }
    }

    void update(PackedValues& values) {
        if (mode == OFF)
            values.unset(expander_bit);
        else if (mode == ON)
            values.set(expander_bit);
        else if (blinker.check()) // blinker mode
            values.toggle(expander_bit);
    }
} VariableLED;