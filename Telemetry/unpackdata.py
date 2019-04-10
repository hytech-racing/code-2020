dbfile = open('can_messages.db')
data = f.read(31)

def unpack(data):
    (timestamp, msg_id) = struct.unpack('=QI', data)

    if msg_id == 0xD5:
        (avg_temp, lo_temp, hi_temp) = struct.unpack_from('=hhh', data, 13)
        return {
            'timestamp': timestamp,
            'msgID': msg_id,
            'avgTemp': avg_temp,
            'loTemp': lo_temp,
            'hiTemp': hi_temp
        }

    if msg_id == 0xD6:
        (id_ic, temp0, temp1) = struct.unpack_from('=hhh', data, 13)

    if msg_id == 0xD7:
        (avg_volt, lo_volt, hi_volt, tot_volt) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xD8:
        (ic_id, volt_0, volt_1, volt_2) = struct.unpack_from('=bhhh', data, 13)

    if msg_id == 0xD9:
        (avg_temp, lo_temp, hi_temp) = struct.unpack_from('=hhh', data, 13)

    if msg_id == 0xDA:
        (id_ic, temp0, temp1) = struct.unpack_from('=bhh', data, 13)

    if msg_id == 0xDB:
        (state, error_flags, cur) = struct.unpack_from('=bhh', data, 13)

    if msg_id == 0xDD:
        (charge_enabled) = struct.unpack_from('=?', data, 13)

    if msg_id == 0xA0:
        (mod_a, mod_b, mod_c,gate_driver) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xA1:
        (control_board, rtd_1, rtd2, rtd3) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xA2:
        (rtd_5, rtd_6, motor_temp, torque_shudder) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xA3:
        (analog_1, analog_2, analog_3, analog_4) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xA4:
        (dig_1, dig_2, dig_3, dig_4, dig_5, dig_6, dig_7, dig_8) = struct.unpack_from('=????????', data, 13)

    if msg_id == 0xA5:
        (motor_angle, motor_speed, elec_freq, delta_filtered) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xA6:
        (phase_a, phase_b, phase_c, dc_cur) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xA7:
        (dc_volt, out_volt, phase_ab, phase_bc) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xAA:
        (vsm, inverter, relay, inverter_run, inverter_command, inverter_enable, direct_command) = struct.unpack_from('=hbbbbbb', data, 13)

    if msg_id == 0xAB:
        (post_lo, post_hi, run_lo, run_hi) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xAC:
        (command_torque, torque_feedback, power_timer) = struct.unpack_from('=hhi', data, 13)

    if msg_id == 0xAD:
        (mod_index, flux_out, id_command, iq_command) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xAE:
        (eprom_code, software_ver, date_mmdd, date_yyyy) = struct.unpack_from('=hhhh', data, 13)

    if msg_id == 0xAF:
        (id_ic, temp0, temp1) = struct.unpack_from('=hhh', data, 13)

    if msg_id == 0xC0:
        (torq_command, ang_vel, direct, invert_enable, command_limit) = struct.unpack_from('=hh?bh', data, 13)

    if msg_id == 0xC1:
        (parameter_add, rw_command, reserved1, data) = struct.unpack_from('=h?bi', data, 13)

    if msg_id == 0xC2:
        (parameter_add, write_suc, reserved1, data) = struct.unpack_from('=h?bi', data, 13)


#define ID_BMS_ONBOARD_TEMPERATURES 0xD5
#define ID_BMS_ONBOARD_DETAILED_TEMPERATURES 0xD6
#define ID_BMS_VOLTAGES 0xD7
#define ID_BMS_DETAILED_VOLTAGES 0xD8
#define ID_BMS_TEMPERATURES 0xD9
#define ID_BMS_DETAILED_TEMPERATURES 0xDA
#define ID_BMS_STATUS 0xDB
#define ID_BMS_BALANCING_STATUS 0xDE
#define ID_FH_WATCHDOG_TEST 0xDC
#define ID_CCU_STATUS 0xDD
#define ID_MC_TEMPERATURES_1 0xA0
#define ID_MC_TEMPERATURES_2 0xA1
#define ID_MC_TEMPERATURES_3 0xA2
#define ID_MC_ANALOG_INPUTS_VOLTAGES 0xA3
#define ID_MC_DIGITAL_INPUT_STATUS 0xA4
#define ID_MC_MOTOR_POSITION_INFORMATION 0xA5
#define ID_MC_CURRENT_INFORMATION 0xA6
#define ID_MC_VOLTAGE_INFORMATION 0xA7
#define ID_MC_FLUX_INFORMATION 0xA8
#define ID_MC_INTERNAL_VOLTAGES 0xA9
#define ID_MC_INTERNAL_STATES 0xAA
#define ID_MC_FAULT_CODES 0xAB
#define ID_MC_TORQUE_TIMER_INFORMATION 0xAC
#define ID_MC_MODULATION_INDEX_FLUX_WEAKENING_OUTPUT_INFORMATION 0xAD
#define ID_MC_FIRMWARE_INFORMATION 0xAE
#define ID_MC_DIAGNOSTIC_DATA 0xAF
#define ID_MC_COMMAND_MESSAGE 0xC0
#define ID_MC_READ_WRITE_PARAMETER_COMMAND 0xC1
#define ID_MC_READ_WRITE_PARAMETER_RESPONSE 0xC2
