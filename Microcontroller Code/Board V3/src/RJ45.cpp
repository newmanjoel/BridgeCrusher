#include "RJ45.h"
#include "Pin.h"

RJ45::RJ45(void)
{
    
}
void RJ45::init()
{
    allPins[0] = pin1;
    allPins[1] = pin2;
    allPins[2] = pin3;
    allPins[3] = pin4;
    allPins[4] = pin5;
    allPins[5] = pin6;
    allPins[6] = pin7;
    allPins[7] = pin8;
}