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
    default:
        return NULL;
    }
}

static PyMethodDef can_methods[] = {
    { "unpack", can_unpack, METH_VARARGS, "Unpack a CAN message." }
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
