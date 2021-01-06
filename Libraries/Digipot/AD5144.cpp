#include "Arduino.h"
#include "SPI.h"
#include "AD5144.h"


AD5144::AD5144(int chipSelectPin) : _chipSelectPin(chipSelectPin) { }


void AD5144::init() {
    pinMode(_chipSelectPin, OUTPUT);
    digitalWrite(_chipSelectPin, HIGH);
}


void AD5144::writeAndUpdate(uint8_t address, uint8_t value) {
    uint16_t message = 1;
    message <<= 4;
    message += address;
    message <<= 8;
    message += value;

    spiWrite(message);
}

void AD5144::reset() {
    uint16_t message = 11;
    message <<= 12;
    spiWrite(message);
}

void AD5144::spiWrite(uint16_t message) {
    SPI.beginTransaction(SPISettings(3000000, MSBFIRST, SPI_MODE1));
    digitalWrite(_chipSelectPin, LOW);

    delay(100);

    SPI.transfer16(message); //transfer 2 bytes of message starting from MSB

    delay(100);

    digitalWrite(_chipSelectPin, HIGH);
    SPI.endTransaction();
}



