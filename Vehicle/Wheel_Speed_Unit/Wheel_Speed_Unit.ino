#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <Metro.h>

Metro timer_can_update_fast = Metro(100);

/**
 * CAN Variables
 */
FlexCAN CAN(500000);
const CAN_filter_t can_filter_ccu_status = {0, 0, ID_CCU_STATUS}; // Note: If this is passed into CAN.begin() it will be treated as a mask. Instead, pass it into CAN.setFilter(), making sure to fill all slots 0-7 with duplicate filters as necessary
static CAN_message_t tx_msg;

TCU_wheel_rpm tcu_wheel_rpm;

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
bool high = false;

int numTeeth = 24;//CHANGE THIS FOR #OF TEETH PER REVOLUTION
void setup()
{
  pinMode(15, INPUT);
  pinMode(10, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(9600);

  CAN.begin();
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

void updateWheelSpeeds() {
  setStates();
  
  if (curStateLeft == 0 && prevStateLeft == 1) {
    setRPMLeft();
    if(high) {
       high = false;
    } else {
       high = true;
    }  
  }

  if (curStateRight == 0 && prevStateRight == 1) {
    setRPMRight();
    if(high) {
       high = false;
    } else {
       high = true;
    }  
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

void loop()
{
  updateWheelSpeeds();
  
  if (timer_can_update_fast.check()) {
        tx_msg.timeout = 10; // Use blocking mode, wait up to ?ms to send each message instead of immediately failing (keep in mind this is slower)

        //tcu_wheel_rpm.set_wheel_rpm_left(rpmLeft);
        //tcu_wheel_rpm.set_wheel_rpm_right(rpmRight);

        tcu_wheel_rpm.set_wheel_rpm_left(1234);
        tcu_wheel_rpm.set_wheel_rpm_right(4321);
        Serial.println("Can sent");
        
        tcu_wheel_rpm.write(tx_msg.buf);
        tx_msg.id = ID_TCU_WHEEL_RPM;
        tx_msg.len = sizeof(CAN_message_tcu_wheel_rpm_t);
        CAN.write(tx_msg);

        tx_msg.timeout = 0;
    }
}
