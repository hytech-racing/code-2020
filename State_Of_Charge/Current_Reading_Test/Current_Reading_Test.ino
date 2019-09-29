const double slope = 0.9768;


void setup() {
  // put your setup code here, to run once:
  pinMode(A3, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(15)* 500.0 / 1024);
  
  delay(500);
}
