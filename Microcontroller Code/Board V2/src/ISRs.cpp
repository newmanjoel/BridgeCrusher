#include "ISRs.h"


extern RotaryEncoderJoel encoder;
extern HydraulicMotor jack;
extern ADS1246 sensor;
extern StopStartCondition safety;

void start_ISR() {
  digitalRead(Start_NO) ? safety.Start = true : safety.Start = false;
  safety.stop_logic();
}

void stop_ISR() {
  digitalRead(Stop_NC) ? safety.Stop = false : safety.Stop = true;
  safety.stop_logic();
}

void lid_ISR() {
  digitalRead(Lid_NC) ? safety.Lid = false : safety.Lid = true;
  safety.stop_logic();
}

void max_ISR() {
  digitalRead(Max_NC) ? safety.Max = false : safety.Max = true;
  safety.stop_logic();
}

void min_ISR() {
  digitalRead(Min_NC) ? safety.Min = false : safety.Min = true;
  safety.stop_logic();
}

void adc_ISR() {
  sensor.setSensorReady();
}
void motor_ISR() {
  jack.motor->getCurrent();
}


void encoder_ISR() {
  encoder.tick();
}