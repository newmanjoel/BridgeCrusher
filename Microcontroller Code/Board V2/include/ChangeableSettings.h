#ifndef StateObject_H
#define StateObject_H
#include "JsonInterface.h"

class IndexableSetting : public JsonInterface
{
public:
    void increment();
    void decrement();
};

template <class T>
class ChangeableSetting : public IndexableSetting
{
public:
    // value to reset to
    T defaultValue;
    // pointer to the actual value
    T *value;
    // minimum allowed
    T min;
    // maximum allowed
    T max;
    // how much to increment or decrement by
    T stepValue;

    ChangeableSetting(){};

    ChangeableSetting(String i_name, T *i_value, T i_defaultValue, T i_min, T i_max, T stepValue)
    {
        name = i_name;
        value = i_value;
        defaultValue = i_defaultValue;
        min = i_min;
        max = i_max;
        stepValue = stepValue;
    };

    void setup(String i_name, T *i_value, T i_defaultValue, T i_min, T i_max, T stepValue)
    {
        name = i_name;
        value = i_value;
        defaultValue = i_defaultValue;
        min = i_min;
        max = i_max;
        stepValue = stepValue;
    }

    // set the value back to the default
    void reset()
    {
        *value = defaultValue;
    }

    // raise the value up by the step value
    void increment()
    {
        if ((*value + stepValue) >= max)
        {
            *value = max;
        }
        else
        {
            *value += stepValue;
        }
    }

    // lower the value down by the step value
    void decrement()
    {
        if ((*value - stepValue) <= min)
        {
            *value = min;
        }
        else
        {
            *value -= stepValue;
        }
    }

    // copy the contents of the setting into a json object
    void toJson(JsonObject inputJsonObject)
    {
        JsonObject changableSettingJson = inputJsonObject.createNestedObject(name);
        changableSettingJson["defaultValue"] = defaultValue;
        changableSettingJson["value"] = *value;
        changableSettingJson["min"] = min;
        changableSettingJson["max"] = max;
        changableSettingJson["stepValue"] = stepValue;
    }

    // change the values insdie the object from a json object
    void fromJson(JsonObject inputJsonObject)
    {
        if (inputJsonObject.containsKey(name))
        {
            JsonObject specificObject = inputJsonObject[name].as<JsonObject>();
            if (specificObject.containsKey("defaultValue"))
            {
                defaultValue = specificObject["defaultValue"].as<T>();
            }
            if (specificObject.containsKey("value"))
            {
                *value = specificObject["value"].as<T>();
            }
            if (specificObject.containsKey("min"))
            {
                min = specificObject["min"].as<T>();
            }
            if (specificObject.containsKey("max"))
            {
                max = specificObject["max"].as<T>();
            }
            if (specificObject.containsKey("stepValue"))
            {
                stepValue = specificObject["stepValue"].as<T>();
            }
        }
    }
};

#endif