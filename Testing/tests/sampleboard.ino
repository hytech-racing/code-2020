#define HYTECH_ARDUINO_TEENSY_35

#include <Arduino.h>

int inputPin = 4;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  Serial.begin(512000);
  // pinMode(inputPin, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.println(millis());
  // Serial.print(" ");
  // Serial.println(analogRead(inputPin));   // turn the LED on (HIGH is the voltage level)
}