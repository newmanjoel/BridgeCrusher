#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <string>
#include "RJ45.h"


// TODO: make all of the configurations singletons


class Configuration
{
    //public:
        //something
    
    //private:
        // likely nothing
};

class HardwareConfiguration : public Configuration
{
    public:
        RJ45 Jacks[8];
        Pin Pins[64];
    
        void setup();
        void init();
        HardwareConfiguration();
    
};

class SoftwareConfiguration : public Configuration
{
    public:
        std::string LoggingLevel;
};

#endif // CONFIG_HPP