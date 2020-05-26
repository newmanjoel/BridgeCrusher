#include "Screen_Run.h"
#include "Settings.h"
#include "Fsm.h"

extern Userinterface UI;
extern Filter lpf;
extern Switch encoderSw;
extern volatile double maxWeight;
extern volatile double nowWeight;
extern NumberSetting pid_setpoint;
extern Fsm screen;
extern int SETTINGS_SCREEN;
extern HydraulicMotor jack;


namespace runScreen
{
    const char numberFormat[] = "%8.3f";
    const char mainFormat[] = "%s: %s %s";
    char buff[12];
    char largeBuffer[20];
    char units[5] = "kgs";
    bool last_select = false;
    int last_n = 0;
};

void run_screen_setup()
{
    UI.encoder->setPosition(0);
    runScreen::last_n = 0;
    
}

// Testing Screen
void run_screen_enter() {
    // Print a message to the LCD.
    sprintf(runScreen::buff, runScreen::numberFormat, jack.safety->test);
    sprintf(runScreen::largeBuffer, runScreen::mainFormat, "", runScreen::buff, "");
    UI.lcd->setCursor(0, 0);
    UI.lcd->print(jack.safety->test);


    sprintf(runScreen::buff, runScreen::numberFormat, jack.motor->lastCurrent*1000);
    sprintf(runScreen::largeBuffer, runScreen::mainFormat, "amp", runScreen::buff, runScreen::units);
    UI.lcd->setCursor(0, 1);
    UI.lcd->print(runScreen::largeBuffer);

    sprintf(runScreen::buff, runScreen::numberFormat, jack.motor->desiredSpeed);
    sprintf(runScreen::largeBuffer, runScreen::mainFormat, "duty", runScreen::buff, runScreen::units);
    UI.lcd->setCursor(0, 2);
    UI.lcd->print(runScreen::largeBuffer);

    sprintf(runScreen::buff, "%4.3f", *pid_setpoint.value);
    sprintf(runScreen::largeBuffer, runScreen::mainFormat, "Speed", runScreen::buff, "kg/s");
    UI.lcd->setCursor(0, 3);
    UI.lcd->print(runScreen::largeBuffer);

}
void run_screen_loop() {
    int n = UI.encoder->getPosition();
    bool select = encoderSw.NO;

    bool newSelect = select==true && runScreen::last_select == false;
    bool increment = n > runScreen::last_n;
    bool decrement = n < runScreen::last_n;
    bool turned = n != runScreen::last_n;

    if(turned)
    {
        if (increment)
        {
            pid_setpoint.increment();
        }
        else if (decrement)
        {
            pid_setpoint.decrement();
        }
    }

    if (newSelect)
    {
        //screen.trigger(SETTINGS_SCREEN);
    }


    runScreen::last_n = n;
    runScreen::last_select = select;

}
void run_screen_exit() {
    //jack.controlSystem->SetMode(MANUAL);
    //jack.motor->setSpeed(0);
    //UI.lcd->clear();
}