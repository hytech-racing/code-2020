#include <cmath>

int RPM = 0, front_rpm = 0, rear_rpm = 0;
float optimal_slip_ratio = 0.7;

void loop() {
  // put your main code here, to run repeatedly:
  update_RPM(); //use the pid to update the slip limiting factor based on current slip ratio

  if (timer_can_update.check()) {
    // Send launch control information
        mcu_launch_control.write(tx_msg.buf);
        tx_msg.id = ID_MCU_LAUNCH_CONTROL;
        tx_msg.len = sizeof(CAN_message_mcu_launch_control_t);
        CAN.write(tx_msg);
  }

}

void parse_can_message() {
  while (CAN.read(rx_msg)) {
    if (rx_msg.id == ID_TCU_WHEEL_RPM_REAR) {
      TCU_wheel_rpm rpms = TCU_wheel_rpm(rx_msg.buf);
      rear_rpm = (rpms.get_wheel_rpm_left() + rpms.get_wheel_rpm_right()) / (2.0 * 100);
    }
    if (rx_msg.id == ID_TCU_WHEEL_RPM_FRONT) {
      TCU_wheel_rpm rpms = TCU_wheel_rpm(rx_msg.buf);
      front_rpm = (rpms.get_wheel_rpm_left() + rpms.get_wheel_rpm_right()) / (1.0 * 100); //Should be devided by 2, currently only one sensor is installed
    }
  }
}

void update_RPM() {
  int optimal_rpm = front_rpm / (1 - optimal_slip_ratio); 
  // send optimal RPM to motor via CAN
}
