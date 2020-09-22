#pragma once
#include <fstream>

class MockSerial {
public:
    MockSerial(int id);
    ~MockSerial();
    void init(std::string filepath);
    void begin(unsigned int baudRate);
    void print(bool);               void println(bool);
    void print(char);               void println(char);
    void print(unsigned char);      void println(unsigned char);
    void print(short);              void println(short);
    void print(unsigned short);     void println(unsigned short);
    void print(int);                void println(int);
    void print(unsigned int);       void println(unsigned int);
    void print(long);               void println(long);
    void print(unsigned long);      void println(unsigned long);
    void print(long long);          void println(long long);
    void print(unsigned long long); void println(unsigned long long);
    void print(float);              void println(float);
    void print(double);             void println(double);
    void print(long double);        void println(long double);
    void print(std::string);        void println(std::string);
    void print(const char*);        void println(const char*);
    template <typename T> inline void print(T value);
    template <typename T> inline void println(T value);
private:
    int fId;
    std::string fFilepath;
    std::ofstream file;
};

extern MockSerial Serial, Serial2;