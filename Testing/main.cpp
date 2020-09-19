#include "Arduino.h"
#include "Configure.h"

extern unsigned long long sys_time;

int main() {
    const unsigned long long SIMULATION_LENGTH = loadConfiguration("some configuration path");
    setup();
    for (sys_time; sys_time <= SIMULATION_LENGTH; sys_time += LOOP_PERIOD)
        loop();
    return 0;
}