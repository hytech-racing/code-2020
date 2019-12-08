#include "HyTech_CAN.h"

TEL_stored_soc::TEL_stored_soc() {
    message = {};
}

TEL_stored_soc::TEL_stored_soc(uint8_t buf[]) {
    load(buf);
}

TEL_stored_soc::TEL_stored_soc(uint32_t total_charge) {
    set_total_charge(total_charge);
}

void TEL_stored_soc::load(uint8_t buf[]) {
    message = {};
    memcpy(&(message.total_charge), &buf[0], sizeof(uint32_t));
}

void TEL_stored_soc::write(uint8_t buf[]) {
    memcpy(&buf[0], &(message.total_charge), sizeof(uint32_t));
}

uint32_t TEL_stored_soc::get_total_charge() {
    return message.total_charge;
}


void TEL_stored_soc::set_total_charge(uint32_t total_charge) {
    message.total_charge = total_charge;
}
