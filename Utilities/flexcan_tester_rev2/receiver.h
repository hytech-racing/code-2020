#include "flexcan_tester_rev2.h"

void synchronize() {
	CAN_message_t response_message;
	CAN_message_t sent_message;
    sent_message.id = ACK;
	while (!(CAN.read(response_message) && response_message.id == SYN)); // wait for sync message
    do {
        CAN.write(sent_message); // send sync message
        delay(2000);
    } while (!(CAN.read(response_message) && response_message.id == SYNACK));
}

template <typename T>
inline T handle_message(unsigned id, T data) {
	CAN_message_t message;
	while (!(CAN.read(message)));

	data.load(message.buf);
	print(message, id, data);
	return data;
}
