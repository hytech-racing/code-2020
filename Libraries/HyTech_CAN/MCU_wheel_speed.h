#ifndef __MCU_WHEEL_SPEED_H__
#define __MCU_WHEEL_SPEED_H__

#include <cstdint>
#include <string>

#pragma pack(push,1)

typedef struct CAN_message_mcu_wheel_rpm_t {
    uint16_t wheel_rpm_front_left;
    uint16_t wheel_rpm_front_right;
    uint16_t wheel_rpm_rear_left;
    uint16_t wheel_rpm_rear_right;
} CAN_message_mcu_wheel_rpm_t;

#pragma pack(pop)

class MCU_wheel_speed {
public:
    MCU_wheel_speed() = default;

    MCU_wheel_speed(uint8_t buf[8]) { load(buf); }

    inline void load(uint8_t buf[8]) {
        message = {};
        memcpy(&message, buf, sizeof(message));
    }

    inline void write(uint8_t buf[8]) const {
        memcpy(buf, &message, sizeof(message));
    }

    // Getters
    inline uint16_t getRpmFrontLeft()  const { return message.wheel_rpm_front_left; }
    inline uint16_t getRpmFrontRight() const { return message.wheel_rpm_front_right; }
    inline uint16_t getRpmRearLeft()   const { return message.wheel_rpm_rear_left; }
    inline uint16_t getRpmRearRight()  const { return message.wheel_rpm_rear_right; }

    // Setters
    inline void setRpmFrontLeft(uint16_t speed)  { message.wheel_rpm_front_left = speed; }
    inline void setRpmFrontRight(uint16_t speed) { message.wheel_rpm_front_right = speed; }
    inline void setRpmRearLeft(uint16_t speed)   { message.wheel_rpm_rear_left = speed; }
    inline void setRpmRearRight(uint16_t speed)  { message.wheel_rpm_rear_right = speed; }

private:
    CAN_message_mcu_wheel_rpm_t message;    
};

#endif