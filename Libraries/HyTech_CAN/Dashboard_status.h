#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

class Dashboard_status {
public:
    Dashboard_status() = default;
    Dashboard_status(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8])  { memcpy(this,buf,sizeof(*this)); }
    inline void write(uint8_t buf[8]) const { memcpy(buf,this,sizeof(*this)); }

    // status getters
    inline uint8_t get_dashboard_states()        const { return (dashboard_states); }
    inline bool get_start_btn()                  const { return (dashboard_states & 0x01); }
    inline bool get_buzzer_active()              const { return (dashboard_states & 0x02); }
    inline bool get_ssok_above_threshold()       const { return (dashboard_states & 0x04); }
    inline bool get_shutdown_h_above_thershold() const { return (dashboard_states & 0x08); }

    // status setters
    inline void set_dashboard_states(uint8_t states)       { dashboard_states = states; }
    inline void set_start_btn(bool pressed)                { dashboard_states = (dashboard_states & 0xFE) | (pressed); }
    inline void set_buzzer_active(bool active)             { dashboard_states = (dashboard_states & 0xFD) | (active  << 1); }
    inline void set_ssok_above_threshold(bool above)       { dashboard_states = (dashboard_states & 0xFB) | (above   << 2); }
    inline void set_shutdown_h_above_threshold(bool above) { dashboard_states = (dashboard_states & 0xF7) | (above << 3); }
    inline void toggle_start_btn()                         { dashboard_states ^= 0x01; }

    // mark button: makes a note in the telemetry log
    // mode button: changes torque mode
    // mc cycle button: restarts the inverter
    // start button: starts car
    // launch_ctrl button: toggles launch control state

    //Button Getters
    inline uint8_t get_button_flags() const { return (button_flags); }
    inline bool get_mark_btn()        const { return (button_flags & 0x01); }
    inline bool get_mode_btn()        const { return (button_flags & 0x02); }
    inline bool get_mc_cycle_btn()    const { return (button_flags & 0x04); }
    inline bool get_launch_ctrl_btn() const { return (button_flags & 0x08); }

    //Button Setters
    inline void set_button_flags (uint8_t flags)           { button_flags = flags; }
    inline void set_mark_btn (bool mark_btn)               { button_flags = (button_flags & 0xFE) | (mark_btn); }
    inline void set_mode_btn (bool mode_btn)               { button_flags = (button_flags & 0xFD) | (mode_btn        << 1); }
    inline void set_mc_cycle_btn (bool mc_cycle_btn)       { button_flags = (button_flags & 0xFB) | (mc_cycle_btn    << 2); }
    inline void set_launch_ctrl_btn (bool launch_ctrl_btn) { button_flags = (button_flags & 0xEF) | (launch_ctrl_btn << 3); }

    //Button Toggles
    inline void toggle_mark_btn()     { button_flags ^= 0x01; }
    inline void toggle_mode_btn()     { button_flags ^= 0x02; }
    inline void toggle_mc_cycle_btn() { button_flags ^= 0x04; }
    inline void toggle_extra_btn()    { button_flags ^= 0x08; }


    //LED Getters
    inline uint8_t get_led_flags()    const { return (led_flags); }
    inline bool get_ams_led()         const { return (led_flags & 0x01); }
    inline bool get_imd_led()         const { return (led_flags & 0x02); }
    inline uint8_t get_mode_led()     const { return (led_flags & 0x0C) >> 2; }//2 bits required to store mode 
    inline bool get_mc_error_led()    const { return (led_flags & 0x10); }
    inline uint8_t get_start_led()    const { return (led_flags & 0x60) >> 5; } //2 bits required to store start 
    inline bool get_launch_ctrl_led() const { return (led_flags & 0x80); }

    //LED Setters
    inline void set_led_flags(uint8_t flags)              { led_flags = flags; }
    inline void set_ams_led(bool ams_led)                 { led_flags = (led_flags & 0xFE) | (ams_led);  }
    inline void set_imd_led(bool imd_led)                 { led_flags = (led_flags & 0xFD) | (imd_led           << 1); }
    inline void set_mode_led(uint8_t mode_led)            { led_flags = (led_flags & 0xF3) | ((mode_led  & 0x3) << 2); }
    inline void set_mc_error_led(bool mc_error_led)       { led_flags = (led_flags & 0xEF) | (mc_error_led      << 4); }
    inline void set_start_led(uint8_t start_led)          { led_flags = (led_flags & 0x9F) | ((start_led & 0x3) << 5); }
    inline void set_launch_ctrl_led(bool launch_ctrl_led) { led_flags = (led_flags & 0x7F) | (led_flags         << 7); }
private:
    /*
     * buzzer active
     * start button pressed
     * ssok_high
     * shutdown h high
     * (4 bits)
     */
    uint8_t dashboard_states;

    /*
     * mark
     * mode
     * mc_cycle
     * launch_ctrl
     * (4 bits)
     */
    uint8_t button_flags;

    /*
     * ams
     * imd
     * mode (2bit)
     * mc_error
     * start (2 bit)
     * launch control
     */
    uint8_t led_flags;
};

#pragma pack(pop)
