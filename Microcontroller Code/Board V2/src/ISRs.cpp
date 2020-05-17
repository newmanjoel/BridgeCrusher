#include "ISRs.h"


extern RotaryEncoderJoel encoder;
extern HydraulicMotor jack;
extern ADS1246 sensor;
extern StopStartCondition safety;

void start_ISR() {
  safety.Start->poll();
  //digitalRead(Start_NO) ? safety.Start = true : safety.Start = false;
  safety.stop_logic();
}

void stop_ISR() {
  safety.Stop->poll();
  //digitalRead(Stop_NC) ? safety.Stop = false : safety.Stop = true;
  safety.stop_logic();
}

void lid_ISR() {
  safety.Lid->poll();
  //digitalRead(Lid_NC) ? safety.Lid = false : safety.Lid = true;
  safety.stop_logic();
}

void max_ISR() {
  safety.Max->poll();
  //digitalRead(Max_NC) ? safety.Max = false : safety.Max = true;
  safety.stop_logic();
}

void min_ISR() {
  safety.Min->poll();
  //digitalRead(Min_NC) ? safety.Min = false : safety.Min = true;
  safety.stop_logic();
}

void adc_ISR() {
  sensor.setSensorReady();
}
void motor_ISR() {
  //jack.motor->lastCurrent;
}


void encoder_ISR() {
  encoder.tick();
}