#ifndef __MCU_STATUS_H__
#define __MCU_STATUS_H__

#include <cstdint>
#include <string>

#define MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE 1
#define MCU_STATE_TRACTIVE_SYSTEM_ACTIVE 2
#define MCU_STATE_ENABLING_INVERTER 3
#define MCU_STATE_WAITING_READY_TO_DRIVE_SOUND 4
#define MCU_STATE_READY_TO_DRIVE 5

#pragma pack(push,1)

typedef struct CAN_message_mcu_status_t {
    uint8_t state;
    uint8_t flags;
    int16_t temperature;
    uint16_t glv_battery_voltage;
} CAN_message_mcu_status_t;

#pragma pack(pop)

class MCU_status {
public:
    MCU_status() { message = {}; }
    
    MCU_status(uint8_t buf[8]){
        message = {};

        memcpy(&(message.state), &buf[0], sizeof(uint8_t));
        memcpy(&(message.flags), &buf[1], sizeof(uint8_t));
        memcpy(&(message.temperature), &buf[2], sizeof(int16_t));
        memcpy(&(message.glv_battery_voltage), &buf[4], sizeof(uint16_t));
    }

    MCU_status(uint8_t state, uint8_t flags, int16_t temperature, uint16_t glv_battery_voltage);
    void load(uint8_t buf[8]);
    void write(uint8_t buf[8]);
    uint8_t get_state();
    uint8_t get_flags();
    bool get_bms_ok_high();
    bool get_imd_okhs_high();
    bool get_inverter_powered();
    bool get_shutdown_b_above_threshold();
    bool get_shutdown_c_above_threshold();
    bool get_shutdown_d_above_threshold();
    bool get_shutdown_e_above_threshold();
    bool get_shutdown_f_above_threshold();
    int16_t get_temperature();
    uint16_t get_glv_battery_voltage();
    void set_state(uint8_t state);
    void set_flags(uint8_t flags);
    void set_bms_ok_high(bool bms_ok_high);
    void set_imd_okhs_high(bool imd_okhs_high);
    void set_inverter_powered(bool inverter_powered);
    void set_shutdown_b_above_threshold(bool shutdown_b_above_threshold);
    void set_shutdown_c_above_threshold(bool shutdown_c_above_threshold);
    void set_shutdown_d_above_threshold(bool shutdown_d_above_threshold);
    void set_shutdown_e_above_threshold(bool shutdown_e_above_threshold);
    void set_shutdown_f_above_threshold(bool shutdown_f_above_threshold);
    void set_temperature(int16_t temperature);
    void set_glv_battery_voltage(uint16_t glv_battery_voltage);
private:
    CAN_message_mcu_status_t message;
};