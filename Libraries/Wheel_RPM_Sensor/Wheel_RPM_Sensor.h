/*
* Copyright (C) 2008 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software< /span>
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/* Update by K. Townsend (Adafruit Industries) for lighter typedefs, and
 * extended sensor support to include color, voltage and current */

#ifndef _WHEEL_SENSOR_H
#define _WHEEL_SENSOR_H

class Wheel_RPM_Sensor {
 public:
   static const int PIN_WHEEL_INPUT_FRONT_LEFT  = 0; // TODO: Assign actual pin numbers
   static const int PIN_WHEEL_INPUT_FRONT_RIGHT = 0;
   static const int PIN_WHEEL_INPUT_REAR_LEFT   = 0;
   static const int PIN_WHEEL_INPUT_REAR_RIGHT  = 0;

  // Constructor(s)
  Wheel_RPM_Sensor(int can, int left, int right) {
    CAN_ID = can;
    pin_left = left;
    pin_right = right;

    pinMode(left, INPUT);
    pinMode(right, INPUT);
  }

  void read();
  int get_can_id() { return CAN_ID };
  uint16_t get_rpm_left() { return rpmLeft; }
  uint16_t get_rpm_right() { return rpmRight; }

 private:
  static const int ROTARY_ENCODER_TEETH = 24;
  int CAN_ID;
  int pin_left;
  int pin_right;

  volatile byte stateLeft = 0;
  volatile byte stateRight = 0;

  int timeLeft = 0;
  int timeRight = 0;

  uint16_t rpmLeft = 0;
  uint16_t rpmRight = 0;
};

#endif
