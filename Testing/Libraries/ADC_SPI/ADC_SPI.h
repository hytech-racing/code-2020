#pragma once
#include <Simulator.h>
#include "LT_SPI.h"
#ifndef __ADC_SPI_H__
#define __ADC_SPI_H__

#include <stdint.h>
#include <stddef.h>

#define DEFAULT_SPI_CS 10
#define DEFAULT_SPI_SPEED 1000000

class ADC_SPI {
	public:
		ADC_SPI();
		ADC_SPI(int CS, byte channels[]);
        ADC_SPI(int CS, unsigned int SPIspeed, byte channels[]);
		void init(int CS, unsigned int SPIspeed, byte adc_channels[]);
		byte read_adc(int channel);
        void write_adc(int channel, byte value);
        byte SPI_transfer(byte byte);
	private:
        byte MOSI = 0;
        byte MISO = 0;
        byte SCK = 0;
		int ADC_SPI_CS;
        unsigned int SPI_SPEED;
        byte channels[8];
};


#endif // !<ADC_SPI.H>
#pragma once