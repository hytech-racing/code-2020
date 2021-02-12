#if defined(MODE) && MODE == RECEIVE

#include "flexcan_tester_rev2.h"

void synchronize() {
	CAN_message_t message;
	while (!(CAN.read(message) && message.id == SYNC)); // wait for sync message
	while (CAN.read(message)); // clear mailboxes
	CAN.write(message); // send response
}

template <typename T>
inline T handle_message(unsigned id, T data) {
	CAN_message_t message;
	while (!CAN.read(message));

	data.load(message.buf);
	print(message, id, data);
	return data;
}

#endif