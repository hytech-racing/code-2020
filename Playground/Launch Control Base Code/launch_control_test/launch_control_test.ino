#include <ADC_SPI.h>
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <kinetis_flexcan.h>
#include <Metro.h>
#include <Wire.h>
#include <MCP23S17.h>
#include <cmath>

/*
 * Global variables
 */
MCU_launch_control mcu_launch_control;
 
// launch control variables
float rear_rpm = 0;
float front_rpm = 0;
float torque_coefficient = 1;
float max_desireable_slip_ratio = 0.2;
float last_excess_slip = 0;
float total_excess_slip = 0;
float KP = 0.15;
float KI = 0.005;
float KD = 0;
bool launch_control_active = true;
unsigned long currentTime, previousTime = 0;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  update_torque_coeff(); //use the pid to update the slip limiting factor based on current slip ratio

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

int calculate_torque() {
    int calculated_torque = 0;

    if (!mcu_pedal_readings.get_accelerator_implausibility()) {
        int torque1 = map(round(filtered_accel1_reading), START_ACCELERATOR_PEDAL_1, END_ACCELERATOR_PEDAL_1, 0, MAX_TORQUE);
        int torque2 = map(round(filtered_accel2_reading), START_ACCELERATOR_PEDAL_2, END_ACCELERATOR_PEDAL_2, 0, MAX_TORQUE);

        // torque values are greater than the max possible value, set them to max
        if (torque1 > MAX_TORQUE) {
            torque1 = MAX_TORQUE;
        }
        if (torque2 > MAX_TORQUE) {
            torque2 = MAX_TORQUE;
        }
        // compare torques to check for accelerator implausibility
        if (abs(torque1 - torque2) * 100 / MAX_TORQUE > 10) {
            mcu_pedal_readings.set_accelerator_implausibility(true);
            Serial.println("ACCEL IMPLAUSIBILITY: COMPARISON FAILED");
        } else {
            calculated_torque = (torque1 + torque2) / 2; //min(torque1, torque2);

            if (debug && timer_debug_raw_torque.check()) {
                Serial.print("TORQUE REQUEST DELTA PERCENT: "); // Print the % difference between the 2 accelerator sensor requests
                Serial.println(abs(torque1 - torque2) / (double) MAX_TORQUE * 100);
                Serial.print("MCU RAW TORQUE: ");
                Serial.println(calculated_torque);
            }
            if (calculated_torque > MAX_TORQUE) {
                calculated_torque = MAX_TORQUE;
            }
            if (calculated_torque < 0) {
                calculated_torque = 0;
            }
            calculated_torque *= torque_coeff; //Reduce torque if slip to high, cannot increase torque ie torque_coeff cannot be >1
        }
    }

    return calculated_torque;
}

//Gets excess slip at wheels 
float get_excess_slip() {
    float front_ang_vel = front_rpm * 2 * M_PI / 60;
    float rear_ang_vel = rear_rpm * 2 * M_PI / 60;
    float slip_ratio = 0; //slip ratio is 0 by default
    if(front_rpm > 10 && rear_rpm > 30) 
        slip_ratio = (rear_ang_vel - front_ang_vel) / rear_ang_vel; //if both front and rear are spinning, calculate the ratio
    mcu_launch_control.set_slip_ratio(slip_ratio * 100);
    float excess_slip = max_desireable_slip_ratio - slip_ratio;
    Serial.print("ESR: ");
    Serial.print(excess_slip);
    return excess_slip;
}

void update_torque_coeff() {
    currentTime = millis();
    elaspedTime = currentTime - previousTime;
    float excess_slip = get_excess_slip();
    total_excess_slip += excess_slip * elapsedTime;
    if (total_excess_slip < 0) total_excess_slip = 0;
    float change_in_excess_slip = (excess_slip - last_excess_slip) / elapsedTime;
    last_excess_slip = excess_slip;
    previousTime = currentTime;
    
    float P = KP * excess_slip;
    float I = KI * total_excess_slip;
    float D = KD * change_in_excess_slip;
    
    PID_out = P + I + D;
    if (PID_out > .2)
    {
      torque_coefficent = 1;
    }
    else
    {
      torque coefficient = map(PID_out, -.8*(KP + KI). 0.2, .5, 1);  
    }
    mcu_launch_control.set_slip_limiting_factor(slip_limiting_factor * 100); //Needs to be changed to match current variable torque_coeff
    Serial.print("    Torque Coeff: ");
    Serial.println(torque_coefficient);
}
