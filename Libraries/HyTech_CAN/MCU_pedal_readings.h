#ifndef __MCU_PEDAL_READINGS_H__
#define __MCU_PEDAL_READINGS_H__

#include <cstdint>
#include <string>

#pragma pack(push,1)

typedef struct CAN_message_mcu_pedal_readings_t {
    uint16_t accelerator_pedal_raw_1;
    uint16_t accelerator_pedal_raw_2;
    uint16_t brake_pedal_raw;
    uint8_t pedal_flags;
    uint8_t torque_map_mode;
} CAN_message_mcu_pedal_readings_t;

#pragma pack(pop)

#endif