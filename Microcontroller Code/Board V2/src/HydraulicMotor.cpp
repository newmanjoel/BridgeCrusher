#include "Arduino.h"
#include "HydraulicMotor.h"
#include <avdweb_AnalogReadFast.h>
// ----- Constructors -----
HydraulicMotor::HydraulicMotor(){

}
HydraulicMotor::HydraulicMotor(int pwmPin, int csPin, int dirPin, int slpPin){
_currentPin = csPin;
_sleepPin = slpPin;
_directionPin = dirPin;
_pwmPin = pwmPin;
dcMotor = new DimmerZero(pwmPin, false);
}

// ----- Begin methods -----
void HydraulicMotor::begin(){
  _currentGain = 0.01; // 10mV/A
  _currentOffset = 0.05; // offset of 50mV

// setup the motor pwm pin with values to get to 40kHz
_maxMotorValue = 300;
frequency = 10000;
dcMotor->setMaxValue(_maxMotorValue); // Set to 300
dcMotor->setFrequency(frequency); // Set to 10000
dcMotor->init();
analogReadResolution(10);
digitalWrite(_sleepPin, HIGH); // pull low to put into sleep mode

}
void HydraulicMotor::begin(int pwmPin, int csPin, int dirPin, int slpPin){
  _currentPin = csPin;
  _sleepPin = slpPin;
  _directionPin = dirPin;
  _pwmPin = pwmPin;
  begin();
}


void HydraulicMotor::setSpeed(double inputSpeed){
// the inputspeed is from -1 -> 0 -> 1
// negative 1 is full speed reverse
// zero is off
// positive 1 is full speed forward
dcMotor->setValue(dutyCycle(inputSpeed));
setDirection(inputSpeed);

}
double HydraulicMotor::getCurrent(){
  lastCurrent = map(analogReadFast(_currentPin),0,1023, 0, 1023); // 23us on SAMD21
  return lastCurrent;
}

void HydraulicMotor::setDirection(double inputPercent){
  digitalWrite(_directionPin, (inputPercent>=0));
}

int HydraulicMotor::dutyCycle(double inputPercent){
speed = (int)(abs(inputPercent)*_maxMotorValue);
return speed;
}

double HydraulicMotor::mapping(double x) {
  return _currentGain * (x-_currentOffset);
}

// Utility functions
double HydraulicMotor::map(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
