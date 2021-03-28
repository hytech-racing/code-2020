#include "UserDefined.h"

CUSTOMFUNC(parse_bms_balancing_status, BMS_balancing_status& data, BMS_balancing_status& prev) {
	int group = data.get_group_id() << 2;
	for (int ic = 0; ic < 4; ++ic) {
		for (int cell = 0; cell < 9; ++cell) {
			bool bal = data.get_cell_balancing(ic, cell);
			if (bal != prev.get_cell_balancing(ic, cell))
				show("BAL_IC_%d_CELL_%d,%d\n", group + ic, cell, bal);
		}
	}
}

CUSTOMFUNC(parse_detailed_voltages, BMS_detailed_voltages& data, BMS_detailed_voltages& prev) {
	int group = data.get_group_id() * 3;
	for (int i = 0; i < 3; ++i) {
		if (data.get_voltage(i) != prev.get_voltage(i))
			show("CELL_%d,%.4f,V\n", group + i, data.get_voltage(i) / (double) 10000);
	}
}

CUSTOMFUNC(parse_mcu_enums, MCU_status& data, MCU_status& prev){
	if(data.get_state() != prev.get_state()){
		switch(data.get_state()){
			case MCU_STATE::STARTUP: 
				show("mcu_state,%s\n", "startup"); break;
			case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE: 
				show("mcu_state,%s\n", "tractive_system_not_active"); break;
			case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE: 
				show("mcu_state,%s\n", "tracive_system_active"); break;
			case MCU_STATE::ENABLING_INVERTER: 
				show("mcu_state,%s\n", "enabling_inverter"); break;
			case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND: 
				show("mcu_state,%s\n", "waiting_ready_to_drive_sound"); break;
			case MCU_STATE::READY_TO_DRIVE: 
				show("mcu_state,%s\n", "ready_to_drive"); break;
		}
	}
}

#define EXTRACT_HELPER(prefix, led, postfix) prefix##led##postfix
#define EXTRACT(var, led) var.EXTRACT_HELPER(get_, led, _led)()

#define PARSE_LED(led, data, prev) \
	if (EXTRACT(data,led) != EXTRACT(prev, led)){ \
		switch (static_cast<BLINK_MODES>(EXTRACT(data, led))){ \
			case BLINK_MODES::OFF: \
				show(stringify(led) "_led,%s\n", "off"); break; \
			case BLINK_MODES::ON: \
				show(stringify(led) "_led,%s\n", "on"); break; \
			case BLINK_MODES::FAST: \
				show(stringify(led) "_led,%s\n", "fast"); break; \
			case BLINK_MODES::SLOW: \
				show(stringify(led) "_led,%s\n", "slow"); break; \
			case BLINK_MODES::FASTER: \
				show(stringify(led) "_led,%s\n", "faster"); break; \
		} \
	}

CUSTOMFUNC(parse_dashboard_leds, Dashboard_status& data, Dashboard_status& prev){
	/* see VariableLed.h for enum definitions */
	enum class BLINK_MODES { OFF = 0, ON = 1, FAST = 2, SLOW = 3 , FASTER = 4};
	PARSE_LED(ams, data, prev);
	PARSE_LED(imd, data, prev);
	PARSE_LED(mode, data, prev);
	PARSE_LED(mc_error, data, prev);
	PARSE_LED(start, data, prev);
	PARSE_LED(launch_ctrl, data, prev);

}

#undef PARSE_LED
#undef EXTRACT
#undef EXTRACT_HELPER