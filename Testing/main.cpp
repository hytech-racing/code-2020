#include <Arduino.h>
#include <Simulator.h>

int main(int argc, char* argv []) {
    Simulator simulator (100);
    while (millis() < 1000) {
        simulator.next();
    }
    simulator.cleanup();
    return 0;
}