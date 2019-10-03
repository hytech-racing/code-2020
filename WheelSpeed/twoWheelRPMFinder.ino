volatile byte curStateLeft = 0;
volatile byte curStateRight = 0;
volatile byte prevStateLeft = 0;
volatile byte prevStateRight = 0;
int curTimeLeft = 0;
int curTimeRight = 0;
int prevTimeLeft = 0;
int prevTimeRight = 0;
double rpmLeft = 0;
double rpmRight = 0;

int numTeeth = 24;//CHANGE THIS FOR #OF TEETH PER REVOLUTION
void setup()
{
  pinMode(15, INPUT);
  pinMode(10, INPUT);
  Serial.begin(9600);
}

void setStates() {
  curStateLeft = digitalRead(15);
  curStateRight = digitalRead(10);
}

void setRPMLeft() {
  curTimeLeft = micros();
  int microsElapsed = curTimeLeft - prevTimeLeft;

  if (microsElapsed > 500) {
    rpmLeft = (60.0 * 1000.0 * 1000.0) / (microsElapsed * 24.0);
    prevTimeLeft = curTimeLeft;

    Serial.print("RPM Left: ");
    Serial.print(rpmLeft);
    Serial.print("    RPM Right: ");
    Serial.println(rpmRight);
  }
}

void setRPMRight() {
  curTimeRight = micros();
  int microsElapsed = curTimeRight - prevTimeRight;

  if (microsElapsed > 500) {
    rpmRight = (60.0 * 1000.0 * 1000.0) / (microsElapsed * 24.0);
    prevTimeRight = curTimeRight;

    Serial.print("RPM Left: ");
    Serial.print(rpmLeft);
    Serial.print("    RPM Right: ");
    Serial.println(rpmRight);
  }
}

void loop()
{
  setStates();
  
  if (curStateLeft == 0 && prevStateLeft == 1) {
    setRPMLeft();    
  }

  if (curStateRight == 0 && prevStateRight == 1) {
    setRPMRight();    
  }

  if (micros() - prevTimeLeft > 500000) {
    if (rpmLeft != 0) {
      rpmLeft = 0;
      Serial.print("RPM Left: ");
      Serial.print(rpmLeft);
      Serial.print("    RPM Right: ");
      Serial.println(rpmRight);
    }
  }

  if (micros() - prevTimeRight > 500000) {
    if (rpmRight != 0) {
      rpmRight = 0;
      Serial.print("RPM Left: ");
      Serial.print(rpmLeft);
      Serial.print("    RPM Right: ");
      Serial.println(rpmRight);
    }
  }

  prevStateLeft = curStateLeft;
  prevStateRight = curStateRight;
}
