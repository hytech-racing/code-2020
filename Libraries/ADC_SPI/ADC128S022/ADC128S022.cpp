#include "ADC128S022.h"



ADC128S022::ADC128S022(int csPin) : _chipSelectPin(csPin) { }

void ADC128S022::init() {
	pinMode(_chipSelectPin, OUTPUT);
	digitalWrite(_chipSelectPin, HIGH);
}

uint8_t ADC128S022::readChannel(uint8_t channel) {
	uint8_t bitshifted_channel = channel << 3;
	SPI.beginTransaction(SPISettings(24000000, MSBFIRST, SPI_MODE1));

	digitalWrite(_chipSelectPin, LOW);
	delay(100);
	uint8_t output = SPI.transfer(bitshifted_channel);
	delay(100);
	digitalWrite(_chipSelectPin, HIGH);

	return output;
}