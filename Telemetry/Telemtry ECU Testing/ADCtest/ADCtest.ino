#include <ADC.h>
//#include "C:\\Program Files (x86)\\Arduino\\hardware\\arduino\\avr\\cores\\arduino\\Arduino\\wiring_analog.c"

ADC *adc = new ADC();
//Arduino *ard = new Arduino();

unsigned short int value2;
unsigned char counter = 0;
int val =0;
const int limit = 100;
unsigned short int vals [limit] = {};

void setup() {
  // put your setup code here, to run once:
  pinMode(16,INPUT);
  Serial.begin(9600);

//  adc->setAveraging(16,ADC_0); // set number of averages
//  adc->setResolution(16,ADC_0); // set bits of resolution
//  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS,ADC_0);
//  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_0); // change the sampling speed

  #if ADC_NUM_ADCS >1
  adc->setAveraging(16, ADC_1); // set number of averages
  adc->setResolution(16, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS, ADC_1); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1); // change the sampling speed
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println(analogRead(16));

//    value = adc->analogRead(readPin); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.

//    Serial.print("Pin: ");
//    Serial.print(readPin);
//    Serial.print(", value ADC0: ");
//    Serial.println(value*3.3/adc->getMaxValue(ADC_0), DEC);

    #if ADC_NUM_ADCS>1
    value2 = adc->analogRead(16, ADC_1);
//
//    Serial.print("Pin: ");
//    Serial.print(16);
//    Serial.print(", value ADC1: ");
//    Serial.println(value2);
//    Serial.print(", ");
//    Serial.println(value2*3.3/adc->getMaxValue(ADC_1), DEC);
    val-=vals[counter];
    val+=value2;
    vals[counter]=value2;
    counter++;
    if(counter == limit){
    counter = 0;
    }
//    Serial.println("\n");
    Serial.println(val/double(limit));
    
    
//    Serial.print(analogRead2(16)*3.3/1023.0);
//    Serial.println(")");
    #endif

}
