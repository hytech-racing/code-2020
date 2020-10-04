#pragma once
#include "fstream"
#include "string"

#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

class MockPin {
public:
    ~MockPin();
    void init(int pin, std::string filepath);
    unsigned value();
    void write(unsigned value);
    void mode(bool mode);
private:
    unsigned long long fNextRefresh;
    int fPin;
    int fValue = -1;
    int fMode = -1;
    std::fstream file;
    std::string fFilepath;
};

std::string decodePinMode(int mode);