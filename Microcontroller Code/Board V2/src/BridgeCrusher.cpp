#include "BridgeCrusherPins.h"
#include "StopStartConditions.h"
#include "RotaryEncoderJoel.h"
#include "ADS1246.h"
#include <ArduinoJson.h>
#include "HydraulicMotor.h"
#include <Fsm.h>
#include <avdweb_AnalogReadFast.h>
#include "Wire.h"
//#include "Adafruit_LiquidCrystal.h"
#include "Filter.h"
#include "ISRs.h"

// requires this library. Version 1.2. Can be found in the arduino library manager.
// http://www.mathertel.de/Arduino/LiquidCrystal_PCF8574.aspx
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

#include "Screen_Run.h"
#include "UserInterface.h"
#include "Settings.h"
#include "SettingsScreen.h"
#include "Motor_Testing.h"
#include "Arduino.h"
#include <SPI.h>




void setup();
void loop();

void stop_logic();
// TODO: move all of these to their own files
void unloaded_enter();
void unloaded_loop();
void unloaded_exit();

void motoring_enter();
void motoring_loop();
void motoring_exit();

void homing_enter();
void homing_loop();
void homing_exit();

void calibrating_enter();
void calibrating_loop();
void calibrating_exit();

//void testing_screen_enter();
//void testing_screen_loop();
//void testing_screen_exit();

void polling_enter();
void polling_loop();
void polling_exit();

void mySerialEvent();



NO_Switch encoderSw(Enc_SW_NO, String("Encoder Switch"));
NO_Switch startSw(Start_NO, String("Start Button"));
NC_Switch stopSw(Stop_NC, String("Stop Button"));
NC_Switch lidSw(Lid_NC, String("Lid Limit Switch"));
NC_Switch pistonMax(Max_NC, String("Piston Max Extension"));
NC_Switch pistonMin(Min_NC, String("Piston Min Extension"));

Switch* allSwitches[] = {&encoderSw,&startSw, &stopSw, &lidSw, &pistonMax, &pistonMin};

NumberSetting pid_kp;
NumberSetting pid_ki;
NumberSetting pid_kd;
NumberSetting pid_setpoint;
NumberSetting weight_offset;
NumberSetting weight_gain;
NumberSetting weight_units;

NumberSetting *total_settings[]={
    &pid_setpoint,
    &pid_kp,
    &pid_ki,
    &pid_kd,
    &weight_offset,
    &weight_gain,
    &weight_units
};


StopStartCondition safety(&stopSw,&startSw,&pistonMax,&pistonMin,&lidSw);
HydraulicMotor jack;
RotaryEncoderJoel encoder(Enc_A, Enc_B, Enc_SW_NO);
ADS1246 sensor;
LiquidCrystal_PCF8574 lcd(0x27);

Userinterface UI(6, allSwitches, &lcd, &encoder);


Filter lpf;
Filter lpf_stage2;

// States
/*
   State state_name(&enter_state_fun, &body_state_fun, &exit_state_fun)
*/

// physical states required
/*
   unloaded (safe mode)
   testing (actively crushing)
   homing (going to unloaded)
   calibrating (unknown)
*/
State unloaded(&unloaded_enter, &unloaded_loop, &unloaded_exit);
State motoring(&motoring_enter, &motoring_loop, &motoring_exit);
State homing(&homing_enter, &homing_loop, &homing_exit);
State calibrating(&calibrating_enter, &calibrating_loop, &calibrating_exit);
State polling_state(&polling_enter, &polling_loop, &polling_exit);

State run_screen(&run_screen_enter, &run_screen_loop, &run_screen_exit);
State settings_screen(&singleSetting_entry,&singleSetting_loop, &singleSetting_exit);
State motor_testing_screen(&motor_testing_screen_enter, &motor_testing_screen_loop, &motor_testing_screen_exit);


//Fsm fsm(&unloaded);
Fsm screen(&run_screen);
Fsm motor(&motoring);
Fsm polling(&polling_state);

volatile boolean StartAdc;
volatile boolean EncoderClick;
volatile long EncoderValue;
volatile double maxWeight;
volatile double nowWeight;

unsigned long time;
unsigned long debugTime;

double offset = 0;
int displayRefreshRate = 1000 / 3; // this is in ms. 3Hz

int RUN_SCREEN = 1;
int SETTINGS_SCREEN = 2;

Reporting *Reporting::s_instance = 0;
Reporting config = *Reporting::instance();


void setup() {
  // Start comms on usb
  Serial.begin(57600);
  Serial.setTimeout(200);
  
  //Reporting config = *Reporting::instance();
  config.defaults();

  UI.lcd->begin(20, 4);
  UI.lcd->home();
  UI.lcd->clear();
  UI.lcd->noBlink();
  UI.lcd->noCursor();
  UI.lcd->setBacklight(255);
 

  EncoderClick = false;
  EncoderValue = 0;


  // setup input and output of each pin
  pinMode(Enc_A, INPUT);
  pinMode(Enc_B, INPUT);
  pinMode(Enc_SW_NO, INPUT);
  pinMode(Mtr_DIR, OUTPUT);
  pinMode(Adc_STR, INPUT);
  pinMode(Adc_RDY, INPUT);
  pinMode(Max_NC, INPUT);
  pinMode(Min_NC, INPUT);
  pinMode(Lid_NC, INPUT);
  pinMode(Adc_CS, INPUT); // pin conflict
  pinMode(Adc_RST, OUTPUT);
  pinMode(Mtr_SLP, OUTPUT);
  pinMode(Dist_VAL, INPUT);
  pinMode(Mtr_CS, INPUT); // pin conflict



  //attachInterrupt(pin, ISR, mode);
  attachInterrupt(Enc_A, encoder_ISR, CHANGE);
  attachInterrupt(Enc_B, encoder_ISR, CHANGE);
  //attachInterrupt(Enc_SW_NO, ISR, RISING);
  attachInterrupt(Adc_RDY, adc_ISR, RISING); // double check change is right mode
  attachInterrupt(Max_NC, max_ISR, CHANGE);
  attachInterrupt(Lid_NC, lid_ISR, CHANGE);
  attachInterrupt(Min_NC, min_ISR, CHANGE);
  attachInterrupt(Stop_NC, stop_ISR, CHANGE);
  attachInterrupt(Start_NO, start_ISR, CHANGE);
  attachInterrupt(Mtr_PWM, motor_ISR, RISING);
  //analogReadResolution(10);
  analogReadResolution(12);


  // setup the filters
  // you can get the calculated values from here https://www.micromodeler.com/dsp/
  //   0.18261049892987757, -0.18757390839139598, 0.1826104989298776, 1.4034582124101722, -0.5117751386002309,// b0, b1, b2, a1, a2
  //   0.25, -0.44774427666635713, 0.25000000000000006, 1.7542884087063622, -0.8399913314609916// b0, b1, b2, a1, a2

  lpf_stage2.begin(0.18261049892987757, -0.18757390839139598, 0.1826104989298776, 1.4034582124101722, -0.5117751386002309);
  lpf.begin(0.25, -0.44774427666635713, 0.25000000000000006, 1.7542884087063622, -0.8399913314609916, &lpf_stage2);

  jack.setup(Mtr_PWM, Mtr_CS, Mtr_DIR, Mtr_SLP, safety);

  sensor.begin(Adc_CS, Adc_STR, Adc_RST);
  SPI.begin();
  sensor.setupADC();

  screen.add_timed_transition(&run_screen, &run_screen, displayRefreshRate, NULL);
  screen.add_timed_transition(&settings_screen, &settings_screen, displayRefreshRate, NULL);
  //screen.add_timed_transition(&motor_testing_screen, &motor_testing_screen, displayRefreshRate, NULL);

  motor.add_timed_transition(&motoring, &motoring, 10, NULL); // update at 100Hz
  screen.add_transition(&settings_screen, &run_screen, RUN_SCREEN, run_screen_setup);
  screen.add_transition(&run_screen, &settings_screen, SETTINGS_SCREEN, run_screen_setup);

  delay(1000);
  double offsetTemp = 0;
  int averageAmount = 0;
  for (int i = 0; i < 100; i++) {
    while (not sensor.newData) {};
    offsetTemp += sensor.readADC();
    averageAmount += 1;
  }
  offset = offsetTemp / averageAmount;

  sensor.offset = -offset;
  initialSettingsSetup();

}

void loop() {
  screen.run_machine();
  polling.run_machine();
  motor.run_machine();
  mySerialEvent();
}



bool readMax() {
  return digitalRead(Max_NC);
}
bool readMin() {
  return digitalRead(Min_NC);
}
bool readLid() {
  return digitalRead(Lid_NC);
}
bool readMotorSleep() {
  return true;
}
double readMotorSpeed() {
  return jack.motor->lastCurrent;
}
bool readMotorDirection() {
  return true;
}
double readMotorCurrent() {
  return jack.motor->lastCurrent;
}


void polling_enter(){}
void polling_exit(){}
void polling_loop()
{
  UI.poll();

  if (((millis() - debugTime) > 100 || sensor.newData)) {
  //if (((millis() - debugTime) > 100)) {
    debugTime = millis();
    //StaticJsonDocument<500> doc;
    DynamicJsonDocument doc(1000);
    config = *Reporting::instance();

    doc["Time"] = debugTime;
    
    if(config.CONFIG)
    {
      JsonObject configJson = doc.createNestedObject("Config");
      config.toJson(configJson);
    }
    

    if(config.SSENSOR){
      JsonObject adcJson = doc.createNestedObject("SSENSOR");
      sensor.toJson(adcJson);
    }
    // Tested Works
    /*
    if (config.FORCE)
    {
      JsonObject adcJson = doc.createNestedObject("FORCE");
      double adcValue = sensor.readADC();
      lpf.next(adcValue + sensor.offset);
      nowWeight = lpf.output * sensor.sensitivity;
      maxWeight = (maxWeight < nowWeight)? maxWeight : nowWeight;
      adcValue -= offset;
      adcJson["STATUS"] ="Tested";
      adcJson["ADC"] = adcValue;
      adcJson["Corrected"] = adcValue + sensor.offset;
      adcJson["KG"] = nowWeight;
      adcJson["FS"] = sensor.SPS[sensor.currentSPS];
      adcJson["LPF"] = lpf.next(adcValue * sensor.sensitivity);
      adcJson["OFFSET"] = sensor.offset;
    }
    */

    // Untested
    if (config.HMI)
    {
      JsonObject hmiJson = doc.createNestedObject("HMI");
      hmiJson["STATUS"] = "Untested";
      //hmiJson["START"] = Start;
      //hmiJson["STOP"] = Stop;
    }

    // Untested
    if (config.LEDS)
    {
      JsonObject ledJson = doc.createNestedObject("LEDS");
      ledJson["STATUS"] = "Untested";
      //ledJson["START_LED"] = readLid();
      //ledJson["STOP_LED"] = readLid();
    }

    // Untested
    if (config.JACK)
    {
      JsonObject motorJson = doc.createNestedObject("JACK");
      jack.toJson(motorJson);
    }

    // Tested Works
    if (config.ENCODER)
    {
      JsonObject encJson = doc.createNestedObject("ENCODER");
      encJson["STATUS"] ="Tested";
      encJson["POSITION"] = encoder.getPosition();
      encJson["DIRECTION"] = encoder.getDirection();
      encJson["BUTTON"] = encoder.getSwitch();
    }


    // Tested Works
    if (config.DISTANCE)
    {
      JsonObject distJson = doc.createNestedObject("DISTANCE");
      distJson["STATUS"] = "Tested";
      distJson["Voltage"] = analogReadFast(Dist_VAL);
    }
    serializeJson(doc, Serial);
    Serial.print('\n');
  }
}


// unloaded
void unloaded_enter() {

}
void unloaded_loop() {

}
void unloaded_exit() {

}
// testing
void motoring_enter() {
  //jack.update();
  
}
void motoring_loop() {
  // this is used for testing the motor
  //jack.motor->getCurrent();
  jack.Input = jack.motor->lastCurrent*1000;
  jack.update();
  jack.motor->setSpeed(jack.Output);
  
}
void motoring_exit() {

}
// homing
void homing_enter() {

}
void homing_loop() {

}
void homing_exit() {

}
// calibrating
void calibrating_enter() {

}
void calibrating_loop() {

}
void calibrating_exit() {

}
// Utility functions
double map(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void mySerialEvent()
{
  if(Serial.available()<10)
  {
    return;
  }
  if(Serial.available()>10)
  {
    StaticJsonDocument<200> sendBack[2];
    int amountToSendBack = -1;
    
    while(Serial.available()>10)
    {
      amountToSendBack++;
      //byte buffer[512];
      //Serial.readBytesUntil('\n',buffer,512);
      //String inputMessage = String((char *)buffer);
      //String inputMessage = Serial.readStringUntil('\n');
      //ReadLoggingStream loggingStream(Serial, Serial);
      

      StaticJsonDocument<200> response ; //= sendBack[amountToSendBack];
      StaticJsonDocument<500> doc;
      DeserializationError err = deserializeJson(doc, Serial);
      switch (err.code()) {
          case DeserializationError::Ok:
              response["Sucess"] = true;
              if (doc.containsKey("Config"))
              {
                Reporting::instance()->json(doc["Config"].as<JsonVariant>()); 
              }
              if (doc.containsKey("SSENSOR"))
              {
               sensor.jsonConfig(doc["SSENSOR"].as<JsonVariant>()); 
              }
              if (doc.containsKey("Command"))
              {
                JsonObject Commands = doc["Command"].as<JsonObject>();
                if(Commands.containsKey("Test"))
                {
                  //jack.safety->json(Commands["Test"].as<JsonObject>());
                  jack.safety->test = Commands["Test"].as<bool>();
                  response["Executed"] = "Test";
                  response["Value"] = jack.safety->test;
                }
              }
              break;
          case DeserializationError::InvalidInput:
              response["Sucess"] = false;
              response["Reason"] = "Invalid Input";
              break;
          case DeserializationError::NoMemory:
              response["Sucess"] = false;
              response["Reason"] = "NoMemory";
              break;
          default:
              response["Sucess"] = false;
              response["Reason"] = "Deserialization failed";
              break;
      };
    
      sendBack[amountToSendBack] = response;
      
    };
    for(int i = 0; i<=amountToSendBack ;i++)
    {
      serializeJson(sendBack[i], Serial);
      Serial.print('\n');
    }

  }
}
