#pragma once
#include "fstream"
#include "string"

#define INPUT 0
#define OUTPUT 1

class MockPin {
public:
    ~MockPin();

    void init(int pin, std::string filepath);
    unsigned value();
    void write(unsigned value);
    void mode(bool mode);
private:
    unsigned long long fNextRefresh;
    unsigned fPin;
    unsigned fValue;
    int fMode = -1;
    std::fstream file;
    std::string fFilepath;
};

char* decodePinMode(int mode);