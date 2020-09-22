#include <fstream>
#include "Exception.h"
#include "Simulator.h"
#include <Serial.h>

const std::string TEST_ROOT = "tests/";
extern MockSerial Serial, Serial2;

template<typename T>
void header(std::ifstream& infile, std::string header, T& response) {
    std::string fromFile; infile >> fromFile;
    if (fromFile != header) throw InvalidHeaderException(header, fromFile);
    infile >> response;
}

MockPin io [NUM_PINS];

unsigned long long Simulator::sys_time = 0;

Simulator Simulator::load(std::string filepath) {
    std::ifstream infile;
    infile.open(filepath);
    
    if (!infile.is_open())
        throw FileNotOpenException(0, filepath);
    
    std::string root;
    unsigned long long runtime, period;
    
    header<std::string>(infile, "ROOT", root);
    header<unsigned long long>(infile, "RUNTIME", runtime);
    header<unsigned long long>(infile, "PERIOD", period);

    int pin;
    std::string path;

    while (!infile.eof()) {
        infile >> pin >> path;
        if (pin > 0)
            io[pin].init(pin, TEST_ROOT + root + path);
        else if (pin == -1)
            Serial.init(TEST_ROOT + root + path);
        else if (pin == -2)
            Serial2.init(TEST_ROOT + root + path);
    }

    return Simulator(runtime, period);
}

Simulator::Simulator(unsigned long long runtime, unsigned long long period) : 
    SIMULATION_LENGTH(runtime),
    LOOP_PERIOD(period)
{
    sys_time = 0;
}

bool Simulator::nextIteration() {
    if (LOOP_PERIOD) {
        sys_time -= sys_time % LOOP_PERIOD; 
        sys_time += LOOP_PERIOD;
    }
    return sys_time <= SIMULATION_LENGTH;
}