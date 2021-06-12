/*
 * Teensy 3.5 Telemetry Control Unit code
 * Written by Soohyun Kim, with assistance by Ryan Gallaway and Nathan Cheek. 
 * 
 * Rev 2 - 4/23/2019
 */

#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <kinetis_flexcan.h>
#include <Wire.h>
#include <TimeLib.h>
#include <Metro.h>
#include <XBTools.h>
#include <Adafruit_GPS.h>
#include <ADC_SPI.h>

#define XB Serial2
#define XBEE_PKT_LEN 15

#define BMS_HIGH 134 // ~3V on BMS_OK line
#define IMD_HIGH 134 // ~3V on OKHS line

FlexCAN CAN(500000);
static CAN_message_t msg_rx;
static CAN_message_t msg_tx;
static CAN_message_t xb_msg;
File logger;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_GPS GPS(&Serial1);
ADC_SPI ADC(A1, 1800000);

Metro timer_debug_mcu_status = Metro(2000);
Metro timer_debug_mcu_pedal_readings = Metro(200);
Metro timer_debug_bms_balancing_status = Metro(3000);
Metro timer_accelerometer = Metro(100);
Metro timer_current = Metro(500);
Metro timer_voltage = Metro(500);
Metro timer_debug_bms_status = Metro(1000);
Metro timer_debug_bms_temperatures = Metro(3000);
Metro timer_debug_bms_detailed_temperatures = Metro(3000);
Metro timer_debug_bms_voltages = Metro(1000);
Metro timer_debug_bms_detailed_voltages = Metro(3000);
Metro timer_debug_bms_coulomb_counts = Metro(1000);
Metro timer_debug_rms_command_message = Metro(200);
Metro timer_debug_rms_current_information = Metro(100);
Metro timer_debug_rms_fault_codes = Metro(2000);
Metro timer_debug_rms_internal_states = Metro(2000);
Metro timer_debug_rms_motor_position_information = Metro(100);
Metro timer_debug_rms_temperatures_1 = Metro(3000);
Metro timer_debug_rms_temperatures_3 = Metro(3000);
Metro timer_debug_rms_torque_timer_information = Metro(200);
Metro timer_debug_rms_voltage_information = Metro(100);
Metro timer_debug_tcu_wheel_rpm_rear = Metro(200);
Metro timer_debug_tcu_wheel_rpm_front = Metro(200);
Metro timer_debug_tcu_distance_traveled = Metro(200);
Metro timer_detailed_voltages = Metro(1000);
Metro timer_status_send = Metro(100);
Metro timer_status_send_xbee = Metro(2000);
Metro timer_gps = Metro(100);
Metro timer_debug_RTC = Metro(1000);
Metro timer_flush = Metro(1000);

MCU_status mcu_status;
MCU_pedal_readings mcu_pedal_readings;
GLV_current_readings current_readings;
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
MC_flux_weakening_output mc_flux_weakening_output;
MC_firmware_information mc_firmware_information;
MC_command_message mc_command_message;
MC_read_write_parameter_command mc_read_write_parameter_command;
MC_read_write_parameter_response mc_read_write_parameter_response;
FCU_accelerometer_values fcu_accelerometer_values;
MCU_GPS_readings mcu_gps_readings;
TCU_wheel_rpm tcu_wheel_rpm_front;
TCU_wheel_rpm tcu_wheel_rpm_rear;
TCU_distance_traveled tcu_distance_traveled;

static bool pending_gps_data;

void parse_can_message();
void write_to_SD(CAN_message_t *msg);
time_t getTeensy3Time();
void setup_accelerometer();
void process_accelerometer();
void process_current();
void process_gps();
int write_xbee_data();
void send_xbee();
void sd_date_time(uint16_t* date, uint16_t* time);

void setup() {
    /* Set up real-time clock */
    //Teensy3Clock.set(9999999999); // set time (epoch) at powerup  (COMMENT OUT THIS LINE AND PUSH ONCE RTC HAS BEEN SET!!!!)
    setSyncProvider(getTeensy3Time); // registers Teensy RTC as system time
    if (timeStatus() != timeSet) {
        Serial.println("RTC not set up - uncomment the Teensy3Clock.set() function call to set the time");
    } else {
        Serial.println("System time set to RTC");
    }

    /* Configure pins */
    pinMode(A12, INPUT); // Current sensor (cooling circuit)
    pinMode(A13, INPUT); // Current sensor (non-cooling circuit)

    /* Set up Serial, XBee and CAN */
    Serial.begin(115200);
    XB.begin(115200);
    FLEXCAN0_MCR &= 0xFFFDFFFF; // Enables CAN message self-reception
    CAN.begin();

    /* Set up accelerometer */
    setup_accelerometer();

    /* Set up GPS */
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // specify data to be received (minimum + fix)
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ); // set update rate (10Hz)
    GPS.sendCommand(PGCMD_ANTENNA); // report data about antenna

    /* Set up SD card */
    Serial.println("Initializing SD card...");
    SdFile::dateTimeCallback(sd_date_time); // Set date/time callback function
    if (!SD.begin(BUILTIN_SDCARD)) { // Begin Arduino SD API (Teensy 3.5)
        Serial.println("SD card failed or not present");
    }
    char filename[] = "data0000.CSV";
    for (uint8_t i = 0; i < 10000; i++) {
        filename[4] = i / 1000     + '0';
        filename[5] = i / 100 % 10 + '0';
        filename[6] = i / 10  % 10 + '0';
        filename[7] = i       % 10 + '0';
        if (!SD.exists(filename)) {
            logger = SD.open(filename, O_WRITE | O_CREAT); // Open file for writing
            break;
        }
        if (i == 9999) { // If all possible filenames are in use, print error
            Serial.println("All possible SD card log filenames are in use - please clean up the SD card");
        }
    }
    if (logger) {
        Serial.println("Successfully opened SD file");
    } else {
        Serial.println("Failed to open SD file");
    }
    logger.println("time,msg.id,msg.len,data"); // Print CSV heading to the logfile
    logger.flush();
}

void loop() {
    /* Process and log incoming CAN messages */
    parse_can_message();

    /* Send messages over XBee */
    send_xbee();

    /* Flush data to SD card occasionally */
    if (timer_flush.check()) {
        logger.flush(); // Flush data to disk (data is also flushed whenever the 512 Byte buffer fills up, but this call ensures we don't lose more than a second of data when the car turns off)
    }

    /* Print timestamp to serial occasionally */
    if (timer_debug_RTC.check()) {
        Serial.println(Teensy3Clock.get());
    }

    if (timer_voltage.check()) {
        process_glv_voltage();
    }

    /* Process accelerometer readings occasionally */
    if (timer_accelerometer.check()) {
        process_accelerometer(); 
    }

    /* Process current sensor readings occasionally */
    if (timer_current.check()) {
        process_current();  
    }

    /* Process GPS readings */
    GPS.read();
    if (GPS.newNMEAreceived()) {
        GPS.parse(GPS.lastNMEA());
        pending_gps_data = true;
    }
    if (timer_gps.check() && pending_gps_data) {
        process_gps();
    }
}

void parse_can_message() {
    while (CAN.read(msg_rx)) {
        write_to_SD(&msg_rx); // Write to SD card buffer (if the buffer fills up, triggering a flush to disk, this will take 8ms)

        // Identify received CAN messages and load contents into corresponding structs
        /*
        if (msg_rx.id == ID_MC_ANALOG_INPUTS_VOLTAGES)
            mc_analog_input_voltages.load(msg_rx.buf);
        if (msg_rx.id == ID_MC_DIGITAL_INPUT_STATUS)
            mc_digital_input_status.load(msg_rx.buf);
        */
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
            case ID_MCU_STATUS:                         mcu_status.load(msg_rx.buf);                        break;
            case ID_MCU_PEDAL_READINGS:                 mcu_pedal_readings.load(msg_rx.buf);                break;
            case ID_GLV_CURRENT_READINGS:               current_readings.load(msg_rx.buf);                  break;
            case ID_BMS_VOLTAGES:                       bms_voltages.load(msg_rx.buf);                      break;
            case ID_BMS_TEMPERATURES:                   bms_temperatures.load(msg_rx.buf);                  break;
            case ID_BMS_ONBOARD_TEMPERATURES:           bms_onboard_temperatures.load(msg_rx.buf);          break;
            case ID_BMS_STATUS:                         bms_status.load(msg_rx.buf);                        break;
            case ID_BMS_COULOMB_COUNTS:                 bms_coulomb_counts.load(msg_rx.buf);                break;
            case ID_CCU_STATUS:                         ccu_status.load(msg_rx.buf);                        break;
            case ID_MC_TEMPERATURES_1:                  mc_temperatures_1.load(msg_rx.buf);                 break;
            case ID_MC_TEMPERATURES_2:                  mc_temperatures_2.load(msg_rx.buf);                 break;
            case ID_MC_TEMPERATURES_3:                  mc_temperatures_3.load(msg_rx.buf);                 break;
            case ID_MC_MOTOR_POSITION_INFORMATION:      mc_motor_position_information.load(msg_rx.buf);     break;
            case ID_MC_CURRENT_INFORMATION:             mc_current_information.load(msg_rx.buf);            break;
            case ID_MC_VOLTAGE_INFORMATION:             mc_voltage_information.load(msg_rx.buf);            break;
            case ID_MC_INTERNAL_STATES:                 mc_internal_states.load(msg_rx.buf);                break;
            case ID_MC_FAULT_CODES:                     mc_fault_codes.load(msg_rx.buf);                    break;
            case ID_MC_TORQUE_TIMER_INFORMATION:        mc_torque_timer_information.load(msg_rx.buf);       break;
            case ID_MC_FLUX_WEAKENING_OUTPUT:       mc_flux_weakening_output.load(msg_rx.buf);      break;
            case ID_MC_FIRMWARE_INFORMATION:            mc_firmware_information.load(msg_rx.buf);           break;
            case ID_MC_COMMAND_MESSAGE:                 mc_command_message.load(msg_rx.buf);                break;
            case ID_MC_READ_WRITE_PARAMETER_COMMAND:    mc_read_write_parameter_command.load(msg_rx.buf);   break;
            case ID_MC_READ_WRITE_PARAMETER_RESPONSE:   mc_read_write_parameter_response.load(msg_rx.buf);  break;
            case ID_FCU_ACCELEROMETER:                  fcu_accelerometer_values.load(msg_rx.buf);          break;
            case ID_TCU_WHEEL_RPM_REAR:                 tcu_wheel_rpm_rear.load(msg_rx.buf);                break;
            case ID_TCU_WHEEL_RPM_FRONT:                tcu_wheel_rpm_front.load(msg_rx.buf);               break;
            case ID_TCU_DISTANCE_TRAVELED:              tcu_distance_traveled.load(msg_rx.buf);             break;            
        }
    }
}

void write_to_SD(CAN_message_t *msg) { // Note: This function does not flush data to disk! It will happen when the buffer fills or when the above flush timer fires
    logger.print(Teensy3Clock.get());
    logger.print("000,");
    logger.print(msg->id, HEX);
    logger.print(",");
    logger.print(msg->len);
    logger.print(",");
    for (int i = 0; i < msg->len; i++) {
        if (msg->buf[i] < 16) {
            logger.print("0");
        }
        logger.print(msg->buf[i], HEX);
    }
    logger.println();
}

time_t getTeensy3Time() {
    return Teensy3Clock.get();
}

void setup_accelerometer() {
    // Initialise the sensor
    if(!accel.begin()) {
        // There was a problem detecting the ADXL345 ... check your connections
        Serial.println("Sensor not detected!!!!!");
    } else {
        accel.setRange(ADXL345_RANGE_4_G);
    }
}

void process_accelerometer() {
    /* Get a new sensor event */
    sensors_event_t event;
    accel.getEvent(&event);
    
    /* Read accelerometer values into accelerometer class */
    fcu_accelerometer_values.set_values((uint8_t) (event.acceleration.x*100), (uint8_t) (event.acceleration.y*100), (uint8_t) (event.acceleration.z*100));
    
    /* Send message over XBee */
    fcu_accelerometer_values.write(xb_msg.buf);
    xb_msg.id = ID_FCU_ACCELEROMETER;
    xb_msg.len = sizeof(FCU_accelerometer_values);
    write_xbee_data();

    /* Send message over CAN */
    fcu_accelerometer_values.write(msg_tx.buf);
    msg_tx.id = ID_FCU_ACCELEROMETER;
    msg_tx.len = sizeof(FCU_accelerometer_values);
    CAN.write(msg_tx);

    /*
    Serial.print("\n\nACCELEROMETER DATA\n\n");
    Serial.print(event.acceleration.x); Serial.print(", ");
    Serial.print(event.acceleration.y); Serial.print(", ");
    Serial.print(event.acceleration.z); Serial.println("\n\n");
    */
}

void process_glv_voltage() {
  float glv_voltage_reading = ADC.read_adc(2);
  float glv_voltage_value = (((glv_voltage_reading/4096) * 5) * 55/12) + 0.14; //ADC->12V conversion + offset likely due to resistor values
  //Serial.print("GLV: ");
  //Serial.println(glv_voltage_value);

  mcu_status.set_glv_battery_voltage(glv_voltage_value * 100);

  mcu_status.write(msg_tx.buf);
  msg_tx.id = ID_MCU_STATUS;
  msg_tx.len = sizeof(MCU_status);
  CAN.write(msg_tx);

  mcu_status.write(xb_msg.buf);
  xb_msg.id = ID_MCU_STATUS;
  xb_msg.len = sizeof(MCU_status);
  write_xbee_data();  
}

void process_current() {
    //self derived
    double current_ecu = ((double)(analogRead(A13)-96))*0.029412;
    double current_cooling = ((double)(analogRead(A12)-96))*0.029412;
    //Serial.println(current_cooling);
    //Serial.println(current_ecu);
      
    current_readings.set_ecu_current_value((short)((int)(current_ecu*100)));
    current_readings.set_cooling_current_value((short)((int)(current_cooling*100)));

    // order of bytes of each value is in reverse: buf[1],buf[0] is x value, buf[3],buf[2] is y value, and etc.
    current_readings.write(msg_tx.buf);
    msg_tx.id = ID_GLV_CURRENT_READINGS;
    msg_tx.len = sizeof(GLV_current_readings);
    CAN.write(msg_tx);

    current_readings.write(xb_msg.buf);
    xb_msg.id = ID_GLV_CURRENT_READINGS;
    xb_msg.len = sizeof(GLV_current_readings);
    write_xbee_data();   
}

void process_gps() {
    mcu_gps_readings.set_latitude(GPS.latitude_fixed);
    mcu_gps_readings.set_longitude(GPS.longitude_fixed);
    //Serial.print("Latitude (x100000): ");
    //Serial.println(mcu_gps_readings_alpha.get_latitude());
    //Serial.print("Longitude (x100000): ");
    //Serial.println(mcu_gps_readings_alpha.get_longitude());
    msg_tx.id = ID_MCU_GPS_READINGS;
    msg_tx.len = sizeof(MCU_GPS_readings);
    CAN.write(msg_tx);

    pending_gps_data = false;
}

int write_xbee_data() {
    /*
     * DECODED FRAME STRUCTURE:
     * [ msg id (4) | msg len (1) | msg contents (8) | checksum (2) ]
     * ENCODED FRAME STRUCTURE:
     * [ fletcher (1) | msg id (4) | msg len (1) | msg contents (8) | checksum (2) | delimiter (1) ]
     */
    uint8_t xb_buf[XBEE_PKT_LEN];
    memcpy(xb_buf, &xb_msg.id, sizeof(xb_msg.id));        // msg id
    memcpy(xb_buf + sizeof(xb_msg.id), &xb_msg.len, sizeof(uint8_t));     // msg len
    memcpy(xb_buf + sizeof(xb_msg.id) + sizeof(uint8_t), xb_msg.buf, xb_msg.len); // msg contents

    // calculate checksum
    uint16_t checksum = fletcher16(xb_buf, XBEE_PKT_LEN - 2);
    //Serial.print("CHECKSUM: ");
    //Serial.println(checksum, HEX);
    memcpy(&xb_buf[XBEE_PKT_LEN - 2], &checksum, sizeof(uint16_t));

    for (int i = 0; i < XBEE_PKT_LEN; i++) {
        //Serial.print(xb_buf[i], HEX);
        //Serial.print(" ");
    }
    //Serial.println();

    uint8_t cobs_buf[2 + XBEE_PKT_LEN];
    cobs_encode(xb_buf, XBEE_PKT_LEN, cobs_buf);
    cobs_buf[XBEE_PKT_LEN+1] = 0x0;

    for (int i = 0; i < XBEE_PKT_LEN+2; i++) {
        //Serial.print(cobs_buf[i], HEX);
        //Serial.print(" ");
    }
    //Serial.println();

    int written = XB.write(cobs_buf, 2 + XBEE_PKT_LEN);
    //Serial.print("Wrote ");
    //Serial.print(written);
    //Serial.println(" bytes");

    memset(xb_buf, 0, sizeof(CAN_message_t));

    return written;
}

void send_xbee() {
    if (timer_debug_rms_temperatures_1.check()) {
        mc_temperatures_1.write(xb_msg.buf);
        xb_msg.len = sizeof(MC_temperatures_1);
        xb_msg.id = ID_MC_TEMPERATURES_1;
        write_xbee_data();
        /*XB.print("MODULE A TEMP: ");
        XB.println(mc_temperatures_1.get_module_a_temperature() / (double) 10, 1);
        XB.print("MODULE B TEMP: ");
        XB.println(mc_temperatures_1.get_module_b_temperature() / (double) 10, 1);
        XB.print("MODULE C TEMP: ");
        XB.println(mc_temperatures_1.get_module_c_temperature() / (double) 10, 1);
        XB.print("GATE DRIVER BOARD TEMP: ");
        XB.println(mc_temperatures_1.get_gate_driver_board_temperature() / (double) 10, 1);*/
    }

    if (timer_debug_rms_temperatures_3.check()) {
        mc_temperatures_3.write(xb_msg.buf);
        xb_msg.len = sizeof(MC_temperatures_3);
        xb_msg.id = ID_MC_TEMPERATURES_3;
        write_xbee_data();
        /*//XB.print("RTD 4 TEMP: "); // These aren't needed since we aren't using RTDs
        //XB.println(mc_temperatures_3.get_rtd_4_temperature());
        //XB.print("RTD 5 TEMP: ");
        //XB.println(mc_temperatures_3.get_rtd_5_temperature());
        XB.print("MOTOR TEMP: ");
        XB.println(mc_temperatures_3.get_motor_temperature() / (double) 10, 1);
        XB.print("TORQUE SHUDDER: ");
        XB.println(mc_temperatures_3.get_torque_shudder() / (double) 10, 1);*/
    }

    if (timer_debug_rms_motor_position_information.check()) {
        mc_motor_position_information.write(xb_msg.buf);
        xb_msg.len = sizeof(MC_motor_position_information);
        xb_msg.id = ID_MC_MOTOR_POSITION_INFORMATION;
        write_xbee_data();
        /*XB.print("MOTOR ANGLE: ");
        XB.println(mc_motor_position_information.get_motor_angle());
        XB.print("MOTOR SPEED: ");
        XB.println(mc_motor_position_information.get_motor_speed());
        XB.print("ELEC OUTPUT FREQ: ");
        XB.println(mc_motor_position_information.get_electrical_output_frequency());
        XB.print("DELTA RESOLVER FILT: ");
        XB.println(mc_motor_position_information.get_delta_resolver_filtered());*/
    }

    if (timer_debug_rms_current_information.check()) {
        mc_current_information.write(xb_msg.buf);
        xb_msg.len = sizeof(MC_current_information);
        xb_msg.id = ID_MC_CURRENT_INFORMATION;
        write_xbee_data();
        /*XB.print("PHASE A CURRENT: ");
        XB.println(mc_current_information.get_phase_a_current() / (double) 10, 1);
        XB.print("PHASE B CURRENT: ");
        XB.println(mc_current_information.get_phase_b_current() / (double) 10, 1);
        XB.print("PHASE C CURRENT: ");
        XB.println(mc_current_information.get_phase_c_current() / (double) 10, 1);
        XB.print("DC BUS CURRENT: ");
        XB.println(mc_current_information.get_dc_bus_current() / (double) 10, 1);*/
    }

    if (timer_debug_rms_voltage_information.check()) {
        mc_voltage_information.write(xb_msg.buf);
        xb_msg.len = sizeof(MC_voltage_information);
        xb_msg.id = ID_MC_VOLTAGE_INFORMATION;
        write_xbee_data();
        /*XB.print("DC BUS VOLTAGE: ");
        XB.println(mc_voltage_information.get_dc_bus_voltage() / (double) 10, 1);
        XB.print("OUTPUT VOLTAGE: ");
        XB.println(mc_voltage_information.get_output_voltage() / (double) 10, 1);
        XB.print("PHASE AB VOLTAGE: ");
        XB.println(mc_voltage_information.get_phase_ab_voltage() / (double) 10, 1);
        XB.print("PHASE BC VOLTAGE: ");
        XB.println(mc_voltage_information.get_phase_bc_voltage() / (double) 10, 1);*/
    }

    if (timer_debug_rms_internal_states.check()) {
        mc_internal_states.write(xb_msg.buf);
        xb_msg.len = sizeof(MC_internal_states);
        xb_msg.id = ID_MC_INTERNAL_STATES;
        write_xbee_data();
        /*XB.print("VSM STATE: ");
        XB.println(mc_internal_states.get_vsm_state());
        XB.print("INVERTER STATE: ");
        XB.println(mc_internal_states.get_inverter_state());
        XB.print("INVERTER RUN MODE: ");
        XB.println(mc_internal_states.get_inverter_run_mode());
        XB.print("INVERTER ACTIVE DISCHARGE STATE: ");
        XB.println(mc_internal_states.get_inverter_active_discharge_state());
        XB.print("INVERTER COMMAND MODE: ");
        XB.println(mc_internal_states.get_inverter_command_mode());
        XB.print("INVERTER ENABLE: ");
        XB.println(mc_internal_states.get_inverter_enable_state());
        XB.print("INVERTER LOCKOUT: ");
        XB.println(mc_internal_states.get_inverter_enable_lockout());
        XB.print("DIRECTION COMMAND: ");
        XB.println(mc_internal_states.get_direction_command());*/
    }

    if (timer_debug_rms_fault_codes.check()) {
        mc_fault_codes.write(xb_msg.buf);
        xb_msg.len = sizeof(MC_fault_codes);
        xb_msg.id = ID_MC_FAULT_CODES;
        write_xbee_data();
        /*XB.print("POST FAULT LO: 0x");
        XB.println(mc_fault_codes.get_post_fault_lo(), HEX);
        XB.print("POST FAULT HI: 0x");
        XB.println(mc_fault_codes.get_post_fault_hi(), HEX);
        XB.print("RUN FAULT LO: 0x");
        XB.println(mc_fault_codes.get_run_fault_lo(), HEX);
        XB.print("RUN FAULT HI: 0x");
        XB.println(mc_fault_codes.get_run_fault_hi(), HEX);*/
    }

    if (timer_debug_rms_torque_timer_information.check()) {
        mc_torque_timer_information.write(xb_msg.buf);
        xb_msg.len = sizeof(MC_torque_timer_information);
        xb_msg.id = ID_MC_TORQUE_TIMER_INFORMATION;
        write_xbee_data();
        /*XB.print("COMMANDED TORQUE: ");
        XB.println(mc_torque_timer_information.get_commanded_torque() / (double) 10, 1);
        XB.print("TORQUE FEEDBACK: ");
        XB.println(mc_torque_timer_information.get_torque_feedback());
        XB.print("RMS UPTIME: ");
        XB.println(mc_torque_timer_information.get_power_on_timer() * .003, 0);*/
    }

    if (timer_debug_bms_voltages.check()) {
        bms_voltages.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_voltages);
        xb_msg.id = ID_BMS_VOLTAGES;
        write_xbee_data();
        /*XB.print("BMS VOLTAGE AVERAGE: ");
        XB.println(bms_voltages.get_average() / (double) 10000, 4);
        XB.print("BMS VOLTAGE LOW: ");
        XB.println(bms_voltages.get_low() / (double) 10000, 4);
        XB.print("BMS VOLTAGE HIGH: ");
        XB.println(bms_voltages.get_high() / (double) 10000, 4);
        XB.print("BMS VOLTAGE TOTAL: ");
        XB.println(bms_voltages.get_total() / (double) 100, 2);*/
    }

    if (timer_debug_bms_detailed_voltages.check()) {
        for (int ic = 0; ic < 8; ic++) {
            for (int group = 0; group < 3; group++) {
                bms_detailed_voltages[ic][group].write(xb_msg.buf);
                xb_msg.len = sizeof(BMS_detailed_voltages);
                xb_msg.id = ID_BMS_DETAILED_VOLTAGES;
                write_xbee_data();
            }
        }
    }

    if (timer_debug_bms_temperatures.check()) {
        bms_temperatures.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_temperatures);
        xb_msg.id = ID_BMS_TEMPERATURES;
        write_xbee_data();
        /*XB.print("BMS AVERAGE TEMPERATURE: ");
        XB.println(bms_temperatures.get_average_temperature() / (double) 100, 2);
        XB.print("BMS LOW TEMPERATURE: ");
        XB.println(bms_temperatures.get_low_temperature() / (double) 100, 2);
        XB.print("BMS HIGH TEMPERATURE: ");
        XB.println(bms_temperatures.get_high_temperature() / (double) 100, 2);*/
    }

    if (timer_debug_bms_detailed_temperatures.check()) {
        for (int ic = 0; ic < 8; ic++) {
            bms_detailed_temperatures[ic].write(xb_msg.buf);
            xb_msg.len = sizeof(BMS_detailed_temperatures);
            xb_msg.id = ID_BMS_DETAILED_TEMPERATURES;
            write_xbee_data();
        }
    }

    if (timer_debug_bms_status.check()) {
        bms_status.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_status);
        xb_msg.id = ID_BMS_STATUS;
        write_xbee_data();
        /*XB.print("BMS STATE: ");
        XB.println(bms_status.get_state());
        XB.print("BMS ERROR FLAGS: 0x");
        XB.println(bms_status.get_error_flags(), HEX);
        XB.print("BMS CURRENT: ");
        XB.println(bms_status.get_current() / (double) 100, 2);*/
    }

    if (timer_debug_bms_coulomb_counts.check()) {
        bms_coulomb_counts.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_coulomb_counts);
        xb_msg.id = ID_BMS_COULOMB_COUNTS;
        write_xbee_data();
    }

    if (timer_debug_rms_command_message.check()) {
        mc_command_message.write(xb_msg.buf);
        xb_msg.len = sizeof(MC_command_message);
        xb_msg.id = ID_MC_COMMAND_MESSAGE;
        write_xbee_data();
        /*XB.print("CMD_MSG TORQUE COMMAND: ");
        XB.println(mc_command_message.get_torque_command() / (double) 10, 1);
        XB.print("CMD_MSG ANGULAR VELOCITY: ");
        XB.println(mc_command_message.get_angular_velocity());
        XB.print("CMD_MSG DIRECTION: ");
        XB.println(mc_command_message.get_direction());
        XB.print("CMD_MSG INVERTER ENABLE: ");
        XB.println(mc_command_message.get_inverter_enable());
        XB.print("CMD_MSG DISCHARGE ENABLE: ");
        XB.println(mc_command_message.get_discharge_enable());
        XB.print("CMD_MSG COMMANDED TORQUE LIMIT: ");
        XB.println(mc_command_message.get_commanded_torque_limit() / (double) 10, 1);*/
    }
    if (timer_debug_mcu_status.check()) {
        mcu_status.write(xb_msg.buf);
        xb_msg.len = sizeof(MCU_status);
        xb_msg.id = ID_MCU_STATUS;
        write_xbee_data();
    }

    if (timer_debug_mcu_pedal_readings.check()) {
        mcu_pedal_readings.write(xb_msg.buf);
        xb_msg.len = sizeof(MCU_pedal_readings);
        xb_msg.id = ID_MCU_PEDAL_READINGS;
        write_xbee_data();
    }
    
    if (timer_debug_bms_balancing_status.check()) {
        for (int i = 0; i < 2; i++) {
            bms_balancing_status[i].write(xb_msg.buf);
            xb_msg.len = sizeof(BMS_balancing_status);
            xb_msg.id = ID_BMS_BALANCING_STATUS;
            write_xbee_data();
        }
    }

    if (timer_debug_tcu_wheel_rpm_rear.check()) {
        tcu_wheel_rpm_rear.write(xb_msg.buf);
        xb_msg.len = sizeof(TCU_wheel_rpm);
        xb_msg.id = ID_TCU_WHEEL_RPM_REAR;
        write_xbee_data();
    }

    if (timer_debug_tcu_wheel_rpm_front.check()) {
        tcu_wheel_rpm_front.write(xb_msg.buf);
        xb_msg.len = sizeof(TCU_wheel_rpm);
        xb_msg.id = ID_TCU_WHEEL_RPM_FRONT;
        write_xbee_data();
    }

    if (timer_debug_tcu_distance_traveled.check()) {
        tcu_distance_traveled.write(xb_msg.buf);
        xb_msg.len = sizeof(TCU_distance_traveled);
        xb_msg.id = ID_TCU_DISTANCE_TRAVELED;
        write_xbee_data();
    }
}

void sd_date_time(uint16_t* date, uint16_t* time) {
    // return date using FAT_DATE macro to format fields
    *date = FAT_DATE(year(), month(), day());

    // return time using FAT_TIME macro to format fields
    *time = FAT_TIME(hour(), minute(), second());
}
