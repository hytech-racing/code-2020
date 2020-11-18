#ifndef _AD5721_H
#define _AD5721_H

#include "Arduino.h"
#include "SPI.h"


class AD5721 {
public:
    AD5721(int chipSelectPin);
    void init();
    void writeAndUpdate(int value);
    void dataReset();
    void fullReset();


private:
    int _chipSelectPin;

    void spiWrite(uint32_t message);
};





#endif