#include "ADC_SPI.h"
#include "LT_SPI.h"
#include <SPI.h>
#include <CAN_sim.h>
#include <Arduino.h>


ADC_SPI::ADC_SPI() {
	byte channels[] = {0, 0, 0, 0, 0, 0, 0, 0};
    init(DEFAULT_SPI_CS, DEFAULT_SPI_SPEED, channels);
}

ADC_SPI::ADC_SPI(int CS, byte channels[]) {
    init(CS, DEFAULT_SPI_SPEED, channels);
}

ADC_SPI::ADC_SPI(int CS, unsigned int SPIspeed, byte channels[]) {
    init(CS, SPI_SPEED, channels);
}

void ADC_SPI::init(int CS, unsigned int SPIspeed, byte adc_channels[]) {
    ADC_SPI_CS = CS;
	SPI_SPEED  = SPIspeed;
	io[CS].sim_pinMode(RESERVED);
	pinMode(ADC_SPI_CS, OUTPUT);
	pinMode(ADC_SPI_CS, HIGH);

	for (int i = 0; i < 8; i++) {
		channels[i] = adc_channels[i];
	}
}

byte ADC_SPI::read_adc(int channel) {
	return channels[channel];
}

void ADC_SPI::write_adc(int channel, byte value) {
	channels[channel] = value;
}


/* a byte transfer in (0,0) mode 
byte ADC_SPI::SPI_transfer(byte received) {
	int counter;
	for(counter = 8; counter; counter--) {
		if (received & 0x80)
			MOSI = 1;
		else
			MOSI = 0;
		received <<= 1;
		SCK = 1;  a slave latches input data bit 
		if (MISO)
			received |= 0x01;
		SCK = 0;  a slave shifts out next output data bit 
	}
	return(received);
}
*/
