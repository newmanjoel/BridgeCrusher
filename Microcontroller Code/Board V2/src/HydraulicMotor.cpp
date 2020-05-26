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

void HydraulicMotor::toJson(JsonObject inputJsonObject)
{
  Reporting config = *Reporting::instance();
  if(config.MOTOR)
  {
    JsonObject motorJson = inputJsonObject.createNestedObject("MOTOR");
    motorJson["STATUS"] = "Untested";
    motorJson["CURRENT"] = motor->lastCurrent;
    motorJson["DIRECTION"] = motor->direction;
    motorJson["SPEED"]  = motor->speed;
    motorJson["SLEEP"] = motor->sleep;
  }
  if (config.PID)
  {
    JsonObject controlJson = inputJsonObject.createNestedObject("PID");
    controlJson["STATUS"] = "Untested";
    controlJson["INPUT"] = Input;
    controlJson["SETPOINT"] = Setpoint;
    controlJson["OUTPUT"]  = Output;
  }
  if(config.LIMITS)
  {
    JsonObject limitsJson = inputJsonObject.createNestedObject("LIMITS");
    limitsJson["STAUTS"] = "Untested";
    limitsJson["MAX"] = safety->Max->NO;
    limitsJson["MIN"] = safety->Min->NO;
    limitsJson["LID"] = safety->Lid->NO;
  }

}
