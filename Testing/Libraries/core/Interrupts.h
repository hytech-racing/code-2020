#pragma once

#if __HT_ARDUINO__ == TEENSY_32

#include <map>
#include "Exception.h"

std::map<int, void(*)()> interruptMap;

void interrupts();
void NVIC_ENABLE_IRQ(int irq);
void attachInterruptVector(int irq, void(*interruptVector)());

#endif