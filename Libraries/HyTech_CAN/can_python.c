#define PY_SSIZE_T_CLEAN
#include <stdlib.h>
#include <Python.h>
#include "HyTech_CAN.h"
#include "../XBTools/XBTools.h"

static PyObject *can_unpack(PyObject *self, PyObject *args)
{
    char *payload_str;
    const char *msg_data;
    Telem_message_t payload;
    if (!PyArg_ParseTuple(args, "y", &payload_str))
        return NULL;
    for (char *c = payload_str; *c; c++) {
        if (*c == ',') {
            *c = 0;
            msg_data = c + 1;
            goto comma_found;
        }
    }

    // TODO add error: message malformed
    return NULL;

comma_found:;
    unsigned long timestamp = strtoul(payload_str, NULL, 10);
    cobs_decode(msg_data, 16, (uint8_t *)&payload);
    uint16_t checksum_calc = fletcher16((uint8_t *)&payload, sizeof(payload));

    if (checksum_calc != payload.checksum) {
        // TODO add error: checksum invalid
        return NULL;
    }

    return Py_BuildValue("ky#", timestamp, &payload, sizeof(payload));
}

static char *msg_id_strings[] = {
    [ID_MCU_STATUS] = "MCU_STATUS",
    [ID_MCU_PEDAL_READINGS] = "MCU_PEDAL_READINGS",
    [ID_RCU_STATUS] = "RCU_STATUS",
    [ID_FCU_STATUS] = "FCU_STATUS",
    [ID_FCU_READINGS] = "FCU_READINGS",
    [ID_FCU_ACCELEROMETER] = "FCU_ACCELEROMETER",
    [ID_RCU_RESTART_MC] = "RCU_RESTART_MC",
    [ID_BMS_ONBOARD_TEMPERATURES] = "BMS_ONBOARD_TEMPERATURES",
    [ID_BMS_ONBOARD_DETAILED_TEMPERATURES] = "BMS_ONBOARD_DETAILED_TEMPERATURES",
    [ID_BMS_VOLTAGES] = "BMS_VOLTAGES",
    [ID_BMS_DETAILED_VOLTAGES] = "BMS_DETAILED_VOLTAGES",
    [ID_BMS_TEMPERATURES] = "BMS_TEMPERATURES",
    [ID_BMS_DETAILED_TEMPERATURES] = "BMS_DETAILED_TEMPERATURES",
    [ID_BMS_STATUS] = "BMS_STATUS",
    [ID_BMS_BALANCING_STATUS] = "BMS_BALANCING_STATUS",
    [ID_BMS_READ_WRITE_PARAMETER_COMMAND] = "BMS_READ_WRITE_PARAMETER_COMMAND",
    [ID_BMS_PARAMETER_RESPONSE] = "BMS_PARAMETER_RESPONSE",
    [ID_BMS_COULOMB_COUNTS] = "BMS_COULOMB_COUNTS",
    [ID_FH_WATCHDOG_TEST] = "FH_WATCHDOG_TEST",
    [ID_CCU_STATUS] = "CCU_STATUS",
    [ID_MC_TEMPERATURES_1] = "MC_TEMPERATURES_1",
    [ID_MC_TEMPERATURES_2] = "MC_TEMPERATURES_2",
    [ID_MC_TEMPERATURES_3] = "MC_TEMPERATURES_3",
    [ID_MC_ANALOG_INPUTS_VOLTAGES] = "MC_ANALOG_INPUTS_VOLTAGES",
    [ID_MC_DIGITAL_INPUT_STATUS] = "MC_DIGITAL_INPUT_STATUS",
    [ID_MC_MOTOR_POSITION_INFORMATION] = "MC_MOTOR_POSITION_INFORMATION",
    [ID_MC_CURRENT_INFORMATION] = "MC_CURRENT_INFORMATION",
    [ID_MC_VOLTAGE_INFORMATION] = "MC_VOLTAGE_INFORMATION",
    [ID_MC_FLUX_INFORMATION] = "MC_FLUX_INFORMATION",
    [ID_MC_INTERNAL_VOLTAGES] = "MC_INTERNAL_VOLTAGES",
    [ID_MC_INTERNAL_STATES] = "MC_INTERNAL_STATES",
    [ID_MC_FAULT_CODES] = "MC_FAULT_CODES",
    [ID_MC_TORQUE_TIMER_INFORMATION] = "MC_TORQUE_TIMER_INFORMATION",
    [ID_MC_MODULATION_INDEX_FLUX_WEAKENING_OUTPUT_INFORMATION] = "MC_MODULATION_INDEX_FLUX_WEAKENING_OUTPUT_INFORMATION",
    [ID_MC_FIRMWARE_INFORMATION] = "MC_FIRMWARE_INFORMATION",
    [ID_MC_DIAGNOSTIC_DATA] = "MC_DIAGNOSTIC_DATA",
    [ID_MC_COMMAND_MESSAGE] = "MC_COMMAND_MESSAGE",
    [ID_MC_READ_WRITE_PARAMETER_COMMAND] = "MC_READ_WRITE_PARAMETER_COMMAND",
    [ID_MC_READ_WRITE_PARAMETER_RESPONSE] = "MC_READ_WRITE_PARAMETER_RESPONSE",
    [ID_GLV_CURRENT_READINGS] = "GLV_CURRENT_READINGS",
    [ID_ECU_GPS_READINGS_ALPHA] = "ECU_GPS_READINGS_ALPHA",
    [ID_ECU_GPS_READINGS_BETA] = "ECU_GPS_READINGS_BETA"
};

static PyObject *can_decode(PyObject *self, PyObject *args)
{
    unsigned long timestamp;
    Telem_message_t msg;
    if (!PyArg_ParseTuple(args, "ky", &timestamp, (char **)&msg))
        return NULL;

    PyObject *msg_data = NULL;
    PyObject *error = Py_BuildValue("{s: s}", "error", "Invalid message type");
    switch (msg.msg_id) {
    case ID_MCU_STATUS:
        msg_data = Py_BuildValue("{s: B, s: B, s: h, s: f}",
            "state",                msg.mcu_status.state,
            "flags",                msg.mcu_status.flags,
            "temperature",          msg.mcu_status.temperature,
            "glv_battery_voltage",  msg.mcu_status.glv_battery_voltage / 100.f);
        break;
    case ID_MCU_PEDAL_READINGS:
        msg_data = Py_BuildValue("{s: H, s: H, s: H, s: B, s: B}",
            "accelerator_pedal_raw_1",  msg.mcu_petal_readings.accelerator_pedal_raw_1,
            "accelerator_pedal_raw_2",  msg.mcu_petal_readings.accelerator_pedal_raw_2,
            "brake_pedal_raw",          msg.mcu_petal_readings.brake_pedal_raw,
            "pedal_flags",              msg.mcu_petal_readings.pedal_flags,
            "torque_map_mode",          msg.mcu_petal_readings.torque_map_mode);
        break;
    case ID_RCU_STATUS:
    case ID_FCU_STATUS:
    case ID_FCU_READINGS:
    case ID_FCU_ACCELEROMETER:
    case ID_RCU_RESTART_MC:
        msg_data = error;
        break;
    case ID_BMS_ONBOARD_TEMPERATURES:
        msg_data = Py_BuildValue("{s: f, s: f, s: f}",
            "average_temperature",  msg.bms_onboard_temperatures.average_temperature / 100.f,
            "low_temperature",      msg.bms_onboard_temperatures.low_temperature / 100.f,
            "high_temperature",     msg.bms_onboard_temperatures.high_temperature / 100.f);
        break;
    case ID_BMS_ONBOARD_DETAILED_TEMPERATURES:
        msg_data = Py_BuildValue("{s: B, s: f, s: f}",
            "ic_id",            msg.bms_onboard_detailed_temperatures.ic_id,
            "temperature_0",    msg.bms_onboard_detailed_temperatures.temperature_0 / 100.f,
            "temperature_1",    msg.bms_onboard_detailed_temperatures.temperature_1 / 100.f);
        break;
    case ID_BMS_VOLTAGES:
        msg_data = Py_BuildValue("{s: f, s: f, s: f, s: f}",
            "average_voltage",  msg.bms_voltages.average_voltage / 1000.f,
            "low_voltage",      msg.bms_voltages.low_voltage / 1000.f,
            "high_voltage",     msg.bms_voltages.high_voltage / 1000.f,
            "total_voltage",    msg.bms_voltages.total_voltage / 100.f);
        break;
    case ID_BMS_DETAILED_VOLTAGES:
        msg_data = Py_BuildValue("{s: B, s: f, s: f, s: f}",
            "ic_id_group_id",   msg.bms_detailed_voltages.ic_id_group_id,
            "voltage_0",        msg.bms_detailed_voltages.voltage_0 / 1000.f,
            "voltage_1",        msg.bms_detailed_voltages.voltage_1 / 1000.f,
            "voltage_2",        msg.bms_detailed_voltages.voltage_2 / 1000.f);
        break;
    case ID_BMS_TEMPERATURES:
        msg_data = Py_BuildValue("{s: f, s: f, s: f}",
            "average_temperature",  msg.bms_temperatures.average_temperature / 100.f,
            "low_temperature",      msg.bms_temperatures.low_temperature / 100.f,
            "high_temperature",     msg.bms_temperatures.high_temperature / 100.f);
        break;
    case ID_BMS_DETAILED_TEMPERATURES:
        msg_data = Py_BuildValue("{s: B, s: f, s: f, s: f}",
            "ic_id",            msg.bms_detailed_temperatures.ic_id,
            "temperature_0",    msg.bms_detailed_temperatures.temperature_0 / 100.f,
            "temperature_1",    msg.bms_detailed_temperatures.temperature_1 / 100.f,
            "temperature_2",    msg.bms_detailed_temperatures.temperature_2 / 100.f);
        break;
    case ID_BMS_STATUS:
        msg_data = Py_BuildValue("{s: B, s: H, s: f, s: B}",
            "state",        msg.bms_status.state,
            "error_flags",  msg.bms_status.error_flags,
            "current",      msg.bms_status.current / 100.f,
            "flags",        msg.bms_status.flags);
    case ID_BMS_BALANCING_STATUS:
//skipped
    case ID_BMS_READ_WRITE_PARAMETER_COMMAND:
//can't find
    case ID_BMS_PARAMETER_RESPONSE:
//can't find
        msg_data = error;
        break;
    case ID_BMS_COULOMB_COUNTS:
        msg_data = Py_BuildValue("{s: I, s: I}",
        "total_charge"          msg.bms_status.total_charge,
        "total_discharge"       msg.bms_status.total_discharge);
    break;

    case ID_FH_WATCHDOG_TEST:
        msg_data = error;
        break;
    case ID_CCU_STATUS:
        msg_data = Py_BuildValue("{s: p}",
        "charge_enabled",        msg.bms_status.charge_enabled);
    break;

    case ID_MC_TEMPERATURES_1:
        msg_data = Py_BuildValue("{s: f, s: f, s: f, s: f}",
            "module_a_temperature",          msg.bms_status.module_a_temperature / 10.f,
            "module_b_temperature",          msg.bms_status.module_b_temperature / 10.f,
            "module_c_temperature",          msg.bms_status.module_c_temperature / 10.f,
            "gate_driver_board_temperature", msg.bms_status.gate_driver_board_temperature / 10.f);
        break;

    case ID_MC_TEMPERATURES_2:
        msg_data = Py_BuildValue("{s: f, s: f, s: f, s: f}",
            "control_board_temperature",    msg.bms_status.control_board_temperature / 10.f,
            "rtd_1_temperature",            msg.bms_status.rtd_1_temperature / 10.f,
            "rtd_2_temperature",            msg.bms_status.rtd_2_temperature / 10.f,
            "rtd_3_temperature",            msg.bms_status.rtd_3_temperature / 10.f,);
        break;

    case ID_MC_TEMPERATURES_3:
        msg_data = Py_BuildValue("{s: f, s: f, s: f, s: f}",
            "rtd_4_temperature",        msg.bms_status.rtd_4_temperature / 10.f,
            "rtd_5_temperature",        msg.bms_status.rtd_5_temperature / 10.f,
            "motor_temperature",        msg.bms_status.motor_temperature / 10.f,
            "torque_shudder",           msg.bms_status.torque_shudder / 10.f);
        break;

    case ID_MC_ANALOG_INPUTS_VOLTAGES:
        msg_data = Py_BuildValue("",
            "analog_input_1",       msg.bms_status.analog_input_1,
            "analog_input_2",       msg.bms_status.analog_input_2,
            "analog_input_3",       msg.bms_status.analog_input_3,
            "analog_input_4",       msg.bms_status.analog_input_4);
        break;

    case ID_MC_DIGITAL_INPUT_STATUS:
        msg_data = Py_BuildValue("",
            "digital_input_1"       msg.bms_status.digital_input_1,
            "digital_input_2"       msg.bms_status.digital_input_2,
            "digital_input_3"       msg.bms_status.digital_input_3,
            "digital_input_4"       msg.bms_status.digital_input_4,
            "digital_input_5"       msg.bms_status.digital_input_5,
            "digital_input_6"       msg.bms_status.digital_input_6,
            "digital_input_7"       msg.bms_status.digital_input_7,
            "digital_input_8"       msg.bms_status.digital_input_8);
        break;

    case ID_MC_MOTOR_POSITION_INFORMATION:
        msg_data = Py_BuildValue("",
            "motor_angle",                      msg.bms_status.motor_angle,
            "motor_speed",                      msg.bms_status.motor_speed,
            "electrical_output_frequency",      msg.bms_status.electrical_output_frequency);
        break;

    case ID_MC_CURRENT_INFORMATION:
        msg_data = Py_BuildValue("",
            "phase_a_current",      msg.bms_status.phase_a_current,
            "phase_b_current",      msg.bms_status.phase_b_current,
            "phase_c_current",      msg.bms_status.phase_c_current,
            "dc_bus_current",       msg.bms_status.dc_bus_current);
        break;

    case ID_MC_VOLTAGE_INFORMATION:
        msg_data = Py_BuildValue("",
            "dc_bus_voltage",       msg.bms_status.dc_bus_voltage,
            "output_voltage",       msg.bms_status.output_voltage,
            "phase_ab_voltage",     msg.bms_status.phase_ab_voltage,
            "phase_bc_voltage",     msg.bms_status.phase_bc_voltage);
        break;

    case ID_MC_FLUX_INFORMATION:
    //can't find
    case ID_MC_INTERNAL_VOLTAGES:
    //can't find
    case ID_MC_INTERNAL_STATES:
        msg_data = Py_BuildValue("",
            "vsm_state",                            msg.bms_status.vsm_state,
            "inverter_state",                       msg.bms_status.inverter_state,
            "relay_state",                          msg.bms_status.relay_state,
            "inverter_run_mode_discharge_state"     msg.bms_status.inverter_run_mode_discharge_state,
            "inverter_command_mode",                msg.bms_status.inverter_command_mode,
            "inverter_enable",                      msg.bms_status.inverter_enable,
            "direction_command",                    msg.bms_status.direction_command);
        break;

    case ID_MC_FAULT_CODES:
        msg_data = Py_BuildValue("",
            "post_fault_lo",            msg.bms_status.post_fault_lo,
            "post_fault_hi",            msg.bms_status.post_fault_hi,
            "run_fault_lo",            msg.bms_status.run_fault_lo,
            "run_fault_hi",            msg.bms_status.run_fault_hi);
        break;
    case ID_MC_TORQUE_TIMER_INFORMATION:
        msg_data = Py_BuildValue("",
            "command_torque",           msg.bms_status.command_torque,
            "torque_feedback",          msg.bms_status.torque_feedback,
            "power_on_timer",           msg.bms_status.power_on_timer);
        break;

    case ID_MC_MODULATION_INDEX_FLUX_WEAKENING_OUTPUT_INFORMATION:
        msg_data = Py_BuildValue("",
            "modulation_index",         msg.bms_status.modulation_index,
            "flux_weakening_output",    msg.bms_status.flux_weakening_output,
            "id_command",               msg.bms_status.id_command,
            "iq_command",               msg.bms_status.iq_command);
        break;

    case ID_MC_FIRMWARE_INFORMATION:
        msg_data = Py_BuildValue("",
            "eeprom_version_project_code",      msg.bms_status.eeprom_version_project_code,
            "software_version",                 msg.bms_status.software_version,
            "date_code_mmdd",                   msg.bms_status.date_code_mmdd,
            "date_code_yyyy",                   msg.bms_status.date_code_yyyy);
        break;

    case ID_MC_DIAGNOSTIC_DATA:
//can't find
    case ID_MC_COMMAND_MESSAGE:
        msg_data = Py_BuildValue("",
            "torque_command",          msg.bms_status.torque_command,
            "angular_velocity",        msg.bms_status.angular_velocity,
            "direction",               msg.bms_status.direction,
            "inverter_enable_discharge_enable", msg.bms_status.inverter_enable_discharge_enable,
            "command_torque_limit",    msg.bms_status.command_torque_limit);
        break;

    case ID_MC_READ_WRITE_PARAMETER_COMMAND:
        msg_data = Py_BuildValue("",
            "parameter_address",       msg.bms_status.parameter);
        break;
    case ID_MC_READ_WRITE_PARAMETER_RESPONSE:
    case ID_GLV_CURRENT_READINGS:
    case ID_ECU_GPS_READINGS_ALPHA:
    case ID_ECU_GPS_READINGS_BETA:
    default:
        msg_data = error;
    }

    if (!msg_data) return NULL;
    else return Py_BuildValue("{s: k, s: I, s: s, s: N}",
        "timestamp",    timestamp,
        "msg_id",       msg.msg_id,
        "msg_id_str",   msg_id_strings[msg.msg_id],
        "data",         msg_data);
}

static PyMethodDef can_methods[] = {
    { "unpack", can_unpack, METH_VARARGS, "Unpack a CAN message." },
    { "decode", can_decode, METH_VARARGS, "Decode a CAN message." }
};

static struct PyModuleDef can_module = {
    PyModuleDef_HEAD_INIT,
    "hytech_can",
    NULL,
    -1,
    can_methods
};

PyMODINIT_FUNC PyInit_hytech_can(void)
{
    return PyModule_Create(&can_module);
}
