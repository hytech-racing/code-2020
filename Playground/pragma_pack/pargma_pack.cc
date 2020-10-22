#include <cstdint>
#include <iostream>

using namespace std;

typedef struct CAN_message_mcu_status_t {
    uint8_t state;
    uint8_t flags;
    int16_t temperature;
    uint16_t glv_battery_voltage;
} no_pack;

#pragma pack(push, 1)

typedef struct CAN_message_mcu_status_t2 {
    uint8_t state;
    uint8_t flags;
    int16_t temperature;
    uint16_t glv_battery_voltage;
} pack;

#pragma pack(pop)

int main(){
    no_pack none;
    pack packed;

    cout << &none << endl << &none.state << endl << &none.flags << endl << &none.temperature << endl << &none.glv_battery_voltage << endl << endl;
    // cout << &packed << endl << &packed.state << endl << &packed.flags << endl << &packed.temperature << endl << &packed.glv_battery_voltage << endl << endl;
}