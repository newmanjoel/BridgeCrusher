#include "DCMotor.h"
#include <avdweb_AnalogReadFast.h>
#include "Wire.h"

DCMotor::DCMotor(){

}
DCMotor::DCMotor(int i_pwmPin, int i_csPin, int i_dirPin, int i_slpPin)
{
    currentPin = i_csPin;
    sleepPin = i_slpPin;
    directionPin = i_dirPin;
    pwmPin = i_pwmPin;
    motorInterface = new DimmerZero(pwmPin, false);
}
// ----- Begin methods -----
void DCMotor::setup(){
    currentGain = 0.01; // 10mV/A
    currentOffset = 0.05; // offset of 50mV

    // setup the motor pwm pin with values to get to 40kHz
    maxMotorValue = 300;
    frequency = 10000;
    motorInterface->setMaxValue(maxMotorValue); // Set to 300
    motorInterface->setFrequency(frequency); // Set to 10000
    motorInterface->init();
    analogReadResolution(10);
    digitalWrite(sleepPin, HIGH); // pull low to put into sleep mode

}
void DCMotor::setup(int i_pwmPin, int i_csPin, int i_dirPin, int i_slpPin){
    currentPin = i_csPin;
    sleepPin = i_slpPin;
    directionPin = i_dirPin;
    pwmPin = i_pwmPin;
    setup();
}
void DCMotor::setSpeed(double inputSpeed){
    // the inputspeed is from -1 -> 0 -> 1
    // negative 1 is full speed reverse
    // zero is off
    // positive 1 is full speed forward
    motorInterface->setValue(dutyCycle(inputSpeed));
    setDirection(inputSpeed);
}
void DCMotor::setDirection(double inputPercent){
    digitalWrite(directionPin, (inputPercent>=0));
}

int DCMotor::dutyCycle(double inputPercent){
    speed = (int)(abs(inputPercent)*maxMotorValue);
    return speed;
}

double DCMotor::mapping(double x) {
    return currentGain * (x-currentOffset);
}
double DCMotor::getCurrent(){
    // somehow this should probably be configured to only work if the pwm is high [TODO]
    lastCurrent = map(analogReadFast(currentPin),0,1023, 0, 1023); // 23us on SAMD21
    return lastCurrent;
}
// Utility functions
double DCMotor::map(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

