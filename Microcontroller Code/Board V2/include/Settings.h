#ifndef Settings_H
#define Settings_H
#include "Wire.h"

class NumberSetting
{
    public:
        
        NumberSetting();
        double DefaultValue;
        double* value; // pointer to the actual value
        double min; // minimum alowable value
        double max; // maximum alowable value
        double stepValue; // when incrementing, the step value

        String longDescription; // long form description
        String jsonName; // single word name
        int type;

        void json(String inputString);
        void setValue(double newValue);
        void increment();
        void decrement();
        void begin();

};



#endif