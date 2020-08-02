#pragma once
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>

class HT_CAN_Util : public FlexCAN {
public:
    HT_CAN_Util(uint32_t baud = 125000) : FlexCAN(baud) {}
    
    void write(uint32_t id, Interface_CAN_Container* can) {
        tx_msg.id = id;
        tx_msg.len = sizeof(can->messageLen());
        can->write(tx_msg.buf);
        FlexCAN::write(tx_msg);
    }

    void write(uint32_t id, Interface_CAN_Container* can, int iterations) {
        tx_msg.id = id;
        tx_msg.len = can->messageLen();
        while(iterations--) {
            can->write(tx_msg.buf);
            FlexCAN::write(tx_msg);
        }
    }

    void set_timeout(uint16_t timeout) { tx_msg.timeout = timeout; }
private:
    CAN_message_t tx_msg;
};