#pragma once
#include <string.h>
#include <cstdint>

#pragma pack(push,1)

#define MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE 1
#define MCU_STATE_TRACTIVE_SYSTEM_ACTIVE 2
#define MCU_STATE_ENABLING_INVERTER 3
#define MCU_STATE_WAITING_READY_TO_DRIVE_SOUND 4
#define MCU_STATE_READY_TO_DRIVE 5


/* lost: need to add
 *  bool MCU_pedal_readings::get_accelerator_implausibility() {
 *  bool MCU_pedal_readings::get_brake_implausibility() {
 *  bool MCU_pedal_readings::get_brake_pedal_active() {
 *  uint8_t MCU_pedal_readings::get_torque_map_mode() {
 */

typedef struct CAN_message_mcu_status_t {
    /*
     * OKHS
     * BMS_OK
     * BSPD
     * software shutdown
     */
    uint8_t shutdown_inputs;
    /*
     * shutdown A
     * shutdown B
     * shutdown C
     * shutdown D
     * shutdown E
     * Current high
     * brake high
     */
    uint8_t shutdown_states;
// state
// pedal_flags
// torque_map_mpde
// distance travelled
} CAN_message_mcu_status_t;


class MCU_status {
public:
    MCU_status() = default;
    MCU_status(uint8_t buf[8]) { load(buf); }
    MCU_status(uint8_t state, uint8_t flags, int16_t temperature, uint16_t glv_battery_voltage) {
        set_state(state);
        set_flags(flags);
        set_temperature(temperature);
        set_glv_battery_voltage(glv_battery_voltage);
    }

    inline void load(uint8_t buf[])     { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[])    { memcpy(buf, this, sizeof(*this)); }

    inline uint8_t get_state()                      const { return state; }
    inline uint8_t get_flags()                      const { return flags; }
    inline bool get_bms_ok_high()                   const { return flags & 0x01; }
    inline bool get_imd_okhs_high()                 const { return flags & 0x02; }
    inline bool get_inverter_powered()              const { return flags & 0x04; }
    inline bool get_shutdown_b_above_threshold()    const { return flags & 0x08; }
    inline bool get_shutdown_c_above_threshold()    const { return flags & 0x10; }
    inline bool get_shutdown_d_above_threshold()    const { return flags & 0x20; }
    inline bool get_shutdown_e_above_threshold()    const { return flags & 0x40; }
    inline bool get_shutdown_f_above_threshold()    const { return flags & 0x80; }
    inline int16_t get_temperature()                const { return temperature; }
    inline uint16_t get_glv_battery_voltage()       const { return glv_battery_voltage; }

    inline void set_state(uint8_t state)                                        { this->state = state; }
    inline void set_flags(uint8_t flags)                                        { this->flags = flags; }
    inline void set_bms_ok_high(bool bms_ok_high)                               { flags = (flags & 0xFE) | bms_ok_high; }
    inline void set_imd_okhs_high(bool imd_okhs_high)                           { flags = (flags & 0xFD) | (imd_okhs_high << 1); }
    inline void set_inverter_powered(bool inverter_powered)                     { flags = (flags & 0xFB) | (inverter_powered << 2); }
    inline void set_shutdown_b_above_threshold(bool shutdown_b_above_threshold) { flags = (flags & 0xF7) | (shutdown_b_above_threshold << 3); }
    inline void set_shutdown_c_above_threshold(bool shutdown_c_above_threshold) { flags = (flags & 0xEF) | (shutdown_c_above_threshold << 4); }
    inline void set_shutdown_d_above_threshold(bool shutdown_d_above_threshold) { flags = (flags & 0xDF) | (shutdown_d_above_threshold << 5); }
    inline void set_shutdown_e_above_threshold(bool shutdown_e_above_threshold) { flags = (flags & 0xBF) | (shutdown_e_above_threshold << 6); }
    inline void set_shutdown_f_above_threshold(bool shutdown_f_above_threshold) { flags = (flags & 0x7F) | (shutdown_f_above_threshold << 7); }
    inline void set_temperature(int16_t temperature)                            { this->temperature = temperature; }
    inline void set_glv_battery_voltage(uint16_t glv_battery_voltage)           { this->glv_battery_voltage = glv_battery_voltage; }
private:
    uint8_t state;
    uint8_t flags;
    int16_t temperature;
    uint16_t glv_battery_voltage;
};

#pragma pack(pop)
