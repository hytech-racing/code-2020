void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  Serial.begin(115200);
  
}

void loop() {
  int value = analogRead(A0);
  Serial.println(value);
  double ratio = value / 1023.00 ;
  double r2 = ratio * 10000 / (1 - ratio);
  Serial.println(r2);
  double r2_kohm = r2/1000;
  double x = sqrt(r2_kohm);
  double degC = 0.06294428957 * pow(x,6) + -1.35233601803 * pow(x,5)+ 11.86041794496 * pow(x,4) + -54.88983094873 * pow(x,3) + 145.54314183963 * pow(x,2) + -232.11806564651 *x + 217.92160567593;
  Serial.println(degC);
  Serial.println();
  delay(1000);
}
