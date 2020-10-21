#include <Arduino.h>
#include <Simulator.h>
#include <HyTech_FlexCAN.h>
#include <stdio.h>

int main(int argc, char* argv []) {
    printf("-1\n");
    Simulator simulator (100);
    printf("A\n");
    // int reading = 1;
    // while (millis() < 1000) {
    //     io[4].setValue(reading++);
    // }
    printf("B\n");
    CAN_message_t msg;

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
    CAN_simulator::push(msg);
    msg.buf[0] = 'H';
    CAN_simulator::push(msg);

    simulator.next();

    while (CAN_simulator::get(msg))
        printf("Received (id %d): %s\n", msg.id, msg.buf);
    // else
    //     printf("FAILED\n");
    printf("C\n");

    return 0;
}