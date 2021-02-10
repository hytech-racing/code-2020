/*
 * Teensy 3.5 Telemetry Control Unit code
 * Written by Soohyun Kim, with assistance by Ryan Gallaway and Nathan Cheek. 
 * 
 * Rev 2 - 4/23/2019
 */

#define HT_DEBUG_EN // allows use of CAN message print() commands

#include "ADC_SPI.h"

#include <SD.h>
#include <Adafruit_Sensor.h>
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <Wire.h>
#include <Metro.h>
#include <Adafruit_GPS.h>
#include <Time.h>

#include "SDUtils.h"
#include "XBUtils.h"

/*
 * ADC pin definitions
 */
#define TEMP_SENSE_CHANNEL 0
#define ECU_CURRENT_CHANNEL 1
#define SUPPLY_READ_CHANNEL 2
#define COOLING_CURRENT_CHANNEL 3
#define ADC_CS A1
#define SCALE_CURRENT_READING(reading) ((reading - 2048) / 151) //self derived
#define ALPHA 0.9772                     // parameter for the sowftware filter used on ADC pedal channels
/*
 * Variables to store filtered values from ADC channels
 */
float filtered_temp_reading{};
float filtered_ecu_current_reading{};
float filtered_supply_reading{};
float filtered_cooling_current_reading{};

ADC_SPI ADC(ADC_CS, ADC_SPI_SPEED);
FlexCAN CAN(500000);
static CAN_message_t xb_msg;
File logfile;

Adafruit_GPS GPS(&Serial1);

MCU_status mcu_status;
MCU_pedal_readings mcu_pedal_readings;
MCU_analog_readings mcu_analog_readings;
MCU_GPS_readings mcu_gps_readings;
MCU_wheel_speed mcu_wheel_speed;
BMS_voltages bms_voltages;
BMS_detailed_voltages bms_detailed_voltages[8][3];
BMS_temperatures bms_temperatures;
BMS_detailed_temperatures bms_detailed_temperatures[8];
BMS_onboard_temperatures bms_onboard_temperatures;
BMS_onboard_detailed_temperatures bms_onboard_detailed_temperatures[8];
BMS_status bms_status;
BMS_balancing_status bms_balancing_status[2];
BMS_coulomb_counts bms_coulomb_counts;
CCU_status ccu_status;
MC_temperatures_1 mc_temperatures_1;
MC_temperatures_2 mc_temperatures_2;
MC_temperatures_3 mc_temperatures_3;
//MC_analog_input_voltages mc_analog_input_voltages;
//MC_digital_input_status mc_digital_input_status;
MC_motor_position_information mc_motor_position_information;
MC_current_information mc_current_information;
MC_voltage_information mc_voltage_information;
MC_internal_states mc_internal_states;
MC_fault_codes mc_fault_codes;
MC_torque_timer_information mc_torque_timer_information;
MC_modulation_index_flux_weakening_output_information mc_modulation_index_flux_weakening_output_information;
MC_firmware_information mc_firmware_information;
MC_command_message mc_command_message;
MC_read_write_parameter_command mc_read_write_parameter_command;
MC_read_write_parameter_response mc_read_write_parameter_response;

time_t getTeensy3Clock() { return Teensy3Clock.get(); }

void setup() {
	// Real-time clock
	//Teensy3Clock.set(9999999999); // set time (epoch) at powerup (COMMENT OUT THIS LINE AND PUSH ONCE RTC HAS BEEN SET!!!!)
	setSyncProvider(getTeensy3Clock); // registers Teensy RTC as system time
	Serial.println(timeStatus() == timeSet ? "System time set to RTC" : "RTC not set up - uncomment Teensy3Clock.set() to set time");

	// Serial / XBee / CAN 
	Serial.begin(115200);
	XB.begin(115200);
	FLEXCAN0_MCR &= 0xFFFDFFFF; // Enables CAN message self-reception
	CAN.begin();

	// GPS
	GPS.begin(9600);
	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // specify data to be received (minimum + fix)
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ); // set update rate (10Hz)
	GPS.sendCommand(PGCMD_ANTENNA); // report data about antenna

	// SD Card
	Serial.println("Initializing SD card...");
	SdFile::dateTimeCallback(HT::SDUtil::datetime_callback); // Set date/time callback function
	Serial.println(HT::SDUtil::initFD(logfile) ? "Successfully opened SD file" : "Failed to open SD file");

	logfile.println("time,msg.id,msg.len,data"); // Print CSV heading to the logfile
	logfile.flush();
}

void loop() {
	parse_can_message();
	check_xbee_timers();
	read_analog_values();

    static Metro timer_flush = Metro(1000);
	if (timer_flush.check()) // Occasionally flush SD buffer (max one second data loss after power-off)
		logfile.flush();

    // GLV Current
    static Metro timer_current = Metro(100);
	if (timer_current.check()) {
        mcu_analog_readings.set_ecu_current_value(SCALE_CURRENT_READING(filtered_ecu_current_reading)* 100);
        mcu_analog_readings.set_cooling_current_value(SCALE_CURRENT_READING(filtered_cooling_current_reading) * 100);
		mcu_analog_readings.set_glv_battery_voltage(filtered_supply_reading /4096 * 55/12 * 1000);
		mcu_analog_readings.set_temperature(filtered_temp_reading);
        send_xbee(ID_MCU_ANALOG_READINGS, mcu_analog_readings, xb_msg);
        CAN.write(xb_msg);
    }

	// GPS
    static Metro timer_gps = Metro(100);
	static bool pending_gps_data;
	GPS.read();
	if (GPS.newNMEAreceived()) {
		GPS.parse(GPS.lastNMEA());
		pending_gps_data = true;
	}
	if (timer_gps.check() && pending_gps_data) {
		pending_gps_data = false;
        mcu_gps_readings.set_latitude(GPS.latitude * 10000);
        mcu_gps_readings.set_longitude(GPS.longitude * 10000);
        send_xbee(ID_MCU_GPS_READINGS, mcu_gps_readings, xb_msg);
        CAN.write(xb_msg);
	}
}

inline void read_analog_values() {
    /* Filter ADC readings */
    filtered_temp_reading 		 	 = ALPHA * filtered_temp_reading 			+ (1 - ALPHA) * ADC.read_adc(TEMP_SENSE_CHANNEL);
    filtered_ecu_current_reading 	 = ALPHA * filtered_ecu_current_reading 	+ (1 - ALPHA) * ADC.read_adc(ECU_CURRENT_CHANNEL);
    filtered_supply_reading 	 	 = ALPHA * filtered_supply_reading 			+ (1 - ALPHA) * ADC.read_adc(SUPPLY_READ_CHANNEL);
    filtered_cooling_current_reading = ALPHA * filtered_cooling_current_reading + (1 - ALPHA) * ADC.read_adc(COOLING_CURRENT_CHANNEL);
}


void parse_can_message() {
    CAN_message_t msg_rx;
	while (CAN.read(msg_rx)) {
		HT::SDUtil::write(logfile, msg_rx); // Write to SD card buffer (if the buffer fills up, triggering a flush to disk, this will take 8ms)

		// Identify received CAN messages and load contents into corresponding structs
		if (msg_rx.id == ID_BMS_DETAILED_VOLTAGES) {
			BMS_detailed_voltages temp = BMS_detailed_voltages(msg_rx.buf);
			bms_detailed_voltages[temp.get_ic_id()][temp.get_group_id()].load(msg_rx.buf);
		}
		else if (msg_rx.id == ID_BMS_DETAILED_TEMPERATURES) {
			BMS_detailed_temperatures temp = BMS_detailed_temperatures(msg_rx.buf);
			bms_detailed_temperatures[temp.get_ic_id()].load(msg_rx.buf);
		}
		else if (msg_rx.id == ID_BMS_ONBOARD_DETAILED_TEMPERATURES) {
			BMS_onboard_detailed_temperatures temp = BMS_onboard_detailed_temperatures(msg_rx.buf);
			bms_onboard_detailed_temperatures[temp.get_ic_id()].load(msg_rx.buf);
		}
		else if (msg_rx.id == ID_BMS_BALANCING_STATUS) {
			BMS_balancing_status temp = BMS_balancing_status(msg_rx.buf);
			bms_balancing_status[temp.get_group_id()].load(msg_rx.buf);
		}
		else switch(msg_rx.id) {
			case ID_MCU_STATUS:							mcu_status.load(msg_rx.buf);						break;
			case ID_MCU_PEDAL_READINGS:					mcu_pedal_readings.load(msg_rx.buf);				break;
			case ID_MCU_ANALOG_READINGS:				mcu_analog_readings.load(msg_rx.buf);					break;
			case ID_BMS_VOLTAGES:						bms_voltages.load(msg_rx.buf);						break;
			case ID_BMS_TEMPERATURES:					bms_temperatures.load(msg_rx.buf);					break;
			case ID_BMS_ONBOARD_TEMPERATURES:			bms_onboard_temperatures.load(msg_rx.buf);			break;
			case ID_BMS_STATUS:							bms_status.load(msg_rx.buf);						break;
			case ID_BMS_COULOMB_COUNTS:					bms_coulomb_counts.load(msg_rx.buf);				break;
			case ID_CCU_STATUS:							ccu_status.load(msg_rx.buf);						break;
			case ID_MC_TEMPERATURES_1:					mc_temperatures_1.load(msg_rx.buf);					break;
			case ID_MC_TEMPERATURES_2:					mc_temperatures_2.load(msg_rx.buf);					break;
			case ID_MC_TEMPERATURES_3:					mc_temperatures_3.load(msg_rx.buf);					break;
			// case ID_MC_ANALOG_INPUTS_VOLTAGES:		mc_analog_input_voltages.load(msg_rx.buf);			break;
			// case ID_MC_DIGITAL_INPUT_STATUS:			mc_digital_input_status.load(msg_rx.buf);			break;
			case ID_MC_MOTOR_POSITION_INFORMATION:		mc_motor_position_information.load(msg_rx.buf);		break;
			case ID_MC_CURRENT_INFORMATION:				mc_current_information.load(msg_rx.buf);			break;
			case ID_MC_VOLTAGE_INFORMATION:				mc_voltage_information.load(msg_rx.buf);			break;
			case ID_MC_INTERNAL_STATES:					mc_internal_states.load(msg_rx.buf);				break;
			case ID_MC_FAULT_CODES:						mc_fault_codes.load(msg_rx.buf);					break;
			case ID_MC_TORQUE_TIMER_INFORMATION:		mc_torque_timer_information.load(msg_rx.buf);		break;
			case ID_MC_MODULATION_INDEX_FLUX_WEAKENING_OUTPUT_INFORMATION: 
				mc_modulation_index_flux_weakening_output_information.load(msg_rx.buf); 
				break;
			case ID_MC_FIRMWARE_INFORMATION:			mc_firmware_information.load(msg_rx.buf);			break;
			case ID_MC_COMMAND_MESSAGE:					mc_command_message.load(msg_rx.buf);				break;
			case ID_MC_READ_WRITE_PARAMETER_COMMAND:	mc_read_write_parameter_command.load(msg_rx.buf);	break;
			case ID_MC_READ_WRITE_PARAMETER_RESPONSE:	mc_read_write_parameter_response.load(msg_rx.buf);	break;
			case ID_MCU_WHEEL_RPM:						mcu_wheel_speed.load(msg_rx.buf);				break;
		}
	}
}

void check_xbee_timers() {

    static Metro xb_metro_100ms = Metro(100);
    if (xb_metro_100ms.check()) {
		send_xbee(ID_MC_MOTOR_POSITION_INFORMATION, mc_motor_position_information, xb_msg);
		send_xbee(ID_MC_CURRENT_INFORMATION, mc_current_information, xb_msg);
		send_xbee(ID_MC_VOLTAGE_INFORMATION, mc_voltage_information, xb_msg);
	}

	static Metro xb_metro_200ms = Metro(200);
    if (xb_metro_200ms.check()) {
		send_xbee(ID_MC_COMMAND_MESSAGE, mc_command_message, xb_msg);
		send_xbee(ID_MC_TORQUE_TIMER_INFORMATION, mc_torque_timer_information, xb_msg);
		send_xbee(ID_MCU_PEDAL_READINGS, mcu_pedal_readings, xb_msg);
		send_xbee(ID_TCU_WHEEL_RPM_REAR, tcu_wheel_rpm_rear, xb_msg);
		send_xbee(ID_TCU_WHEEL_RPM_FRONT, tcu_wheel_rpm_front, xb_msg);
		send_xbee(ID_TCU_DISTANCE_TRAVELED, tcu_distance_traveled, xb_msg);
	}
	
	static Metro xb_metro_1s = Metro(1000);
    if (xb_metro_1s.check()) {
		send_xbee(ID_BMS_VOLTAGES, bms_voltages, xb_msg);
		send_xbee(ID_BMS_STATUS, bms_status, xb_msg);
		send_xbee(ID_BMS_COULOMB_COUNTS, bms_coulomb_counts, xb_msg);
	}

	static Metro xb_metro_2s = Metro(2000);
    if (xb_metro_2s.check()) {
		send_xbee(ID_MC_INTERNAL_STATES, mc_internal_states, xb_msg);
		send_xbee(ID_MC_FAULT_CODES, mc_fault_codes, xb_msg);
		send_xbee(ID_MCU_STATUS, mcu_status, xb_msg);
	}

	static Metro xb_metro_3s = Metro(3000);
    if (xb_metro_3s.check()) {
		send_xbee(ID_MC_TEMPERATURES_1, mc_temperatures_1, xb_msg);
		send_xbee(ID_MC_TEMPERATURES_2, mc_temperatures_2, xb_msg);
		send_xbee(ID_MC_TEMPERATURES_3, mc_temperatures_3, xb_msg);
		send_xbee(ID_BMS_TEMPERATURES, bms_temperatures, xb_msg);

		for (int ic = 0; ic < 8; ic++)
			for (int group = 0; group < 3; group++)
				send_xbee(ID_BMS_DETAILED_VOLTAGES, bms_detailed_voltages[ic][group], xb_msg);
		for (int ic = 0; ic < 8; ic++)
			send_xbee(ID_BMS_DETAILED_TEMPERATURES, bms_detailed_temperatures[ic], xb_msg);
		for (int i = 0; i < 2; i++)
			send_xbee(ID_BMS_BALANCING_STATUS, bms_balancing_status[i], xb_msg);
	}
}
