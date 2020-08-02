
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <Metro.h>

#define IS_FRONT true // Set based on which board you are uploading to

Metro timer = Metro(10);

// CAN Variables
FlexCAN CAN(500000);
const CAN_filter_t can_filter_ccu_status = {0, 0, ID_CCU_STATUS}; // Note: If this is passed into CAN.begin() it will be treated as a mask. Instead, pass it into CAN.setFilter(), making sure to fill all slots 0-7 with duplicate filters as necessary
static CAN_message_t tx_msg;

TCU_wheel_rpm tcu_wheel_rpm;
TCU_distance_traveled tcu_distance_traveled;

const int NUM_TEETH = 24;//CHANGE THIS FOR #OF TEETH PER REVOLUTION
const float WHEEL_CIRCUMFERENCE = 1.300619; //CIRCUMFERENCE OF WHEEL IN METERS

typedef struct {
 	volatile byte state = 0;
  	int time = 0;
  	float rpm = 0;
} Wheeldata;

Wheeldata left, right;
int ticks = 0;
float total_revs = 0;

void setup() {
	pinMode(15, INPUT);
  	pinMode(10, INPUT);
  	pinMode(13, OUTPUT);
  	digitalWrite(13, HIGH);
  	Serial.begin(9600);
  	Serial.println("Starting up");
  	CAN.begin();
}

void set_rpm(Wheeldata &wheel, uint8_t pin) {
	volatile byte state = digitalRead(pin);
	bool edge = (state == 0 && wheel.state == 1);

	wheel.state = state;
	if (edge) {
 	 	int now = micros();
  		int micros_elapsed = now - wheel.time;

	  	if (micros_elapsed > 500) {
    		wheel.rpm = (60.0 * 1000.0 * 1000.0) / (micros_elapsed * 24.0);
    		wheel.time = now;
			++ticks;
			print_rpms();
		}

		#if IS_FRONT
			total_revs = ticks / (1.0 * NUM_TEETH); //Should be devided by 2 * num_teeth, but currently only one wheel is sensed
		#endif
	}

  	if (micros() - wheel.time > 500000 && wheel.rpm != 0) {
    	wheel.rpm = 0;
    	print_rpms();
  	}
}

void print_rpms() {
    Serial.print("RPM Left: ");
    Serial.print(left.rpm);
    Serial.print("    RPM Right: ");
    Serial.print(right.rpm);
    #if IS_FRONT
		Serial.print("    Total Revs: ");
      	Serial.print(total_revs);
	#endif
    Serial.println();
}

void loop() {
  	set_rpm(left, 15);
  	set_rpm(right, 10);
  
  	if (!timer.check())
		return;

	tx_msg.timeout = 10; // Use blocking mode, wait up to ?ms to send each message instead of immediately failing (keep in mind this is slower)

	tcu_wheel_rpm.set_wheel_rpm_left(left.rpm * 100);
	tcu_wheel_rpm.set_wheel_rpm_right(right.rpm * 100);

	#if IS_FRONT
		tx_msg.id = ID_TCU_WHEEL_RPM_FRONT;
	#else
		tx_msg.id = ID_TCU_WHEEL_RPM_REAR;
	#endif

	tcu_wheel_rpm.write(tx_msg.buf);			
	tx_msg.len = tcu_wheel_rpm.messageLen();
	CAN.write(tx_msg);
	tx_msg.timeout = 0;

	#if IS_FRONT
		tx_msg.timeout = 10;
		tcu_distance_traveled.set_distance_traveled(total_revs * WHEEL_CIRCUMFERENCE * 100);
		tx_msg.id = ID_TCU_DISTANCE_TRAVELED;
		tcu_distance_traveled.write(tx_msg.buf);
		tx_msg.len = tcu_distance_traveled.messageLen();
		CAN.write(tx_msg);
		tx_msg.timeout = 0;
	#endif
}
