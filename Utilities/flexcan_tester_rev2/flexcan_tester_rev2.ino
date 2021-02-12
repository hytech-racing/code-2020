#define RECEIVE 'R'
#define SEND 'S'

#define MODE SEND
// #define HT_DEBUG_EN // only uncomment this line if tcu-refactor merged in

#include "flexcan_tester_rev2.h"

FlexCAN CAN(500000);

void setup() {
    CAN.begin();
  	Serial.begin(9600);
}

void loop() {
	synchronize();

	handle_message(ID_GLV_CURRENT_READINGS, GLV_current_readings(100, 200));
}
