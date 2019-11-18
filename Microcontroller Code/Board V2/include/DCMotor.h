#ifndef DCMotor_H
#define DCMotor_H
#include <DimmerZero.h>
class DCMotor
{
    public:
        volatile int speed;
        volatile int frequency;
        DimmerZero* motorInterface;
        volatile double lastCurrent;
        volatile int maxMotorValue;

        // ----- Constructor -----
        DCMotor(int pwmPin, int csPin, int dirPin, int slpPin);
        DCMotor();

        // starting points
        void setup(int pwmPin, int csPin, int dirPin, int slpPin);
        void setup();

        // set the speed of the motor
        void setSpeed(double inputSpeed);

        // get the current from the motor
        double getCurrent();

        // set the direction of the motor
        void setDirection(double inputPercent);

        double map(double x, double in_min, double in_max, double out_min, double out_max);

  private:
    volatile int currentPin, sleepPin, pwmPin, directionPin;
    volatile double currentGain, currentOffset;


    // calculate the duty cycle required
    int dutyCycle(double inputPercent);
    double mapping(double x);
};



#endif