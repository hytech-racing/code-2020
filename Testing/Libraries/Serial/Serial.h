#pragma once
#include <fstream>
#include <sstream>
#include <bitset>

class MockSerial {
public:
    MockSerial(int id);
    ~MockSerial();
    void init(std::string filepath);
    void begin(unsigned int baudRate);
    template <typename T> inline void print(T value) { validate(); file << value; }
    template <typename T> inline void print(T value, int base) { validate(); file << format(value, base).rdbuf(); }
    template <typename T> inline void println(T value = "") { validate(); file << value << '\n'; }
    template <typename T> inline void println(T value, int base) { validate(); file << format(value, base).rdbuf() << '\n'; }
    inline void write(uint8_t* buf, int size);
private:
    int fId;
    std::string fFilepath;
    std::ofstream file;
    void validate();

    template <typename T> inline std::stringstream format(T value, int base) {
        std::stringstream ss;
        if (base == 2) ss << std::bitset<sizeof(value) << 3>(value);
        else if (base == 8) ss << std::oct << value;
        else if (base == 16) ss << std::hex << value;
        else ss << value;
        return ss;
    }
};

extern MockSerial Serial, Serial2;