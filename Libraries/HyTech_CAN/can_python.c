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

static PyObject *can_decode(PyObject *self, PyObject *args)
{
    unsigned long timestamp;
    Telem_message_t msg;
    if (!PyArg_ParseTuple(args, "ky", &timestamp, (char **)&msg))
        return NULL;

    switch (msg.msg_id) {
    case ID_MCU_STATUS:
        return Py_BuildValue("{s: B, s: B, s: h, s: H}",
            "state",                msg.mcu_status.state,
            "flags",                msg.mcu_status.flags,
            "temperature",          msg.mcu_status.temperature,
            "glv_battery_voltage",  msg.mcu_status.glv_battery_voltage);
    case ID_MCU_PEDAL_READINGS:
        return Py_BuildValue("{s: H, s: H, s: H, s: B, s: B}",
            "accelerator_pedal_raw_1",  msg.mcu_petal_readings.accelerator_pedal_raw_1,
            "accelerator_pedal_raw_2",  msg.mcu_petal_readings.accelerator_pedal_raw_2,
            "brake_pedal_raw",          msg.mcu_petal_readings.brake_pedal_raw,
            "pedal_flags",              msg.mcu_petal_readings.pedal_flags,
            "torque_map_mode",          msg.mcu_petal_readings.torque_map_mode);
    case ID_RCU_STATUS:
        break;
    case ID_FCU_STATUS:
        break;
    case ID_FCU_READINGS:
        break;
    case ID_FCU_ACCELEROMETER:
        break;
    case ID_RCU_RESTART_MC:
        break;
    case ID_BMS_ONBOARD_TEMPERATURES:
        return Py_BuildValue("{s: h, s: h, s: h}",
            "average_temperature",  msg.bms_onboard_temperatures.average_temperature,
            "low_temperature",      msg.bms_onboard_temperatures.low_temperature,
            "high_temperature",     msg.bms_onboard_temperatures.high_temperature);
    case ID_BMS_ONBOARD_DETAILED_TEMPERATURES:
        return Py_BuildValue("{s: B, s: h, s: h}",
            "ic_id",            msg.bms_onboard_detailed_temperatures.ic_id,
            "temperature_0",    msg.bms_onboard_detailed_temperatures.temperature_0,
            "temperature_1",    msg.bms_onboard_detailed_temperatures.temperature_1);
    case ID_BMS_VOLTAGES:
        return Py_BuildValue("{s: H, s: H, s: H, s: H}",
            "average_voltage",  msg.bms_voltages.average_voltage,
            "low_voltage",      msg.bms_voltages.low_voltage,
            "high_voltage",     msg.bms_voltages.high_voltage,
            "total_voltage",    msg.bms_voltages.total_voltage);
    case ID_BMS_DETAILED_VOLTAGES:
    case ID_BMS_TEMPERATURES:
    case ID_BMS_DETAILED_TEMPERATURES:
    case ID_BMS_STATUS:
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
    }
    return NULL;
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
