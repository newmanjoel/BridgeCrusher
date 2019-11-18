#include "UserInterface.h"


Switch::Switch(){
    pin = -1;
    invert = false;
    NO = false;
    NC = false;
}

Switch::Switch(int i_pin){
    pin = i_pin;
    invert = false;
    NO = false;
    NC = false;
}

Switch::Switch(int i_pin, bool i_invert){
    pin = i_pin;
    invert = i_invert;
    NO = false;
    NC = false;
}

Switch::Switch(int i_pin, const String i_name){
    pin = i_pin;
    invert = false;
    NO = false;
    NC = false;
    name = i_name;
    
}
Switch::Switch(int i_pin, const String i_name, bool i_invert){
    pin = i_pin;
    invert = i_invert;
    NO = false;
    NC = false;
    name = i_name;
    
}
void Switch::poll(){
    NO = digitalRead(pin)==invert ? false:true;
    NC = !NO;
    //Serial.print(name);
    //Serial.print(":");
    //Serial.print(NO);
    //Serial.print("|");
}

Userinterface::Userinterface(int i_switchNum, Switch *i_switches[], LiquidCrystal_PCF8574* i_lcd, RotaryEncoderJoel* i_encoder)
{
    
        switchLength = i_switchNum;
        for(int i = 0; i<switchLength; i++){
            switches[i] = i_switches[i];
        }
        
        lcd = i_lcd;
        encoder = i_encoder;
}

void Userinterface::poll(){
    for(int i = 0; i<switchLength; i++){
        switches[i]->poll();
    }
    Serial.println("");
}