
#include "mcp_can.h"



void MCP_CAN::begin(int pin) {
    io[pin].~MockPin();
}
unsigned char MCP_CAN::checkReceive(int pin) {
    if(io[pin].value() != NULL) {
        return CAN_OK;
    }
    else {
        return CAN_FAILINIT;
    }
}
void MCP_CAN::setMsg(unsigned long id, unsigned char ext, unsigned char len, unsigned char *pData)
{
    ext_flg = ext;
    can_id  = id;
    dta_len  = len < MAX_CHAR_IN_MESSAGE ? len : MAX_CHAR_IN_MESSAGE;
    for(int i = 0; i<dta_len; i++)
    {
        dta[i] = *(pData+i);
    }
}
void MCP_CAN::sendMsgBuff(int pin,unsigned long id, unsigned char ext, unsigned char len, unsigned char *buf) {
    setMsg(id, ext, len, buf);
    io[pin].write(*buf);
}
unsigned char MCP_CAN::readMsgBuf(int pin) {
    return io[pin].value();
}
unsigned long MCP_CAN::getCanID() {
    return can_id;
}