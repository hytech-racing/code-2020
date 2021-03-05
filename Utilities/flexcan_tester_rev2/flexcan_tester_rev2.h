#pragma once

#include <HyTech_CAN.h>

// I'm lazy so this is how we're handling CAN library switching
#ifdef HT_CAN_MODE_FLEXCAN
#include <HyTech_FlexCAN.h>
extern FlexCAN CAN;

#elif defined(HT_CAN_MODE_FLEXCAN_T4)
#include <FlexCAN_T4.h>
extern FlexCAN_T4<CAN1> CAN;

#elif defined(HT_CAN_MODE_MCP)
#include <mcp_can.h>
extern MCP_CAN CAN;
#endif

#define SYNC	0x90
#define ACK		0x91
#define BEGIN 0x92

template<typename T>
inline void print(CAN_message_t& message, unsigned id, T& data) {
	Serial.println	("CAN Message");
	Serial.println	("-----------");
	Serial.print	("ID  = "); Serial.println	(message.id, HEX);
	Serial.print	("LEN = "); Serial.println	(message.len);

	if (id != message.id) {
		Serial.println("Warning: ID Mismatch -- Expected ");
		Serial.println(id, HEX);
	}
	if (sizeof(T) != message.len) {
		Serial.println("Warning: Length Mismatch -- Expected ");
		Serial.println(sizeof(T));
	}
	// this may not exist yet if you haven't merged in the tcu-refactor branch as an alternative, add if/elses here or print in the loop() function
	#ifdef HT_DEBUG_EN
	data.print();
	#endif
}
