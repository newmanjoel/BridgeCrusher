#include "Pin.h"

Pin::Pin(int iPin, std::string iFunction)
{
    pin = iPin;
    function = iFunction;
}

Pin::Pin(int iPin)
{
    pin = iPin;
}