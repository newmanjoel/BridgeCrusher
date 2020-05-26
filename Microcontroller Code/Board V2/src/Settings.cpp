#include "Settings.h"
#include "Wire.h"
#include "ArduinoJson.h"



NumberSetting::NumberSetting()
{
    type=2;
}

void NumberSetting::begin()
{
    setValue(DefaultValue);
}

void NumberSetting::json(String inputString)
{
    StaticJsonDocument<100> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, inputString);
    if (error)
    Serial.println(F("Failed to read file, using default configuration"));
    if (doc.containsKey(jsonName)) 
    {
        // Yes!
        setValue(doc[jsonName].as<double>());
    }
}

void NumberSetting::increment()
{
    setValue(*value + stepValue);
}

void NumberSetting::decrement()
{
    setValue(*value - stepValue);
}

void NumberSetting::setValue(double newValue)
{
    if(newValue > max)
    {
        newValue =  max;
    }
    else if(newValue < min)
    {
        newValue = min;
    }
    *value = newValue;

}

Reporting::Reporting()
{
    defaults();
}

void Reporting::defaults()
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

bool Reporting::json(String inputString)
{
    // TODO
    return false;
}
bool Reporting::json(JsonVariant inputJsonVariant)
{
    bool somethingChanged = false;
    if(inputJsonVariant.containsKey("FORCE"))
    {
        FORCE = inputJsonVariant["FORCE"].as<bool>();
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("LIMITS"))
    {
        LIMITS = inputJsonVariant["LIMITS"].as<bool>();
        if(LIMITS)
        {
            JACK = true;
        }
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("HMI"))
    {
        HMI = inputJsonVariant["HMI"].as<bool>();
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("LEDS"))
    {
        LEDS = inputJsonVariant["LEDS"].as<bool>();
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("JACK"))
    {
        JACK = inputJsonVariant["JACK"].as<bool>();
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("MOTOR"))
    {
        MOTOR = inputJsonVariant["MOTOR"].as<bool>();
        if(MOTOR)
        {
            JACK = true;
        }
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("PID"))
    {
        PID = inputJsonVariant["PID"].as<bool>();
        if(PID)
        {
            JACK = true;
        }
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("DISTANCE"))
    {
        DISTANCE = inputJsonVariant["DISTANCE"].as<bool>();
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("ENCODER"))
    {
        ENCODER = inputJsonVariant["ENCODER"].as<bool>();
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("CONFIG"))
    {
        CONFIG = inputJsonVariant["CONFIG"].as<bool>();
        somethingChanged = true;
    }
    if(inputJsonVariant.containsKey("SSENSOR"))
    {
        SSENSOR = inputJsonVariant["SSENSOR"].as<bool>();
        somethingChanged = true;
    }
    


    return somethingChanged;
}

void Reporting::toJson(JsonObject inputJsonObject)
{
    // TODO: double check that this wont cause a memory leak
    //JsonObject reportingJson = inputDocument->createNestedObject("REPORTING");

    inputJsonObject["FORCE"] = FORCE;
    inputJsonObject["LIMITS"] = LIMITS;
    inputJsonObject["HMI"] = HMI;
    inputJsonObject["LEDS"] = LEDS;
    inputJsonObject["MOTOR"] = MOTOR;
    inputJsonObject["PID"] = PID;
    inputJsonObject["DISTANCE"] = DISTANCE;
    inputJsonObject["ENCODER"] = ENCODER;
    inputJsonObject["CONFIG"] = CONFIG;
    inputJsonObject["JACK"] = JACK;
    inputJsonObject["SSENSOR"] = SSENSOR;
    //return reportingJson;
}

Reporting* Reporting::instance()
{
    if(s_instance == NULL)
    {
        s_instance = new Reporting();
    }
    return s_instance;
}
