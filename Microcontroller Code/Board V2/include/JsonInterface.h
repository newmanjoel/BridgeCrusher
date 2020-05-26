#ifndef JsonInterface_H
#define JsonInterface_H
#include "Wire.h"
#include "ArduinoJson.h"

class JsonInterface
{
    private:
        /* data */
    public:
        // the json name
        String name; 

        // append the objects data into a json Object
        virtual void toJson(JsonObject inputJsonObject) = 0;

        // set data inside the object from the json Object
        virtual void fromJson(JsonObject inputJsonObject) = 0;

        // to to allow runtime setting of names
        void begin(){};
};



#endif