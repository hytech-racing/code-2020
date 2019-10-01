#define in_pin A3
 
void setup() {
  // put your setup code here, to run once:
 
  pinMode(in_pin, INPUT);
  Serial.begin(115200);
  
}

void loop() {
  int value = analogRead(in_pin);
  Serial.println(value);
  double ratio = value / 1023.00 ;
  double r2 = ratio * 10000 / (1 - ratio);
  Serial.println(r2);
  double degKCalc = (3950 * 298.15)/(298.15*log(r2/10000)+3950); 
  double degCCalc = degKCalc - 273.15;
  double adjustment = -0.0000000000069136 * pow(degCCalc,6) + 0.0000000018528289 * pow(degCCalc,5) -0.0000001759253389 * pow(degCCalc,4) 
    + 0.0000127535398116 * pow(degCCalc,3) -0.0014376940338028 * pow(degCCalc,2) +0.0763055743553628 *degCCalc -1.1574790782205100;
  double degC = degCCalc + adjustment;
  Serial.println(degC);
  Serial.println();
  delay(1000);
}
