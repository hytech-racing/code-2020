HyTech 2019 BMS Control Unit
Init 2017-04-11
Configured for HV Board Rev 11
Monitors cell voltages and temperatures, sends BMS_OK signal to close Shutdown Circuit

Shutdown circuit notes:
1. GLV control system latches shutdown circuit closed.
2. AIR's close.
3. High voltage is available to the motor controller, TSAL is lit.
4. Any faults (IMD OKHS, BMS_OK, BSPD) will open shutdown circuit, opening AIR's.

Operation notes:
1. BMS sensors can be powered at all times.
2. Once Teensy gets power from external power lines, give BMS_OK signal.
3. No need to check DC bus voltage, because all batteries read their true voltages at all times. (They are continuous with each other at all times, due to no relay.)
4. Once temperatures go too high, current goes too high, or cell voltages go too high or too low, drive the BMS_OK signal low.

LTC6804 state / communication notes:
The operation of the LTC6804 is divided into two separate sections: the core circuit and the isoSPI circuit. Both sections have an independent set of operating states, as well as a shutdown timeout. See LTC6804 Datasheet Page 20.
When sending an ADC conversion or diagnostic command, wake up the core circuit by calling wakeup_sleep()
When sending any other command (such as reading or writing registers), wake up the isoSPI circuit by calling wakeup_idle().

The tx_cfg[][6] store the LTC6804 configuration data that is going to be written
to the LTC6804 ICs on the daisy chain. The LTC6804 configuration data that will be
written should be stored in blocks of 6 bytes. The array should have the following format:

|  tx_cfg[0][0]| tx_cfg[0][1] |  tx_cfg[0][2]|  tx_cfg[0][3]|  tx_cfg[0][4]|  tx_cfg[0][5]| tx_cfg[1][0] |  tx_cfg[1][1]|  tx_cfg[1][2]|  .....    |
|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|-----------|
|IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC2 CFGR0     |IC2 CFGR1     | IC2 CFGR2    |  .....    |


the rx_cfg[][8] array stores the data that is read back from a LTC6804
The configuration data for each IC is stored in blocks of 8 bytes. Below is an table illustrating the array organization:

|rx_config[0][0]|rx_config[0][1]|rx_config[0][2]|rx_config[0][3]|rx_config[0][4]|rx_config[0][5]|rx_config[0][6]  |rx_config[0][7] |rx_config[1][0]|rx_config[1][1]|  .....    |
|---------------|---------------|---------------|---------------|---------------|---------------|-----------------|----------------|---------------|---------------|-----------|
|IC1 CFGR0      |IC1 CFGR1      |IC1 CFGR2      |IC1 CFGR3      |IC1 CFGR4      |IC1 CFGR5      |IC1 PEC High     |IC1 PEC Low     |IC2 CFGR0      |IC2 CFGR1      |  .....    |
*/
