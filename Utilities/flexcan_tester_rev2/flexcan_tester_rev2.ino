#define RECEIVE 'R'
#define SEND 'S'

#define MODE RECEIVE
// #define HT_DEBUG_EN // only uncomment this line if tcu-refactor merged in

#define HT_CAN_MODE_FLEXCAN
#include "flexcan_tester_rev2.h"

#ifdef HT_CAN_MODE_FLEXCAN
FlexCAN CAN(500000);
void setupCAN() {
	CAN.begin();
}

#elif defined(HT_CAN_MODE_FLEXCAN_T4)
FlexCAN_T4<CAN1> CAN;
void setupCAN() {
    CAN.begin();
    CAN.setBaudRate(500000);
}

#elif defined(HT_CAN_MODE_MCP)
MCP_CAN CAN(11);
void setupCAN() {
	while (CAN_OK != CAN.begin(CAN_500KBPS))
		delay(200);
}
#endif

void setup() {
	setupCAN();
  	Serial.begin(9600);
}

void loop() {
	synchronize();

	handle_message(ID_GLV_CURRENT_READINGS, GLV_current_readings(100, 200));
}
