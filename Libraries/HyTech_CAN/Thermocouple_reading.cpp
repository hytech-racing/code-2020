#include "HyTech_CAN.h"

Thermocouple_reading::Thermocouple_reading() {
  message = {};
}

Thermocouple_reading::Thermocouple_reading(uint8_t buf[]) {
  load(buf);
}

Thermocouple_reading::Thermocouple_reading(uint16_t reading_plug_1, uint16_t reading_plug_2) {
  set_reading_plug_1(reading_plug_1);
  set_reading_plug_2(reading_plug_2);
}

void Thermocouple_reading::load(uint8_t buf[]) {
  message = {};
  memcpy(&(message.reading_plug_1), &buf[0], sizeof(uint16_t));
  memcpy(&(message.reading_plug_2), &buf[2], sizeof(uint16_t));
}

void Thermocouple_reading::write(uint8_t buf[]) {
  memcpy(&buf[0], &(message.reading_plug_1), sizeof(uint16_t));
  memcpy(&buf[2], &(message.reading_plug_2), sizeof(uint16_t));
}

uint16_t Thermocouple_reading::get_reading_plug_1() {
  return message.reading_plug_1;
}

uint16_t Thermocouple_reading::get_reading_plug_2() {
  return message.reading_plug_2;
}

void Thermocouple_reading::set_reading_plug_1(uint16_t value) {
  message.reading_plug_1 = value;
}

void Thermocouple_reading::set_reading_plug_2(uint16_t value) {
  message.reading_plug_2 = value;
}
