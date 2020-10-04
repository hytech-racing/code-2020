#pragma once
#include <thread>
#include <Serial.h>
#include <Simulator.h>

#define LOW 0
#define HIGH 1

typedef char byte;

// defined in microcontroller code
extern void setup();
extern void loop();

// used to manage pin i/o
extern MockPin io [NUM_PINS];

bool digitalRead(int pin);
void digitalWrite(int pin, bool value);
unsigned analogRead(int pin);
void analogWrite(int pin, unsigned value);
void pinMode(int pin, unsigned mode);

// used for time management
unsigned long long millis();
void delay(unsigned long long time);