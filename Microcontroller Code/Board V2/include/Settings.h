#ifndef Settings_H
#define Settings_H
#include "Wire.h"
#include "ArduinoJson.h"

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

class Reporting
{
    static Reporting *s_instance;
    public:
        Reporting();
        bool json(String inputString);
        bool json(JsonVariant inputJsonVariant);
        String toString();
        void toJson(JsonObject inputJsonObject);
        void defaults();
        static Reporting* instance();
        bool FORCE;
        bool LIMITS;
        bool HMI;
        bool LEDS;
        bool MOTOR;
        bool PID;
        bool DISTANCE;
        bool ENCODER;
        bool CONFIG;
        bool JACK;
        bool SSENSOR;
};



#endif