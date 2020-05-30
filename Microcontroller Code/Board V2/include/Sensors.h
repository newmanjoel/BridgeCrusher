#ifndef Sensors_H
#define Sensors_H
#include "BridgeCrusherPins.h"
#include "JsonInterface.h"
#include "ProgramUtilities.h"
#include "avdweb_AnalogReadFast.h"
#include "RotaryEncoderJoel.h"
#include "ADS1246.h"

class SensorBase : public JsonInterface
{
public:
    SensorBase(){};
    SensorBase(String i_name)
    {
        name = i_name;
    };
    void toJson(JsonObject inputJsonObject)
    {
        // Nothing
    };

    void fromJson(JsonObject inputJsonObject){
        // Nothing
    };

};
class PolledSensor : public SensorBase
{
public:
    virtual void poll() = 0;
};

class HardwareSensor
{
public:
    volatile int pin;
};

class AnalogInput : public PolledSensor
{
public:
    // the measured voltage
    volatile double value;
    ChangeableSetting<double> scale;
    ChangeableSetting<double> offset;

protected:
    double _scale;
    double _offset;
};

class DigitalInput : public PolledSensor
{
public:
    // if the value needs to be inverted from what is read
    volatile bool invert;
    // the state of the digital input
    volatile bool value;
};

class SingleSwitch : public HardwareSensor, public DigitalInput
{
public:
    enum Type
    {
        NO = 0,
        NC = 1
    };
    volatile bool NO_contact;
    volatile bool NC_contact;
    SingleSwitch(){};

    SingleSwitch(int i_pin, bool i_invert, SingleSwitch::Type i_type, String i_name)
    {
        pin = i_pin;
        invert = i_invert;
        _type = i_type;
        name = i_name;
    };

    void setup(int i_pin, bool i_invert, SingleSwitch::Type i_type, String i_name)
    {
        pin = i_pin;
        invert = i_invert;
        _type = i_type;
        name = i_name;
    };

    void poll()
    {
        value = digitalRead(pin) == invert ? true : false;

        if (_type == Type::NO)
        {
            NO_contact = value;
            NC_contact = !value;
        }
        else if (_type == Type::NC)
        {
            NO_contact = !value;
            NC_contact = value;
        }
    };

    void toJson(JsonObject inputJsonObject)
    {
        JsonObject switchJson = inputJsonObject.createNestedObject(name);
        switchJson["value"] = value;
        switchJson["invert"] = invert;
        switchJson["type"] = _type;
    };

    void fromJson(JsonObject inputJsonObject){
        // TODO
    };

protected:
    SingleSwitch::Type _type;
};

class Distance : public HardwareSensor, public AnalogInput
{
public:
    static Distance *s_instance;

        static Distance* instance()
        {
            if(s_instance == __null)
            {
                s_instance = new Distance();
            }
            return s_instance;
        };
    // distance in mm
    double distance;
    Distance()
    {
        pin = Dist_VAL;
        name = "Distance";
    };
    void begin(){

    };
    void poll()
    {
        value = linearMap((double)analogReadFast(pin), 0.0, 4096.0, 0.0, 3.3);
        mapping();
    };

    void toJson(JsonObject inputJsonObject)
    {
        JsonObject distanceJson = inputJsonObject.createNestedObject(name);
        distanceJson["value"] = value;
        distanceJson["distance"] = distance;
        distanceJson["pin"] = pin;
    };

    void fromJson(JsonObject inputJsonObject){
        // TODO
    };

protected:
    void mapping()
    {
        // Note that this polynomial is only valid between
        // 5mm @ 3.1V to 80mm @ 0.4V
        distance = polynomial(
            value,
            5.0212E+00,
            -4.1384E-01,
            1.8237E-02,
            -4.4976E-04,
            6.2621E-06,
            -4.6149E-08,
            1.4036E-10);
    };
};

class Force : public AnalogInput
{

public:
    static Force *s_instance;

        static Force* instance()
        {
            if(s_instance == __null)
            {
                s_instance = new Force();
            }
            return s_instance;
        };

    int maxPollRate;
    bool isReady;
    ADS1246 sensor;

    // force in Newtons
    double force;
    // effective mass in Kg
    double mass;

    Force()
    {
        name = "Force";
    };

    void toJson(JsonObject inputJsonObject)
    {
        JsonObject forceJson = inputJsonObject.createNestedObject(name);
        sensor.toJson(forceJson);
        scale.toJson(forceJson);
        offset.toJson(forceJson);
        forceJson["force"] = force;
        forceJson["mass"] = mass;
    };

    void fromJson(JsonObject inputJsonObject){
        // TODO
    };

    void poll()
    {
        value = sensor.getConvertedOutput();
        mapping();
    };

    void begin()
    {
        sensor.begin(Adc_CS, Adc_STR, Adc_RST);
        sensor.setupADC();
        scale.setup("scale", &_scale, 100, 90, 110, 0.1);
        offset.setup("offset", &_offset, 0, -5, 5, 0.1);
    };

protected:
    void mapping()
    {
        //mass = linearMap<double>(value, -5, 5, -500, 500);
        mass = value * (*scale.value) + (*offset.value);
        force = mass / 9.81;
    };
};

class Switches : public JsonInterface
{
public:
static Switches *s_instance;

        static Switches* instance()
        {
            if(s_instance == __null)
            {
                s_instance = new Switches();
            }
            return s_instance;
        };
    
    Switches()
    {
        name = String("Switches");
    };

    Switches(String i_name)
    {
        name = i_name;
    };

    void toJson(JsonObject inputJsonObject)
    {
        JsonObject switchesJson = inputJsonObject.createNestedObject(name);
        start.toJson(switchesJson);
        stop.toJson(switchesJson);
        min.toJson(switchesJson);
        max.toJson(switchesJson);
        lid.toJson(switchesJson);
        encoderClick.toJson(switchesJson);
    };

    void fromJson(JsonObject inputJsonObject)
    {
        if (inputJsonObject.containsKey(start.name))
        {
            start.fromJson(inputJsonObject[start.name].as<JsonObject>());
        }
        if (inputJsonObject.containsKey(stop.name))
        {
            stop.fromJson(inputJsonObject[stop.name].as<JsonObject>());
        }
        if (inputJsonObject.containsKey(min.name))
        {
            min.fromJson(inputJsonObject[min.name].as<JsonObject>());
        }
        if (inputJsonObject.containsKey(max.name))
        {
            max.fromJson(inputJsonObject[max.name].as<JsonObject>());
        }
        if (inputJsonObject.containsKey(lid.name))
        {
            lid.fromJson(inputJsonObject[lid.name].as<JsonObject>());
        }
        if (inputJsonObject.containsKey(encoderClick.name))
        {
            encoderClick.fromJson(inputJsonObject[encoderClick.name].as<JsonObject>());
        }
    };

    SingleSwitch start = SingleSwitch(Start_NO, false, SingleSwitch::Type::NO, "Start Button");
    SingleSwitch stop = SingleSwitch(Stop_NC, false, SingleSwitch::Type::NC, "Stop Button");
    SingleSwitch min = SingleSwitch(Min_NC, false, SingleSwitch::Type::NC, "Min Limit");
    SingleSwitch max = SingleSwitch(Max_NC, false, SingleSwitch::Type::NC, "Max Limit");
    SingleSwitch lid = SingleSwitch(Lid_NC, false, SingleSwitch::Type::NC, "Lid Limit");
    SingleSwitch encoderClick = SingleSwitch(Enc_SW_NO, false, SingleSwitch::Type::NO, "Encoder Push");
    SingleSwitch allSwitches[6];

    void begin()
    {
        SingleSwitch allSwitches[] = {start, stop, min, max, lid, encoderClick};
    };
    void poll()
    {
        for (int i = 0; i < 6; i++)
        {
            allSwitches[i].poll();
        }
    };
};

class Sensors : public JsonInterface
{

public:

    static Sensors *s_instance;

        static Sensors* instance()
        {
            if(s_instance == __null)
            {
                s_instance = new Sensors();
            }
            return s_instance;
        };

    Sensors()
    {
        name = "Sensors";
    };

    Switches switches;
    Distance distance;
    Force force;
    RotaryEncoderJoel encoder;

    void toJson(JsonObject inputJsonObject)
    {
        JsonObject sensorsJson = inputJsonObject.createNestedObject(name);
        switches.toJson(sensorsJson);
        distance.toJson(sensorsJson);
        force.toJson(sensorsJson);
    };

    void fromJson(JsonObject inputJsonObject)
    {
        if (inputJsonObject.containsKey(switches.name))
        {
            switches.fromJson(inputJsonObject[switches.name].as<JsonObject>());
        }
        if (inputJsonObject.containsKey(distance.name))
        {
            distance.fromJson(inputJsonObject[distance.name].as<JsonObject>());
        }
        if (inputJsonObject.containsKey(force.name))
        {
            force.fromJson(inputJsonObject[force.name].as<JsonObject>());
        }
    };

    void begin()
    {
        switches = *Switches::instance();
        switches.begin();

        distance = *Distance::instance();
        distance.begin();

        force = *Force::instance();
        force.begin();

        encoder.begin3(Enc_A, Enc_B, Enc_SW_NO);
    };
    void poll()
    {
        switches.poll();
    };

protected:
    void initialize(){

    };
};

#endif