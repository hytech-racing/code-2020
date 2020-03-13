
#include <ADC.h>

//initialize Teensy 16-bit ADC object
ADC *adc = new ADC();

//define physical constants for converting voltage to force
const double A = .05112942;
const double E = 2.97*pow(10,7);
const double G = 2.0;
const double R = 350;
const double Vs = 3.290;

//values to use for ADC value smoothing
const int num_measurements = 5;
unsigned short int reading[num_measurements] = {};
unsigned int counter = 0; //array index variable
int val[num_measurements] = {};
const int limit = 10;
unsigned short int vals[num_measurements][limit]= {{}};

//pins
const byte half_v_pin = 0;
const byte wb_ti_pin = A6;
const byte sg_ti_pin = A9;
const byte wb_pj_pin = A0;
const byte sg_pj_pin = A3; 
const byte led = 13;
bool state;

void setup() {
  // put your setup code here, to run once:
  pinMode(half_v_pin, INPUT);
  pinMode(wb_ti_pin, INPUT);
  pinMode(sg_ti_pin,INPUT);
  pinMode(wb_pj_pin,INPUT);
  pinMode(sg_pj_pin,INPUT);
  pinMode(led, OUTPUT);

  //set up ADC_0
  adc->setAveraging(16,ADC_0); // set number of averages
  adc->setResolution(16,ADC_0); // set bits of resolution
  //adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS,ADC_0);
  //adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_0); // change the sampling speed

  //set up ADC_1 if it exists
  #if ADC_NUM_ADCS >1
  adc->setAveraging(16, ADC_1); // set number of averages
  adc->setResolution(16, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS, ADC_1); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1); // change the sampling speed
  #endif

  state=true;
  Serial.begin(9600);



}

void loop() {
  // put your main code here, to run repeatedly:

  //read the values for each signal
  reading[0]=adc->analogRead(half_v_pin);
  reading[1]=adc->analogRead(wb_ti_pin);
  reading[2]=adc->analogRead(sg_ti_pin);
  reading[3]=adc->analogRead(wb_pj_pin);
  reading[4]=adc->analogRead(sg_pj_pin);
  Serial.println("Time:");
  Serial.println(millis());
  Serial.println("Reading: ");
  Serial.println(reading[3]);

  //adjust the running total for each measurement by removing the nth previous measurement
  //as defined by the variable limit and store the current reading in the array as given by
  //by the variable counter
  for(int i = 0; i<num_measurements; i++){
    val[i]-=vals[i][counter];
    val[i]+=reading[i];
    //Serial.println(val[i]);
    vals[i][counter]=reading[i];  
  }
  Serial.println("Val[2]:");
  Serial.println(val[3]/limit);
  //loop forward counter once per cycle
  counter++;
  if(counter == limit){
  counter = 0;
    if(state){
    digitalWrite(led,state);
    state=false;
  } else{
    digitalWrite(led,state);
    state=true;
  }
  }

  //convert the 16-bit value into the voltage and then adjust by the gain to get the real voltage
  double half_vV = 1.637; //Vs*(val[0]/limit)/adc->getMaxValue();
  double wb_ti_V = (Vs*(val[1]/limit)/adc->getMaxValue()-half_vV)/230;
  double sg_ti_V = (Vs*(val[2]/limit)/adc->getMaxValue()-half_vV)/461;
  double wb_pj_V = Vs*(val[3]/limit)/adc->getMaxValue()/1001;
  double sg_pj_V = Vs*(val[4]/limit)/adc->getMaxValue()/1001;
//  Serial.print("Voltage: ");
//  Serial.println(sg_ti_V);

  //get resistance change from voltage value
  double wb_ti_dR=7900*wb_ti_V/(3.3-wb_ti_V);
  double sg_ti_dR=700 *sg_ti_V/(1.65-sg_ti_V);
  double wb_pj_dR=(2.0 *R*wb_pj_V)/(Vs-wb_pj_V);
  double sg_pj_dR=(-2.0*R*sg_pj_V)/(sg_pj_V+Vs/2);

  //convert change in resistance to force
  double wb_ti_F = E*A*wb_ti_dR/R/G;
  double sg_ti_F = E*A*sg_ti_dR/R/G;
  double wb_pj_F = E*A*wb_pj_dR/R/G;
  double sg_pj_F = E*A*sg_pj_dR/R/G;
  Serial.println("Force:");
  Serial.println(sg_ti_F);
  Serial.println();
  delay(50);

}
