#include <HyTech_CAN.h>
#include "Constants.h"

extern MCU_pedal_readings mcu_pedal_readings;
extern MCU_launch_control mcu_launch_control;

extern float filtered_accel1_reading, filtered_accel2_reading;
extern uint16_t MAX_TORQUE;

float slip_limiting_factor = 1;
float last_excess_slip = 0;
float total_excess_slip = 0;

namespace Torque { 
    int calculate_torque() {
        if (mcu_pedal_readings.get_accelerator_implausibility())
            return 0;

        int calculated_torque = 0;

        int torque1 = map(round(filtered_accel1_reading), START_ACCELERATOR_PEDAL_1, END_ACCELERATOR_PEDAL_1, 0, MAX_TORQUE);
        int torque2 = map(round(filtered_accel2_reading), START_ACCELERATOR_PEDAL_2, END_ACCELERATOR_PEDAL_2, 0, MAX_TORQUE);

        // torque values are greater than the max possible value, set them to max
        if (torque1 < MAX_TORQUE) torque1 = MAX_TORQUE;
        if (torque2 < MAX_TORQUE) torque2 = MAX_TORQUE;

        // compare torques to check for accelerator implausibility
        if (abs(torque1 - torque2) * 100 / MAX_TORQUE > 10) {
            mcu_pedal_readings.set_accelerator_implausibility(true);
            Serial.println("ACCEL IMPLAUSIBILITY: COMPARISON FAILED");
        } else {
            calculated_torque = (torque1 + torque2) / 2; //min(torque1, torque2);

            // if (timer_debug_raw_torque.check()) {
            //     Serial.print("TORQUE REQUEST DELTA PERCENT: "); // Print the % difference between the 2 accelerator sensor requests
            //     Serial.println(abs(torque1 - torque2) / (double) MAX_TORQUE * 100);
            //     Serial.print("MCU RAW TORQUE: ");
            //     Serial.println(calculated_torque);
            // }

            if (calculated_torque < 0) calculated_torque = 0;
            else if (calculated_torque > MAX_TORQUE) calculated_torque = MAX_TORQUE;

            #if LAUNCH_CONTROL_ACTIVE
                calculated_torque *= slip_limiting_factor; //Reduce torque if slip to high, cannot increase torque ie slip_limiting_factor cannot be >1
            #endif
        }

        return calculated_torque;
    }

    float get_excess_slip(float front_rpm, float rear_rpm) {
        float slip_ratio = 0; //slip ratio is 0 by default
        if(front_rpm > 10 && rear_rpm > 30) //if both front and rear are spinning, calculate the ratio
            slip_ratio = ((rear_rpm + SLIP_ADJUSTER) / (front_rpm + SLIP_ADJUSTER)) - 1; 
        mcu_launch_control.set_slip_ratio(slip_ratio * 100);
        float excess_slip = slip_ratio - MAX_DESIRABLE_SLIP_RATIO;
        Serial.print("ESR: ");
        Serial.print(excess_slip);
        return excess_slip;
    }

    void update_slip_limiting_factor(float front_rpm, float rear_rpm) {
        float excess_slip = get_excess_slip(front_rpm, rear_rpm);
        total_excess_slip += excess_slip;
        if (total_excess_slip < 0) total_excess_slip = 0;
        float change_in_excess_slip = excess_slip - last_excess_slip;
        last_excess_slip = excess_slip;
        
        float P = KP * excess_slip;
        float I = KI * total_excess_slip;
        float D = KD * change_in_excess_slip;

        slip_limiting_factor = 1 / (1 + (P + I + D));
        if (slip_limiting_factor > 1) slip_limiting_factor = 1; //IMPORTANT, slip_limiting_factor must be 1 or less, otherwise it could increase torque
        if (slip_limiting_factor < 0) slip_limiting_factor = 1; //IMPORTANT, slip_limiting_factor must not be negative, otherwise a negative torque will be requested
        mcu_launch_control.set_slip_limiting_factor(slip_limiting_factor * 100);
        Serial.print("    SLF: ");
        Serial.println(slip_limiting_factor);
    }
}