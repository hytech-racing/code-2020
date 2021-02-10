#include <stdint.h>

class CAN_msg {
public:
  inline void do_nothing(bool val) { this-> vals = val; }
  inline void do_with_flags(bool val) { vals = (vals & 0xEF) | (val << 3);}
  inline void do_with_ifs(bool val) { (val) ? vals |= (1 << 3) : vals &= ~(1 << 3);}
private:
  uint8_t vals;
};

void setup() {
  Serial.begin(115200);
  
  CAN_msg msg{};
  srand(0);
  int start_millis = millis();
  for (int i = 0; i < 1e9; i++){
    bool val = rand() % 2;
    msg.do_nothing(val);
  }
  int end_millis = millis();
  Serial.print("Do nothing: ");
  Serial.println(end_millis - start_millis);

  srand(0);
  start_millis = millis();
  for (int i = 0; i < 1e9; i++){
    bool val = rand() % 2;
    msg.do_with_flags(val);
  }
  end_millis = millis();
  Serial.print("Do with flags: ");
  Serial.println(end_millis - start_millis);

  srand(0);
  start_millis = millis();
  for (int i = 0; i < 1e9; i++){
    bool val = rand() % 2;
    msg.do_with_ifs(val);
  }
  end_millis = millis();
  Serial.print("Do with if block: ");
  Serial.println(end_millis - start_millis);

}

void loop() {
  // put your main code here, to run repeatedly:

}
