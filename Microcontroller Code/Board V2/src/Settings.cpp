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

