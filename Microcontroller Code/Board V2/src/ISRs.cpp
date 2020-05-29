#include "ISRs.h"
extern Sensors io;

void start_ISR() {
  //safety.Start->poll();
  //safety.stop_logic();
  io.switches.start.poll();
}

void stop_ISR() {
  //safety.Stop->poll();
  //safety.stop_logic();

  io.switches.stop.poll();
}

void lid_ISR() {
  //safety.Lid->poll();
  //safety.stop_logic();
  io.switches.lid.poll();
}

void max_ISR() {
  //safety.Max->poll();
  //safety.stop_logic();
  io.switches.max.poll();
}

void min_ISR() {
  //safety.Min->poll();
  //safety.stop_logic();
  io.switches.min.poll();
}

void adc_ISR() {
  //sensor.setSensorReady();
  io.force.sensor.setSensorReady();
}
void motor_ISR() {
  //jack.motor->lastCurrent;
}


void encoder_ISR() {
  //encoder.tick();
  io.encoder.tick();
}