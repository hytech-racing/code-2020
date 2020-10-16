#ifdef HT_ARDUINO_TEENSY_32
#ifndef __HT_INTERRUPTS__
#define __HT_INTERRUPTS__

#include <map>
#include <HTException.h>

extern std::map<int, void(*)()> interruptMap;

void interrupts();
void NVIC_ENABLE_IRQ(int irq);
void attachInterruptVector(int irq, void(*interruptVector)());

#endif
#endif