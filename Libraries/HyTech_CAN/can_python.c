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
        msg_data = Py_BuildValue("{s: h, s: h, s: h}",
            "average_temperature",  msg.bms_onboard_temperatures.average_temperature,
            "low_temperature",      msg.bms_onboard_temperatures.low_temperature,
            "high_temperature",     msg.bms_onboard_temperatures.high_temperature);
            break;
    case ID_BMS_ONBOARD_DETAILED_TEMPERATURES:
        msg_data = Py_BuildValue("{s: B, s: h, s: h}",
            "ic_id",            msg.bms_onboard_detailed_temperatures.ic_id,
            "temperature_0",    msg.bms_onboard_detailed_temperatures.temperature_0,
            "temperature_1",    msg.bms_onboard_detailed_temperatures.temperature_1);
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
        msg_data = Py_BuildValue("",
            "state",        msg.bms_status.state,
            "error_flags",  msg.bms_status.error_flags,
            "current",      msg.bms_status.current,
            "flags",        msg.bms_status.flags);
    case ID_BMS_BALANCING_STATUS:
    case ID_BMS_READ_WRITE_PARAMETER_COMMAND:
    case ID_BMS_PARAMETER_RESPONSE:
    case ID_BMS_COULOMB_COUNTS:
    case ID_FH_WATCHDOG_TEST:
    case ID_CCU_STATUS:
    case ID_MC_TEMPERATURES_1:
    case ID_MC_TEMPERATURES_2:
    case ID_MC_TEMPERATURES_3:
    case ID_MC_ANALOG_INPUTS_VOLTAGES:
    case ID_MC_DIGITAL_INPUT_STATUS:
    case ID_MC_MOTOR_POSITION_INFORMATION:
    case ID_MC_CURRENT_INFORMATION:
    case ID_MC_VOLTAGE_INFORMATION:
    case ID_MC_FLUX_INFORMATION:
    case ID_MC_INTERNAL_VOLTAGES:
    case ID_MC_INTERNAL_STATES:
    case ID_MC_FAULT_CODES:
    case ID_MC_TORQUE_TIMER_INFORMATION:
    case ID_MC_MODULATION_INDEX_FLUX_WEAKENING_OUTPUT_INFORMATION:
    case ID_MC_FIRMWARE_INFORMATION:
    case ID_MC_DIAGNOSTIC_DATA:
    case ID_MC_COMMAND_MESSAGE:
    case ID_MC_READ_WRITE_PARAMETER_COMMAND:
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
