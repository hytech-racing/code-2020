#include <Arduino.h>
#include "MockPin.h"
#include "Exception.h"

MockPin::~MockPin() {
    if (file.is_open())
        file.close();
}

void MockPin::init(int pin, std::string filepath) {
    fPin = pin;
    fFilepath = filepath;
}

void MockPin::write(unsigned value) {
    if (fMode != OUTPUT)
        throw InvalidPinConfigurationException(fPin, OUTPUT, fMode);
    file << millis() << ' ' << value << '\n';
}

void MockPin::mode(bool mode) {
    if (fMode != -1)
        throw DoublePinModeException(fPin, fMode, mode);

    fMode = mode;
    file.open(fFilepath, mode == OUTPUT ? std::ios::ios_base::out : std::ios::ios_base::in);
    if (!file.is_open())
        throw FileNotOpenException(fPin, fFilepath);
    if (fMode == INPUT)
        file >> fNextRefresh;
}

unsigned MockPin::value() {
    if (fMode != INPUT)
        throw InvalidPinConfigurationException(fPin, INPUT, fMode);

    if (millis() > fNextRefresh && fNextRefresh)
        file >> fValue >> fNextRefresh;
    
    return fValue;
}

std::string decodePinMode(int mode) {
    if (mode == -1) return "__NOT INITIALIZED__";
    else if (mode == 0) return "INPUT";
    else if (mode == 1) return "OUTPUT";
    else return "__UNEXPECTED VALUE__";
}