#include <Arduino.h>
#include <iostream>
#include "Exception.h"
#include "Serial.h"

MockSerial::MockSerial(int id) { fId = id; }

MockSerial::~MockSerial() {
    if (file.is_open())
        file.close();
}

void MockSerial::init(std::string filepath) { fFilepath = filepath; }

void MockSerial::begin(unsigned int baudRate) {
    if (file.is_open())
        throw DoublePinModeException(fId, OUTPUT, OUTPUT);
    file.open(fFilepath, std::ios::ios_base::out);
    if (!file.is_open())
        throw FileNotOpenException(fId, fFilepath);
}

void MockSerial::validate() {
    if (!file.is_open())
        throw InvalidPinConfigurationException(-1, OUTPUT, -1);
}

MockSerial Serial(-1);
MockSerial Serial2(-2);