#ifndef hydraulic_jack_h
#define hydraulic_jack_h

#include <PID_v1.h>
#include <Arduino.h>

#define PIDMODE 1
#define MANUALMODE 2


class HydraulicJack{
  public:
    HydraulicJack(int dirPin, int speedPin);
    void extend(void);
    void retract(void);
    void stop(void);
    void setSpeed(int speed);
    void setMode(int mode);
    bool running(void);
    void spin(double currentFeedback);
  private:
    double Kp , Ki, Kd;
    double output, setpoint;
    double input, oldInput;
    bool motorRunning;
    int motorSpeed;
    int motorDirPin;
    int motorSpeedPin;    
    int mode;
    PID *myPID;
};

#endif

