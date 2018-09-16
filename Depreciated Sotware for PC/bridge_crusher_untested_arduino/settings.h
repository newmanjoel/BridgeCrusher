#ifndef settings_h
#define settings_h

#include <Arduino.h>

#define KGS2LBS 2.20462
//#define VOLTS2KGS 0.03814755474  // volts2kgs = voltage*resolution/supply // based on calculations
#define VOLTS2KGS 0.02771364  // based on emprical data

class Settings {
  public:
    Settings();
    bool motorMaxSwitch;
    bool motorMinSwitch;
    bool lidSwitch;
    bool safetyOverride;
    double offset;

    double voltage;
    double kgs;
    double maxKgs;
    unsigned long readTime;
    unsigned long lastTime;
    double kgRise;
    double lastKgs;
    void updateVoltage(double newVoltage);
};

#endif
