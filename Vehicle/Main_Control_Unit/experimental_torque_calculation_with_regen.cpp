/*
int constrain(int& val, int min, int max) { 
    if (val < min) val = min; 
    else if (val > max) val = max;
}

// NOT TESTED YET - once validated, will serve as drop-in replacement for Main_Control_Unit::calculate_torque
int calculate_torque_with_regen() {
    if (mc_motor_position_information.get_motor_speed() < MIN_RPM_FOR_REGEN) {
        MAX_ACCEL_REGEN = 0;
        MAX_BRAKE_REGEN = 0;
    }

    int calculated_torque = 0;
    double lowest_cell_voltage = bms_voltages.get_low() / 10000.0;

    if (lowest_cell_voltage < TORQUE_ADJUSTMENT_VOLTAGE) { // TODO: adjust this
        MAX_TORQUE = map(lowest_cell_voltage, 0, TORQUE_ADJUSTMENT_VOLTAGE, 0, MAX_POSSIBLE_TORQUE);
    }

    int torque1 = map(round(filtered_accel1_reading), START_ACCEL1_PEDAL_WITH_REGEN, END_ACCELERATOR_PEDAL_1, MAX_ACCEL_REGEN, MAX_TORQUE);
    int torque2 = map(round(filtered_accel2_reading), START_ACCEL2_PEDAL_WITH_REGEN, END_ACCELERATOR_PEDAL_2, MAX_ACCEL_REGEN, MAX_TORQUE);
    int torque3 = map(round(filtered_brake_reading), START_BRAKE_PEDAL_WITH_REGEN, END_BRAKE_PEDAL_WITH_REGEN, 0, MAX_BRAKE_REGEN);

    constrain(torque1, MAX_ACCEL_REGEN, MAX_TORQUE);
    constrain(torque2, MAX_ACCEL_REGEN, MAX_TORQUE);
    constrain(torque3, MAX_ACCEL_REGEN, 0);

    //Serial.println((torque1 + torque2) / 2);
    // compare torques to check for accelerator implausibility
    if (0){//abs(torque1 - torque2) * 100 / MAX_TORQUE > 10) {
        mcu_pedal_readings.set_accelerator_implausibility(true);
        Serial.println("ACCEL IMPLAUSIBILITY: COMPARISON FAILED");
    } else {
        calculated_torque = (torque1 + torque2) / 2 + torque3;

        #ifdef HYTECH_LOGGING_EN
            if (timer_debug_raw_torque.check()) {
                Serial.print("TORQUE REQUEST DELTA PERCENT: "); // Print the % difference between the 2 accelerator sensor requests
                Serial.println(abs(torque1 - torque2) / (double) MAX_TORQUE * 100);
                Serial.print("MCU RAW TORQUE: ");
                Serial.println(calculated_torque);
            }
        #endif

        constrain(calculated_torque, MAX_ACCEL_REGEN + MAX_BRAKE_REGEN, MAX_TORQUE)
    }

    return calculated_torque;
}
*/