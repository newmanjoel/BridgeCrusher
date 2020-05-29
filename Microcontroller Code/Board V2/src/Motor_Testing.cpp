
#include "Motor_Testing.h"
#include "Wire.h"
#include "BridgeCrusherPins.h"
#include "avdweb_AnalogReadFast.h"
#include "Settings.h"
#include "Fsm.h"
#include "UserInterface.h"
#include "HydraulicMotor.h"
#include <Wire.h>

extern Userinterface UI;
extern Fsm screen;
extern int SETTINGS_SCREEN;

extern HydraulicMotor jack;
extern double nowWeight;

namespace motor_testing
{
    const char mainFormat[] = "%s: %4.3f";
    char buff[12];
    char largeBuffer[20];
    double dutyCycle = 0;
    byte Dist_pin = 1;
    double reading =0;
};

double motor_map(double x, double in_min, double in_max, double out_min, double out_max);


// Testing Screen
void  motor_testing_screen_enter() {
    // Print a message to the LCD.
    /*
    UI.lcd->clear();

    sprintf(motor_testing::largeBuffer, motor_testing::mainFormat, "Input: ", nowWeight);
    UI.lcd->setCursor(0, 0);
    UI.lcd->printf(motor_testing::largeBuffer);
    
    sprintf(motor_testing::largeBuffer, motor_testing::mainFormat, "Set: ", motor_testing::dutyCycle);
    UI.lcd->setCursor(0, 1);
    UI.lcd->printf(motor_testing::largeBuffer);

    sprintf(motor_testing::largeBuffer, motor_testing::mainFormat, "Amp: ", jack.motor->lastCurrent);
    UI.lcd->setCursor(0, 2);
    UI.lcd->printf(motor_testing::largeBuffer);

    sprintf(motor_testing::largeBuffer, motor_testing::mainFormat, "Out: ", jack.Output);
    UI.lcd->setCursor(0, 3);
    UI.lcd->printf(motor_testing::largeBuffer);
    */
    

}
void  motor_testing_screen_loop() {
    motor_testing::reading = (double)analogReadFast(motor_testing::Dist_pin);
    motor_testing::dutyCycle = motor_map(motor_testing::reading,0,1023, 0,-6);
    //jack.Output = motor_testing::dutyCycle;
    jack.Input = nowWeight;
    jack.Setpoint = motor_testing::dutyCycle;
    //jack.motor->setSpeed(motor_testing::dutyCycle); 

}
void  motor_testing_screen_exit() {

}

double motor_map(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
