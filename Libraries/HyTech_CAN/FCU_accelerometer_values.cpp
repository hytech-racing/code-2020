/*
 * FCU_accelerometer_values.cpp - retrieving and packaging accelerometer values via CAN
 * Created by Soohyun Kim, July 12, 2018.
 */

#include "HyTech_CAN.h"

FCU_accelerometer_values::FCU_accelerometer_values() : Abstract_CAN_Container() {};
FCU_accelerometer_values::FCU_accelerometer_values(uint8_t buf []) : Abstract_CAN_Container(buf) {};

uint8_t FCU_accelerometer_values::get_x() {
   return message.XValue_x100;
}

uint8_t FCU_accelerometer_values::get_y() {
   return message.YValue_x100;
}

uint8_t FCU_accelerometer_values::get_z() {
   return message.ZValue_x100;
}

void FCU_accelerometer_values::set_values(uint8_t x, uint8_t y, uint8_t z) {
   message.XValue_x100 = x;
   message.YValue_x100 = y;
   message.ZValue_x100 = z;
}

#ifdef HTECH_LOGGING_EN
   void FCU_accelerometer_values::print(Stream& serial) {
      serial.print("\n\nACCELEROMETER DATA\n\n");
      serial.print(get_x() / 100.); serial.print(", ");
      serial.print(get_y() / 100.); serial.print(", ");
      serial.print(get_z() / 100.); serial.println("\n\n");
      
   }
#endif