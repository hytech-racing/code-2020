#include <string.h>
#include <cstdint>

enum MCU_STATE {STARTUP                      = 0,
                      TRACTIVE_SYSTEM_NOT_ACTIVE   = 1,
                      TRACTIVE_SYSTEM_ACTIVE       = 2,
                      ENABLING_INVERTER            = 3,
                      WAITING_READY_TO_DRIVE_SOUND = 4,
                      READY_TO_DRIVE               = 5};

enum SHUTDOWN_INPUTS {LOW            = 0,
                            UNKNOWN_ERROR  = 1,
                            HIGH_LATCHED   = 2,
                            HIGH_UNLATCHED = 3};

enum TORQUE_MAP_MODE {MODE_0 = 0,
                            MODE_1 = 1,
                            MODE_2 = 2,
                            MODE_3 = 3};

class MCU_status {
public:
    MCU_status(){};
    MCU_status(const int buf[8]) { load(buf); }

    inline void load(const int buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(int buf[]) const { memcpy(buf, this, sizeof(*this)); }

    inline int get_shutdown_inputs()           const { return shutdown_inputs; }
    inline int get_imd_ok_state()      const { return (shutdown_inputs & 0x03); }
    inline int get_bms_ok_state()      const { return (shutdown_inputs & 0x0C) >> 2; }
    inline int get_bspd_ok_state()     const { return (shutdown_inputs & 0x30) >> 4; }
    inline int get_software_ok_state() const { return (shutdown_inputs & 0xF0) >> 6; }

    inline void set_shutdown_inputs(const int inputs)          { shutdown_inputs = inputs; }
    inline void set_imd_ok_state(const int input)      { shutdown_inputs = (shutdown_inputs & 0xFC) | (input); }
    inline void set_bms_ok_state(const int input)      { shutdown_inputs = (shutdown_inputs & 0xF3) | (input << 2); }
    inline void set_bspd_ok_state(const int input)     { shutdown_inputs = (shutdown_inputs & 0xCF) | (input << 4); }
    inline void set_software_ok_state(const int input) { shutdown_inputs = (shutdown_inputs & 0x3F) | (input << 6); }


    inline int get_shutdown_states()         const { return shutdown_states; }
    inline bool get_shutdown_b_above_threshold() const { return shutdown_states & 0x01; }
    inline bool get_shutdown_c_above_threshold() const { return shutdown_states & 0x02; }
    inline bool get_shutdown_d_above_thershold() const { return shutdown_states & 0x04; }
    inline bool get_shutdown_e_abive_threshold() const { return shutdown_states & 0x08; }
    inline bool get_bspd_current_high()          const { return shutdown_states & 0x10; }
    inline bool get_bspd_brake_high()            const { return shutdown_states & 0x20; }
    inline bool get_software_is_ok()             const { return shutdown_states & 0x40; }

    inline void set_shutdown_states(const int states)        { this->shutdown_states = states; }
    inline void set_shutdown_b_above_threshold(const bool above) { shutdown_states = (shutdown_states & 0xFE) | (above); }
    inline void set_shutdown_c_above_threshold(const bool above) { shutdown_states = (shutdown_states & 0xFD) | (above << 1); }
    inline void set_shutdown_d_above_threshold(const bool above) { shutdown_states = (shutdown_states & 0xFB) | (above << 2); }
    inline void set_shutdown_e_above_threshold(const bool above) { shutdown_states = (shutdown_states & 0xF7) | (above << 3); }
    inline void set_bspd_current_high(const bool high)           { shutdown_states = (shutdown_states & 0xEF) | (high  << 4); }
    inline void set_bspd_brake_high(const bool high)             { shutdown_states = (shutdown_states & 0xDF) | (high  << 5); }
    inline void set_software_is_ok(const bool is_ok)             { shutdown_states = (shutdown_states & 0xBF) | (is_ok << 6); }
    
    inline int get_ecu_states()              const { return (ecu_states); }
    inline int get_mcu_state()             const { return (ecu_states & 0x07); }
    inline int get_torque_map_mode() const { return (ecu_states & 0x18) >> 3; }
    inline bool get_inverter_powered()           const { return (ecu_states & 0x20); }
    inline bool get_energy_meter_present()       const { return (ecu_states & 0x40); }

    inline void set_ecu_states(const int states)             { this->ecu_states = ecu_states; }
    inline void set_mcu_status(const int state)            { ecu_states = (ecu_states & 0xF8) | (state); }
    inline void set_torque_map_mode(const int mode)  { ecu_states = (ecu_states & 0xE7) | (mode    << 3); }
    inline void set_inverter_powered(const bool powered)         { ecu_states = (ecu_states & 0xDF) | (powered << 5); }
    inline void set_energy_meter_present(const bool present)     { ecu_states = (ecu_states & 0xBF) | (present << 6); }

    inline int get_distance_travelled() const { return distance_travelled; }
    inline void set_distance_travelled(const int distance) { distance_travelled = distance; }

private:
    /*
     * IMD_OK (2)
     * BMS_OK (2)
     * BSPD (2)
     * software shutdown (2)
     */
    int shutdown_inputs;
    /*
     * shutdown B
     * shutdown C
     * shutdown D
     * shutdown E
     * Current high
     * brake high
     * software_is_ok
     * (empty bit)
     */
    int shutdown_states;

    /**
     * state (3 bits)
     * torque_mode (2 bits)
     * inverter powered
     * energy_meter
     * (empty bit)
     */
    int ecu_states;
    int distance_travelled;
};

MCU_status mcu_status;

#define RELAY_INPUT_LOW_FAULT 33
#define RELAY_INPUT_HIGH_LATCHED 540
#define RELAY_INPUT_HIGH_UNLATCHED 720

void func(int BMS_fault) {
if(BMS_fault < RELAY_INPUT_LOW_FAULT)
    mcu_status.set_bms_ok_state(LOW);
else if (BMS_fault < RELAY_INPUT_HIGH_LATCHED)
    mcu_status.set_bms_ok_state(UNKNOWN_ERROR);
else if (BMS_fault < RELAY_INPUT_HIGH_UNLATCHED)
    mcu_status.set_bms_ok_state(HIGH_LATCHED);
else
    mcu_status.set_bms_ok_state(HIGH_UNLATCHED);
}

void func2(int BMS_fault) {
    if (BMS_fault >= RELAY_INPUT_HIGH_LATCHED)
        // adding the comparison should be faster than adding another if block
        mcu_status.set_bms_ok_state(2 + BMS_fault >= RELAY_INPUT_HIGH_UNLATCHED);
    else
        mcu_status.set_bms_ok_state(BMS_fault >= RELAY_INPUT_LOW_FAULT);
}