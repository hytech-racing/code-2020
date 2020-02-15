#include <SPI.h>
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <Metro.h>
/*
 * Serial Clock frequency 5 MHz
 * Chip Select low to read, high to update
 * 14 clock cycles for thermocouple temp
 * can get internal temp too
 * https://datasheets.maximintegrated.com/en/ds/MAX31855.pdf
 */

Metro timer_can_update_fast = Metro(100);

FlexCAN CAN(500000);
static CAN_message_t tx_msg;

Thermocouple_reading thermocouple_reading;

const int slaveSelectPlug1 = 8;
const int slaveSelectPlug2 = 9;
const int slaveSelectStraight = 11;

const int clockSpeed = 5000000;

float temps[3];
int sensors[3];

void setup() {
  Serial.begin(115200);
  pinMode(slaveSelectPlug1, OUTPUT);
  pinMode(slaveSelectPlug2, OUTPUT);
  pinMode(slaveSelectStraight, OUTPUT);
  digitalWrite(slaveSelectPlug1, HIGH);
  digitalWrite(slaveSelectPlug2, HIGH);
  digitalWrite(slaveSelectStraight, HIGH);

 // float temps[3]; //does this float arithmetic work? i don't think so
  sensors[0] = 8;
  sensors[1] = 9;
  sensors[2] = 11;

  SPI.begin();
  CAN.begin();
}

//how does of the digital output convert to a float?
//first bit sign, then MSB 2^10 down to LSB 2^-2

void loop() {
  for (int i = 0; i < 2; i++) {
    SPI.beginTransaction(SPISettings(clockSpeed,MSBFIRST, SPI_MODE0));
    digitalWrite(sensors[i], LOW);
    byte firstByte = SPI.transfer(255); //receive the first 8 bits. should 255 be 0? last 7 bits are numbers (first sign)
    byte secondByte = SPI.transfer(255); //receive the second 8 bits. first 6 are part of the number (first 4 positive exponent,last 2 neg)
    byte firstPart = firstByte & 0x7F; //the last 7 bits, ignoring the sign (should be positive)
    byte secondPart = (secondByte >> 4) & 0xF; //first 4 bits
    int total = firstPart;
    total = (total << 4) | secondPart; //concatenate the two parts
    float totalDec = (float) total; //convert to float
    byte negExpo = (secondByte >> 2) & 0b11;
    if (negExpo & 0x2) totalDec += 0.5;
    if (negExpo & 0x1) totalDec += 0.25;
    temps[i] = totalDec;
    digitalWrite(sensors[i], HIGH);
    SPI.endTransaction();
  }
  Serial.printf("Temp 1: %f\n", temps[0]);
  Serial.printf("Temp 2: %f\n", temps[1]);

  uint16_t temp1ToWrite = (uint16_t) (temps[0] * 100);       //convert from float to uint16
  uint16_t temp2ToWrite = (uint16_t) (temps[1] * 100);       //convert from float to uint16
  Serial.printf("temp1TOWRITE: %d\n", temp1ToWrite);

  tx_msg.timeout = 4;

  thermocouple_reading.set_reading_plug_1(temp1ToWrite);
  thermocouple_reading.set_reading_plug_2(temp2ToWrite);
  Serial.println("Can sent");

  thermocouple_reading.write(tx_msg.buf);
  tx_msg.id = ID_THERMOCOUPLE_READING;
  tx_msg.len = sizeof(CAN_message_thermocouple_reading_t);
  CAN.write(tx_msg);

  delay(100);
  
  /*
  if (timer_can_update_fast.check()) { //for some reason, putting it in this loop makes the temp print wrong?
      Serial.printf("Temp 1: %f\n", temps[0]);
      Serial.printf("Temp 2: %f\n", temps[1]);
      uint16_t temp1ToWrite = (uint16_t) (temps[0] * 100);       //convert from float to uint16
      uint16_t temp2ToWrite = (uint16_t) (temps[1] * 100);       //convert from float to uint16

      tx_msg.timeout = 4;

      thermocouple_reading.set_reading_plug_1(temp1ToWrite);
      thermocouple_reading.set_reading_plug_2(temp2ToWrite);
      Serial.println("Can sent");

      thermocouple_reading.write(tx_msg.buf);
      tx_msg.id = ID_THERMOCOUPLE_READING;
      tx_msg.len = sizeof(CAN_message_thermocouple_reading_t);
      CAN.write(tx_msg);

      tx_msg.timeout = 0;
  }*/

}
