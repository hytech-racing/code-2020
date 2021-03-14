#include <stdint.h>
#include <cstring>
#include <iostream>
#include <type_traits>

using namespace std;


#pragma pack(push,1)

class MCU_wheel_speed {
public:
    MCU_wheel_speed() = default;

    MCU_wheel_speed(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8]) {
         memcpy(&wheel_rpm_front_left, &buf[0], sizeof(wheel_rpm_front_left));
         memcpy(&wheel_rpm_front_right, &buf[2], sizeof(wheel_rpm_front_right));
         memcpy(&wheel_rpm_rear_left, &buf[4], sizeof(wheel_rpm_rear_left));
         memcpy(&wheel_rpm_rear_right, &buf[6], sizeof(wheel_rpm_rear_right));
    }

    inline void write(uint8_t buf[8]) const {
         memcpy(&buf[0], &wheel_rpm_front_left, sizeof(wheel_rpm_front_left));
         memcpy(&buf[2], &wheel_rpm_front_right, sizeof(wheel_rpm_front_right));
         memcpy(&buf[4], &wheel_rpm_rear_left, sizeof(wheel_rpm_rear_left));
         memcpy(&buf[6], &wheel_rpm_rear_right, sizeof(wheel_rpm_rear_right));
    }

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

int main(){
    cout << is_pod<MCU_wheel_speed>() << endl;
    cout << is_standard_layout<MCU_wheel_speed>() << endl;
    cout << is_trivial<MCU_wheel_speed>() << endl;
    cout << sizeof(MCU_wheel_speed) << endl;
}