#include <ADC.h>

//initialize Teensy 16-bit ADC object
ADC *adc = new ADC();

#define axisX A2
#define axisY A3
#define axisZ A4 

//values to use for ADC value smoothing
const char num_measurements = 3;
unsigned short int reading[num_measurements] = {};
unsigned char counter = 0; //array index variable
int val[num_measurements] = {};
const char limit = 10;
unsigned short int vals[num_measurements][limit]= {{}};

void setup() {
  // put your setup code here, to run once:
  pinMode(axisX, INPUT);
  pinMode(axisY, INPUT);
  pinMode(axisZ,INPUT);
  pinMode(13, OUTPUT);

  //set up ADC_0
  adc->setAveraging(16,ADC_0); // set number of averages
  adc->setResolution(16,ADC_0); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS,ADC_0);
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_0); // change the sampling speed

  //set up ADC_1 if it exists
  #if ADC_NUM_ADCS >1
  adc->setAveraging(16, ADC_1); // set number of averages
  adc->setResolution(16, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS, ADC_1); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1); // change the sampling speed
  #endif
  Serial.begin(7600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  reading[0]=adc->analogRead(axisX);
  reading[1]=adc->analogRead(axisY);
  reading[2]=adc->analogRead(axisZ);

  //adjust the running total for each measurement by removing the nth previous measurement
  //as defined by the variable limit and store the current reading in the array as given by
  //by the variable counter
  for(int i = 0; i<num_measurements; i++){
    val[i]-=vals[i][counter];
    val[i]+=reading[i];
    vals[i][counter]=reading[i];  
  }
  //loop forward counter once per cycle
  counter++;
  if(counter == limit)
    counter = 0;

  double xAxis = (3.3*val[0]/limit/adc->getMaxValue()-1.655)*1.5105740181268882175226586102719*1000/6.67;
  double yAxis = (3.3*val[1]/limit/adc->getMaxValue()-1.655)*1.5105740181268882175226586102719*1000/6.67;
  double zAxis = (3.3*val[2]/limit/adc->getMaxValue()-1.655)*1.5105740181268882175226586102719*1000/6.67;

  Serial.printf("X axis: %f\t" ,xAxis);
  Serial.printf("Y axis: %f\t" ,yAxis);
  Serial.printf("Z axis: %f\n" ,zAxis);


  delay(5);
  
  
}
