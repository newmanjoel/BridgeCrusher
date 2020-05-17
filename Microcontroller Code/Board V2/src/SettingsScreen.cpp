#include "UserInterface.h"
#include "Settings.h"
#include "HydraulicMotor.h"
#include "ADS1246.cpp"
#include "Fsm.h"
#include "stdio.h"
#include "SettingsScreen.h"



extern Userinterface UI;
extern HydraulicMotor jack;
extern ADS1246 sensor;
extern Fsm screen;
extern int RUN_SCREEN;
extern Switch encoderSw;

extern NumberSetting pid_kp;
extern NumberSetting pid_ki;
extern NumberSetting pid_kd;
extern NumberSetting pid_setpoint;
extern NumberSetting weight_offset;
extern NumberSetting weight_gain;
extern NumberSetting weight_units;
extern NumberSetting *total_settings[];

namespace s_screen
{
    int number_of_settings; // maximum index of settings
    int current_setting; // the current setting that is in the menu

    const String menuText = "exit next prev rst  ";
    int menuIndex;

    int last_n = 0;
    bool last_select = 0;
    bool change_value = true;
    char buff[12];
    char empty_buff[12];
    const int changeValueIndex = 4;
    const int exitIndex = 0;
    const int nextIndex = 1;
    const int prevIndex = 2;
    const int resetIndex = 3;
    const char numberFormat[] = "%7.3f";
};

void settings_transition()
{
    UI.encoder->setPosition(0);
    s_screen::last_n = 0;
    s_screen::last_select = 0;
    s_screen::current_setting = 0;
    s_screen::menuIndex = s_screen::changeValueIndex;
    s_screen::change_value = false;
}


void initialSettingsSetup()
{
    s_screen::number_of_settings = 4;
    s_screen::current_setting = 0;
    s_screen::menuIndex = s_screen::changeValueIndex;
    s_screen::change_value = false;

    pid_setpoint.jsonName = "Speed";
    pid_setpoint.DefaultValue = 180;
    pid_setpoint.value = &jack.Setpoint; 
    pid_setpoint.min = 0.00;
    pid_setpoint.max = 1000;
    pid_setpoint.stepValue = 1;
    pid_setpoint.longDescription = "Crushing Rate : kg/s";
    pid_setpoint.begin();

    pid_kp.jsonName = "PropGain";
    pid_kp.DefaultValue = 0.01;
    pid_kp.value = &jack.Kp;
    pid_kp.min = 0.0;
    pid_kp.max = 100000;
    pid_kp.stepValue = 0.01;
    pid_kp.longDescription = "PID P settings";
    pid_kp.begin();

    pid_ki.jsonName = "IntGain";
    pid_ki.DefaultValue = 1;
    pid_ki.value = &jack.Ki;
    pid_ki.min = 0.0;
    pid_ki.max = 1000;
    pid_ki.stepValue = 0.1;
    pid_ki.longDescription = "PID I settings";
    pid_ki.begin();

    pid_kd.jsonName = "DevGain";
    pid_kd.DefaultValue = 0.0;
    pid_kd.value =&jack.Kd;
    pid_kd.min = 0.0;
    pid_kd.max = 10;
    pid_kd.stepValue = 0.01;
    pid_kd.longDescription = "PID D settings";
    pid_kd.begin();

    weight_offset.jsonName = "Offset";
    weight_offset.DefaultValue = 0;
    weight_offset.value = &sensor.offset; 
    weight_offset.min = -100;
    weight_offset.max = 100;
    weight_offset.stepValue = 0.1;
    weight_offset.longDescription = "Weight offset in kg";
    weight_offset.begin();

    weight_gain.jsonName = "Gain";
    weight_gain.DefaultValue = 1;
    weight_gain.value = &sensor.corrected_gain; 
    weight_gain.min = -5;
    weight_gain.max = 5;
    weight_gain.stepValue = 0.01;
    weight_gain.longDescription = "Weight gain";
    weight_gain.begin();

    
    weight_units.jsonName = "Units";
    weight_units.DefaultValue = 1;
    weight_units.value = &sensor.corrected_gain; 
    weight_units.min = -5;
    weight_units.max = 5;
    weight_units.stepValue = 0.01;
    weight_units.longDescription = "Weight gain";
    weight_units.begin();

}



void singleSetting_entry()
{
    //UI.lcd->clear();
    
    UI.lcd->setCursor(0, 0);
    // UI.lcd->print("");
    
    UI.lcd->printf(total_settings[(s_screen::current_setting)%s_screen::number_of_settings]->jsonName.c_str());
    UI.lcd->setCursor(0, 1);
    UI.lcd->printf(total_settings[(s_screen::current_setting)%s_screen::number_of_settings]->longDescription.c_str());
    UI.lcd->setCursor(0,2);
    //UI.lcd->printf("Value:");

    
    char largeBuffer[18];
    sprintf(largeBuffer, "Value: %s %d", s_screen::buff, s_screen::menuIndex);
    UI.lcd->printf(largeBuffer);
    
    UI.lcd->setCursor(0,3);
    UI.lcd->printf(s_screen::menuText.c_str());
    if (s_screen::menuIndex == s_screen::changeValueIndex && !s_screen::change_value)
    {
        UI.lcd->setCursor(5, 2);
        UI.lcd->printf(">");
    }
    else if (s_screen::menuIndex != s_screen::changeValueIndex) {
        UI.lcd->setCursor(s_screen::menuIndex*5+4, 3);
        UI.lcd->printf("<");
    }
}

void singleSetting_loop()
{

    int n = UI.encoder->getPosition();
    bool select = encoderSw.NO;

    bool newSelect = select==true && s_screen::last_select == false;
    bool increment = n > s_screen::last_n;
    bool decrement = n < s_screen::last_n;
    bool turned = n != s_screen::last_n;

    NumberSetting temp = *total_settings[(s_screen::current_setting)%s_screen::number_of_settings];
    sprintf(s_screen::buff, s_screen::numberFormat, *temp.value);

    
    if(newSelect && s_screen::menuIndex == s_screen::exitIndex)
    {
        screen.trigger(RUN_SCREEN);
    }
    else if(newSelect && s_screen::menuIndex == s_screen::resetIndex)
    {
        initialSettingsSetup();
    }
    else if(newSelect && s_screen::menuIndex == s_screen::nextIndex)
    {
        s_screen::current_setting = s_screen::current_setting + 1;
        s_screen::current_setting = s_screen::current_setting % s_screen::number_of_settings;
        UI.lcd->clear();
    }
    else if(newSelect && s_screen::menuIndex == s_screen::prevIndex)
    {
        s_screen::current_setting = s_screen::current_setting - 1;
        s_screen::current_setting = abs(s_screen::current_setting) % s_screen::number_of_settings;
        UI.lcd->clear();
    }else if (newSelect && s_screen::menuIndex == s_screen::changeValueIndex)
    {
        s_screen::change_value = !s_screen::change_value;
    }
    
    
    
    if(s_screen::change_value && turned && s_screen::menuIndex == s_screen::changeValueIndex)
    {
        if (increment)
        {
            total_settings[(s_screen::current_setting)%s_screen::number_of_settings]->increment();
        }
        else if (decrement)
        {
            total_settings[(s_screen::current_setting)%s_screen::number_of_settings]->decrement();
        }
    }
    else if(!s_screen::change_value && turned)
    {

        if(increment)
        {
            s_screen::menuIndex = s_screen::menuIndex + 1;
        }
        else if(decrement)
        {
            s_screen::menuIndex = s_screen::menuIndex - 1;
        }
        s_screen::menuIndex = abs(s_screen::menuIndex)%5;
    }

    s_screen::last_n = n;
    s_screen::last_select = select;
    
    
}

void singleSetting_exit()
{
    //current_setting = 0;
}