#include "Arduino.h"
#include <MockPin.h>
#include <Simulator.h>

// time management constants
unsigned long long millis() { return Simulator::sys_time; }
void delay (unsigned long long time) { Simulator::sys_time += time; }

// pin i/o
bool digitalRead(int pin) { return io[pin].value(); };
void digitalWrite(int pin, bool value) { io[pin].write(value); }
unsigned analogRead(int pin) { return io[pin].value(); }
void analogWrite(int pin, unsigned value) { io[pin].write(value); }
void pinMode(int pin, unsigned mode) { io[pin].mode(mode); }