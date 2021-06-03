#include "MockCAN.h"
#include "Dashboard.h"
#include "DebouncedButton.h"
#include "gtest/gtest.h"
#include "HyTech_CAN.h"
#include "mcp_can.h"
#include "Simulator.h"
#include "VariableLED.h"

#define next() Simulator::next()
#define digitalWrite(pin,val) Simulator::digitalWrite(pin, val);

class MCUTesting : public ::testing::Test {
protected:
	MCUTesting () {}
	void SetUp() {
        // declare as extern all variables in mcu code that need to be reset for each new test
		extern bool is_mc_err;
		extern VariableLED variable_led_start;
		extern Metro timer_can_update;
		extern MCP_CAN CAN;
		extern Dashboard_status dashboard_status;
		extern MCU_status mcu_status;

		MockCAN::teardown();
        //clear those variables
		is_mc_err = false;
		variable_led_start = VariableLED(LED_START);
		timer_can_update = Metro(100);
		CAN = MCP_CAN(SPI_CS);
		dashboard_status = {};
		mcu_status = {};
		
        
		Simulator::begin();
	}

	void TearDown() {
		Simulator::teardown();
	}
};

TEST_F(MCUTesting, STARTUP){
	CAN_message_t msg;
	delay(100);
	next();
	ASSERT_TRUE(MockCAN::read(msg));
	ASSERT_EQ(msg.id,ID_MCU_STATUS);
	/*
	MCU_status status;
	status.load(msg.buf);
	EXPECT_EQ(status.get_button_flags(),0);
	EXPECT_EQ(status.get_led_flags(),3);
	*/
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}