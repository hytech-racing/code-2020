#ifndef __HYTECH_ARDUINO__
#define __HYTECH_ARDUINO__

#define ARDUINO 0

#include <Serial.h>
#include <Simulator.h>
#include <Interrupts.h>

#define LOW 0
#define HIGH 1

typedef char byte;

// defined in microcontroller code
extern void setup();
extern void loop();

// used to manage pin i/o
extern MockPin* io;

bool digitalRead(int pin);
void digitalWrite(int pin, bool value);
unsigned analogRead(int pin);
void analogWrite(int pin, unsigned value);
void pinMode(int pin, unsigned mode);

// used for time management
unsigned long long millis();
void delay (unsigned long long time);

#endif