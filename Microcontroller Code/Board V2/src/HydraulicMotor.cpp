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
  controlSystem->SetOutputLimits(-1,1);
  controlSystem->SetMode(AUTOMATIC);
}

void HydraulicMotor::update(){
  controlSystem->Compute();
  motor->setSpeed(Output);
}

