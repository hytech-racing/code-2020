#include <Metro.h>

// print frequency every 100 ms
Metro printer(100);

#define FRONT_LEFT_WHEEL 10
#define FRONT_RIGHT_WHEEL 15
#define TIME_OUT 500000

float rpm_front_left{};
float rpm_front_right{};

void setup() {
  // put your setup code here, to run once:
  pinMode(FRONT_RIGHT_WHEEL, INPUT);
  pinMode(FRONT_LEFT_WHEEL, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(9600);
  Serial.println("Starting up");
}

void loop() {
  read_wheel_speed();
  if (printer.check()) {
    Serial.print("RPM Left: ");
    Serial.print(rpm_front_left);
    Serial.print("    RPM Right: ");
    Serial.print(rpm_front_right);
    Serial.println();
  }
}

void read_wheel_speed() {
  static int micros_elapsed{};
  {
    static uint8_t cur_state_front_left{};
    static uint8_t prev_state_front_left{};
    static int cur_time_front_left{};
    static int prev_time_front_left{};

    cur_state_front_left = digitalRead(FRONT_LEFT_WHEEL);

    if (cur_state_front_left == 0 && prev_state_front_left == 1) {
      cur_time_front_left = micros();
      micros_elapsed = cur_time_front_left - prev_time_front_left;
      if (micros_elapsed > 500) {
        rpm_front_left = 1.0 / micros_elapsed;
        prev_time_front_left = cur_time_front_left;
      }
    }

    if (micros() - prev_time_front_left > TIME_OUT && rpm_front_left) {
      rpm_front_left = 0;
    }

    prev_state_front_left = cur_state_front_left;
  }
  {
    static uint8_t cur_state_front_right{};
    static uint8_t prev_state_front_right{};
    static int cur_time_front_right{};
    static int prev_time_front_right{};

    cur_state_front_right = digitalRead(FRONT_RIGHT_WHEEL);

    if (cur_state_front_right == 0 && prev_state_front_right == 1) {
      cur_time_front_right = micros();
      micros_elapsed = cur_time_front_right - prev_time_front_right;
      if (micros_elapsed > 500) {
        rpm_front_right = 1.0 / micros_elapsed;
        prev_time_front_right = cur_time_front_right;
      }
    }

    if (micros() - prev_time_front_right > TIME_OUT && rpm_front_right) {
      rpm_front_right = 0;
    }

    prev_state_front_right = cur_state_front_right;
  }
}
