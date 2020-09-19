#pragma once
#include <exception>
#include <string>

struct CustomException : public std::exception {
    const char* what() const throw();
    protected: char msg [128];
};

struct InvalidPinConfigurationException : public CustomException {
    InvalidPinConfigurationException(unsigned pin, bool expectedMode, int actualMode);
};
struct DoublePinModeException : public CustomException {
    DoublePinModeException(unsigned pin, bool currentMode, bool newMode);
};
struct FileNotOpenException : public CustomException {
    FileNotOpenException(unsigned pin, std::string filepath);
};