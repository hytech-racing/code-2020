#include "Arduino.h"
#include "SPI.h"
#include "AD5721.h"


AD5271::AD5721(int chipSelectPin) _chipSelectPin(chipSelectPin) { }


void AD5271::init() {
    pinMode(_chipSelectPin, OUTPUT);
}


void AD5271::writeAndUpdate(int value) {
    uint32_t message = 14;
    message <<= 4;
    message += 3; // command
    message <<= 12;
    message += value; //value
    message << 4; //don't care about next 4 bits
    spiWrite(message);
}


void AD5271::dataReset() {
    uint32_t message = 14;
    message <<= 4;
    message += 7; // command
    message <<= 16; //no value, so don't care about next 16 bits
    spiWrite(message);
}


void AD5271::fullReset() {
    uint32_t message = 14;
    message <<= 4;
    message += 15; // command
    message <<= 16; //no value, so don't care about next 16 bits
    spiWrite(message);
}


void AD5271::spiWrite(uint32_t message) {
    SPI.beginTransaction(SPISettings(50000000, MSBFIRST, SPI_MODE1));
    digitalWrite(_chipSelectPin, LOW);

    delay(100);

    SPI.transfer(&message, 3); //transfer 3 bytes of message starting from MSB

    delay(100);

    digitalWrite(_chipSelectPin, HIGH);
    SPI.endTransaction();
}




