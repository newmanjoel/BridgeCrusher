#ifndef DCMotor_H
#define DCMotor_H
#include <DimmerZero.h>

class DCMotor
{
    public:
        volatile int speed;
        volatile int frequency;
        DimmerZero* motorInterfaceA;
        DimmerZero* motorInterfaceB;
        volatile double lastCurrent;
        volatile int maxMotorValue;
        volatile bool direction;
        volatile bool sleep;
        volatile double desiredSpeed;

        // ----- Constructor -----
        DCMotor(int pwmPin, int csPin, int dirPin, int slpPin);
        DCMotor();

        // starting points
        void setup(int pwmPin, int csPin, int dirPin, int slpPin);
        void setup();

        // set the speed of the motor
        void setSpeed(double inputSpeed);

        void setDirection(bool forward);

        double map(double x, double in_min, double in_max, double out_min, double out_max);

  private:
    volatile int currentPin, sleepPin, pwmPin, directionPin;
    volatile double currentGain, currentOffset;


    // calculate the duty cycle required
    int dutyCycle(double inputPercent);
    double current_mapping(double x);
};



#endif