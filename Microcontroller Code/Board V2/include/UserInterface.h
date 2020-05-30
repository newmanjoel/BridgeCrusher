#ifndef UserInterface_H
#define UserInterface_H
#include "Arduino.h"
#include "Wire.h"

#include "ProgramUtilities.h"
#include "Screen.h"
#include "Sensors.h"
#include "JsonInterface.h"


class Userinterface: public JsonInterface
{
    public:
        static Userinterface *s_instance;

        static Userinterface* instance()
        {
            if(s_instance == __null)
            {
                s_instance = new Userinterface();
            }
            return s_instance;
        };
        int switchLength;
        LCD lcd;

        Userinterface();
        void setup();
        void poll();
        void begin();

        void toJson(JsonObject inputJsonObject)
        {
            JsonObject uiJson = inputJsonObject.createNestedObject(name);
            lcd.toJson(uiJson);
        };

        void fromJson(JsonObject inputJsonObject)
        {
            // TODO
        };
        
};




#endif