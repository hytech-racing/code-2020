/*
 * Teensy 3.5 Telemetry Control Unit code
 * Written by Soohyun Kim, with assistance by Ryan Gallaway and Nathan Cheek.
 *
 * Rev 2 - 4/23/2019
 */

// #define HYTECH_LOGGING_EN
#define HYTECH_COMPRESSION_EN

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <kinetis_flexcan.h>
#include <Wire.h>
#include <Metro.h>
#include <XBTools.h>
#include <Adafruit_GPS.h>
#include <TimeLib.h>
#include <Telemetry.h>
#include <HyTech_SD.h>
#include <HT_CAN_Util.h>

#define XB Serial2
#define XBEE_PKT_LEN 15

#define BMS_HIGH 134 // ~3V on BMS_OK line
#define IMD_HIGH 134 // ~3V on OKHS line

HT_CAN_Util CAN(500000);
static CAN_message_t msg_rx;
static CAN_message_t xb_msg;
Hytech_SD hytech_SD;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_GPS GPS(&Serial1);

Telemetry telemetry;

BMS_balancing_status bms_balancing_status[2];
BMS_detailed_voltages bms_detailed_voltages[8][3];
BMS_detailed_temperatures bms_detailed_temperatures[8];
BMS_onboard_detailed_temperatures bms_onboard_detailed_temperatures[8];

Metro timer_current = Metro(500);
GLV_current_readings current_readings;

Metro timer_accelerometer = Metro(100);
FCU_accelerometer_values fcu_accelerometer_values;

Metro timer_gps = Metro(100);
MCU_GPS_readings_alpha mcu_gps_readings_alpha;
MCU_GPS_readings_beta mcu_gps_readings_beta;
MCU_GPS_readings_gamma mcu_gps_readings_gamma;

Metro timer_debug_RTC = Metro(1000);

static bool pending_gps_data;

time_t getTime() { return Teensy3Clock.get(); }

void setup() {
    setupTelemetry();

    /* Set up real-time clock */
    //Teensy3Clock.set(9999999999); // set time (epoch) at powerup  (COMMENT OUT THIS LINE AND PUSH ONCE RTC HAS BEEN SET!!!!)
    setSyncProvider(getTime); // registers Teensy RTC as system time
    Serial.println(timeStatus() == timeSet ? "System time set to RTC" : "RTC not set up - uncomment the Teensy3Clock.set() function call to set the time");

    /* Configure pins */
    pinMode(A12, INPUT); // Current sensor (cooling circuit)
    pinMode(A13, INPUT); // Current sensor (non-cooling circuit)

    /* Set up Serial, XBee and CAN */
    Serial.begin(115200);
    XB.begin(115200);
    FLEXCAN0_MCR &= 0xFFFDFFFF; // Enables CAN message self-reception
    CAN.begin();

    /* Set up accelerometer... check connections if problem occurs */
    if (accel.begin())
        accel.setRange(ADXL345_RANGE_4_G);
    else Serial.println("Sensor not detected!!!!!");

    /* Set up GPS */
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // specify data to be received (minimum + fix)
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ); // set update rate (10Hz)
    GPS.sendCommand(PGCMD_ANTENNA); // report data about antenna

    hytech_SD.initialize();
}

void loop() {
    if (timer_debug_RTC.check())    // Print timestamp to serial occasionally
        Serial.println(Teensy3Clock.get());

    parse_can_message();    // Process and log incoming CAN messages
    telemetry.write(write_xbee_data); // Send messages over XBee
    hytech_SD.flush();      // Empty SD buffer so we don't lose > 1s 

    if (timer_accelerometer.check())    // Process accelerometer readings occasionally
        process_accelerometer();

    if (timer_current.check()) // Process current sensor readings occasionally
        process_current();

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

void setupTelemetry() {
    telemetry.insert(ID_MCU_STATUS, new MCU_status(), new Metro(2000));
    telemetry.insert(ID_MCU_PEDAL_READINGS, new MCU_pedal_readings(), new Metro(200));
    telemetry.insert(ID_BMS_VOLTAGES, new BMS_voltages(), new Metro(1000));
    telemetry.insert(ID_BMS_TEMPERATURES, new BMS_temperatures(), new Metro(3000));
    telemetry.insert(ID_BMS_STATUS, new BMS_status(), new Metro(1000));
    telemetry.insert(ID_BMS_COULOMB_COUNTS, new BMS_coulomb_counts(), new Metro(1000));
    telemetry.insert(ID_MC_TEMPERATURES_1, new MC_temperatures_1(), new Metro(3000));
    telemetry.insert(ID_MC_TEMPERATURES_3, new MC_temperatures_3(), new Metro(3000));
    telemetry.insert(ID_MC_MOTOR_POSITION_INFORMATION, new MC_motor_position_information(), new Metro(100));
    telemetry.insert(ID_MC_CURRENT_INFORMATION, new MC_current_information(), new Metro(100));
    telemetry.insert(ID_MC_VOLTAGE_INFORMATION, new MC_voltage_information(), new Metro(100));
    telemetry.insert(ID_MC_INTERNAL_STATES, new MC_internal_states(), new Metro(2000));
    telemetry.insert(ID_MC_FAULT_CODES, new MC_fault_codes(), new Metro(2000));
    telemetry.insert(ID_MC_TORQUE_TIMER_INFORMATION, new MC_torque_timer_information(), new Metro(200));
    telemetry.insert(ID_MC_COMMAND_MESSAGE, new MC_command_message(), new Metro(200));
    telemetry.insert(ID_TCU_WHEEL_RPM_FRONT, new TCU_wheel_rpm(), new Metro(200));
    telemetry.insert(ID_TCU_WHEEL_RPM_REAR, new TCU_wheel_rpm(), new Metro(200));
    telemetry.insert(ID_TCU_DISTANCE_TRAVELED, new TCU_distance_traveled(), new Metro(200));
    telemetry.insert(ID_MCU_LAUNCH_CONTROL, new MCU_launch_control(), new Metro(200));
    telemetry.insert(ID_BMS_BALANCING_STATUS, new Abstract_CAN_Set<BMS_balancing_status>(&bms_balancing_status[0], 2), new Metro(3000));
    telemetry.insert(ID_BMS_DETAILED_VOLTAGES, new Abstract_CAN_Set<BMS_detailed_voltages>(&bms_detailed_voltages[0][0], 24), new Metro(3000));
    telemetry.insert(ID_BMS_DETAILED_TEMPERATURES, new Abstract_CAN_Set<BMS_detailed_temperatures>(&bms_detailed_temperatures[0], 8), new Metro(3000));
    telemetry.insert(ID_BMS_ONBOARD_DETAILED_TEMPERATURES, new Abstract_CAN_Set<BMS_onboard_detailed_temperatures>(&bms_onboard_detailed_temperatures[0], 8), new Metro(3000));
}

void parse_can_message() {
    while (CAN.read(msg_rx)) {
        if (telemetry.load(msg_rx.id, msg_rx.buf));
            hytech_SD.write(&msg_rx); // Write to SD card buffer (if the buffer fills up, triggering a flush to disk, this will take 8ms)

        // Identify received CAN messages and load contents into corresponding structs
        if (msg_rx.id == ID_GLV_CURRENT_READINGS && current_readings.load(msg_rx.buf))
            hytech_SD.write(&msg_rx);
        if (msg_rx.id == ID_FCU_ACCELEROMETER && fcu_accelerometer_values.load(msg_rx.buf))
            hytech_SD.write(&msg_rx);
    }
}

void process_accelerometer() {
    /* Get a new sensor event */
    sensors_event_t event;
    accel.getEvent(&event);

    /* Read accelerometer values into accelerometer class */
    fcu_accelerometer_values.set_values((uint8_t) (event.acceleration.x*100), (uint8_t) (event.acceleration.y*100), (uint8_t) (event.acceleration.z*100));

    write_xbee_data(ID_FCU_ACCELEROMETER, &fcu_accelerometer_values); // Send message over XBee
    CAN.write(ID_FCU_ACCELEROMETER, &fcu_accelerometer_values); // Send message over CAN
}

void process_current() {
    double current_ecu = ((double)(analogRead(A13)-96))*0.029412;
    double current_cooling = ((double)(analogRead(A12)-96))*0.029412;
    //Serial.println(current_cooling);
    //Serial.println(current_ecu);

    // order of bytes of each value is in reverse: buf[1],buf[0] is x value, buf[3],buf[2] is y value, and etc.
    current_readings.set_ecu_current_value((short)((int)(current_ecu*100)));
    current_readings.set_cooling_current_value((short)((int)(current_cooling*100)));

    write_xbee_data(ID_GLV_CURRENT_READINGS, &current_readings);
    CAN.write(ID_GLV_CURRENT_READINGS, &current_readings);
}

void process_gps() {
    mcu_gps_readings_alpha.set_latitude(GPS.latitude * 10000);
    mcu_gps_readings_alpha.set_longitude(GPS.longitude * 10000);
    CAN.write(ID_MCU_GPS_READINGS_ALPHA, &mcu_gps_readings_alpha);

    mcu_gps_readings_beta.set_altitude(GPS.altitude * 10000);
    mcu_gps_readings_beta.set_speed(GPS.speed * 10000);
    CAN.write(ID_MCU_GPS_READINGS_BETA, &mcu_gps_readings_beta);

    mcu_gps_readings_gamma.set_fix_quality(GPS.fixquality);
    mcu_gps_readings_gamma.set_satellite_count(GPS.satellites);
    TimeElements tm = {Second : GPS.seconds, Minute : GPS.minute, Hour : GPS.hour, Wday : 0, Day : GPS.day, Month : GPS.month, Year : GPS.year};
    mcu_gps_readings_gamma.set_timestamp_seconds((uint32_t) makeTime(tm));
    mcu_gps_readings_gamma.set_timestamp_milliseconds(GPS.milliseconds);
    CAN.write(ID_MCU_GPS_READINGS_GAMMA, &mcu_gps_readings_gamma);

    pending_gps_data = false;
}

void write_xbee_data(uint32_t id, Interface_CAN_Container* can) {
    xb_msg.id = id;
    can->write(xb_msg.buf);
    xb_msg.len = can->messageLen();

    // if (id == "<SOME_ID_TO_DEBUG>")
    //     can->print(XB);

    /*
     * DECODED FRAME STRUCTURE: [ msg id (4) | msg len (1) | msg contents (8) | checksum (2) ]
     * ENCODED FRAME STRUCTURE: [ fletcher (1) | msg id (4) | msg len (1) | msg contents (8) | checksum (2) | delimiter (1) ]
     */
    uint8_t xb_buf[XBEE_PKT_LEN];
    memcpy(xb_buf, &xb_msg.id, sizeof(xb_msg.id));        // msg id
    memcpy(xb_buf + sizeof(xb_msg.id), &xb_msg.len, sizeof(uint8_t));     // msg len
    memcpy(xb_buf + sizeof(xb_msg.id) + sizeof(uint8_t), xb_msg.buf, xb_msg.len); // msg contents

    // calculate checksum
    uint16_t checksum = fletcher16(xb_buf, XBEE_PKT_LEN - 2);
    //Serial.print("CHECKSUM: "); Serial.println(checksum, HEX);

    memcpy(&xb_buf[XBEE_PKT_LEN - 2], &checksum, sizeof(uint16_t));
    // hexPrint(xb_buf, XBEE_PKT_LEN);

    uint8_t cobs_buf[2 + XBEE_PKT_LEN];
    cobs_encode(xb_buf, XBEE_PKT_LEN, cobs_buf);
    cobs_buf[XBEE_PKT_LEN+1] = 0x0;
    // hexPrint(cobs_buf, XBEE_PKT_LEN+2);

    #ifdef HYTECH_LOGGING_EN
        int written = XB.write(cobs_buf, 2 + XBEE_PKT_LEN);
        Serial.print("Wrote ");
        Serial.print(written);
        Serial.println(" bytes");
    #else
        XB.write(cobs_buf, 2 + XBEE_PKT_LEN);
    #endif

    memset(xb_buf, 0, sizeof(CAN_message_t));
}

// void printHex(uint8_t *buf, int len) {
    // for (int i = 0; i < len; i++) {
        //Serial.print(buf[i], HEX);
        //Serial.print(" ");
    // }
    //Serial.println();
// })