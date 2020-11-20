#ifndef _AD5684_H
#define _AD5684_H

#include "Arduino.h"
#include "SPI.h"


class AD5684 {
public:
    AD5684(int chipSelectPin);
    void init();
    void writeAndUpdate(uint8_t address, uint8_t value);
    void power();
    void reset();


private:
    int _chipSelectPin;

    void spiWrite(uint8_t* message);
};






#endif
