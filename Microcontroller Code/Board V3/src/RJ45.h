#ifndef RJ45_HPP
#define RJ4_HPP
#include "Pin.h"

class RJ45
{
    public:
        Pin* pin1;
        Pin* pin2;
        Pin* pin3;
        Pin* pin4;
        Pin* pin5;
        Pin* pin6;
        Pin* pin7;
        Pin* pin8;
        Pin* allPins[8];

        RJ45(void);
};

#endif  //RJ45_HPP