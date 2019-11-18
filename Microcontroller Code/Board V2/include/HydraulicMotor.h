#ifndef HydraulicMotor_H
#define HydraulicMotor_H
#include <DimmerZero.h>
class HydraulicMotor
{
  public:
    // ----- variables -----
    volatile int speed;
    int frequency;
    DimmerZero* dcMotor;
    volatile double lastCurrent;


    // ----- Constructor -----
    HydraulicMotor(int pwmPin, int csPin, int dirPin, int slpPin);
    HydraulicMotor();

    // starting points
    void begin(int pwmPin, int csPin, int dirPin, int slpPin);
    void begin();

    // set the speed of the motor
    void setSpeed(double inputSpeed);

    // get the current from the motor
    double getCurrent();

    // set the direction of the motor
    void setDirection(double inputPercent);

    double map(double x, double in_min, double in_max, double out_min, double out_max);

  private:
    volatile int _currentPin, _sleepPin, _pwmPin, _directionPin;
    volatile int _maxMotorValue;
    volatile double _currentGain, _currentOffset;


    volatile int8_t _oldState;
    volatile long _position;         // Internal position (4 times _positionExt)
    volatile long _positionExt;      // External position
    volatile long _positionExtPrev;  // External position (used only for direction checking)

    // calculate the duty cycle required
    int dutyCycle(double inputPercent);
    double mapping(double x);
};

#endif
