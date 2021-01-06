#ifndef _ADC128S022_H
#define _ADC128S022_H


class ADC128S022 {
private:
	int _chipSelectPin;
public:
	ADC128S022(int csPin);
	void init();
	uint8_t readChannel(uint8_t channel);
};


#endif