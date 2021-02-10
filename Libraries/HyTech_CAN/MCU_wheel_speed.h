#pragma once

#include <stdint.h>
#include <cstring>

#pragma pack(push,1)

class MCU_wheel_speed {
public:
    MCU_wheel_speed() = default;

    MCU_wheel_speed(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[8]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_rpm_front_left()  const { return wheel_rpm_front_left; }
    inline uint16_t get_rpm_front_right() const { return wheel_rpm_front_right; }
    inline uint16_t get_rpm_back_left()   const { return wheel_rpm_back_left; }
    inline uint16_t get_rpm_back_right()  const { return wheel_rpm_back_right; }

    // Setters
    inline void set_rpm_front_left(uint16_t speed)  { wheel_rpm_front_left = speed; }
    inline void set_rpm_front_right(uint16_t speed) { wheel_rpm_front_right = speed; }
    inline void set_rpm_back_left(uint16_t speed)   { wheel_rpm_back_left = speed; }
    inline void set_rpm_back_right(uint16_t speed)  { wheel_rpm_back_right = speed; }

private:
    uint16_t wheel_rpm_front_left;
    uint16_t wheel_rpm_front_right;
    uint16_t wheel_rpm_back_left;
    uint16_t wheel_rpm_back_right;   
};

#pragma pack(pop)