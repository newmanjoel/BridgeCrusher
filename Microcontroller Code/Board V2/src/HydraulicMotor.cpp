#include "HydraulicMotor.h"

// ----- Constructors -----
HydraulicMotor::HydraulicMotor(){};
HydraulicMotor::HydraulicMotor(int i_pwmPin, int i_csPin, int i_dirPin, int i_slpPin, StopStartCondition& i_safety)
{
  setup(i_pwmPin, i_csPin, i_dirPin, i_slpPin, i_safety);
}

// ----- Begin methods -----
void HydraulicMotor::setup(int i_pwmPin, int i_csPin, int i_dirPin, int i_slpPin, StopStartCondition& i_safety ){
  motor = new DCMotor(i_pwmPin,i_csPin,i_dirPin,i_slpPin);
  safety = &i_safety;
  controlSystem = new PID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
  controlSystem->SetOutputLimits(0.0,1.0);
  controlSystem->SetMode(AUTOMATIC);
  controlSystem->SetSampleTime(10);
}

void HydraulicMotor::update(){
  safety->stop_logic();
  controlSystem->Compute();
  /*
  if (safety->StopDown | safety->StopUp)
  {
    controlSystem->SetMode(MANUAL);

    if (not safety->last_StopDown & safety->StopDown)
    {
      motor->setSpeed(0);
    }
    if (not safety->last_StopUp & safety->StopUp)
    {
      motor->setSpeed(0);
    }
  }
  else
  {
    motor->setSpeed(Output);
  }
  */
  
}
