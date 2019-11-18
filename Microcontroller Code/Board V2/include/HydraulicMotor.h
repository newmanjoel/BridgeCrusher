#ifndef HydraulicMotor_H
#define HydraulicMotor_H
#include "DCMotor.h"
#include "PID_v1.h"
#include "StopStartConditions.h"

class HydraulicMotor
{
  public:
    // ------ variables ------
    // PID Variables
    double Setpoint, Input, Output;
    // Default PID Values
    double Kp=1, Ki=1, Kd=0;
    // ----- object ----- (has a relationships)
    DCMotor* motor;
    StopStartCondition* safety;
    PID* controlSystem;
    

    // ----- Constructor -----
    HydraulicMotor();
    HydraulicMotor(int i_pwmPin, int i_csPin, int i_dirPin, int i_slpPin, StopStartCondition& i_safety );

    // starting points
    void setup(int i_pwmPin, int i_csPin, int i_dirPin, int i_slpPin, StopStartCondition& i_safety );

    // primary looping point
    void update();

    
  private:
    
};

#endif
