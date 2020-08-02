uint16_t aux_voltages[TOTAL_IC][6]; // contains auxiliary pin voltages for each IC in this order: [Cell Term 1] [Cell Therm 2] [Cell Therm 3] [Onboard Therm 1] [Onboard Therm 2] [Voltage reference]

void poll_cell_voltages() {
    wakeup_sleep(); // Wake up LTC6804 ADC core
    LTC6804_adcv(); // Start cell ADC conversion
    delay(202); // Need to wait at least 201.317ms for conversion to finish, due to filtered sampling mode (26Hz) - See LTC6804 Datasheet Table 5
    wakeup_idle(); // Wake up isoSPI
    delayMicroseconds(1200); // Wait 4*t_wake for wakeup command to propogate and all 4 chips to wake up - See LTC6804 Datasheet page 54
    uint8_t error = LTC6804_rdcv(0, TOTAL_IC, cell_voltages); // Reads voltages from ADC registers and stores in cell_voltages.
    if (error == -1) {
        Serial.println("A PEC error was detected in cell voltage data");
    }
    // Move C7-C10 down by one in the array, skipping C6. This abstracts the missing cell from the rest of the program.
    for (int i=0; i<TOTAL_IC; i++) // Loop through ICs
        for (int j=6; j<10; j++) // Loop through C7-C10
            cell_voltages[i][j-1] = cell_voltages[i][j];
}


void poll_aux_voltages() {
    wakeup_sleep();
    //delayMicroseconds(200) // TODO try this if we are still having intermittent 6.5535 issues, maybe the last ADC isn't being given enough time to wake up
    LTC6804_adax(); // Start GPIO ADC conversion
    delay(202); // Need to wait at least 201.317ms for conversion to finish, due to filtered sampling mode (26Hz) - See LTC6804 Datasheet Table 5
    wakeup_idle(); // Wake up isoSPI
    delayMicroseconds(1200); // Wait 4*t_wake for wakeup command to propogate and all 4 chips to wake up - See LTC6804 Datasheet page 54
    uint8_t error = LTC6804_rdaux(0, TOTAL_IC, aux_voltages);
    if (error == -1) 
        Serial.println("A PEC error was detected in auxiliary voltage data");
    // print_aux();
    delay(200);
}