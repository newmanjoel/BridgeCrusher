#include "Settings.h"
#include "Wire.h"
#include "ArduinoJson.h"

void initialSettingsSetup()
{

}




void StringSetting::json(String inputString)
{
    StaticJsonDocument<100> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, inputString);
    if (error)
    Serial.println(F("Failed to read file, using default configuration"));
    if (doc.containsKey(jsonName)) {
    // Yes!
        Value = doc[jsonName].as<char*>();;

    }
}



void NumberSetting::json(String inputString)
{
    StaticJsonDocument<100> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, inputString);
    if (error)
    Serial.println(F("Failed to read file, using default configuration"));
    if (doc.containsKey(jsonName)) {
    // Yes!
        Value = doc[jsonName].as<double>();;

    }
}

