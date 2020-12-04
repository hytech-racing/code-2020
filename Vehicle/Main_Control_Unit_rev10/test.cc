#include <cstdint>

enum class Test : uint8_t { ZERO = 0, ONE =1, TWO = 2};

Test getTest1(uint8_t val){
    return Test{val};
}

int main(){
    uint8_t val;
    Test t = Test{val};
    Test t2 = static_cast<Test>(val);
}