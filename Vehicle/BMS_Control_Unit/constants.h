
/*************************************
 * Begin general configuration
 *************************************/

/*
 * Set Board Version
 * Uncomment whichever board this code is being uploaded to
 * Used to set pins correctly and only enable features compatible with board
 */
#define BOARD_VERSION_HYTECH_2019_HV_REV_11

/*
 * Set Accumulator Version
 * If installing in an Accumulator, set the version here for BMS to ignore problematic sensor readings unique to each accumulator
 */
//#define ACCUMULATOR_VERSION_HYTECH_2018_ACCUMULATOR
#define ACCUMULATOR_VERSION_HYTECH_2019_ACCUMULATOR

/*
 * Set Bench Test Mode
 * Set to true to place BMS in Bench Test Mode, set to false to disable
 * When in Bench Test Mode, the BMS will automatically ignore ICs that did not respond at startup
 */
#define MODE_BENCH_TEST false

/*
 * Set Charge Override Mode
 * Set to true to place BMS in Charge Override Mode, set to false to disable
 * When the BMS is in Charge Override Mode, it will balance without checking for the presence of the Charger ECU on CAN bus or a high signal on the Shutdown Circuit
 * This mode is useful when bench testing
 */
#define MODE_CHARGE_OVERRIDE false

/*
 * Set ADC Ignore Mode
 * Set to true to place BMS in ADC Ignore Mode, set to false to disable
 * When the BMS is in ADC Ignore Mode, it will not use data received from the ADC for determining faults, or for restricting cell balancing
 */
#define MODE_ADC_IGNORE true

/*************************************
 * End general configuration
 *************************************/

// Pin definitions
#ifdef BOARD_VERSION_HYTECH_2019_HV_REV_11 // 2019 HV Board rev11
#define ADC_CS 9
#define BMS_OK A1
#define LED_STATUS 7
#define LTC6820_CS 10
#define WATCHDOG A0
#endif

// Constant definitions
#define TOTAL_IC 8                      // Number of ICs in the system
#define CELLS_PER_IC 9                  // Number of cells per IC
#define THERMISTORS_PER_IC 3            // Number of cell thermistors per IC
#define PCB_THERM_PER_IC 2              // Number of PCB thermistors per IC
#define IGNORE_FAULT_THRESHOLD 10       // Number of fault-worthy values to read in succession before faulting
#define CURRENT_FAULT_THRESHOLD 5       // Number of fault-worthy electrical current values to read in succession before faulting
#define SHUTDOWN_HIGH_THRESHOLD 1500    // Value returned by ADC above which the shutdown circuit is considered powered (balancing not allowed when AIRs open)
#define BALANCE_LIMIT_FACTOR 3          // Reciprocal of the cell balancing duty cycle (3 means balancing can happen during 1 out of every 3 loops, etc)
#define COULOUMB_COUNT_INTERVAL 10000   // Microseconds between current readings

// Current Sensor ADC Channel definitions
#define CH_CUR_SENSE  0
#define CH_TEMP_SENSE 1
#define CH_SHUTDOWN_G 3
#define CH_5V         4
#define CH_SHUTDOWN_H 5

// Temperature Boundary Definitions
#define ONBOARD_TEMP_BALANCE_DISABLE 6000  // 60.00C
#define ONBOARD_TEMP_BALANCE_REENABLE 5000 // 50.00C
#define ONBOARD_TEMP_CRITICAL_HIGH 7000 // 70.00C

// Voltage and Temperature configuration constants
#define VOLTAGE_CUTOFF_LOW 29800 // 2.9800V
#define VOLTAGE_CUTOFF_HIGH 42000 // 4.2000V
#define TOTAL_VOLTAGE_CUTOFF 30000 // 300.00V
#define DISCHARGE_CURRENT_CONSTANT_HIGH 22000 // 220.00A
#define CHARGE_CURRENT_CONSTANT_HIGH -11000 // 110.00A
#define CHARGE_TEMP_CELL_CRITICAL_HIGH 4400 // 44.00C
#define DISCHARGE_TEMP_CELL_CRITICAL_HIGH 6000 // 60.00C
#define VOLTAGE_DIFFERENCE_THRESHOLD 150 // 0.0150V

// Thermistor Constant definitions
#define THERMISTOR_CELL_B 3984
#define THERMISTOR_ONBOARD_B 3380
#define THERMISTOR_T0 298.15 // 25C in Kelvin
#define THERMISTOR_R0 10000  // Resistance of thermistor at 25C
