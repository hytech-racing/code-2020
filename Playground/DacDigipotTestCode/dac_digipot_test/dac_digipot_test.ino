#include <Arduino.h>
#include <SPI.h>

#include <AD5721.h>
#include <AD5684.h>
#include <AD5263.h>

AD5721 dac12v(10);
AD5684 dac5v(6, 5);
AD5263 digipot(9);


void setup() {
  dac12v.init();
  dac5v.init();
  digipot.init();
  Serial.begin(115200);
}


void waitForSerial() {
  while(!Serial.available());
  Serial.flush();
}

//testing loop, send data over serial to continue to the next testing line
void loop() {
  dac12v.writeAndUpdate(0); //0 V
  waitForSerial();
  dac12v.writeAndUpdate(4095); //12V
  waitForSerial();
  dac12v.writeAndUpdate(2048); //6V
  waitForSerial();
  dav12v.dataReset(); //reset
  waitForSerial();


  dac5v.writeAndUpdate(15, 0); //all to 0V
  waitForSerial();
  dac5v.writeAndUpdate(3, 4095); //A and B to 5V
  dac5v.writeAndUpdate(12, 2048); //C and D to 2.5V
  waitForSerial();
  dac5v.reset();
  waitForSerial();
  
  digipot.writeAndUpdate(0, 255); //200 on 1 A to W
  digipot.writeAndUpdate(1, 128); // 10k on 2 A to W
  digipot.writeAndUpdate(2, 1); //20k on 3 A to W
  digipot.writeAndUpdate(3, 75); //~14k on 4 A to W
  waitForSerial();
  digitpot.writeAndUpdate(0, 255); //200 on all A to W
  digitpot.writeAndUpdate(1, 255);
  digitpot.writeAndUpdate(2, 255);
  digitpot.writeAndUpdate(3, 255);
  waitForSerial();

  //now you can connect the shutdown jumper and see what that does to the potentiometers
  
}
