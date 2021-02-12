#pragma once

#include <HyTech_CAN.h>
#include <HyTech_FlexCAN.h>

#define SYNC 0x90
#define ACK 0x91

extern FlexCAN CAN;

template<typename T>
inline void print(CAN_message_t& message, unsigned id, T& data) {
	Serial.println	("CAN Message");
	Serial.println	("-----------");
	Serial.print	("ID  = "); Serial.println	(message.id, HEX);
	Serial.print	("LEN = "); Serial.println	(message.len);

	if (id != message.id)
		Serial.println("Warning: ID Mismatch -- Expected "); Serial.println(id, HEX);
	if (sizeof(T) != message.len)
		Serial.println("Warning: Length Mismatch -- Expected "); Serial.println(sizeof(T));

	// this may not exist yet if you haven't merged in the tcu-refactor branch as an alternative, add if/elses here or print in the loop() function
	#ifdef HT_DEBUG_EN
	data.print();
	#endif
}

template<typename T> inline T handle_message(unsigned id, T data);
void synchronize();