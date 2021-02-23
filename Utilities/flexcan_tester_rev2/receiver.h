#include "flexcan_tester_rev2.h"

void synchronize() {
	CAN_message_t message;
	while (!(CAN.read(message) && message.id == SYNC)); // wait for sync message
	CAN.write(message); // send response
	while (!(CAN.read(message) && message.id == ACK)); // clear extra syncs from mailbox
}

template <typename T>
inline T handle_message(unsigned id, T data) {
	CAN_message_t message;
	while (!CAN.read(message));

	data.load(message.buf);
	print(message, id, data);
	return data;
}
