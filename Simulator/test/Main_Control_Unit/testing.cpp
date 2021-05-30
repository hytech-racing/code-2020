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
        // values to be set for MCU testing
		int eintTORQUE1;
        int eintTORQUE2;
        int eintTORQUE3;
        bool eboolDEBUG;
        bool eboolBMS_DEBUG_ENABLE;
        bool eboolREGEN_ENABLE;
        bool eboolAV_ENABLE;
        int eintMAP_MODE;

        // declare as extern all variables in mcu code that need to be reset for each new test

		MockCAN::teardown();
        //clear those variables
		
        
		Simulator::begin();
	}

	void TearDown() {
		Simulator::teardown();
	}
};
