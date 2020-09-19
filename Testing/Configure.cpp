#include <fstream>
#include "Configure.h"
#include "Exception.h"

extern MockPin io [NUM_PINS];

unsigned long long loadConfiguration(std::string filepath) {
    std::ifstream infile;
    infile.open(filepath);
    
    unsigned long long simulationLength;
    infile >> simulationLength;
    
    int pin;
    std::string path;

    if (!infile.is_open())
        throw FileNotOpenException(-1, filepath);
    while (!infile.eof()) {
        infile >> pin >> path;
        io[pin].init(pin, path);
    }

    return simulationLength;
}