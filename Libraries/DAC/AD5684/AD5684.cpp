#include "Arduino.h"
#include "SPI.h"
#include "AD5684.h"


AD5684::AD5684(int chipSelectPin) _chipSelectPin(chipSelectPin) { }


void AD5684::init() {
    pinMode(_chipSelectPin, OUTPUT);
}


void AD5684::writeAndUpdate(uint8_t address, uint8_t value) {
    uint8_t message[3];
    message[0] = (3 << 4) + address;

    value <<= 4;
    message[1] = *((uint8_t*)&value);
    message[2] = *((uint8_t*)&value + 1);

    spiWrite(message);
}

void AD5684::power() {
    uint8_t message[3];
    message[0] = 4 << 4;
    message[1] = 0;
    message[2] = 0;

    spiWrite(message);
}


void AD5684::reset() {
    uint8_t message[3];
    message[0] = 6 << 4;
    message[1] = 0;
    message[2] = 0;

    spiWrite(message);
}


void AD5684::spiWrite(uint8_t* message) {
    SPI.beginTransaction(SPISettings(50000000, MSBFIRST, SPI_MODE1));
    digitalWrite(_chipSelectPin, LOW);

    delay(100);

    SPI.transfer(message, 3); //transfer 3 bytes of message starting from MSB

    delay(100);

    digitalWrite(_chipSelectPin, HIGH);
    SPI.endTransaction();
}




