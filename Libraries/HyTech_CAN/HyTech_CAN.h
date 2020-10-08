#ifndef __HYTECH_CAN_H__
#define __HYTECH_CAN_H__

#include <string.h>
#include <stdint.h>

/*
 * ECU state definitions // TODO make these enums?
 */
#define MCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE 1
#define MCU_STATE_TRACTIVE_SYSTEM_ACTIVE 2
#define MCU_STATE_ENABLING_INVERTER 3
#define MCU_STATE_WAITING_READY_TO_DRIVE_SOUND 4
#define MCU_STATE_READY_TO_DRIVE 5

#define FCU_STATE_TRACTIVE_SYSTEM_NOT_ACTIVE 1
#define FCU_STATE_TRACTIVE_SYSTEM_ACTIVE 2
#define FCU_STATE_ENABLING_INVERTER 3
#define FCU_STATE_WAITING_READY_TO_DRIVE_SOUND 4
#define FCU_STATE_READY_TO_DRIVE 5

#define BMS_STATE_DISCHARGING 1
#define BMS_STATE_CHARGING 2
#define BMS_STATE_BALANCING 3
#define BMS_STATE_BALANCING_OVERHEATED 4

/*
 * CAN ID definitions
 */
#define ID_MC_TEMPERATURES_1 0xA0
#define ID_MC_TEMPERATURES_2 0xA1
#define ID_MC_TEMPERATURES_3 0xA2
#define ID_MC_ANALOG_INPUTS_VOLTAGES 0xA3
#define ID_MC_DIGITAL_INPUT_STATUS 0xA4
#define ID_MC_MOTOR_POSITION_INFORMATION 0xA5
#define ID_MC_CURRENT_INFORMATION 0xA6
#define ID_MC_VOLTAGE_INFORMATION 0xA7
#define ID_MC_FLUX_INFORMATION 0xA8
#define ID_MC_INTERNAL_VOLTAGES 0xA9
#define ID_MC_INTERNAL_STATES 0xAA
#define ID_MC_FAULT_CODES 0xAB
#define ID_MC_TORQUE_TIMER_INFORMATION 0xAC
#define ID_MC_MODULATION_INDEX_FLUX_WEAKENING_OUTPUT_INFORMATION 0xAD
#define ID_MC_FIRMWARE_INFORMATION 0xAE
#define ID_MC_DIAGNOSTIC_DATA 0xAF
#define ID_MC_COMMAND_MESSAGE 0xC0
#define ID_MC_READ_WRITE_PARAMETER_COMMAND 0xC1
#define ID_MC_READ_WRITE_PARAMETER_RESPONSE 0xC2
#define ID_MCU_STATUS 0xC3
#define ID_MCU_PEDAL_READINGS 0xC4
#define ID_GLV_CURRENT_READINGS 0xCC
#define ID_BMS_ONBOARD_TEMPERATURES 0xD5
#define ID_BMS_ONBOARD_DETAILED_TEMPERATURES 0xD6 // TODO rename to bms_detailed_onboard_temperatures when we're not in the middle of a development cycle
#define ID_BMS_VOLTAGES 0xD7
#define ID_BMS_DETAILED_VOLTAGES 0xD8
#define ID_BMS_TEMPERATURES 0xD9
#define ID_BMS_DETAILED_TEMPERATURES 0xDA
#define ID_BMS_STATUS 0xDB
#define ID_FH_WATCHDOG_TEST 0xDC
#define ID_CCU_STATUS 0xDD
#define ID_BMS_BALANCING_STATUS 0xDE // TODO rename to bms_balancing_cells when we're not in the middle of a development cycle
#define ID_FCU_ACCELEROMETER 0xDF // TODO rename to mcu_accelerometer_readings when we're not in the middle of a development cycle
#define ID_BMS_READ_WRITE_PARAMETER_COMMAND 0xE0 // TODO define this message
#define ID_BMS_PARAMETER_RESPONSE 0xE1 // TODO define this message
#define ID_BMS_COULOMB_COUNTS 0xE2
#define ID_MCU_GPS_READINGS_ALPHA 0xE7
#define ID_MCU_GPS_READINGS_BETA 0xE8
#define ID_MCU_GPS_READINGS_GAMMA 0xE9
#define ID_TCU_WHEEL_RPM_REAR 0xEA
#define ID_TCU_WHEEL_RPM_FRONT 0xEB
#define ID_MCU_LAUNCH_CONTROL 0xEC
#define ID_TCU_DISTANCE_TRAVELED 0xED

/*

 * A GENERAL_NOTE: the load functions in these classes take a byte array containing raw CAN buffer data.
 * The data contained in this byte array is used to populate the struct.
 *
 * A GENERAL_NOTE: The write functions in these classes take a byte array that is meant to be populated
 * with the data contained in the object. The byte array can then be used as the raw CAN buffer.
 *
 * TODO: If you make changes here, make sure to update https://hytechracing.me.gatech.edu/wiki/CAN_Data_Formats
 */

// For compatibility with C.
#ifndef __cplusplus
#define bool char
#define true 1
#define false 0
#endif

/*
 * CAN message structs and classes
 */
#pragma pack(push,1)

typedef struct CAN_message_bms_balancing_status_t {
	uint8_t balancing_status[5];
} CAN_message_bms_balancing_status_t;

typedef struct CAN_message_bms_coulomb_counts_t {
    uint32_t total_charge;
    uint32_t total_discharge;
} CAN_message_bms_coulomb_counts_t;

typedef struct CAN_message_bms_detailed_temperatures_t {
	uint8_t ic_id;
    int16_t temperature_0;
    int16_t temperature_1;
    int16_t temperature_2;
} CAN_message_bms_detailed_temperatures_t;

typedef struct CAN_message_bms_detailed_voltages_t {
	uint8_t ic_id_group_id;
    uint16_t voltage_0;
    uint16_t voltage_1;
    uint16_t voltage_2;
} CAN_message_bms_detailed_voltages_t;

typedef struct CAN_message_bms_onboard_detailed_temperatures_t {
	uint8_t ic_id;
    int16_t temperature_0;
    int16_t temperature_1;
} CAN_message_bms_onboard_detailed_temperatures_t;

typedef struct CAN_message_bms_onboard_temperatures_t {
    int16_t average_temperature;
    int16_t low_temperature;
    int16_t high_temperature;
} CAN_message_bms_onboard_temperatures_t;

typedef struct CAN_message_bms_status_t {
	uint8_t state;
    uint16_t error_flags;
    int16_t current;
    uint8_t flags;
} CAN_message_bms_status_t;

typedef struct CAN_message_bms_temperatures_t {
    int16_t average_temperature;
    int16_t low_temperature;
    int16_t high_temperature;
} CAN_message_bms_temperatures_t;

typedef struct CAN_message_bms_voltages_t {
    uint16_t average_voltage;
    uint16_t low_voltage;
    uint16_t high_voltage;
    uint16_t total_voltage;
} CAN_message_bms_voltages_t;

typedef struct CAN_message_ccu_status_t {
    bool charger_enabled;
} CAN_message_ccu_status_t;

typedef struct CAN_message_fcu_readings_t {
	uint16_t accelerator_pedal_raw_1;
	uint16_t accelerator_pedal_raw_2;
	uint16_t brake_pedal_raw;
	int16_t temperature;
} CAN_msg_fcu_readings;

typedef struct CAN_message_fcu_status_t {
    uint8_t state;
    uint8_t flags;
    uint8_t start_button_press_id;
} CAN_msg_fcu_status;

typedef struct CAN_message_glv_current_readings_t {
	uint16_t ecu_current_value;
	uint16_t cooling_current_value;
} CAN_message_glv_current_readings_t;

typedef struct CAN_message_mc_analog_input_voltages_t {
    int16_t analog_input_1;
    int16_t analog_input_2;
    int16_t analog_input_3;
    int16_t analog_input_4;
} CAN_message_mc_analog_input_voltages_t;

typedef struct CAN_message_mc_command_message_t {
    int16_t torque_command;
    int16_t angular_velocity;
    bool direction;
    uint8_t inverter_enable_discharge_enable;
    int16_t commanded_torque_limit;
} CAN_message_mc_command_message_t;

typedef struct CAN_message_mc_current_information_t {
    int16_t phase_a_current;
    int16_t phase_b_current;
    int16_t phase_c_current;
    int16_t dc_bus_current;
} CAN_message_mc_current_information_t;

typedef struct CAN_message_mc_digital_input_status_t {
    bool digital_input_1;
    bool digital_input_2;
    bool digital_input_3;
    bool digital_input_4;
    bool digital_input_5;
    bool digital_input_6;
    bool digital_input_7;
    bool digital_input_8;
} CAN_message_mc_digital_input_status_t;

typedef struct CAN_message_mc_fault_codes_t {
    uint16_t post_fault_lo;
    uint16_t post_fault_hi;
    uint16_t run_fault_lo;
    uint16_t run_fault_hi;
} CAN_message_mc_fault_codes_t;

typedef struct CAN_message_mc_firmware_information_t {
    uint16_t eeprom_version_project_code;
    uint16_t software_version;
    uint16_t date_code_mmdd;
    uint16_t date_code_yyyy;
} CAN_message_mc_firmware_information_t;

typedef struct CAN_message_mc_internal_states_t {
    uint16_t vsm_state;
    uint8_t inverter_state;
    uint8_t relay_state;
    uint8_t inverter_run_mode_discharge_state;
    uint8_t inverter_command_mode;
    uint8_t inverter_enable;
    uint8_t direction_command;
} CAN_message_mc_internal_states_t;

typedef struct CAN_message_mc_modulation_index_flux_weakening_output_information_t {
    uint16_t modulation_index; // TODO Signed or Unsigned?
    int16_t flux_weakening_output;
    int16_t id_command;
    int16_t iq_command;
} CAN_message_mc_modulation_index_flux_weakening_output_information_t;

typedef struct CAN_message_mc_motor_position_information_t {
    int16_t motor_angle;
    int16_t motor_speed;
    int16_t electrical_output_frequency;
    int16_t delta_resolver_filtered;
} CAN_message_mc_motor_position_information_t;

typedef struct CAN_message_mc_read_write_parameter_command_t {
    uint16_t parameter_address;
    bool rw_command;
    uint8_t reserved1;
    uint32_t data;
} CAN_message_mc_read_write_parameter_command_t;

typedef struct CAN_message_mc_read_write_parameter_response_t {
    uint16_t parameter_address;
    bool write_success;
    uint8_t reserved1;
    uint32_t data;
} CAN_message_mc_read_write_parameter_response_t;

typedef struct CAN_message_mc_temperatures_1_t {
    int16_t module_a_temperature;
    int16_t module_b_temperature;
    int16_t module_c_temperature;
    int16_t gate_driver_board_temperature;
} CAN_message_mc_temperatures_1_t;

typedef struct CAN_message_mc_temperatures_2_t {
    int16_t control_board_temperature;
    int16_t rtd_1_temperature;
    int16_t rtd_2_temperature;
    int16_t rtd_3_temperature;
} CAN_message_mc_temperatures_2_t;

typedef struct CAN_message_mc_temperatures_3_t {
    int16_t rtd_4_temperature;
    int16_t rtd_5_temperature;
    int16_t motor_temperature;
    int16_t torque_shudder;
} CAN_message_mc_temperatures_3_t;

typedef struct CAN_message_mc_torque_timer_information_t {
    int16_t commanded_torque;
    int16_t torque_feedback;
    uint32_t power_on_timer;
} CAN_message_mc_torque_timer_information_t;

typedef struct CAN_message_mc_voltage_information_t {
    int16_t dc_bus_voltage;
    int16_t output_voltage;
    int16_t phase_ab_voltage;
    int16_t phase_bc_voltage;
} CAN_message_mc_voltage_information_t;

typedef struct CAN_message_fcu_accelerometer_values_t {
   uint8_t XValue_x100;
   uint8_t YValue_x100;
   uint8_t ZValue_x100;
} CAN_message_fcu_accelerometer_values_t;

typedef struct CAN_message_mcu_gps_readings_alpha_t {
    int32_t latitude;
    int32_t longitude;
} CAN_message_mcu_gps_readings_alpha_t;

typedef struct CAN_message_mcu_gps_readings_beta_t {
    int32_t altitude;
    int32_t speed;
} CAN_message_mcu_gps_readings_beta_t;

typedef struct CAN_message_mcu_gps_readings_gamma_t {
    uint8_t fix_quality;
    uint8_t satellite_count;
    uint32_t timestamp_seconds;
    uint16_t timestamp_milliseconds;
} CAN_message_mcu_gps_readings_gamma_t;

typedef struct CAN_message_mcu_pedal_readings_t {
    uint16_t accelerator_pedal_raw_1;
    uint16_t accelerator_pedal_raw_2;
    uint16_t brake_pedal_raw;
    uint8_t pedal_flags;
    uint8_t torque_map_mode;
} CAN_message_mcu_pedal_readings_t;

typedef struct CAN_message_mcu_status_t {
    uint8_t state;
    uint8_t flags;
    int16_t temperature;
    uint16_t glv_battery_voltage;
} CAN_message_mcu_status_t;

typedef struct CAN_message_rcu_status_t {
    uint8_t state;
    uint8_t flags;
    uint16_t glv_battery_voltage;
    int16_t temperature;
} CAN_msg_rcu_status;

typedef struct CAN_message_tcu_wheel_rpm_t {
    int16_t wheel_rpm_left;
    int16_t wheel_rpm_right;
} CAN_message_tcu_wheel_rpm_t;

typedef struct CAN_message_mcu_launch_control_t {
    int16_t slip_ratio;
    int16_t slip_limiting_factor;
} CAN_message_mcu_launch_control_t;

typedef struct CAN_message_tcu_distanced_traveled_t {
    int16_t distance_traveled;
} CAN_message_tcu_distance_traveled_t;

typedef struct Telem_message {
    //bool cobs_flag;
    uint32_t msg_id;
    uint8_t length;
    union {
        CAN_message_bms_balancing_status_t      bms_balancing_status;
        CAN_message_bms_coulomb_counts_t        bms_coulomb_counts;
        CAN_message_bms_detailed_temperatures_t bms_detailed_temperatures;
        CAN_message_bms_detailed_voltages_t     bms_detailed_voltages;
        CAN_message_bms_onboard_detailed_temperatures_t bms_onboard_detailed_temperatures;
        CAN_message_bms_onboard_temperatures_t  bms_onboard_temperatures;
        CAN_message_bms_status_t                bms_status;
        CAN_message_bms_temperatures_t          bms_temperatures;
        CAN_message_bms_voltages_t              bms_voltages;
        CAN_message_ccu_status_t                ccu_status;
        CAN_message_fcu_accelerometer_values_t  fcu_accelerometer_values;
        CAN_msg_fcu_status                      fcu_status;
        CAN_msg_fcu_readings                    fcu_readings;
        CAN_message_glv_current_readings_t      glv_current_readings;
        CAN_message_mc_analog_input_voltages_t  mc_analog_input_voltages;
        CAN_message_mc_command_message_t        mc_command_message;
        CAN_message_mc_current_information_t    mc_current_information;
        CAN_message_mc_digital_input_status_t   mc_digital_input_status;
        CAN_message_mc_fault_codes_t            mc_fault_codes;
        CAN_message_mc_firmware_information_t   mc_firmware_information;
        CAN_message_mc_internal_states_t        mc_internal_states;
        CAN_message_mc_modulation_index_flux_weakening_output_information_t
                mc_modulation_index_flux_weakening_output_information;
        CAN_message_mc_motor_position_information_t mc_motor_position_information;
        CAN_message_mc_read_write_parameter_command_t mc_read_write_parameter_command;
        CAN_message_mc_read_write_parameter_response_t mc_read_write_parameter_response;
        CAN_message_mc_temperatures_1_t         mc_temperatures_1;
        CAN_message_mc_temperatures_2_t         mc_temperatures_2;
        CAN_message_mc_temperatures_3_t         mc_temperatures_3;
        CAN_message_mc_torque_timer_information_t mc_torque_timer_information;
        CAN_message_mc_voltage_information_t    mc_voltage_information;
        CAN_message_mcu_gps_readings_alpha_t    mcu_gps_readings_alpha;
        CAN_message_mcu_gps_readings_beta_t     mcu_gps_readings_beta;
        CAN_message_mcu_gps_readings_gamma_t    mcu_gps_readings_gamma;
        CAN_message_mcu_pedal_readings_t        mcu_pedal_readings;
        CAN_message_mcu_status_t                mcu_status;
        CAN_msg_rcu_status                      rcu_status;
        CAN_message_tcu_wheel_rpm_t             tcu_wheel_rpm;
        CAN_message_mcu_launch_control_t        mcu_launch_control;
        CAN_message_tcu_distance_traveled_t     tcu_distance_traveled;
    } contents;
    uint16_t checksum;
} Telem_message_t;

#pragma pack(pop)

#ifdef __cplusplus

class BMS_balancing_status {
    public:
        BMS_balancing_status();
        BMS_balancing_status(uint8_t buf[]);
        BMS_balancing_status(uint8_t group_id, int64_t balancing_status);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        uint8_t get_group_id();
        uint64_t get_balancing();
        uint16_t get_ic_balancing(uint8_t ic_id);
        bool get_cell_balancing(uint8_t ic_id, uint16_t cell_id);

        void set_group_id(uint8_t group_id);
        void set_balancing(uint64_t balancing_status);
        void set_ic_balancing(uint8_t ic_id, uint16_t balancing_status);
        void set_cell_balancing(uint8_t ic_id, uint8_t cell_id, bool balancing_status);
    private:
        uint64_t message; // Using a 64-bit datatype here instead of CAN_message_bms_balancing_status_t because it is much easier than dealing with an array
};

class BMS_coulomb_counts {
    public:
        BMS_coulomb_counts();
        BMS_coulomb_counts(uint8_t buf[]);
        BMS_coulomb_counts(uint32_t total_charge, uint32_t total_discharge);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        uint32_t get_total_charge();
        uint32_t get_total_discharge();
        void set_total_charge(uint32_t total_charge);
        void set_total_discharge(uint32_t total_discharge);
    private:
        CAN_message_bms_coulomb_counts_t message;
};

class BMS_detailed_temperatures {
    public:
        BMS_detailed_temperatures();
        BMS_detailed_temperatures(uint8_t buf[]);
        BMS_detailed_temperatures(uint8_t ic_id, int16_t temperature_0, int16_t temperature_1, int16_t temperature_2);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        uint8_t get_ic_id();
        int16_t get_temperature_0();
        int16_t get_temperature_1();
        int16_t get_temperature_2();
        int16_t get_temperature(uint8_t temperature_id);
        void set_ic_id(uint8_t ic_id);
        void set_temperature_0(int16_t temperature_0);
        void set_temperature_1(int16_t temperature_1);
        void set_temperature_2(int16_t temperature_2);
        void set_temperature(uint8_t temperature_id, int16_t temperature);
    private:
        CAN_message_bms_detailed_temperatures_t message;
};

class BMS_detailed_voltages {
    public:
        BMS_detailed_voltages();
        BMS_detailed_voltages(uint8_t buf[]);
        BMS_detailed_voltages(uint8_t ic_id, uint8_t group_id, uint16_t voltage_0, uint16_t voltage_1, uint16_t voltage_2);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        uint8_t get_ic_id();
        uint8_t get_group_id();
        uint16_t get_voltage_0();
        uint16_t get_voltage_1();
        uint16_t get_voltage_2();
        uint16_t get_voltage(uint8_t voltage_id);
        void set_ic_id(uint8_t ic_id);
        void set_group_id(uint8_t group_id);
        void set_voltage_0(uint16_t voltage_0);
        void set_voltage_1(uint16_t voltage_1);
        void set_voltage_2(uint16_t voltage_2);
        void set_voltage(uint8_t voltage_id, uint16_t voltage);
    private:
        CAN_message_bms_detailed_voltages_t message;
};

class BMS_onboard_detailed_temperatures {
    public:
        BMS_onboard_detailed_temperatures();
        BMS_onboard_detailed_temperatures(uint8_t buf[]);
        BMS_onboard_detailed_temperatures(uint8_t ic_id, int16_t temperature_0, int16_t temperature_1);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        uint8_t get_ic_id();
        int16_t get_temperature_0();
        int16_t get_temperature_1();
        int16_t get_temperature(uint8_t temperature_id);
        void set_ic_id(uint8_t ic_id);
        void set_temperature_0(int16_t temperature_0);
        void set_temperature_1(int16_t temperature_1);
        void set_temperature(uint8_t temperature_id, int16_t temperature);
    private:
        CAN_message_bms_onboard_detailed_temperatures_t message;
};

class BMS_onboard_temperatures {
    public:
        BMS_onboard_temperatures();
        BMS_onboard_temperatures(uint8_t buf[]);
        BMS_onboard_temperatures(int16_t average_temperature, int16_t low_temperature, int16_t high_temperature);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        int16_t get_average_temperature();
        int16_t get_low_temperature();
        int16_t get_high_temperature();
        void set_average_temperature(int16_t average_temperature);
        void set_low_temperature(int16_t low_temperature);
        void set_high_temperature(int16_t high_temperature);
    private:
        CAN_message_bms_onboard_temperatures_t message;
};

class BMS_status {
    public:
        BMS_status();
        BMS_status(uint8_t buf[]);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        uint8_t get_state();
        uint16_t get_error_flags();
        bool get_overvoltage();
        bool get_undervoltage();
        bool get_total_voltage_high();
        bool get_discharge_overcurrent();
        bool get_charge_overcurrent();
        bool get_discharge_overtemp();
        bool get_charge_overtemp();
        bool get_undertemp();
        bool get_onboard_overtemp();
        int16_t get_current();
        uint8_t get_flags();
        bool get_shutdown_g_above_threshold();
        bool get_shutdown_h_above_threshold();

        void set_state(uint8_t state);
        void set_error_flags(uint16_t error_flags);
        void set_overvoltage(bool overvoltage);
        void set_undervoltage(bool undervoltage);
        void set_total_voltage_high(bool total_voltage_high);
        void set_discharge_overcurrent(bool discharge_overcurrent);
        void set_charge_overcurrent(bool charge_overcurrent);
        void set_discharge_overtemp(bool discharge_overtemp);
        void set_charge_overtemp(bool charge_overtemp);
        void set_undertemp(bool undertemp);
        void set_onboard_overtemp(bool onboard_overtemp);
        void set_current(int16_t current);
        void set_flags(uint8_t flags);
        void set_shutdown_g_above_threshold(bool shutdown_g_above_threshold);
        void set_shutdown_h_above_threshold(bool shutdown_h_above_threshold);
    private:
        CAN_message_bms_status_t message;
};

class BMS_temperatures {
    public:
        BMS_temperatures();
        BMS_temperatures(uint8_t buf[]);
        BMS_temperatures(int16_t average_temperature, int16_t low_temperature, int16_t high_temperature);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        int16_t get_average_temperature();
        int16_t get_low_temperature();
        int16_t get_high_temperature();
        void set_average_temperature(int16_t average_temperature);
        void set_low_temperature(int16_t low_temperature);
        void set_high_temperature(int16_t high_temperature);
    private:
        CAN_message_bms_temperatures_t message;
};

class BMS_voltages {
    public:
        BMS_voltages();
        BMS_voltages(uint8_t buf[]);
        BMS_voltages(uint16_t average_voltage, uint16_t low_voltage, uint16_t high_voltage, uint16_t total_voltage);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        uint16_t get_average();
        uint16_t get_low();
        uint16_t get_high();
        uint16_t get_total();
        void set_average(uint16_t average_voltage);
        void set_low(uint16_t low_voltage);
        void set_high(uint16_t high_voltage);
        void set_total(uint16_t total_voltage);
    private:
        CAN_message_bms_voltages_t message;
};

class CCU_status {
    public:
        CCU_status();
        CCU_status(uint8_t buf[]);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        bool get_charger_enabled();
        void set_charger_enabled(bool charger_enabled);
    private:
        CAN_message_ccu_status_t message;
};

class FCU_accelerometer_values {
    public:
        FCU_accelerometer_values();
        FCU_accelerometer_values(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint8_t get_x();
        uint8_t get_y();
        uint8_t get_z();
        void set_values(uint8_t x, uint8_t y, uint8_t z);
    private:
        CAN_message_fcu_accelerometer_values_t message;
};

class FCU_readings {
    public:
        FCU_readings();
        FCU_readings(uint8_t buf[8]);
        FCU_readings(uint16_t accelerator_pedal_raw_1, uint16_t accelerator_pedal_raw_2, uint16_t brake_pedal_raw, int16_t temperature);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint16_t get_accelerator_pedal_raw_1();
        uint16_t get_accelerator_pedal_raw_2();
        uint16_t get_brake_pedal_raw();
        int16_t get_temperature();
        void set_accelerator_pedal_raw_1(uint16_t accelerator_pedal_raw_1);
        void set_accelerator_pedal_raw_2(uint16_t accelerator_pedal_raw_2);
        void set_brake_pedal_raw(uint16_t brake_pedal_raw);
        void set_temperature(int16_t temperature);
    private:
        CAN_message_fcu_readings_t message;
};

class FCU_status {
    public:
        FCU_status();
        FCU_status(uint8_t buf[8]);
        FCU_status(uint8_t state, uint8_t flags, uint8_t start_button_press_id);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint8_t get_state();
        uint8_t get_flags();
        bool get_accelerator_implausibility();
        bool get_accelerator_boost_mode();
        bool get_brake_implausibility();
        bool get_brake_pedal_active();
        uint8_t get_start_button_press_id();
        void set_state(uint8_t state);
        void set_flags(uint8_t flags);
        void set_accelerator_implausibility(bool accelerator_implausibility);
        void set_accelerator_boost_mode(bool accelerator_boost_mode);
        void set_brake_implausibility(bool brake_implausibility);
        void set_brake_pedal_active(bool brake_pedal_active);
        void set_start_button_press_id(uint8_t start_button_press_id);
    private:
        CAN_message_fcu_status_t message;
};

class GLV_current_readings {
	public:
		GLV_current_readings();
		GLV_current_readings(uint8_t buf[8]);
		GLV_current_readings(uint16_t ecu_current_value, uint16_t cooling_current_value);
		void load(uint8_t buf[8]);
		void write(uint8_t buf[8]);
		uint16_t get_ecu_current_value();
		uint16_t get_cooling_current_value();
		void set_ecu_current_value(uint16_t ecu_current_value);
		void set_cooling_current_value(uint16_t cooling_current_value);
	private:
		CAN_message_glv_current_readings_t message;
};

class MC_analog_input_voltages {
    public:
        MC_analog_input_voltages();
        MC_analog_input_voltages(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_analog_input_1();
        int16_t get_analog_input_2();
        int16_t get_analog_input_3();
        int16_t get_analog_input_4();
    private:
        CAN_message_mc_analog_input_voltages_t message;
};

class MC_command_message {
    public:
        MC_command_message();
        MC_command_message(uint8_t buf[8]);
        MC_command_message(int16_t torque_command, int16_t angular_velocity, bool direction, bool inverter_enable, bool discharge_enable, int16_t commanded_torque_limit);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_torque_command();
        int16_t get_angular_velocity();
        bool get_direction();
        bool get_inverter_enable();
        bool get_discharge_enable();
        int16_t get_commanded_torque_limit();
        void set_torque_command(int16_t torque_command);
        void set_angular_velocity(int16_t angular_velocity);
        void set_direction(bool direction);
        void set_inverter_enable(bool inverter_enable);
        void set_discharge_enable(bool discharge_enable);
        void set_commanded_torque_limit(int16_t commanded_torque_limit);
    private:
        CAN_message_mc_command_message_t message;
};

class MC_current_information {
    public:
        MC_current_information();
        MC_current_information(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_phase_a_current();
        int16_t get_phase_b_current();
        int16_t get_phase_c_current();
        int16_t get_dc_bus_current();
    private:
        CAN_message_mc_current_information_t message;
};

class MC_digital_input_status {
    public:
        MC_digital_input_status();
        MC_digital_input_status(uint8_t buf[]);
        void load(uint8_t buf[]);
        void write(uint8_t buf[]);
        bool get_digital_input_1();
        bool get_digital_input_2();
        bool get_digital_input_3();
        bool get_digital_input_4();
        bool get_digital_input_5();
        bool get_digital_input_6();
        bool get_digital_input_7();
        bool get_digital_input_8();
    private:
        CAN_message_mc_digital_input_status_t message;
};

class MC_fault_codes {
    public:
        MC_fault_codes();
        MC_fault_codes(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint16_t get_post_fault_lo();
        uint16_t get_post_fault_hi();
        uint16_t get_run_fault_lo();
        uint16_t get_run_fault_hi();
        bool get_post_lo_hw_gate_desaturation_fault();
        bool get_post_lo_hw_overcurrent_fault();
        bool get_post_lo_accelerator_shorted();
        bool get_post_lo_accelerator_open();
        bool get_post_lo_current_sensor_low();
        bool get_post_lo_current_sensor_high();
        bool get_post_lo_module_temperature_low();
        bool get_post_lo_module_temperature_high();
        bool get_post_lo_ctrl_pcb_temperature_low();
        bool get_post_lo_ctrl_pcb_temperature_high();
        bool get_post_lo_gate_drive_pcb_temperature_low();
        bool get_post_lo_gate_drive_pcb_temperature_high();
        bool get_post_lo_5v_sense_voltage_low();
        bool get_post_lo_5v_sense_voltage_high();
        bool get_post_lo_12v_sense_voltage_low();
        bool get_post_lo_12v_sense_voltage_high();
        bool get_post_hi_25v_sense_voltage_low();
        bool get_post_hi_25v_sense_voltage_high();
        bool get_post_hi_15v_sense_voltage_low();
        bool get_post_hi_15v_sense_voltage_high();
        bool get_post_hi_dc_bus_voltage_high();
        bool get_post_hi_dc_bus_voltage_low();
        bool get_post_hi_precharge_timeout();
        bool get_post_hi_precharge_voltage_failure();
        bool get_post_hi_eeprom_checksum_invalid();
        bool get_post_hi_eeprom_data_out_of_range();
        bool get_post_hi_eeprom_update_required();
        bool get_post_hi_reserved1(); // TODO delete these?
        bool get_post_hi_reserved2();
        bool get_post_hi_reserved3();
        bool get_post_hi_brake_shorted();
        bool get_post_hi_brake_open();
        bool get_run_lo_motor_overspeed_fault();
        bool get_run_lo_overcurrent_fault();
        bool get_run_lo_overvoltage_fault();
        bool get_run_lo_inverter_overtemperature_fault();
        bool get_run_lo_accelerator_input_shorted_fault();
        bool get_run_lo_accelerator_input_open_fault();
        bool get_run_lo_direction_command_fault();
        bool get_run_lo_inverter_response_timeout_fault();
        bool get_run_lo_hardware_gatedesaturation_fault();
        bool get_run_lo_hardware_overcurrent_fault();
        bool get_run_lo_undervoltage_fault();
        bool get_run_lo_can_command_message_lost_fault();
        bool get_run_lo_motor_overtemperature_fault();
        bool get_run_lo_reserved1(); // TODO delete these?
        bool get_run_lo_reserved2();
        bool get_run_lo_reserved3();
        bool get_run_hi_brake_input_shorted_fault();
        bool get_run_hi_brake_input_open_fault();
        bool get_run_hi_module_a_overtemperature_fault();
        bool get_run_hi_module_b_overtemperature_fault();
        bool get_run_hi_module_c_overtemperature_fault();
        bool get_run_hi_pcb_overtemperature_fault();
        bool get_run_hi_gate_drive_board_1_overtemperature_fault();
        bool get_run_hi_gate_drive_board_2_overtemperature_fault();
        bool get_run_hi_gate_drive_board_3_overtemperature_fault();
        bool get_run_hi_current_sensor_fault();
        bool get_run_hi_reserved1(); // TODO delete these?
        bool get_run_hi_reserved2();
        bool get_run_hi_reserved3();
        bool get_run_hi_reserved4();
        bool get_run_hi_resolver_not_connected();
        bool get_run_hi_inverter_discharge_active();
    private:
        CAN_message_mc_fault_codes_t message;
};

class MC_firmware_information {
    public:
        MC_firmware_information();
        MC_firmware_information(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint16_t get_eeprom_version_project_code();
        uint16_t get_software_version();
        uint16_t get_date_code_mmdd();
        uint16_t get_date_code_yyyy();
    private:
        CAN_message_mc_firmware_information_t message;
};

class MC_internal_states {
    public:
        MC_internal_states();
        MC_internal_states(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint8_t get_vsm_state();
        uint8_t get_inverter_state();
        bool get_relay_active_1();
        bool get_relay_active_2();
        bool get_relay_active_3();
        bool get_relay_active_4();
        bool get_relay_active_5();
        bool get_relay_active_6();
        bool get_inverter_run_mode();
        uint8_t get_inverter_active_discharge_state();
        bool get_inverter_command_mode();
        bool get_inverter_enable_state();
        bool get_inverter_enable_lockout();
        bool get_direction_command();
    private:
        CAN_message_mc_internal_states_t message;
};

class MC_modulation_index_flux_weakening_output_information {
    public:
        MC_modulation_index_flux_weakening_output_information();
        MC_modulation_index_flux_weakening_output_information(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint16_t get_modulation_index();
        int16_t get_flux_weakening_output();
        int16_t get_id_command();
        int16_t get_iq_command();
    private:
        CAN_message_mc_modulation_index_flux_weakening_output_information_t message;
};

class MC_motor_position_information {
    public:
        MC_motor_position_information();
        MC_motor_position_information(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_motor_angle();
        int16_t get_motor_speed();
        int16_t get_electrical_output_frequency();
        int16_t get_delta_resolver_filtered();
    private:
        CAN_message_mc_motor_position_information_t message;
};

class MC_read_write_parameter_command {
    public:
        MC_read_write_parameter_command();
        MC_read_write_parameter_command(uint8_t buf[8]);
        MC_read_write_parameter_command(uint16_t parameter_address, bool rw_command, uint32_t data);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint16_t get_parameter_address();
        bool get_rw_command();
        uint32_t get_data();
        void set_parameter_address(uint16_t parameter_address);
        void set_rw_command(bool rw_command);
        void set_data(uint32_t data);
    private:
        CAN_message_mc_read_write_parameter_command_t message;
};

class MC_read_write_parameter_response {
    public:
        MC_read_write_parameter_response();
        MC_read_write_parameter_response(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint16_t get_parameter_address();
        bool get_write_success();
        uint32_t get_data();
    private:
        CAN_message_mc_read_write_parameter_response_t message;
};

class MC_temperatures_1 {
    public:
        MC_temperatures_1();
        MC_temperatures_1(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_module_a_temperature();
        int16_t get_module_b_temperature();
        int16_t get_module_c_temperature();
        int16_t get_gate_driver_board_temperature();
    private:
        CAN_message_mc_temperatures_1_t message;
};

class MC_temperatures_2 {
    public:
        MC_temperatures_2();
        MC_temperatures_2(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_control_board_temperature();
        int16_t get_rtd_1_temperature();
        int16_t get_rtd_2_temperature();
        int16_t get_rtd_3_temperature();
    private:
        CAN_message_mc_temperatures_2_t message;
};

class MC_temperatures_3 {
    public:
        MC_temperatures_3();
        MC_temperatures_3(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_rtd_4_temperature();
        int16_t get_rtd_5_temperature();
        int16_t get_motor_temperature();
        int16_t get_torque_shudder();
    private:
        CAN_message_mc_temperatures_3_t message;
};

class MC_torque_timer_information {
    public:
        MC_torque_timer_information();
        MC_torque_timer_information(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_commanded_torque();
        int16_t get_torque_feedback();
        uint32_t get_power_on_timer();
    private:
        CAN_message_mc_torque_timer_information_t message;
};

class MC_voltage_information {
    public:
        MC_voltage_information();
        MC_voltage_information(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_dc_bus_voltage();
        int16_t get_output_voltage();
        int16_t get_phase_ab_voltage();
        int16_t get_phase_bc_voltage();
    private:
        CAN_message_mc_voltage_information_t message;
};

class MCU_GPS_readings_alpha {
    public:
        MCU_GPS_readings_alpha();
        MCU_GPS_readings_alpha(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int32_t get_latitude();
        int32_t get_longitude();
        void set_latitude(int32_t latitude);
        void set_longitude(int32_t longitude);
    private:
        CAN_message_mcu_gps_readings_alpha_t message;
};

class MCU_GPS_readings_beta {
    public:
        MCU_GPS_readings_beta();
        MCU_GPS_readings_beta(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int32_t get_altitude();
        int32_t get_speed();
        void set_altitude(int32_t altitude);
        void set_speed(int32_t speed);
    private:
        CAN_message_mcu_gps_readings_beta_t message;
};

class MCU_GPS_readings_gamma {
    public:
        MCU_GPS_readings_gamma();
        MCU_GPS_readings_gamma(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint8_t get_fix_quality();
        uint8_t get_satellite_count();
        uint32_t get_timestamp_seconds();
        uint16_t get_timestamp_milliseconds();
        void set_fix_quality(uint8_t fix_quality);
        void set_satellite_count(uint8_t satellite_count);
        void set_timestamp_seconds(uint32_t timestamp_seconds);
        void set_timestamp_milliseconds(uint16_t timestamp_milliseconds);
    private:
        CAN_message_mcu_gps_readings_gamma_t message;
};

class MCU_pedal_readings {
    public:
        MCU_pedal_readings();
        MCU_pedal_readings(uint8_t buf[8]);
        MCU_pedal_readings(uint16_t accelerator_pedal_raw_1, uint16_t accelerator_pedal_raw_2, uint16_t brake_pedal_raw, uint8_t pedal_flags, uint8_t torque_map_mode);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint16_t get_accelerator_pedal_raw_1();
        uint16_t get_accelerator_pedal_raw_2();
        uint16_t get_brake_pedal_raw();
        uint8_t get_pedal_flags();
        bool get_accelerator_implausibility();
        bool get_brake_implausibility();
        bool get_brake_pedal_active();
        uint8_t get_torque_map_mode();
        void set_accelerator_pedal_raw_1(uint16_t accelerator_pedal_raw_1);
        void set_accelerator_pedal_raw_2(uint16_t accelerator_pedal_raw_2);
        void set_brake_pedal_raw(uint16_t brake_pedal_raw);
        void set_pedal_flags(uint8_t pedal_flags);
        void set_accelerator_implausibility(bool accelerator_implausibility);
        void set_brake_implausibility(bool brake_implausibility);
        void set_brake_pedal_active(bool brake_pedal_active);
        void set_torque_map_mode(uint8_t torque_map_mode);
    private:
        CAN_message_mcu_pedal_readings_t message;
};

class MCU_status {
    public:
        MCU_status();
        MCU_status(uint8_t buf[8]);
        MCU_status(uint8_t state, uint8_t flags, int16_t temperature, uint16_t glv_battery_voltage);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint8_t get_state();
        uint8_t get_flags();
        bool get_bms_ok_high();
        bool get_imd_okhs_high();
        bool get_inverter_powered();
        bool get_shutdown_b_above_threshold();
        bool get_shutdown_c_above_threshold();
        bool get_shutdown_d_above_threshold();
        bool get_shutdown_e_above_threshold();
        bool get_shutdown_f_above_threshold();
        int16_t get_temperature();
        uint16_t get_glv_battery_voltage();
        void set_state(uint8_t state);
        void set_flags(uint8_t flags);
        void set_bms_ok_high(bool bms_ok_high);
        void set_imd_okhs_high(bool imd_okhs_high);
        void set_inverter_powered(bool inverter_powered);
        void set_shutdown_b_above_threshold(bool shutdown_b_above_threshold);
        void set_shutdown_c_above_threshold(bool shutdown_c_above_threshold);
        void set_shutdown_d_above_threshold(bool shutdown_d_above_threshold);
        void set_shutdown_e_above_threshold(bool shutdown_e_above_threshold);
        void set_shutdown_f_above_threshold(bool shutdown_f_above_threshold);
        void set_temperature(int16_t temperature);
        void set_glv_battery_voltage(uint16_t glv_battery_voltage);
    private:
        CAN_message_mcu_status_t message;
};

class RCU_status {
    public:
        RCU_status();
        RCU_status(uint8_t buf[8]);
        RCU_status(uint8_t state, uint8_t flags, uint16_t glv_battery_voltage, int16_t temperature);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        uint8_t get_state();
        uint8_t get_flags();
        bool get_bms_ok_high();
        bool get_imd_okhs_high();
        bool get_bms_imd_latched();
        bool get_inverter_powered();
        uint16_t get_glv_battery_voltage();
        int16_t get_temperature();
        void set_state(uint8_t state);
        void set_flags(uint8_t flags);
        void set_bms_ok_high(bool bms_ok_high);
        void set_imd_okhs_high(bool imd_okhs_high);
        void set_bms_imd_latched(bool bms_imd_latched);
        void set_inverter_powered(bool inverter_powered);
        void set_glv_battery_voltage(uint16_t glv_battery_voltage);
        void set_temperature(int16_t temperature);
    private:
        CAN_message_rcu_status_t message;
};

class TCU_wheel_rpm {
    public:
        TCU_wheel_rpm();
        TCU_wheel_rpm(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_wheel_rpm_left();
        int16_t get_wheel_rpm_right();
        void set_wheel_rpm_left(uint16_t value);
        void set_wheel_rpm_right(uint16_t value);
    private:
        CAN_message_tcu_wheel_rpm_t message;
};

class MCU_launch_control {
    public:
        MCU_launch_control();
        MCU_launch_control(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_slip_ratio();
        int16_t get_slip_limiting_factor();
        void set_slip_ratio(uint16_t value);
        void set_slip_limiting_factor(uint16_t value);
    private:
        CAN_message_mcu_launch_control_t message;
};

class TCU_distance_traveled {
    public:
        TCU_distance_traveled();
        TCU_distance_traveled(uint8_t buf[8]);
        void load(uint8_t buf[8]);
        void write(uint8_t buf[8]);
        int16_t get_distance_traveled();
        void set_distance_traveled(uint16_t value);
    private:
        CAN_message_tcu_distance_traveled_t message;
};

#endif
#endif
