#include <Arduino.h>
#include <Simulator.h>

#if __HT_ARDUINO__ == TEENSY_32
    #include <Interrupts.h>
    #include <map>

    extern std::map<int, void(*)()> interruptMap;
#endif

int main(int argc, char* argv []) {
    if (argc < 2) {
        printf("USAGE: simulate <filepath>\n");
        return 0;
    }
    Simulator simulator = Simulator::load(argv [1]);
    setup();
    while (simulator.nextIteration()) {
        #if _HT_ARDUINO == TEENSY_32
            for (auto &it : interruptMap) it.second();
        #endif
        loop();
    }
    simulator.cleanup();
    return 0;
}