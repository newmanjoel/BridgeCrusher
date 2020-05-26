#ifndef Sensors_H
#define Sensors_H
#include "ADS1246.h"

class jsonInterface
{
    public:
        String name; // name for json
        virtual void toJson(JsonObject inputJsonObject); 
        virtual void json(JsonObject inputJsonObject);
};

class SensorBase: public jsonInterface
{
    public:
        SensorBase();
        SensorBase(String i_name);
        virtual void begin();

};
class PolledSensor: public SensorBase
{
    public:
        virtual void poll();
};

class HardwareSensor
{
    public:
        volatile int pin;
};

class ExternalSensor
{
    public:
        int maxPollRate;
        bool isReady;
};

class AnalogInput : public PolledSensor
{
    public:
        volatile double value;
        AnalogInput();
};

class DigitalInput : public PolledSensor
{
    public:
        volatile bool invert;
        volatile bool value;
        DigitalInput(bool i_invert, String i_name);
};

class Switch : public HardwareSensor, public DigitalInput
{
    public:
        enum Type
        {
            NO,
            NC,
            Both
        };
        volatile bool NO;
        volatile bool NC;

        Switch(int i_pin, bool i_invert, Switch::Type i_type, String i_name);
    private:
        Switch::Type _type;
};

class Distance: public HardwareSensor, public AnalogInput
{
    public:
        double distance;
    private:
        void mapping();
};

class Force: public ExternalSensor, public AnalogInput
{
    // add stuff in here for reading from the ADS1246 class
    public:
        double force; // force in Newtons
        double mass; // effective mass in Kg
    
    private:
        void mapping();
    

};

class Sensors
{
    static Sensors *s_instance;
    public:
        Sensors();
        static Sensors* instance();
        
        Switch *switches[10]; // setting 10 positions open, this could be increased in the future
        Distance *distance;
        Force *force;
    private:
        void initialize();
};


#endif