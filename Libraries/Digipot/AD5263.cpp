#include "Arduino.h"
#include "SPI.h"
#include "AD5263.h"


AD5263::AD5263(int chipSelectPin) _chipSelectPin(chipSelectPin) { }


void AD5263::init() {
    pinMode(_chipSelectPin, OUTPUT);
}


void AD5263::writeAndUpdate(uint8_t address, uint8_t value) {
    uint8_t message[2];

    message[0] = (address << 6) + (value >> 2);
    message[1] = value << 6;

    spiWrite(message);
}


void AD5263::spiWrite(uint8_t* message) {
    SPI.beginTransaction(SPISettings(50000000, MSBFIRST, SPI_MODE1));
    digitalWrite(_chipSelectPin, LOW);

    delay(100);

    SPI.transfer(message, 2); //transfer 2 bytes of message starting from MSB

    delay(100);

    digitalWrite(_chipSelectPin, HIGH);
    SPI.endTransaction();
}




