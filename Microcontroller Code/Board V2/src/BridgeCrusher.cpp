#include "BridgeCrusherPins.h"
#include "StopStartConditions.h"
//#include <Arduino.h>
//#include <Encoder.h>
#include "RotaryEncoderJoel.h"
#include "ADS1246.cpp"
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

#include "Screen_Testing.h"

void setup();
void loop();

void stop_logic();

void unloaded_enter();
void unloaded_loop();
void unloaded_exit();

void testing_enter();
void testing_loop();
void testing_exit();

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


HydraulicMotor dcmotor(Mtr_PWM, Mtr_CS, Mtr_DIR, Mtr_SLP);
RotaryEncoderJoel encoder(Enc_A, Enc_B, Enc_SW_NO);
ADS1246 sensor;
StopStartCondition safety;

LiquidCrystal_PCF8574 lcd(0x27);
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
State testing(&testing_enter, &testing_loop, &testing_exit);
State homing(&homing_enter, &homing_loop, &homing_exit);
State calibrating(&calibrating_enter, &calibrating_loop, &calibrating_exit);
State testing_screen(&testing_screen_enter, &testing_screen_loop, &testing_screen_exit);
State polling_state(&polling_enter, &polling_loop, &polling_exit);


Fsm fsm(&unloaded);
Fsm screen(&testing_screen);
Fsm motor(&testing);
Fsm polling(&polling_state);

/*
volatile boolean Start;
volatile boolean Stop;
volatile boolean Lid;
volatile boolean Max;
volatile boolean Min;
volatile boolean StopDown;
volatile boolean StopUp;
volatile boolean StartDown;
volatile boolean StartUp;
*/
volatile boolean StartAdc;
volatile boolean EncoderClick;
volatile long EncoderValue;

unsigned long time;
unsigned long debugTime;
double offset = 0;
int displayRefreshRate = 1000 / 3; // this is in ms. 3Hz


void setup() {
  // Start comms on usb
  Serial.begin(115200);
  lcd.begin(20, 4);
  lcd.home();
  lcd.clear();
  lcd.noBlink();
  lcd.noCursor();
  lcd.setBacklight(255);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("ADC:");
  lcd.setCursor(0, 1);
  lcd.print("ENC:");

  // initialize volatile variables
  /*Start = false;
  Stop = false;
  Lid = false;
  Max = false;
  Min = false;
  StopDown = false;
  StopUp = false;
  StartDown = false;
  StartUp = false;
  StartAdc = false;
  */
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
  attachInterrupt(Mtr_PWM, motor_ISR, FALLING);
  analogReadResolution(12);


  // setup the filters
  // you can get the calculated values from here https://www.micromodeler.com/dsp/
  //   0.18261049892987757, -0.18757390839139598, 0.1826104989298776, 1.4034582124101722, -0.5117751386002309,// b0, b1, b2, a1, a2
  //   0.25, -0.44774427666635713, 0.25000000000000006, 1.7542884087063622, -0.8399913314609916// b0, b1, b2, a1, a2

  lpf_stage2.begin(0.18261049892987757, -0.18757390839139598, 0.1826104989298776, 1.4034582124101722, -0.5117751386002309);
  lpf.begin(0.25, -0.44774427666635713, 0.25000000000000006, 1.7542884087063622, -0.8399913314609916, &lpf_stage2);


  sensor.begin(Adc_CS, Adc_STR, Adc_RST);
  SPI.begin();
  sensor.setupADC();

  dcmotor.begin();
  dcmotor.setSpeed(0.6);

  screen.add_timed_transition(&testing_screen, &testing_screen, displayRefreshRate, NULL);


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


}

void loop() {
  //static double duty = 0;
  //static unsigned long start = 0;
  screen.run_machine();
  polling.run_machine();
  motor.run_machine();

  //duty = encoder.getPosition();
  //duty /= 300;
  //dcMotor.setValue(int(encoder.getPosition()));

  //digitalWrite(Mtr_PWM, Start);
  //Start = !Start;
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
  return dcmotor.speed;
}
bool readMotorDirection() {
  return true;
}
double readMotorCurrent() {
  return dcmotor.lastCurrent;
}


void polling_enter(){}
void polling_exit(){}
void polling_loop(){
  if ((millis() - debugTime) > 1000 || sensor.newData) {
    debugTime = millis();
    StaticJsonDocument<300> doc;
    // Tested Works
    JsonObject adcJson = doc.createNestedObject("FORCE");
    double adcValue = sensor.readADC();
    lpf.next(adcValue + sensor.offset);
    //adcValue -= offset;
    //adcJson["ADC"] = adcValue;
    //adcJson["Corrected"] = adcValue + sensor.offset;
    adcJson["KG"] = lpf.output * sensor.sensitivity;
    //adcJson["FS"] = sensor.SPS[sensor.currentSPS];
    //adcJson["LPF"] = lpf.next(adcValue * sensor.sensitivity);
    //adcJson["OFFSET"] = sensor.offset;


    // Untested
    //JsonObject limitsJson = doc.createNestedObject("LIMITS");
    //limitsJson["MAX"] = Max;
    //limitsJson["MIN"] = Min;
    //limitsJson["LID"] = Lid;

    // Untested
    //JsonObject hmiJson = doc.createNestedObject("HMI");
    //hmiJson["START"] = Start;
    //hmiJson["STOP"] = Stop;

    // Untested
    //JsonObject ledJson = doc.createNestedObject("LEDS");
    //ledJson["START_LED"] = readLid();
    //ledJson["STOP_LED"] = readLid();

    // Untested
    //JsonObject motorJson = doc.createNestedObject("MOTOR");
    //motorJson["CURRENT"] = dcmotor.lastCurrent;
    //motorJson["DIRECTION"] = readMotorDirection();
    //motorJson["SPEED"] = readMotorSpeed();
    //motorJson["SLEEP"] = readMotorSleep();

    // Tested Works
    //JsonObject encJson = doc.createNestedObject("ENCODER");
    //encJson["POSITION"] = encoder.getPosition();
    //encJson["DIRECTION"] = encoder.getDirection();
    //encJson["BUTTON"] = encoder.getSwitch();


    // Tested Works
    //JsonObject encJson = doc.createNestedObject("DISTANCE");
    //encJson["Voltage"] = map((double)analogReadFast(Dist_VAL),0,1023, 0,3.3);

    //serializeJson(doc, Serial);
    //Serial.println();
    Serial.println(lpf.output*sensor.sensitivity,10);
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
void testing_enter() {

}
void testing_loop() {
  // this is used for testing the motor
  

}
void testing_exit() {

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
