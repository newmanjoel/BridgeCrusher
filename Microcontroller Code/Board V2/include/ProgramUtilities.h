#ifndef ProgramUtilities_h
#define ProgramUtilities_h

#include "math.h"

template <class T>
T linearMap(T inputValue, T inputMin, T inputMax, T outputMin, T outputMax)
{
    return (inputValue - inputMin) * (outputMax - outputMin) / (inputMax - inputMin) + outputMin;
};

template <class T2>
double polynomial(T2 inputValue, double order0, double order1)
{
    return inputValue*order1 + order0;
};

template <class T7>
double polynomial(T7 inputValue, double order0=0, double order1=0, double order2=0, double order3=0, double order4=0, double order5=0, double order6=0)
{
    return 
    pow(inputValue, 6)*order6 +pow(inputValue, 5)*order5+
    pow(inputValue, 4)*order4+pow(inputValue, 3)*order3+
    pow(inputValue,2)*order2+inputValue*order1+order0;
};


#endif