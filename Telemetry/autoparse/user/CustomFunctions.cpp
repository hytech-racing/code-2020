#include "UserDefined.h"

CUSTOMFUNC(parse_bms_balancing_status, BMS_balancing_status& data) {
	int group = data.get_group_id() << 2;
	for (int ic = 0; ic < 4; ++ic) {
		for (int cell = 0; cell < 9; ++cell)
			show("BAL_IC_%d_CELL_%d,%d\n", group + ic, cell, data.get_cell_balancing(ic, cell));
	}
}

CUSTOMFUNC(parse_detailed_voltages, BMS_detailed_voltages& data) {
	int group = data.get_group_id() * 3;
	for (int i = 0; i < 3; ++i)
		show("CELL_%d,%.4f,V\n", group + i, data.get_voltage(i) / (double) 10000);
}

CUSTOMFUNC(print_shutdown_status, MCU_status& data) {
    show("bms_fault,%d\n", !data.get_bms_ok_high());
    show("imd_fault,%d\n", !data.get_imd_ok_high());
    show("inverter_power,%d\n", data.get_inverter_powered());

	char shutdown [8];
	int idx = 0;
	if (data.get_shutdown_b_above_threshold()) shutdown[idx++] = 'B';
	if (data.get_shutdown_c_above_threshold()) shutdown[idx++] = 'C';
	if (data.get_shutdown_d_above_threshold()) shutdown[idx++] = 'D';
	if (data.get_shutdown_e_above_threshold()) shutdown[idx++] = 'E';
	shutdown[idx++] = '\0';
	show("shutdown_above_thresh,%s\n", shutdown);
}

CUSTOMFUNC(parse_mcu_enums, MCU_status& data, MCU_status& prev){
	if(data.get_torque_mode() != prev.get_torque_mode()){
		switch(data.get_torque_mode()){
			case TORQUE_MODE::MAX_0:   show("torque_mode,0\n"); break;
			case TORQUE_MODE::MAX_60:  show("torque_mode,60\n"); break;
			case TORQUE_MODE::MAX_100: show("torque_mode,100\n"); break;
			case TORQUE_MODE::MAX_120: show("torque_mode,120\n"); break;
		}
	}
	if(data.get_state() != prev.get_state()){
		switch(data.get_state()){
			case MCU_STATE::STARTUP: 
				show("mcu_state,startup\n"); break;
			case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE: 
				show("mcu_state,tractive_system_not_active\n"); break;
			case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE: 
				show("mcu_state,tracive_system_active\n"); break;
			case MCU_STATE::ENABLING_INVERTER: 
				show("mcu_state,enabling_inverter\n"); break;
			case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND: 
				show("mcu_state,waiting_ready_to_drive_sound\n"); break;
			case MCU_STATE::READY_TO_DRIVE: 
				show("mcu_state,ready_to_drive\n"); break;
		}
	}
}