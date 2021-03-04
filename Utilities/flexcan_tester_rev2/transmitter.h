#include "flexcan_tester_rev2.h"
#include <Metro.h>

Metro timer(3000);

void synchronize() {
	CAN_message_t sent_message;
	sent_message.id = SYN;
    CAN_message_t response_message;
	do {
		CAN.write(sent_message); // send sync message
		delay(2000);
	} while (!(CAN.read(response_message) && response_message.id == ACK));
	sent_message.id = SYNACK;
	CAN.write(sent_message);
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
