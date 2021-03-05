#include "flexcan_tester_rev2.h"
#include <Metro.h>

Metro timer(1000);

void synchronize() {
	CAN_message_t message;
	message.id = SYNC;
  message.len = 0;
  message.timeout = 0;
  Serial.println("before sending the SYNC\n");
	do {
    Serial.println("sending the SYNC\n");
		CAN.write(message); // send sync message
		delay(500);
	} while (!(CAN.read(message) && message.id == ACK));
  Serial.println("received the ACK\n");
	message.id = BEGIN;
  message.len = 0;
	CAN.write(message);
  Serial.println("sent BEGIN\n");
	timer.reset(); // reset send timer
}



template<typename T>
inline T handle_message(unsigned id, T data) {
	CAN_message_t message;

	while (!timer.check());
	message.id = id;
	message.len = sizeof(T);
	data.write(message.buf);

	print(message, id, data);
	CAN.write(message);
	return data;
}
