This utility operates on the assumption that you have two Teensy controllers, one running the transmitter code and one running the receiver code.

Switch between send and receive configurations by switching which header is included in [flexcan_tester_rev2.ino](./flexcan_tester_rev2.ino).

[receiver.h](./receiver.h) and [transmitter.h](./transmitter.h) each defines two functions:

```
void synchronize();
template<typename T> inline T handle_message(unsigned id, T data);
```

This utility is capable of transmitting a series of messages and validating the ouput. `synchronize()` is used to ensure that both programs are on the same page regarding which frame is being sent. `handle_message(id, data)` is used to actually send or receive a message over FlexCAN. data should always be a member class of the [HyTech_CAN](../Libraries/HyTech_CAN) library.

There is a `print()` helper function defined in [flexcan_tester_rev2.h](./flexcan_tester_rev2.h). This prints the ID and length of the messages received. There is no provision for printing the contents of the message - it is recommended that users either extend the `print()` function to suit their purposes or print the data in `loop()` using the data returned by `handle_message()`. If the [tcu-refactor](https://github.com/hytech-racing/code-2020/tree/tcu-refactor) branch has been merged in, then most classes should already have some kind of `print()` member function, which can be activated by uncommenting `#define HT_DEBUG_EN` in [flexcan_tester_rev2.ino](./flexcan_tester_rev2.ino).