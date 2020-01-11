#ifndef Settings_H
#define Settings_H
#include "Wire.h"
class SettableSetting
{
    virtual void json(String inputString);
};
class StringSetting : public SettableSetting
{
    public:
        String jsonName;
        String DefaultValue;
        String Value;
        void json(String inputString);

};
class NumberSetting : public SettableSetting
{
    public:
        String jsonName;
        double DefaultValue;
        double Value;
        void json(String inputString);

};



#endif