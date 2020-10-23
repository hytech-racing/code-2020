#include <cstdint>
#include <cstdio>

using namespace std;

typedef struct CAN_message_mcu_status_t {
    uint8_t state;
    uint8_t flags;
    int16_t temperature;
    uint16_t glv_battery_voltage;
} no_pack;

#pragma pack(push, 128)

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

    printf("%d | %p %p %p %p %p \n", sizeof(none), &none, &none.state, &none.flags, &none.temperature, &none.glv_battery_voltage);

    printf("%d | %p %p %p %p %p \n", sizeof(packed), &packed, &packed.state, &packed.flags, &packed.temperature, &packed.glv_battery_voltage);
    
}