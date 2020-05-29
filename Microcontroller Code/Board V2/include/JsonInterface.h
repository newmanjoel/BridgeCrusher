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
        void toJson(JsonObject inputJsonObject){};

        // set data inside the object from the json Object
        void fromJson(JsonObject inputJsonObject){};

        // to to allow runtime setting of names
        void begin(){};
};



#endif