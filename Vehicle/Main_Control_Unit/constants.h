// Constant definitions
// TODO some of these values need to be calibrated once hardware is installed
#define BRAKE_ACTIVE 600                // Threshold for brake pedal active
#define MIN_ACCELERATOR_PEDAL_1 1850     // Low accelerator implausibility threshold
#define START_ACCELERATOR_PEDAL_1 2050   // Position to start acceleration
#define END_ACCELERATOR_PEDAL_1 2400     // Position to max out acceleration
#define MAX_ACCELERATOR_PEDAL_1 2500    // High accelerator implausibility threshold
#define MIN_ACCELERATOR_PEDAL_2 2250    // Low accelerator implausibility threshold
#define START_ACCELERATOR_PEDAL_2 2050  // Position to start acceleration
#define END_ACCELERATOR_PEDAL_2 1700    // Position to max out acceleration
#define MAX_ACCELERATOR_PEDAL_2 1590    // High accelerator implausibility threshold
#define MIN_HV_VOLTAGE 500              // Volts in V * 0.1 - Used to check if Accumulator is energized
#define BMS_HIGH 134                    // ~3V on BMS_OK line
#define IMD_HIGH 134                    // ~3V on OKHS line
#define SHUTDOWN_B_HIGH 530             // ~5V on SHUTDOWN_B line
#define SHUTDOWN_C_HIGH 350             // ~5V on SHUTDOWN_C line
#define SHUTDOWN_D_HIGH 350             // ~5V on SHUTDOWN_D line ??????
#define SHUTDOWN_E_HIGH 350             // ~5V on SHUTDOWN_E line
#define SHUTDOWN_F_HIGH 350             // ~5V on SHUTDOWN_F line
#define FAN_1_DUTY_CYCLE 127            // TODO: figure out correct duty cycle (0 = 0%, 255 = 100%)
#define FAN_2_DUTY_CYCLE 127            // TODO: figure out correct duty cycle (0 = 0%, 255 = 100%)
#define BMS_HIGH_BATTERY_TEMPERATURE 50 // TODO: figure out correct value
#define GLV_VOLTAGE_MULTIPLIER 5.5963   // TODO: calibrate this constant
#define MIN_RPM_FOR_REGEN 100           // TODO: calibrate this constant
#define START_ACCEL1_PEDAL_WITH_REGEN 190  // TODO: calibrate this constant
#define START_ACCEL2_PEDAL_WITH_REGEN 3890 // TODO: calibrate this constant
#define START_BRAKE_PEDAL_WITH_REGEN 450   // TODO: calibrate this constant
#define END_BRAKE_PEDAL_WITH_REGEN 1000    // TODO: calibrate this constant
#define ALPHA 0.9772                    // parameter for the sowftware filter used on ADC pedal channels
#define ADC_SPI_SPEED 1800000           // max SPI clokc frequency for MCP3208 is 2MHz in ideal conditions
#define TORQUE_ADJUSTMENT_VOLTAGE 3.5242   //
#define MAX_POSSIBLE_TORQUE 1600        //

// Teensy Pin definitions
#define SSR_INVERTER 6
#define SSR_BRAKE_LIGHT 7
#define ADC_CS 9
#define SENSE_SHUTDOWN_B A2
#define SENSE_SHUTDOWN_C A3
#define SENSE_SHUTDOWN_E A0
#define SENSE_SHUTDOWN_F A1
#define PUMP_CTRL A6
#define SOFTWARE_SHUTDOWN_RELAY A7
#define FAN_1 A8
#define FAN_2 A9

// ADC pin definitions
#define ADC_BRAKE_CHANNEL 0
#define ADC_ACCEL_1_CHANNEL 1
#define ADC_ACCEL_2_CHANNEL 2
#define ADC_12V_SUPPLY_CHANNEL 3
#define ADC_SHUTDOWN_D_READ_CHANNEL 4
#define ADC_BMS_OK_CHANNEL 5
#define ADC_OKHS_CHANNEL 6
#define ADC_TEMPSENSE_CHANNEL 7

// Slip calculation constants
#define MAX_DESIRABLE_SLIP_RATIO 0.2
#define SLIP_ADJUSTER 10.0
#define KP 0.15
#define KI 0.005
#define KD 0
