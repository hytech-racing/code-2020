#pragma once
#include <string>
#include "MockPin.h"

#define NUM_PINS 21
#define LOOP_PERIOD 100

MockPin io [NUM_PINS];

unsigned long long loadConfiguration(std::string filepath);