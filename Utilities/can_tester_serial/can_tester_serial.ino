#include <kinetis_flexcan.h>
#include <HyTech_FlexCAN.h>

FlexCAN CANbus(125000,0,0,0);
CAN_message_t rxmsg;

void setup() {
  CANbus.begin();
  Serial.begin(115200);
}

void loop() {
  if (CANbus.available())
  {
    CANbus.read(rxmsg);
    Serial.print("Message ID: ");
    Serial.println(rxmsg.id);
    Serial.print("Message: ");
    int i;
    for (i = 0; i < rxmsg.len; i++)
    {
      Serial.print(rxmsg.buf[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}
