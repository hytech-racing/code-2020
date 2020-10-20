#include "HyTech_FlexCAN.h"
#include <Interrupts.h>

extern bool interruptsEnabled;

FlexCAN::FlexCAN(uint32_t baud, uint8_t id, uint8_t txAlt, uint8_t rxAlt) {
	defaultMask = { 0, 0, 0 };
	for (int i = 0; i < 8; ++i) allMasks[i] = defaultMask;

	if (baud != 500000)
		throw CustomException("CAN bus baud rate must be 500000");

	#ifdef HYTECH_ARDUINO_TEENSY_32
		txPin = 3; rxPin = 4;
	#endif

	#ifdef HYTECH_ARDUINO_TEENSY_35
		txPin = 3; rxPin = 4;
	#endif
}

void FlexCAN::begin(const CAN_filter_t &mask) {
	io[txPin].sim_pinMode(RESERVED);
	io[rxPin].sim_pinMode(RESERVED);

	#ifdef HYTECH_ARDUINO_TEENSY_32
		if (interruptsEnabled && FLEXCAN0_IMASK1 != FLEXCAN_IMASK1_BUF5M && interruptsEnabled)
			throw CustomException("If interrupts enabled, Teensy 3.2 requires FLEXCAN0_IMASK1 = FLEXCAN_IMASK1_BUF5M");
	#endif

	#ifdef HYTECH_ARDUINO_TEENSY_35
	    if (FLEXCAN0_MCR != 0xFFFDFFFF)
			throw CustomException("Teensy 3.5 expects CAN self-reception enabled (FLEXCAN0_MCR = 0xFFFDFFFF)");
	#endif

	valid = true;
}

void FlexCAN::setFilter(const CAN_filter_t &filter, uint8_t n) {
	if (n < 8)
		allMasks[n] = filter;
	valid = true;
	for (int i = 0; valid && i < 8; ++i)
		if (allMasks[i].rtr != filter.rtr || allMasks[i].ext != filter.ext || allMasks[i].id != filter.id)
			valid = false;
}
void FlexCAN::end(void) { io[txPin].sim_pinMode(-1); io[rxPin].sim_pinMode(-1); }

int FlexCAN::available(void) { return true; }

int FlexCAN::write(const CAN_message_t &msg) { 
	if (!valid) 
		throw CustomException("CAN configuration not valid");
	CAN_simulator::outbox.push(msg); 
	return true;
}

int FlexCAN::read(CAN_message_t &msg) {
	if (!valid) throw CustomException("CAN configuration not valid");
	do {
		printf("MESSAGES: %d\n", CAN_simulator::inbox.size());
		if (CAN_simulator::inbox.empty())
			return false;
		msg = CAN_simulator::inbox.front(); 
		CAN_simulator::inbox.pop(); 
	} while (msg.rtr == allMasks[0].rtr && msg.id == allMasks[0].id && msg.ext == allMasks[0].ext);
	return true;
}

namespace CAN_simulator {
	std::queue<CAN_message_t> inbox;
	std::queue<CAN_message_t> outbox;

	int push(const CAN_message_t& msg) { inbox.push(msg); return true; }

	int get(CAN_message_t &msg) { 
		if (outbox.empty())
			return false;
		msg = outbox.front(); outbox.pop();
		return true;
	}
}

#ifdef HYTECH_ARDUINO_TEENSY_32
	unsigned long long FLEXCAN0_IMASK1 = 0;
#endif
#ifdef HYTECH_ARDUINO_TEENSY_35
	unsigned long long FLEXCAN0_MCR = 0xFFFFFFFF;
#endif