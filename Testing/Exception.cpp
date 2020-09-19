#pragma once
#include "Exception.h"
#include "MockPin.h"

InvalidPinConfigurationException::
InvalidPinConfigurationException(unsigned pin, bool expectedMode, int actualMode) {
    sprintf(&msg[0], "Invalid pin configuration -- Pin %d expected %s but got %s\n",
        pin, decodePinMode(expectedMode), decodePinMode(actualMode));
}

DoublePinModeException::
DoublePinModeException(unsigned pin, bool currentMode, bool newMode) {
    sprintf(&msg[0], "Pin mode set twice -- Pin %d was %s was set to %s\n",
        pin, decodePinMode(currentMode), decodePinMode(newMode));
}

FileNotOpenException::
FileNotOpenException(unsigned pin, std::string filepath) {
    sprintf(&msg[0], "Pin %d was unable to open file %s\n", pin, filepath);
}

