#pragma once

#include <Arduino.h>
#include <queue>

typedef struct CAN_message_t {
	uint32_t id; // can identifier
	uint8_t ext; // identifier is extended
	uint8_t rtr; // remote transmission request packet type
	uint8_t len; // length of data
	uint16_t timeout; // milliseconds, zero will disable waiting
	uint8_t buf[8];
} CAN_message_t;

typedef struct CAN_filter_t {
	uint8_t rtr;
	uint8_t ext;
	uint32_t id;
} CAN_filter_t;

class FlexCAN {
private:
	struct CAN_filter_t defaultMask;
	uint32_t flexcanBase;

	struct CAN_filter_t allMasks [8];
	int rxPin, txPin;
	bool valid = false;

public: 
	FlexCAN(uint32_t baud = 125000, uint8_t id = 0, uint8_t txAlt = 0, uint8_t rxAlt = 0);
	void begin(const CAN_filter_t &mask);
	inline void begin()
	{
		begin(defaultMask);
	}
	void setFilter(const CAN_filter_t &filter, uint8_t n);
	void end(void);
	int available(void);
	int write(const CAN_message_t &msg);
	int read(CAN_message_t &msg);
};

namespace CAN_simulator {
	int push(const CAN_message_t& msg);
	int get(CAN_message_t &msg);
	extern std::queue<CAN_message_t> inbox;
	extern std::queue<CAN_message_t> outbox;
};

#ifdef HYTECH_ARDUINO_TEENSY_32
	#define IRQ_CAN_MESSAGE 29
	extern unsigned long long FLEXCAN0_IMASK1;
	#define FLEXCAN_IMASK1_BUF5M (0x00000020)
#endif
#ifdef HYTECH_ARDUINO_TEENSY_35
  	extern unsigned long long FLEXCAN0_MCR;
#endif