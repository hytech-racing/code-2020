#include <Arduino.h>
#include <HyTech_FlexCAN.h>

FlexCAN CAN(500000);

int inputPin = 5;

// the setup routine runs once when you press reset:
void setup() {
    // initialize the digital pin as an output.
    Serial.begin(512000);
    pinMode(inputPin, INPUT_PULLUP);
  
//   interrupts();
    FLEXCAN0_MCR &= 0xFFFDFFFF; // Enable self-reception
    CAN.begin();
}

// the loop routine runs over and over again forever:
void loop() {
    Serial.print(millis());
    Serial.print(" ");
    Serial.println(analogRead(inputPin));   // turn the LED on (HIGH is the voltage level)

    CAN_message_t msg;

    while (CAN.read(msg)) {
        Serial.print("Received (id");
        Serial.print(msg.id);
        Serial.print("): ");
        Serial.println(msg.buf);
    }

    msg.id = 0x1;
    msg.len = 8;
    msg.buf[0] = 'A';
    msg.buf[1] = 'B';
    msg.buf[2] = 'C';
    msg.buf[3] = 'D';
    msg.buf[4] = 'E';
    msg.buf[5] = 'F';
    msg.buf[6] = 'G';
    msg.buf[7] = '\0';
    Serial.print(CAN.write(msg));
    msg.buf[0] = 'H';
    Serial.print(CAN.write(msg));
}