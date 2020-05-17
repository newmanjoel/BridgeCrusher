#ifndef Motor_Testing_h
#define Motor_Testing_h

#include "BridgeCrusherPins.h"
#include <avdweb_AnalogReadFast.h>
#include "Settings.h"
#include "Fsm.h"

// Motor Testing Screen

void motor_testing_screen_enter();
void motor_testing_screen_loop();
void motor_testing_screen_exit();


#endif