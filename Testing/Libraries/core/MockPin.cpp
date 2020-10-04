#include <Arduino.h>
#include "MockPin.h"
#include "Exception.h"

inline bool isInput(int pinMode) { return (pinMode & 1) == INPUT; }

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
    if (isInput(fMode))
        file >> fNextRefresh;
}

unsigned MockPin::value() {
    if (!isInput(fMode))
        throw InvalidPinConfigurationException(fPin, INPUT, fMode);

    if (millis() > fNextRefresh && fNextRefresh)
        file >> fValue >> fNextRefresh;

    if (fValue < 0)
        if (fMode == INPUT_PULLUP) return 1;
        else throw InvalidPinConfigurationException(fPin, INPUT_PULLUP, fMode);
    
    return fValue;
}

std::string decodePinMode(int mode) {
    if (mode == -1) return "__NOT INITIALIZED__";
    else if (mode == INPUT) return "INPUT";
    else if (mode == OUTPUT) return "OUTPUT";
    else if (mode == INPUT_PULLUP) return "INPUT_PULLUP";
    else return "__UNEXPECTED VALUE__";
}