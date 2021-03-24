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

CUSTOMFUNC(parse_dashboard_leds, Dashboard_status& data, Dashboard_status& prev){
	// see VariableLed.h for enum definitions
	enum class BLINK_MODES { OFF = 0, ON = 1, FAST = 2, SLOW = 3 };
	if (data.get_ams_led() != prev.get_ams_led()){
		switch (static_cast<BLINK_MODES>(data.get_ams_led())){
			case BLINK_MODES::OFF:
				show("ams_led,%s\n", "off"); break;
			case BLINK_MODES::ON:
				show("ams_led,%s\n", "on"); break;
			case BLINK_MODES::FAST:
				show("ams_led,%s\n", "fast"); break;
			case BLINK_MODES::SLOW:
				show("ams_led,%s\n", "slow"); break;
		}
	}
	if (data.get_imd_led() != prev.get_imd_led()){
		switch (static_cast<BLINK_MODES>(data.get_imd_led())){
			case BLINK_MODES::OFF:
				show("imd_led,%s\n", "off"); break;
			case BLINK_MODES::ON:
				show("imd_led,%s\n", "on"); break;
			case BLINK_MODES::FAST:
				show("imd_led,%s\n", "fast"); break;
			case BLINK_MODES::SLOW:
				show("imd_led,%s\n", "slow"); break;
		}
	}
	if (data.get_mode_led() != prev.get_mode_led()){
		switch (static_cast<BLINK_MODES>(data.get_mode_led())){
			case BLINK_MODES::OFF:
				show("mode_led,%s\n", "off"); break;
			case BLINK_MODES::ON:
				show("mode_led,%s\n", "on"); break;
			case BLINK_MODES::FAST:
				show("mode_led,%s\n", "fast"); break;
			case BLINK_MODES::SLOW:
				show("mode_led,%s\n", "slow"); break;
		}
	}
	if (data.get_mc_error_led() != prev.get_mc_error_led()){
		switch (static_cast<BLINK_MODES>(data.get_mc_error_led())){
			case BLINK_MODES::OFF:
				show("mc_error_led,%s\n", "off"); break;
			case BLINK_MODES::ON:
				show("mc_error_led,%s\n", "on"); break;
			case BLINK_MODES::FAST:
				show("mc_error_led,%s\n", "fast"); break;
			case BLINK_MODES::SLOW:
				show("mc_error_led,%s\n", "slow"); break;
		}
	}
	if (data.get_start_led() != prev.get_start_led()){
		switch (static_cast<BLINK_MODES>(data.get_start_led())){
			case BLINK_MODES::OFF:
				show("start_led,%s\n", "off"); break;
			case BLINK_MODES::ON:
				show("start_led,%s\n", "on"); break;
			case BLINK_MODES::FAST:
				show("start_led,%s\n", "fast"); break;
			case BLINK_MODES::SLOW:
				show("start_led,%s\n", "slow"); break;
		}
	}
	if (data.get_launch_ctrl_led() != prev.get_launch_ctrl_led()){
		switch (static_cast<BLINK_MODES>(data.get_launch_ctrl_led())){
			case BLINK_MODES::OFF:
				show("launch_ctrl_led,%s\n", "off"); break;
			case BLINK_MODES::ON:
				show("launch_ctrl_led,%s\n", "on"); break;
			case BLINK_MODES::FAST:
				show("launch_ctrl_led,%s\n", "fast"); break;
			case BLINK_MODES::SLOW:
				show("launch_ctrl_led,%s\n", "slow"); break;
		}
	}
}