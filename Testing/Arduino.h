#pragma once
#include <map>
#include <thread>
#include "Arduino.h"
#include "Configure.h"

// defined in microcontroller code
extern void setup();
extern void loop();

// time management
unsigned long long millis();

// used to manage pin i/o
extern MockPin io [NUM_PINS];

bool digitalRead(unsigned pin);
void digitalWrite(unsigned pin, bool value);
unsigned analogRead(unsigned pin);
void analogWrite(unsigned pin, unsigned value);
unsigned pinMode(int pin, unsigned value);