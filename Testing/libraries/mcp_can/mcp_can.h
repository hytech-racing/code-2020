#pragma once
#include <Simulator.h>



#ifndef MCP_CAN_H
#define DATE_H
#define MAX_CHAR_IN_MESSAGE 8
#define CAN_OK (0)
#define CAN_FAILINIT (1)
extern MockPin io[NUM_PINS];

class MCP_CAN {
private:
    unsigned long can_id;
    unsigned char ext_flg;
    unsigned long dta_len;
    unsigned long dta[MAX_CHAR_IN_MESSAGE];
public:
    void begin(int pin);
    unsigned char checkReceive(int pin);
    unsigned long getCanID();
    void setMsg(unsigned long id, unsigned char ext, unsigned char len, unsigned char *pData);
    void sendMsgBuff(int pin,unsigned long id, unsigned char ext, unsigned char len,  unsigned char *buf);
    unsigned char readMsgBuf(int pin);
};
#endif