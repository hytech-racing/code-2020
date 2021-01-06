#ifndef _AD5144_H
#define _AD5144_H

#include "Arduino.h"
#include "SPI.h"


class AD5144 {
public:
    AD5144(int chipSelectPin);
    void init();
    void writeAndUpdate(uint8_t address, uint8_t value);
    void reset();


private:
    int _chipSelectPin;

    void spiWrite(uint16_t message);
};

#endif
