#include <stdint.h>

typedef struct CAN_message_mcu_status_t {
    uint8_t state;
    int16_t temperature;
    uint16_t glv_battery_voltage;
    uint8_t flags;
} no_pack;

#pragma pack(push, 1)

typedef struct CAN_message_mcu_status_t2 {
    uint8_t state;
    int16_t temperature;
    uint16_t glv_battery_voltage;
    uint8_t flags;
} pack;

#pragma pack(pop)

void setup(){
    Serial.begin(115200);
    no_pack none;
    pack packed;

    Serial.printf("%d | %p %p %p %p %p \n", sizeof(none), &none, &none.state, &none.flags, &none.temperature, &none.glv_battery_voltage);

    Serial.printf("%d | %p %p %p %p %p \n", sizeof(packed), &packed, &packed.state, &packed.flags, &packed.temperature, &packed.glv_battery_voltage);
    
}

void loop(){
  
}
