#pragma once
#include <string>
#include "MockPin.h"

// (add an extra pin because pins are 1-indexed)
#if __HT_ARDUINO__ == TEENSY_32
    #define NUM_PINS 22
#elif __HT_ARDUINO__ == TEENSY_35
    #define NUM_PINS 22
#elif __HT_ARDUINO__ == TEENSY_40
    #define NUM_PINS 22
#elif __HT_ARDUINO__ == UNO
    #define NUM_PINS 15
#else
    #error "Failed to specify board type"
#endif

extern MockPin io [NUM_PINS];

class Simulator {
public:
    static Simulator load(std::string filepath);
    Simulator(unsigned long long runtime, unsigned long long period);
    bool nextIteration();
    void cleanup();
    friend unsigned long long millis();
    friend void delay(unsigned long long);
private:
    static unsigned long long sys_time;
    const unsigned long long LOOP_PERIOD;
    const unsigned long long SIMULATION_LENGTH;
};