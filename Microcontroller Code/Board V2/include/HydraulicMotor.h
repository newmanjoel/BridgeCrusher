#ifndef HydraulicMotor_H
#define HydraulicMotor_H
#include "DCMotor.h"
#include "PID_v1.h"
#include "StopStartConditions.h"
#include "Settings.h"

class HydraulicMotor
{
  public:
    // ------ variables ------
    // PID Variables
    double Setpoint, Input, Output;
    // Default PID Values
    double Kp=0.5, Ki=0.1, Kd=0;
    // ----- object ----- (has a relationships)
    DCMotor* motor;
    //StopStartCondition* safety;
    PID* controlSystem;
    

    // ----- Constructor -----
    HydraulicMotor();
    HydraulicMotor(int i_pwmPin, int i_csPin, int i_dirPin, int i_slpPin);

    // starting points
    void setup(int i_pwmPin, int i_csPin, int i_dirPin, int i_slpPin );

    // primary looping point
    void update();

    void toJson(JsonObject inputJsonObject);

    
  private:
    
};

#endif
