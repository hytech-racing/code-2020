#include "Constants.h"
extern int8_t ignore_cell_therm[TOTAL_IC][THERMISTORS_PER_IC];

namespace printers {
    namespace { bool error_flags_history = 0; }
    void print_temps();
    void print_cells();
    void print_current();
    void print_aux();
    void print_uptime();
    void print_bms_status();
    void print_all();
}

void printers::print_all() {
    print_temps(); // Print cell and pcb temperatures to serial
    print_cells(); // Print the cell voltages and balancing status to serial
    print_current(); // Print measured current sensor value
    print_uptime(); // Print the BMS uptime to serial
    print_bms_status();
}

void printers::print_temps() {
    Serial.println("------------------------------------------------------------------------------------------------------------------------------------------------------------");
    Serial.println("\t\tCell Temperatures\t\t\t\t\t\t\t\t\t   PCB Temperatures");
    Serial.println("\tTHERM 0\t\tTHERM 1\t\tTHERM 2\t\t\t\t\t\t\t\tTHERM 0\t\tTHERM 1");
    for (int ic = 0; ic < TOTAL_IC; ic++) {
        Serial.print("IC"); Serial.print(ic); Serial.print("\t");
        for (int therm = 0; therm < THERMISTORS_PER_IC; therm++) {
            if (!ignore_cell_therm[ic][therm]) {
                double temp = ((double) bms_detailed_temperatures[ic].get_temperature(therm)) / 100;
                Serial.print(temp, 2);
                Serial.print(" ºC");
            } else {
                Serial.print("IGN\t");
            }
            Serial.print("\t");
        }
        Serial.print("\t\t\t\t\t\t");
        for (int therm = 0; therm < PCB_THERM_PER_IC; therm++) {
            if (!ignore_pcb_therm[ic][therm]) {
                double temp = ((double) bms_onboard_detailed_temperatures[ic].get_temperature(therm)) / 100;
                Serial.print(temp, 2);
                Serial.print(" ºC");
            } else {
                Serial.print("IGN\t");
            }
            Serial.print("\t");
        }
        Serial.println();
    }
    Serial.print("\nCell temperature statistics\t\t Average: ");
    Serial.print(bms_temperatures.get_average_temperature() / (double) 100, 2);
    Serial.print(" ºC\t\t");
    Serial.print("Min: ");
    Serial.print(bms_temperatures.get_low_temperature() / (double) 100, 2);
    Serial.print(" ºC\t\t");
    Serial.print("Max: ");
    Serial.print(bms_temperatures.get_high_temperature() / (double) 100, 2);
    Serial.println(" ºC\n");
    Serial.print("PCB temperature statistics\t\t Average: ");
    Serial.print(bms_onboard_temperatures.get_average_temperature() / (double) 100, 2);
    Serial.print(" ºC\t\t");
    Serial.print("Min: ");
    Serial.print(bms_onboard_temperatures.get_low_temperature() / (double) 100, 2);
    Serial.print(" ºC\t\t");
    Serial.print("Max: ");
    Serial.print(bms_onboard_temperatures.get_high_temperature() / (double) 100, 2);
    Serial.println(" ºC\n");
}

void printers::print_cells() {
    Serial.println("------------------------------------------------------------------------------------------------------------------------------------------------------------");
    Serial.println("\t\t\t\tRaw Cell Voltages\t\t\t\t\t\t\tCell Status (Ignoring or Balancing)");
    Serial.println("\tC0\tC1\tC2\tC3\tC4\tC5\tC6\tC7\tC8\t\tC0\tC1\tC2\tC3\tC4\tC5\tC6\tC7\tC8");
    for (int ic = 0; ic < TOTAL_IC; ic++) {
        Serial.print("IC"); Serial.print(ic); Serial.print("\t");
        for (int cell = 0; cell < CELLS_PER_IC; cell++) {
            int ignored = ignore_cell[ic][cell];
            if (!MODE_BENCH_TEST || !ignored) { // Don't clutter test bench UI with ignored cells
                double voltage = cell_voltages[ic][cell] * 0.0001;
                Serial.print(voltage, 4); Serial.print("V\t");
            } else {
                Serial.print("\t");
            }
        }
        Serial.print("\t");
        for (int cell = 0; cell < CELLS_PER_IC; cell++) {
            int balancing = bms_balancing_status[ic / 4].get_cell_balancing(ic % 4, cell);
            int ignored = ignore_cell[ic][cell];
            if (balancing) {
                Serial.print("BAL");
            } else if (ignored) {
                Serial.print("IGN");
            }
            Serial.print("\t");
        }
        Serial.println();
    }
    Serial.println();
    Serial.println("\t\t\t\tDelta from Min Cell");
    Serial.println("\tC0\tC1\tC2\tC3\tC4\tC5\tC6\tC7\tC8");
    for (int ic = 0; ic < TOTAL_IC; ic++) {
        Serial.print("IC"); Serial.print(ic); Serial.print("\t");
        for (int cell = 0; cell < CELLS_PER_IC; cell++) {
            if (!ignore_cell[ic][cell]) {
                double voltage = (cell_voltages[ic][cell]-bms_voltages.get_low()) * 0.0001;
                Serial.print(voltage, 4);
                Serial.print("V");
            }
            Serial.print("\t");
        }
        Serial.println();
    }
    Serial.println();
    Serial.print("Cell voltage statistics\t\tTotal: "); Serial.print(bms_voltages.get_total() / (double) 1e2, 4); Serial.print("V\t\t");
    Serial.print("Average: "); Serial.print(bms_voltages.get_average() / (double) 1e4, 4); Serial.print("V\t\t");
    Serial.print("Min: "); Serial.print(bms_voltages.get_low() / (double) 1e4, 4); Serial.print("V\t\t");
    Serial.print("Max: "); Serial.print(bms_voltages.get_high() / (double) 1e4, 4); Serial.println("V");
}

void printers::print_current() {
    Serial.print("\nCurrent Sensor: ");
    Serial.print(bms_status.get_current() / (double) 100, 2);
    Serial.println("A");
}

void printers::print_aux() {
    for (int current_ic = 0; current_ic < TOTAL_IC; current_ic++) {
        Serial.print("IC: ");
        Serial.println(current_ic);
        for (int i = 0; i < 6; i++) {
            Serial.print("Aux-"); Serial.print(i+1); Serial.print(": ");
            float voltage = aux_voltages[current_ic][i] * 0.0001;
            Serial.println(voltage, 4);
        }
        Serial.println();
    }
}

/*
 * Print ECU uptime
 */
void printers::print_uptime() {
    Serial.print("\nECU uptime: ");
    Serial.print(millis() / 1000);
    Serial.print(" seconds (");
    Serial.print(millis() / 1000 / 60);
    Serial.print(" minutes, ");
    Serial.print(millis() / 1000 % 60);
    Serial.println(" seconds)\n");
}

void printers::print_bms_status() {
    Serial.print("State: ");
    switch(bms_status.get_state()) {
        case BMS_STATE_DISCHARGING: Serial.println("DISCHARGING"); break;
        case BMS_STATE_CHARGING: Serial.println("CHARGING"); break;
        case BMS_STATE_BALANCING: Serial.println("BALANCING"); break;
        case BMS_STATE_BALANCING_OVERHEATED: Serial.println("BALANCING_OVERHEATED"); break;
    }

    if (bms_status.get_error_flags()) { // BMS error - drive BMS_OK signal low
        error_flags_history |= bms_status.get_error_flags();
        digitalWrite(BMS_OK, LOW);
        Serial.print("---------- STATUS NOT GOOD * Error Code 0x");
        Serial.print(bms_status.get_error_flags(), HEX);
        Serial.println(" ----------");
    } else {
        digitalWrite(BMS_OK, HIGH);
        Serial.println("---------- STATUS GOOD ----------");
        if (error_flags_history) {
            Serial.println("An Error Occured But Has Been Cleared");
            Serial.print("Error code: 0x");
            Serial.println(error_flags_history, HEX);
        }
    }
}