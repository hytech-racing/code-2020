#ifndef _AD5263_H
#define _AD5263_H

#include "Arduino.h"
#include "SPI.h"


class AD5263 {
public:
    AD5263(int chipSelectPin);
    void init();
    void writeAndUpdate(uint8_t address, uint8_t value);


private:
    int _chipSelectPin;

    void spiWrite(uint8_t* message);
};






#endif
