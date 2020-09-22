#include <Arduino.h>
#include <Simulator.h>

int main(int argc, char* argv []) {
    if (argc < 2) {
        printf("USAGE: simulate <filepath>\n");
        return 0;
    }
    Simulator simulator = Simulator::load(argv [1]);
    setup();
    while (simulator.nextIteration())
        loop();
    return 0;
}