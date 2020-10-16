#ifndef __HYTECH_SIMULATOR__
#define __HYTECH_SIMULATOR__

#include <string>
#include "MockPin.h"

// (add an extra pin because pins are 1-indexed)
#ifdef HYTECH_ARDUINO_TEENSY_32
    #define NUM_PINS 22
#endif
#ifdef HYTECH_ARDUINO_TEENSY_35
    #define NUM_PINS 22
#endif
#ifdef HYTECH_ARDUINO_TEENSY_40
    #define NUM_PINS 22
#endif
#ifdef HYTECH_ARDUINO_UNO
    #define NUM_PINS 15
#endif
#ifndef NUM_PINS
    #define NUM_PINS 0
    // #error "Failed to specify board type"
#endif

extern MockPin* io;

class Simulator {
public:
    Simulator(unsigned long long period);
    void next();
    void cleanup();
    friend unsigned long long millis();
    friend void delay(unsigned long long);
private:
    static unsigned long long sys_time;
    const unsigned long long LOOP_PERIOD;
};

#endif