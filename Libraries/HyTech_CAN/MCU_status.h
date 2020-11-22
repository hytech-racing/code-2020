#pragma once

#include <string.h>
#include <cstdint>

enum class MCU_STATE {TRACTIVE_SYSTEM_NOT_ACTIVE = 0,
                      TRACTIVE_SYSTEM_ACTIVE = 1,
                      ENABLING_INVERTER = 2,
                      WAITING_READY_TO_DRIVE_SOUND = 3,
                      READY_TO_DRIVE = 4};

enum class SHUTDOWN_INPUTS {HIGH_UNLATCHED = 0,
                            HIGH_LATCHED = 1,
                            UNKNOWN_ERROR = 2,
                            LOW = 3};

enum class TORQUE_MAP_MODE {MODE_0 = 0,
                            MODE_1 = 1,
                            MODE_2 = 2,
                            MODE_3 = 3};

#pragma pack(push,1)

class MCU_status {
public:
    MCU_status() = default;
    MCU_status(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    inline uint8_t get_shutdown_inputs()           const { return shutdown_inputs; }
    inline SHUTDOWN_INPUTS get_imd_ok_state()        const { return (shutdown_inputs & 0x03); }
    inline SHUTDOWN_INPUTS get_bms_ok_state()      const { return (shutdown_inputs & 0x0C) >> 2; }
    inline SHUTDOWN_INPUTS get_bspd_ok_state()     const { return (shutdown_inputs & 0x30) >> 4; }
    inline SHUTDOWN_INPUTS get_software_ok_state() const { return (shutdown_inputs & 0xF0) >> 6; }

    inline void set_shutdown_inputs(const uint8_t inputs)          { shutdown_inputs = inputs; }
    inline void set_imd_ok_state(const SHUTDOWN_INPUTS input)        { shutdown_inputs = (shutdown_inputs & 0xFC) | (input); }
    inline void set_bms_ok_state(const SHUTDOWN_INPUTS input)      { shutdown_inputs = (shutdown_inputs & 0xF3) | (input << 2); }
    inline void set_bspd_ok_state(const SHUTDOWN_INPUTS input)     { shutdown_inputs = (shutdown_inputs & 0xCF) | (input << 4); }
    inline void set_software_ok_state(const SHUTDOWN_INPUTS input) { shutdown_inputs = (shutdown_inputs & 0x3F) | (input << 6); }


    inline uint8_t get_shutdown_states()         const { return shutdown_states; }
    inline bool get_shutdown_a_above_threshold() const { return shutdown_states & 0x01; }
    inline bool get_shutdown_b_above_threshold() const { return shutdown_states & 0x02; }
    inline bool get_shutdown_c_above_threshold() const { return shutdown_states & 0x04; }
    inline bool get_shutdown_d_above_thershold() const { return shutdown_states & 0x08; }
    inline bool get_shutdown_e_abive_threshold() const { return shutdown_states & 0x10; }
    inline bool get_bspd_current_high()          const { return shutdown_states & 0x20; }
    inline bool get_bspd_brake_high()            const { return shutdown_states & 0x40; }
    inline bool get_software_is_ok()             const { return shutdown_states & 0x80; }

    inline void set_shutdown_states(const uint8_t states)        { this->shutdown_states = states; }
    inline void set_shutdown_a_above_threshold(const bool above) { shutdown_states = (shutdown_states & 0xFE) | (above); }
    inline void set_shutdown_b_above_threshold(const bool above) { shutdown_states = (shutdown_states & 0xFD) | (above << 1); }
    inline void set_shutdown_c_above_threshold(const bool above) { shutdown_states = (shutdown_states & 0xFB) | (above << 2); }
    inline void set_shutdown_d_above_threshold(const bool above) { shutdown_states = (shutdown_states & 0xF7) | (above << 3); }
    inline void set_shutdown_e_above_threshold(const bool above) { shutdown_states = (shutdown_states & 0xEF) | (above << 4); }
    inline void set_bspd_current_high(const bool high)           { shutdown_states = (shutdown_states & 0xDF) | (high  << 5); }
    inline void set_bspd_brake_high(const bool high)             { shutdown_states = (shutdown_states & 0xBF) | (high  << 6); }
    inline void set_software_is_ok(const bool is_ok)             { shutdown_states = (shutdown_states & 0x7F) | (is_ok << 7); }
    
    inline uint8_t get_ecu_states()              const { return (ecu_states); }
    inline MCU_STATE get_mcu_state()             const { return (ecu_states & 0x07); }
    inline TORQUE_MAP_MODE get_torque_map_mode() const { return (ecu_states & 0x18) >> 3}
    inline bool get_inverter_powered()           const { return (ecu_states & 0x20); }
    inline bool get_energy_meter_present()       const { return (ecu_states & 0x40); }

    inline void set_ecu_states(const uint8_t states)             { this->ecu_states = ecu_states; }
    inline void set_mcu_status(const MCU_STATE state)            { ecu_states = (ecu_states & 0xF8) | (state); }
    inline void set_torque_map_mode(const TORQUE_MAP_MODE mode)  { ecu_states = (ecu_states & 0xE7) | (mode    << 3); }
    inline void set_inverter_powered(const bool powered)         { ecu_states = (ecu_states & 0xDF) | (powered << 5); }
    inline void set_energy_meter_present(const bool present)     { ecu_states = (ecu_states & 0xBF) | (present << 6); }

    inline uint16_t get_distance_travelled() const { return distance_travelled; }
    inline void set_distance_travelled(const uint16_t distance) { distance_travelled = distance; }

private:
    /*
     * IMD_OK (2)
     * BMS_OK (2)
     * BSPD (2)
     * software shutdown (2)
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
     * software_is_ok
     */
    uint8_t shutdown_states;

    /**
     * state (3 bits)
     * torque_mode (2 bits)
     * inverter powered
     * energy_meter
     */
    uint8_t ecu_states;
    uint16_t distance_travelled;
};

#pragma pack(pop)
