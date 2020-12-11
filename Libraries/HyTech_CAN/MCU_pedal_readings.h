#pragma once
#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

class MCU_pedal_readings {
public:
    MCU_pedal_readings() = default;

    MCU_pedal_readings(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8]) { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[8]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_accelerator_pedal_1() const { return accelerator_pedal_1; }
    inline uint16_t get_accelerator_pedal_2() const { return accelerator_pedal_2; }
    inline uint16_t get_brake_transducer_1()  const { return brake_transducer_1; }
    inline uint16_t get_brake_transducer_2()  const { return brake_transducer_2 & 0x0FFF; }

    // Setters
    inline void set_accelerator_pedal_1(uint16_t reading) { accelerator_pedal_1 = reading; }
    inline void set_accelerator_pedal_2(uint16_t reading) { accelerator_pedal_2 = reading; }
    inline void set_brake_transducer_1(uint16_t reading)  { brake_transducer_1 = reading; }
    inline void set_brake_transducer_2(uint16_t reading)  { brake_transducer_2 = (brake_transducer_2 & 0xF000) | (reading & 0x0FFF); }

    inline bool get_accel_implausability() const { return brake_transducer_2 & 0x1000; }
    inline bool get_brake_implausability() const { return brake_transducer_2 & 0x2000; }
    inline bool get_brake_pedal_active()   const { return brake_transducer_2 & 0x4000; }

    inline void set_accel_implausability(const bool implausable) { brake_transducer_2 = (brake_transducer_2 & 0xEFFF) | (implausable << 12); }
    inline void set_brake_implausability(const bool implausable) { brake_transducer_2 = (brake_transducer_2 & 0xDFFF) | (implausable << 13); }
    inline void set_brake_pedal_active(const bool pressed)       { brake_transducer_2 = (brake_transducer_2 & 0xBFFF) | (pressed     << 14); }
private:
    uint16_t accelerator_pedal_1;
    uint16_t accelerator_pedal_2;
    uint16_t brake_transducer_1;
    /**
     * 12 bits for brake transducer read
     * accel_implausability
     * brake_implausability
     * brake_pressed
     */
    uint16_t brake_transducer_2;
};

#pragma pack(pop)