// #define HT_DEBUG_EN // only uncomment this line if tcu-refactor merged in

#define HT_CAN_MODE_FLEXCAN
//#include "receiver.h"
#include "transmitter.h"

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

	GLV_current_readings glv = handle_message(ID_GLV_CURRENT_READINGS, GLV_current_readings(100, 200));
	Serial.println(glv.get_cooling_current_value());
	Serial.println(glv.get_ecu_current_value());

	MCU_pedal_readings mpr = handle_message(ID_MCU_PEDAL_READINGS, MCU_pedal_readings(1, 2, 3, 4, 5));
	Serial.println(mpr.get_accelerator_implausibility());
	Serial.println(mpr.get_accelerator_pedal_raw_1());
	Serial.println(mpr.get_accelerator_pedal_raw_2());
	Serial.println(mpr.get_brake_implausibility());
	Serial.println(mpr.get_brake_pedal_active());
	Serial.println(mpr.get_brake_pedal_raw());
	Serial.println(mpr.get_pedal_flags());
	Serial.println(mpr.get_torque_map_mode());
}
