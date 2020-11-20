#include "Arduino.h"
#include "SPI.h"
#include "AD5721.h"


AD5271::AD5721(int chipSelectPin) _chipSelectPin(chipSelectPin) { }


void AD5271::init() {
    pinMode(_chipSelectPin, OUTPUT);
}


void AD5271::writeAndUpdate(uint16_t value) {
    uint8_t message[3];
    message[0] = 3;
    value <<= 4;
    message[1] = *((uint8_t*)&value);
    message[2] = *((uint8_t*)&value + 1);

    spiWrite(message);
}


void AD5271::dataReset() {
    uint8_t message[3];
    message[0] = 7;
    message[1] = 0;
    message[2] = 0;

    spiWrite(message);
}


void AD5271::fullReset() {
    uint8_t message[3];
    message[0] = 15;
    message[1] = 0;
    message[2] = 0;

    spiWrite(message);
    spiWrite(message);
}


void AD5271::spiWrite(uint8_t* message) {
    SPI.beginTransaction(SPISettings(50000000, MSBFIRST, SPI_MODE1));
    digitalWrite(_chipSelectPin, LOW);

    delay(100);

    SPI.transfer(message, 3); //transfer 3 bytes of message starting from MSB

    delay(100);

    digitalWrite(_chipSelectPin, HIGH);
    SPI.endTransaction();
}




