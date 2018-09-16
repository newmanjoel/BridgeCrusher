#include "hydraulic_jack.h"

HydraulicJack::HydraulicJack(int dirPin, int speedPin) {
  motorDirPin = dirPin;
  motorSpeed = speedPin;
  motorRunning = false;
  motorSpeed = 255;
  mode = PIDMODE;
  setpoint = 1; // The unit is kg/sec
  input = 0;
  Kp = 1;
  Ki = 10;
  Kd = 0;
  myPID = new PID(&input, &output, &setpoint, Kp, Ki, Kd, P_ON_E , DIRECT);
  myPID->SetMode(MANUAL);
}

void HydraulicJack::extend(void) {
  digitalWrite(motorDirPin, HIGH);
  motorRunning = true;
}

void HydraulicJack::retract(void) {
  digitalWrite(motorDirPin, LOW);
  motorRunning = true;
}

void HydraulicJack::stop(void) {
  motorRunning = false;
  analogWrite(motorSpeedPin, 0);
}

void HydraulicJack::setSpeed(int speed) {
  speed = speed>255 ? 255 : (speed<0 ? 0 : speed);
  motorSpeed = speed;
}

void HydraulicJack::setMode(int mode) {
  if (mode == PIDMODE) {
    myPID->SetMode(AUTOMATIC);
  }
  if (mode == MANUALMODE) {
    myPID->SetMode(MANUAL);
  }
}

bool HydraulicJack::running(void) {
  return motorRunning;
}

void HydraulicJack::spin(double currentFeedback) {
  input = currentFeedback;
  if (motorRunning == true) {
    myPID->Compute();
    if (mode == MANUALMODE) {
      analogWrite(motorSpeedPin, motorSpeed);
    }
    if (mode == PIDMODE) {
      analogWrite(motorSpeedPin, output);
    }
  }
}

