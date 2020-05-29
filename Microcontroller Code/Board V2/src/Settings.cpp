#include "Settings.h"
#include "Wire.h"
#include "ArduinoJson.h"



Reporting::Reporting()
{
    begin();
}

void Reporting::begin()
{
    FORCE = false;
    LIMITS = false;
    HMI = false;
    LEDS = false;
    MOTOR = false;
    PID = false;
    DISTANCE = false;
    ENCODER = false;
    CONFIG = true;
    JACK = false;
    SSENSOR = false;
}


void Reporting::fromJson(JsonObject inputJsonVariant)
{
    
    if(inputJsonVariant.containsKey("FORCE"))
    {
        FORCE = inputJsonVariant["FORCE"].as<bool>();
    }
    if(inputJsonVariant.containsKey("LIMITS"))
    {
        LIMITS = inputJsonVariant["LIMITS"].as<bool>();
        if(LIMITS)
        {
            JACK = true;
        }
    }
    if(inputJsonVariant.containsKey("HMI"))
    {
        HMI = inputJsonVariant["HMI"].as<bool>();
    }
    if(inputJsonVariant.containsKey("LEDS"))
    {
        LEDS = inputJsonVariant["LEDS"].as<bool>();
    }
    if(inputJsonVariant.containsKey("JACK"))
    {
        JACK = inputJsonVariant["JACK"].as<bool>();
    }
    if(inputJsonVariant.containsKey("MOTOR"))
    {
        MOTOR = inputJsonVariant["MOTOR"].as<bool>();
        if(MOTOR)
        {
            JACK = true;
        }
    }
    if(inputJsonVariant.containsKey("PID"))
    {
        PID = inputJsonVariant["PID"].as<bool>();
        if(PID)
        {
            JACK = true;
        }
    }
    if(inputJsonVariant.containsKey("DISTANCE"))
    {
        DISTANCE = inputJsonVariant["DISTANCE"].as<bool>();
    }
    if(inputJsonVariant.containsKey("ENCODER"))
    {
        ENCODER = inputJsonVariant["ENCODER"].as<bool>();
    }
    if(inputJsonVariant.containsKey("CONFIG"))
    {
        CONFIG = inputJsonVariant["CONFIG"].as<bool>();
    }
    if(inputJsonVariant.containsKey("SSENSOR"))
    {
        SSENSOR = inputJsonVariant["SSENSOR"].as<bool>();
    }
}

void Reporting::toJson(JsonObject inputJsonObject)
{
    // TODO: double check that this wont cause a memory leak
    //JsonObject reportingJson = inputDocument->createNestedObject("REPORTING");
    JsonObject reportingJsonObject = inputJsonObject.createNestedObject(name);
    reportingJsonObject["FORCE"] = FORCE;
    reportingJsonObject["LIMITS"] = LIMITS;
    reportingJsonObject["HMI"] = HMI;
    reportingJsonObject["LEDS"] = LEDS;
    reportingJsonObject["MOTOR"] = MOTOR;
    reportingJsonObject["PID"] = PID;
    reportingJsonObject["DISTANCE"] = DISTANCE;
    reportingJsonObject["ENCODER"] = ENCODER;
    reportingJsonObject["CONFIG"] = CONFIG;
    reportingJsonObject["JACK"] = JACK;
    reportingJsonObject["SSENSOR"] = SSENSOR;
    //return reportingJson;
}
