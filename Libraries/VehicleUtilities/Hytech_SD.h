#include <SD.h>
#include <TimeLib.h>
#include <HyTech_FlexCAN.h>
#include <Metro.h>

void sd_date_time(uint16_t* date, uint16_t* time) {
    // return date using FAT_DATE macro to format fields
    *date = FAT_DATE(year(), month(), day());

    // return time using FAT_TIME macro to format fields
    *time = FAT_TIME(hour(), minute(), second());
}

class Hytech_SD {
    private:
        File logger;
        Metro timer_flush;
    public:
        Hytech_SD();
        void write(CAN_message_t *msg);
        void initialize();
        void flush();
};

/* Set up SD card */
Hytech_SD::Hytech_SD() : timer_flush(1000) {}

void Hytech_SD::initialize() {
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

void Hytech_SD::write(CAN_message_t *msg) { // Note: This function does not flush data to disk! It will happen when the buffer fills or when the above flush timer fires
    logger.print(Teensy3Clock.get());
    logger.print(",");
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

void Hytech_SD::flush() {
    if (timer_flush.check())    // Flush data to SD card occasionally
        logger.flush(); // (data is also flushed whenever the 512B buffer fills up, but this call ensures we don't lose more than a second of data when the car turns off)
}