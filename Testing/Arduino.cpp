#include <map>
#include "Arduino.h"
#include "MockPin.h"

// time management constants
unsigned long long sys_time = 0;
unsigned long long millis() { return sys_time; }

// pin i/o
bool digitalRead(int pin) { return io[pin].value(); };
void digitalWrite(int pin, bool value) { io[pin].write(value); }
unsigned analogRead(int pin) { return io[pin].value(); }
void analogWrite(int pin, unsigned value) { io[pin].write(value); }
unsigned pinMode(int pin, bool mode) { io[pin].mode(mode); }