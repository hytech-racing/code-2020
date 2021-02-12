#if defined(MODE) && MODE == SEND

#include "flexcan_tester_rev2.h"
#include <Metro.h>

Metro timer(3000);

void synchronize() {
	CAN_message_t message;
	message.id = SYNC;

	do {
		CAN.write(message); // send sync message
		delay(2);
	} while (!(CAN.read(message) && message.id == SYNC));
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

#endif
