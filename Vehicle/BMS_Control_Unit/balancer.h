#include <DebouncedButton.h>
#include <PackedValues.h>

extern BMS_voltages bms_voltages;
extern int8_t ignore_cell[TOTAL_IC][CELLS_PER_IC];
extern uint16_t cell_voltages[TOTAL_IC][12];

uint8_t balance_offcycle = 0; // Tracks which loops balancing will be disabled on
bool charge_mode_entered = false; // Used to enter charge mode immediately at startup instead of waiting for timer
Metro timer_charge_enable_limit = Metro(30000, 1); // Don't allow charger to re-enable more than once every 30 seconds
BMS_balancing_status bms_balancing_status[(TOTAL_IC + 3) / 4];

namespace Balancer {
    void modify_discharge_config(int ic, int cell, bool setDischarge);
    void stop_discharge_all(bool skip_clearing_status = false);
    void balance_cells();
    bool can_start_charging();
}

void Balancer::modify_discharge_config(int ic, int cell, bool setDischarge) { // TODO unify language about "balancing" vs "discharging"
    if (ic < TOTAL_IC && cell < CELLS_PER_IC) {
        bms_balancing_status[ic / 4].set_cell_balancing(ic % 4, cell, setDischarge);
        if (cell > 4) 
            cell++; // Increment cell, skipping the disconnected C5. This abstracts the missing cell from the rest of the program.
        if (cell < 8) 
            tx_cfg[ic][4] = setDischarge ? PackedValues::set(tx_cfg[ic][4], cell) : PackedValues::unset(tx_cfg[ic][4], cell);
        else
            tx_cfg[ic][5] = setDischarge ? PackedValues::set(tx_cfg[ic][5], cell - 8) : PackedValues::unset(tx_cfg[ic][5], cell - 8);
    }
}

void Balancer::stop_discharge_all(bool skip_clearing_status) {
    for (int i = 0; i < TOTAL_IC; i++) {
        if (!skip_clearing_status) // Optionally leave bms_balancing_status alone - useful if only temporarily disabling balancing
            bms_balancing_status[i / 4].set_ic_balancing(i % 4, 0x0);
        tx_cfg[i][4] = 0b0;
        tx_cfg[i][5] = 0b0;
    }
    wakeup_idle();
    //delayMicroseconds(1200); // Wait 4*t_wake for wakeup command to propogate and all 4 chips to wake up - See LTC6804 Datasheet page 54
    LTC6804_wrcfg(TOTAL_IC, tx_cfg);
}

void Balancer::balance_cells() {
    balance_offcycle = (balance_offcycle + 1) % BALANCE_LIMIT_FACTOR; // Only allow balancing on 1/BALANCE_LIMIT_FACTOR cycles
    bool cells_balancing = false; // This gets set to true later if it turns out we are balancing any cells this loop

    if (bms_voltages.get_low() > VOLTAGE_CUTOFF_LOW // TODO technically this could keep a widely spread out pack from ever balancing
        && !bms_status.get_error_flags()
        && bms_status.get_state() >= BMS_STATE_CHARGING
        && bms_status.get_state() <= BMS_STATE_BALANCING
        && (bms_status.get_shutdown_h_above_threshold() || MODE_CHARGE_OVERRIDE || MODE_ADC_IGNORE)) { // Don't check shutdown circuit if in Charge Override Mode or ADC Ignore Mode
        if (balance_offcycle) // One last check - are we in an off-cycle? If so, we want to disable balancing but preserve bmc_balancing_status
            stop_discharge_all(true); // Stop all cells from discharging, but don't clear bms_balancing_status - this way we can view what will shortly be balancing again
        else { // Proceed with balancing
            for (int ic = 0; ic < TOTAL_IC; ic++) { // Loop through ICs
                for (int cell = 0; cell < CELLS_PER_IC; cell++) { // Loop through cells
                    if (!ignore_cell[ic][cell]) { // Ignore any cells specified in ignore_cell
                        uint16_t cell_voltage = cell_voltages[ic][cell]; // current cell voltage in mV
                        if (cell_voltage < bms_voltages.get_low() + VOLTAGE_DIFFERENCE_THRESHOLD) {
                            modify_discharge_config(ic, cell, false); // Modify our local version of the discharge configuration
                        } else if (cell_voltage > bms_voltages.get_low() + VOLTAGE_DIFFERENCE_THRESHOLD) {
                            modify_discharge_config(ic, cell, true); // Modify our local version of the discharge configuration
                            cells_balancing = true;
                        }
                    }
                }
            }
            if (cells_balancing) // Cells currently balancing
                bms_status.set_state(BMS_STATE_BALANCING);
            else if (timer_charge_enable_limit.check()) // Balancing allowed, but no cells currently balancing
                bms_status.set_state(BMS_STATE_CHARGING);
            wakeup_idle();
            LTC6804_wrcfg(TOTAL_IC, tx_cfg); // Write the new discharge configuration to the LTC6804s
        }
    } else {
        stop_discharge_all(); // Make sure none of the cells are discharging
    }
}

bool Balancer::can_start_charging() {
    if (timer_charge_enable_limit.check() || !charge_mode_entered) {
        charge_mode_entered = true;
        return true;
    }
    return false;
}