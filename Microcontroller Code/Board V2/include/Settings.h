#ifndef Settings_H
#define Settings_H
#include "Wire.h"
#include "ArduinoJson.h"
#include "ProgramUtilities.h"
#include "JsonInterface.h"

class Reporting: public JsonInterface, public SingleInstance<Reporting>
{
    
    public:
        Reporting();
        String toString();
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
        void toJson(JsonObject inputJsonObject);
        void fromJson(JsonObject inputJsonObject);
        void begin();
};



#endif