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
    motorInterfaceA = new DimmerZero(pwmPin, false);
    motorInterfaceB = new DimmerZero(directionPin,false);
    setup();
}
// ----- Begin methods -----
void DCMotor::setup(){
    sleep = true;
    currentGain = 0.0008126; // 
    currentOffset = -0.0210246; // offset of 50mV

    // setup the motor pwm pin with values to get to 40kHz
    maxMotorValue = 300;
    frequency = 10000;
    motorInterfaceA->setMaxValue(maxMotorValue); // Set to 300
    motorInterfaceA->setFrequency(frequency); // Set to 10000
    motorInterfaceA->init();
    motorInterfaceB->setMaxValue(maxMotorValue);
    motorInterfaceB->setFrequency(frequency);
    motorInterfaceB->init();
    lastCurrent = 0;
    //analogReadResolution(10);
    digitalWrite(sleepPin, sleep); // pull low to put into sleep mode

}
void DCMotor::setup(int i_pwmPin, int i_csPin, int i_dirPin, int i_slpPin){
    currentPin = i_csPin;
    sleepPin = i_slpPin;
    directionPin = i_dirPin;
    pwmPin = i_pwmPin;
    setup();
}
void DCMotor::setSpeed(double inputSpeed){
    // the inputspeed is from  0 -> 1
    // zero is off
    // positive 1 is full speed in the direction
    //direction = inputSpeed>0;
    desiredSpeed = inputSpeed;
    dutyCycle(inputSpeed);
    motorInterfaceA->setValue(direction?speed:0);
    motorInterfaceB->setValue(direction?0:speed);
    lastCurrent = current_mapping(analogReadFast(currentPin));

}

void DCMotor::setDirection(bool forward)
{
    direction = forward==true;
}

int DCMotor::dutyCycle(double inputPercent){
    speed = (int)(abs(inputPercent)*maxMotorValue);
    return speed;
}

double DCMotor::current_mapping(double prescale_current) {
    double scaled_current = map(prescale_current,0,4095,0,4095);
    return currentGain * scaled_current +currentOffset;
}

//double DCMotor::getCurrent(){
//    lastCurrent = mapping(analogReadFast(currentPin));
//    return lastCurrent;
//}


// Utility functions
double DCMotor::map(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

