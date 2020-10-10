#pragma once
#include <string>
#include "MockPin.h"

// (add an extra pin because pins are 1-indexed)
#if SIMULATOR_BOARD == TEENSY
    #define NUM_PINS 22
#elif SIMULATOR_BOARD == UNO
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
    unsigned long long sys_time;
    const unsigned long long LOOP_PERIOD;
    const unsigned long long SIMULATION_LENGTH;
};