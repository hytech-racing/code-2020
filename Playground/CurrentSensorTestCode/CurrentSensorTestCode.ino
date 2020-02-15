#define REFERENCE 3.3 // define reference voltage
#define CURRENT_LIMIT 100 // define current limit
#define ADC_RESOLUTION 1023 // define adc resolution
const float slope = (REFERENCE * 0.9 - REFERENCE * 0.1) / (2 * CURRENT_LIMIT); // define the change in output voltage per unit current

void setup() {
  pinMode(14, INPUT); // use pin 14 to read the output of current sensor
  Serial.begin(9600);
}

void loop() {
  float voltage = analogRead(14) * REFERENCE / ADC_RESOLUTION; // read output of current sensor
  float current = -1 * CURRENT_LIMIT + (voltage - REFERENCE * 0.1) / slope; // calculate current value
  Serial.println(current);
  delay(500);
}
