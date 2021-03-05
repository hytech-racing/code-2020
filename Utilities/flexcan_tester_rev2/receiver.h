#include "flexcan_tester_rev2.h"

void synchronize() {
	CAN_message_t message;
  Serial.println("before the SYNC\n");
  message.timeout = 0;
	while (!(CAN.read(message) && message.id == SYNC)) { // wait for sync message
    Serial.println("waiting on SYNC");
    delay(500);
	}
  Serial.println("got the SYNC, send the ACK\n");
  message.id = ACK;
  message.len = 0;
	CAN.write(message); // send response
	while (!(CAN.read(message) && message.id == BEGIN)); // clear extra syncs from mailbox
  Serial.println("got BEGIN\n");
}

template <typename T>
inline T handle_message(unsigned id, T data) {
	CAN_message_t message;
	while (!CAN.read(message));

	data.load(message.buf);
	print(message, id, data);
	return data;
}
