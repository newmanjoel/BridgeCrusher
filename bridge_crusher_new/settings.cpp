#include "settings.h"

Settings::Settings(){
  lidSwitch = false;
  motorMinSwitch = false;
  motorMaxSwitch = false;
  safetyOverride = false;
  offset = 0.0;
}

void Settings::updateVoltage(double newVoltage){
  voltage = newVoltage;
  kgs = voltage * VOLTS2KGS - offset;
  maxKgs = (maxKgs < kgs) ? kgs : maxKgs;
  readTime = millis();
  kgRise = ((kgs - lastKgs) * 1000) / ((double)(readTime - lastTime));
  lastTime = readTime;
  lastKgs = kgs;
}

