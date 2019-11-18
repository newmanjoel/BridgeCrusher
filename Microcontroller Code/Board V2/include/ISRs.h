#ifndef ISRS_H
#define ISRS_H

#include <Arduino.h>
#include "Wire.h"
#include "BridgeCrusherPins.h"
#include "RotaryEncoderJoel.h"
#include "HydraulicMotor.h"
#include "ADS1246.cpp"
#include "StopStartConditions.h"


void start_ISR();
void stop_ISR();
void lid_ISR();
void max_ISR();
void min_ISR();
void motor_ISR();
void adc_ISR();
void encoder_ISR();

#endif