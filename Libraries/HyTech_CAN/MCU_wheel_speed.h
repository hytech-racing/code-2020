#ifndef __MCU_WHEEL_SPEED_H__
#define __MCU_WHEEL_SPEED_H__

#include <cstdint>
#include <cstring>

#pragma pack(push,1)

class MCU_wheel_speed {
public:
    MCU_wheel_speed() = default;

    MCU_wheel_speed(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8]) { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[8]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t getRpmFrontLeft()  const { return wheel_rpm_front_left; }
    inline uint16_t getRpmFrontRight() const { return wheel_rpm_front_right; }
    inline uint16_t getRpmRearLeft()   const { return wheel_rpm_rear_left; }
    inline uint16_t getRpmRearRight()  const { return wheel_rpm_rear_right; }

    // Setters
    inline void setRpmFrontLeft(uint16_t speed)  { wheel_rpm_front_left = speed; }
    inline void setRpmFrontRight(uint16_t speed) { wheel_rpm_front_right = speed; }
    inline void setRpmRearLeft(uint16_t speed)   { wheel_rpm_rear_left = speed; }
    inline void setRpmRearRight(uint16_t speed)  { wheel_rpm_rear_right = speed; }

private:
    uint16_t wheel_rpm_front_left;
    uint16_t wheel_rpm_front_right;
    uint16_t wheel_rpm_rear_left;
    uint16_t wheel_rpm_rear_right;   
};

#pragma pack(pop)

#endif