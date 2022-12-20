#include "Configuration.h"
#include "Pin.h"

HardwareConfiguration::HardwareConfiguration()
{

};

void HardwareConfiguration::init()
{
    for(int i = 0; i<64; i++)
    {
        Pins[i] = Pin(i);
    }
    for(int i=0; i<8; i++)
    {
        Jacks[i] = RJ45();
        Jacks[i].pin1 = &Pins[i+0];
        Jacks[i].pin2 = &Pins[i+1];
        Jacks[i].pin3 = &Pins[i+2];
        Jacks[i].pin4 = &Pins[i+3];
        Jacks[i].pin5 = &Pins[i+4];
        Jacks[i].pin6 = &Pins[i+5];
        Jacks[i].pin7 = &Pins[i+6];
        Jacks[i].pin8 = &Pins[i+7];
        Jacks[i].init();
    }
    
};