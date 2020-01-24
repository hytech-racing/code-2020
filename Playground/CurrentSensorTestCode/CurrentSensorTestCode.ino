#define REFERENCE 3.3 // define reference voltage
const float slope = (REFERENCE * 0.9 - REFERENCE * 0.1) / 200; // define the change in output voltage per unit current

void setup() {
  pinMode(14, INPUT); // use pin 14 to read the output of current sensor
  Serial.begin(9600);
}

void loop() {
  float voltage = analogRead(14)*REFERENCE/1023; // read output of current sensor
  float current = -100 + (voltage - REFERENCE * 0.1) / slope; // calculate current value
  Serial.println(current);
  delay(1000); 
}
