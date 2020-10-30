#ifndef __MCU_PEDAL_READINGS_H__
#define __MCU_PEDAL_READINGS_H__

#include <cstdint>
#include <string>

#pragma pack(push,1)

typedef struct CAN_message_mcu_pedal_readings_t {
    uint16_t accelerator_pedal_raw_1;
    uint16_t accelerator_pedal_raw_2;
    uint16_t brake_transducer1;
    uint8_t pedal_flags;
    uint8_t torque_map_mode;
} CAN_message_mcu_pedal_readings_t;

class MCU_pedal_readings {
public:
    MCU_pedal_readings() = default;

    MCU_pedal_readings(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8]) { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[8]) const { memccpy(buf, this, sizeof(*this)); }

    void write(uint8_t buf[8]);
    uint16_t get_accelerator_pedal_raw_1();
    uint16_t get_accelerator_pedal_raw_2();
    uint16_t get_brake_pedal_raw();
    uint8_t get_pedal_flags();
    bool get_accelerator_implausibility();
    bool get_brake_implausibility();
    bool get_brake_pedal_active();
    uint8_t get_torque_map_mode();
    void set_accelerator_pedal_raw_1(uint16_t accelerator_pedal_raw_1);
    void set_accelerator_pedal_raw_2(uint16_t accelerator_pedal_raw_2);
    void set_brake_pedal_raw(uint16_t brake_pedal_raw);
    void set_pedal_flags(uint8_t pedal_flags);
    void set_accelerator_implausibility(bool accelerator_implausibility);
    void set_brake_implausibility(bool brake_implausibility);
    void set_brake_pedal_active(bool brake_pedal_active);
    void set_torque_map_mode(uint8_t torque_map_mode);
private:
    uint16_t accelerator_pedal_raw_1;
    uint16_t accelerator_pedal_raw_2;
    uint16_t brake_transducer1;
    uint16_t brake_transducer2;
};

#pragma pack(pop)

#endif