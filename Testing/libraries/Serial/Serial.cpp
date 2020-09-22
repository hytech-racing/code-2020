#include <Arduino.h>
#include "Exception.h"
#include "Serial.h"

MockSerial::MockSerial(int id) { fId = id; }

MockSerial::~MockSerial() {
    if (file.is_open())
        file.close();
}

void MockSerial::init(std::string filepath) { fFilepath = filepath; }
void MockSerial::begin(unsigned int baudRate) {
    if (file.is_open())
        throw DoublePinModeException(fId, OUTPUT, OUTPUT);
    file.open(fFilepath, std::ios::ios_base::out);
    if (!file.is_open())
        throw FileNotOpenException(fId, fFilepath);
}

template <typename T>  
inline void MockSerial::print(T value) {
    if (!file.is_open())
        throw InvalidPinConfigurationException(-1, OUTPUT, -1);
    file << value;
}

template <typename T>
inline void MockSerial::println(T value) {
    if (!file.is_open())
        throw InvalidPinConfigurationException(-1, OUTPUT, -1);
    file << value << '\n';
}

void MockSerial::print(bool value)           { print<bool>(value); }               
void MockSerial::print(char value)           { print<char>(value); }               
void MockSerial::print(unsigned char value)  { print<unsigned char>(value); }      
void MockSerial::print(short value)          { print<short>(value); }              
void MockSerial::print(unsigned short value) { print<unsigned short>(value); }     
void MockSerial::print(int value)            { print<int>(value); }                
void MockSerial::print(unsigned int value)   { print<unsigned int>(value); }       
void MockSerial::print(long value)           { print<long>(value); }               
void MockSerial::print(unsigned long value)  { print<unsigned long>(value); }      
void MockSerial::print(long long value)      { print<long long>(value); }          
void MockSerial::print(unsigned long long value) { print<unsigned long long>(value); } 
void MockSerial::print(float value)          { print<float>(value); }              
void MockSerial::print(double value)         { print<double>(value); }             
void MockSerial::print(long double value)    { print<long double>(value); }        
void MockSerial::print(std::string value)    { print<std::string>(value); }        
void MockSerial::print(const char* value)    { print<const char*>(value); }        

void MockSerial::println(bool value)           { println<bool>(value); }               
void MockSerial::println(char value)           { println<char>(value); }               
void MockSerial::println(unsigned char value)  { println<unsigned char>(value); }      
void MockSerial::println(short value)          { println<short>(value); }              
void MockSerial::println(unsigned short value) { println<unsigned short>(value); }     
void MockSerial::println(int value)            { println<int>(value); }                
void MockSerial::println(unsigned int value)   { println<unsigned int>(value); }       
void MockSerial::println(long value)           { println<long>(value); }               
void MockSerial::println(unsigned long value)  { println<unsigned long>(value); }      
void MockSerial::println(long long value)      { println<long long>(value); }          
void MockSerial::println(unsigned long long value) { println<unsigned long long>(value); } 
void MockSerial::println(float value)          { println<float>(value); }              
void MockSerial::println(double value)         { println<double>(value); }             
void MockSerial::println(long double value)    { println<long double>(value); }        
void MockSerial::println(std::string value)    { println<std::string>(value); }        
void MockSerial::println(const char* value)    { println<const char*>(value); }        

MockSerial Serial(-1);
MockSerial Serial2(-2);