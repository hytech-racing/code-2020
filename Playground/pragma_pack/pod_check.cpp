#include <iostream>
#include <type_traits>

#include "../../Libraries/HyTech_CAN/HyTech_CAN.h"

using namespace std;

int main()
{
    BMS_detailed_temperatures bms = {};
    cout << is_standard_layout<BMS_detailed_temperatures>::value
     << " " << is_trivial<BMS_detailed_temperatures>::value << endl;
}