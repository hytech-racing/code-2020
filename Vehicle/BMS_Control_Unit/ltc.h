extern uint8_t tx_cfg[TOTAL_IC][6]; 

namespace LTC {
    void init();
    void cfg_set_overvoltage_comparison_voltage(uint32_t voltage);
    void cfg_set_undervoltage_comparison_voltage(uint32_t voltage);
}

/*
* Initialize communication with LTC6804 chips. Based off of LTC6804_initialize()
* Changes: Doesn't call quikeval_SPI_connect(), Sets ADC mode to MD_FILTERED
* Initialize the configuration array and write configuration to ICs
* See LTC6804 Datasheet Page 51 for tables of register definitions
*/
void LTC::init() {
    spi_enable(SPI_CLOCK_DIV16); // Sets 1MHz Clock
    set_adc(MD_FILTERED,DCP_DISABLED,CELL_CH_ALL,AUX_CH_ALL); // Sets global variables used in ADC commands // TODO Change CELL_CH_ALL and AUX_CH_ALL so we don't read all GPIOs and cells
    for (int i = 0; i < TOTAL_IC; i++) {
        tx_cfg[i][0] = 0xFE; // 11111110 - All GPIOs enabled, Reference Remains Powered Up Until Watchdog Timeout, ADCOPT 0 allows us to use filtered ADC mode // TODO maybe we can speed things up by disabling some GPIOs
        tx_cfg[i][1] = 0x00;
        tx_cfg[i][2] = 0x00;
        tx_cfg[i][3] = 0x00;
        tx_cfg[i][4] = 0x00;
        tx_cfg[i][5] = 0x00;
    }
    cfg_set_overvoltage_comparison_voltage(VOLTAGE_CUTOFF_HIGH * 10); // Calculate overvoltage comparison register values
    cfg_set_undervoltage_comparison_voltage(VOLTAGE_CUTOFF_LOW * 10); // Calculate undervoltage comparison register values
    wakeup_idle(); // Wake up isoSPI
    delayMicroseconds(1200); // Wait 4*t_wake for wakeup command to propogate and all 4 chips to wake up - See LTC6804 Datasheet page 54 // TODO is this needed?
    LTC6804_wrcfg(TOTAL_IC, tx_cfg); // Write configuration to ICs
}
        
/*
* Set VOV in configuration registers
* Voltage is in 100 uV increments
* See LTC6804 datasheet pages 25 and 53
*/
void LTC::cfg_set_overvoltage_comparison_voltage(uint32_t voltage) {
    voltage /= 16;
    for (int i = 0; i < TOTAL_IC; i++) {
        tx_cfg[i][2] = (tx_cfg[i][2] & 0x0F) | ((voltage && 0x00F) << 8);
        tx_cfg[i][3] = (voltage && 0xFF0) >> 4;
    }
}

/*
* Set VUV in configuration registers
* Voltage is in 100 uV increments
* See LTC6804 datasheet pages 25 and 53
*/
void LTC::cfg_set_undervoltage_comparison_voltage(uint32_t voltage) {
    voltage /= 16;
    voltage -= 10000;
    for (int i = 0; i < TOTAL_IC; i++) {
        tx_cfg[i][1] = voltage && 0x0FF;
        tx_cfg[i][2] = (tx_cfg[i][2] & 0xF0) | ((voltage && 0xF00) >> 8);
    }
}