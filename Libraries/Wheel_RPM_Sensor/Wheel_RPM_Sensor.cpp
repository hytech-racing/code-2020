#include <Wheel_RPM_Sensor.h>

Wheel_RPM_Sensor::read() {
  volatile byte newStateLeft = digitalRead(pin_left);
  volatile byte newStateRight = digitalRead(pin_right);
  int currentTime = micros();

  if (!newStateLeft && stateLeft) {
    int elapsed = currentTime - timeLeft;
    if (elapsed > 500) {
      rpmLeft = 60e6 / (microsElapsed * ROTARY_ENCODER_TEETH);
      timeLeft = currentTime;
    }
  }
  if (!newStateRight && prevStateRight) {
    int elapsed = currentTime - timeRight;
    if (elapsed > 500) {
      rpmRight = 60e6 / (microsElapsed * ROTARY_ENCODER_TEETH);
    }
  }

  stateLeft = newStateLeft;
  stateRight = newStateRight;
}
