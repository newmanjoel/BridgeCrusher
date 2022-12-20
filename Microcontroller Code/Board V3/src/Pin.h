#ifndef PIN_HPP
#define PIN_HPP
#include <string>

class Pin
{
    public:
        std::string function;
        int pin;

        Pin(void){};
        Pin(int i);
        Pin(int iPin, std::string iFunction);
};

#endif // PIN_HPP