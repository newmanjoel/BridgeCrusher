#ifndef UserInterface_H
#define UserInterface_H
#include "Arduino.h"
#include "Wire.h"
#include "LiquidCrystal_PCF8574.h"
#include "RotaryEncoderJoel.h"

class Switch
{
    public:
        volatile int pin;
        volatile bool invert;
        volatile bool NO;
        volatile bool NC;
        String name; // human name of the switch
        Switch();
        Switch(int i_pin);
        Switch(int i_pin, bool i_invert);
        Switch(int i_pin, const String i_name);
        Switch(int i_pin, const String i_name, bool i_invert);

    public:
        void poll();

};

class NO_Switch : public Switch
{
    public:
        NO_Switch(int i_pin, const String i_name);
        void poll();
};
class NC_Switch : public Switch
{
    public:
        NC_Switch(int i_pin, const String i_name);
        void poll();
};

class Userinterface
{
    public:
        int switchLength;
        Switch *switches[10]; // setting 10 positions open, this could be increased in the future
        LiquidCrystal_PCF8574 *lcd;
        RotaryEncoderJoel *encoder;

        Userinterface(int i_switchNum, Switch  *i_switches[6], LiquidCrystal_PCF8574* i_lcd, RotaryEncoderJoel* i_encoder);
        void poll();
};




#endif