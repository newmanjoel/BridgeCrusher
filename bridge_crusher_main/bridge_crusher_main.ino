

/*
  Bridge Crusher Code for APEGBC

  Description:
  This code drives a hydraulic pump, stores measurements from an S force sensor, and displays values to an LCD display.
  This code is for a bridge crusher, and its purpose is to test the strength of popsickle bridge designs of students.

  The circuit:
   refer to "[insert something here]"

  created 14 Jan. 2017
  by Matthew McCann with help from the majestic Joel Newman

  Modified on APR 24 by Joel
  Change: added safety override func

  This example code is in the public domain.

*/




//************************** INSERT COMMENTS **************************//
#include <Arduino.h>
#include <LiquidTWI.h>
#include <Wire.h>
#include <Encoder.h>
#include <MenuSystem.h>
#include <Adafruit_ADS1015.h>
#include <PID_v1.h>

//************************** INSERT COMMENTS **************************//
// analog pin assignments AS DIGITAL INPUTS (i.e. analog 0 = 0+54)
#define NOT_AN_INTERRUPT -1
#define motor_flt 54 //Motor controller fault pin
#define motor_dir 48 //Motor controller direcion pin
#define motor_pwm 46 // pwm signal used to control motor
#define motor_enable 44 // motor controller enable

#define rotary_A 18 // channel A of the rotary encoder (is this correct?)
#define rotary_B 31 // channel B of the rotary encoder (was on pin 17)
#define rotary_C 29 // channel C of the rotary encoder (was on pin 16)

#define button_start 23  // start button for user control (was on pin 6)

#define button_stop 19  // stop button for user control
#define lid_switch 33   // tie together with the interrupt somehow
#define motor_max 35    // INTERRUPT 4 for bump sensor when the MAXIMUM extension of the hydraulic pump occur (LOW=motor @ max)
#define motor_min 37    // INTERRUPT 5 for bump sensor when the MINIMUM  extension of the hydraulic pump occurs (LOW=motor @ min)
#define switch_int 3 // the interrupt that goes though all of the limit swiches
#define motor_amp A15

#define RUNSCREEN 1
#define JOGSCREEN 2
#define SETTINGSCREEN 3

#define KGS2LBS 2.20462
//#define VOLTS2KGS 0.03814755474  // volts2kgs = voltage*resolution/supply // based on calculations
#define VOLTS2KGS 0.02771364  // based on emprical data

#define stop_led 27 // was on pin 7
#define start_led 25 // was on pin 8

#define numOfButtons  6
#define adcPort 3

//************************** INSERT COMMENTS **************************//


volatile boolean stopButton = 0;
volatile boolean motorRunning = 0;

//************************** INSERT COMMENTS **************************//
volatile float _currentInput;
volatile float _maxInput;
volatile float offset;
double voltageRise, output, setpoint, lastReadVoltage;
unsigned long lastReadTime;
double Kp = 1, Ki = 10, Kd = 0;
volatile int _currentScreen;   // this is the current screen that I am on
volatile int _motorSpeed;      // will go from 50-100, default = 100
volatile int _motorDirection;  // will go from 0-1, default = 0
volatile int _jogSpeed;        // will go from 50-100, default = 70
volatile bool _motorEnable;    // will go from on-off, default = on
volatile bool _usbMode;        // will go from on-off, default = off
volatile bool _safetyOverride; // will go from on-off, default = off
volatile bool _usbDebug;       // will go from on-off, default = off
volatile bool lidSwitch = false;
volatile bool motorMinSwitch = false;
volatile bool motorMaxSwitch = false;
volatile bool limits = false;
volatile const bool jogging = true;





//************************** DECLARING OBJECTS FOR PERIPHERALS **************************//
Encoder knob(rotary_A, rotary_B);   // rotary encoder for user control
LiquidTWI _lcdscreen(0);            // lcd screen for user display
Adafruit_ADS1115 adc;               // 16-bit ADC to communicate with the LCD and force sensor (s-sensor)
PID myPID(&voltageRise, &output, &setpoint, Kp, Ki, Kd, P_ON_E , DIRECT);

//************************** DIAGNOSTIC PROGRAM INITIALIZERS **************************//
void motor_diagnostics();
void userInterface_diagnostics();
void myDelay(int);

//************************** INSERT A COMMENT HERE **************************//
void on_mu2_mi1_selected(MenuItem* p_menu_item);
void on_mu2_mi2_selected(MenuItem* p_menu_item);
void on_mu2_mi3_selected(MenuItem* p_menu_item);
void on_mu2_mi4_selected(MenuItem* p_menu_item);
void on_mu2_mi5_selected(MenuItem* p_menu_item);
void on_mi1_selected(MenuItem* p_menu_item);
void on_mi3_selected(MenuItem* p_menu_item);

//************************** PERIPHERAL CONTROL FUNCTIONS **************************//
void motorRaise(bool joggingVariable);
void motorLower(bool joggingVariable);
void motorStop();
void rotary_encoder(void);
void getADCValue(void);
//************************** DEFINING THE MENU SYSTEM **************************//
MenuSystem ms;  // top level menu
Menu mm("");
MenuItem _mi1("JOG");       // this will be the jog screen
Menu _mu2("SETTINGS");       // this will be the setting screen
MenuItem _mi3("HOME");       // this will be the HOME position

//************************** MENU ITEMS FOR THE SETTINGS SCREEN **************************//
MenuItem _mu2_mi1("MOTOR SPEED    "); // this will be Motor speed menu item
MenuItem _mu2_mi2("JOG SPEED      "); // this will be Jog speed menu item
MenuItem _mu2_mi3("USB MODE       "); // this will be usb mode menu item
MenuItem _mu2_mi4("SAFE OVERRIDE  "); // this will be override menu item
MenuItem _mu2_mi5("USB DEBUG      "); // this will be usb debug menu item
MenuItem _mu2_mi6("Calibration    ");
MenuItem _mu2_mi7("BACK           "); // this is the back menu

//************************** INTERRUPT SERVICE RUITINE CODE **************************//
void ISR_motor_max(void);
void ISR_motor_min(void);
void ISR_button_stop(void);
void ISR_lid_sensor(void);


////////********* START OF THE SETUP() FUNCTION  *********////////
////////*********                                *********////////

void setup() {
  mm.add_item(&_mi1, &on_mi1_selected);
  mm.add_menu(&_mu2);
  mm.add_item(&_mi3, &on_mi3_selected);
  ms.set_root_menu(&mm);
  _mu2.add_item(&_mu2_mi1, &on_mu2_mi1_selected);
  _mu2.add_item(&_mu2_mi2, &on_mu2_mi2_selected);
  _mu2.add_item(&_mu2_mi3, &on_mu2_mi3_selected);
  _mu2.add_item(&_mu2_mi4, &on_mu2_mi4_selected);
  _mu2.add_item(&_mu2_mi5, &on_mu2_mi5_selected);
  _mu2.add_item(&_mu2_mi6, &on_mu2_mi6_selected);
  _mu2.add_item(&_mu2_mi7, &on_mu2_mi7_selected);

  // set the defaults
  offset = 0.0;
  _currentScreen = RUNSCREEN;
  _motorSpeed = 255;
  _motorDirection = HIGH;
  _jogSpeed = 200;
  _motorEnable = true;
  _usbMode = false;
  _safetyOverride = false;
  _usbDebug = false;
  motorRunning = false;
  lidSwitch = false;
  motorMinSwitch = false;
  motorMaxSwitch = false;

  setpoint = 1; // 5 kg/sec?
  voltageRise = 0;
  output = 0;
  lastReadTime = 0;
  lastReadVoltage = 0;


  // initialize i2c adc
  adc.begin();
  adc.setGain(GAIN_TWOTHIRDS);// 2/3x gain +/- 6.144V  1bit = 0.1875mV (default)

  Serial.begin(115200);
  Serial.println("this is the setup");



  // motor stuff
  pinMode (motor_flt, INPUT);           // analog input, measures the current of the motor (only while running)
  pinMode (motor_dir, OUTPUT);          // digital output, changes the direction of the motor
  pinMode (motor_pwm, OUTPUT);          // digital output, sets the speed of the motor
  pinMode (motor_enable, OUTPUT);       // digital output, allows movement of the motor
  pinMode (motor_amp, INPUT);


  // stop conditions
  pinMode(motor_max, INPUT);
  pinMode(motor_min, INPUT);
  pinMode(button_stop, INPUT_PULLUP);
  pinMode(lid_switch, INPUT);
  pinMode(switch_int, INPUT);
  digitalWrite(switch_int, LOW);


  // other buttons
  pinMode (button_start, INPUT_PULLUP); // digital input, button used for starting the motor
  pinMode(rotary_C, INPUT_PULLUP);      // digital input, the "click" of the rotary encoder

  // lights
  pinMode (stop_led, OUTPUT);
  pinMode (start_led, OUTPUT);
  digitalWrite(stop_led, HIGH);
  digitalWrite(start_led, HIGH);


  attachInterrupt(digitalPinToInterrupt(button_stop), ISR_button_stop, CHANGE);
  attachInterrupt(digitalPinToInterrupt(switch_int), ISR_safety_switches, CHANGE);
  // int 3 taken by rotary encoder



  Serial.println("Before the lcd");
  _lcdscreen.begin(20, 4);
  _lcdscreen.setBacklight(HIGH);
  errorScreen("LOADING COMPLETE");
  Serial.println("after the lcd");

  //ms.select();

  changeScreen(RUNSCREEN);

  // this is the calibrate from the settings
  _currentInput = adc.readADC_SingleEnded(adcPort); // this is the sensor value in V
  offset = _currentInput * VOLTS2KGS;

  Serial.println("Setup is all done");

}// end of void setup()



////////********* START OF THE MAIN LOOP         *********////////
////////*********                                *********////////

bool stopConditions() {

}
bool startConditions() {
  return (!digitalRead(button_start) and !motorRunning and digitalRead(button_stop) and !pollLimits());
}

void getADCValue() {

  _currentInput = adc.readADC_SingleEnded(adcPort); // this is the sensor value in V
  _currentInput = _currentInput * VOLTS2KGS - offset; // convert voltage to kg

  // these next two lines are to avoid unbounded values which have no significance...
  if (_currentInput >= 12287) {
    _currentInput = 0;
  }
  _maxInput = (_maxInput < _currentInput) ? _currentInput : _maxInput;
  voltageRise = ((_currentInput - lastReadVoltage) * 1000) / ((double)(millis() - lastReadTime));
  lastReadTime = millis();
  lastReadVoltage = _currentInput;
  /*
    if(digitalRead(motor_pwm)){
    motor_amps = analogRead(motor_amp);
    motor_amps -= 5; // remove 50mV offset
    }
  */


}

void sendVoltageOverUSB() {
  char buffers[16];
  dtostrf(_currentInput, 8, 3, buffers);
  Serial.print("V:");
  Serial.println(buffers);
  //Serial.print(" I:");
  //Serial.println(analogRead(motor_amp) - 5);
  Serial.flush();
}

void loop() {
  //rotary encoder input from the user
  rotary_encoder();
  limits = pollLimits();

  // note: !motorRunning returns true, this represents that the motor is not running
  if (startConditions()) {
    motorRunning = true;
    myPID.SetMode(AUTOMATIC);
    motorRaise(not jogging);
  }

  // we want to stop the motor if notRunning
  // this routine will get called a shit ton
  if (!motorRunning) {
    myPID.SetMode(MANUAL);
    analogWrite(motor_pwm, 0);
    motorStop();
  }
  else {
    if (myPID.Compute()) {
      analogWrite(motor_pwm, output);
      // Serial.print("PID OUTPUT ");
      // Serial.print(output);
      // Serial.print(" PID INPUT: ");
      // Serial.println(voltageRise);
    }
  }

  //limits = pollLimits();
  getADCValue();
  sendVoltageOverUSB();


}

void serialEvent() {
  char temp = Serial.read();
  switch (temp) {
    case 'U':
      motorRunning = true;
      myPID.SetMode(AUTOMATIC);
      motorRaise(not jogging);
      break;
    case 'D':
      motorRunning = true;
      myPID.SetMode(MANUAL);
      motorLower(not jogging);
      break;
    case 'S':
      motorStop();
      motorRunning = false;
      break;
    default:
      Serial.print("Arduino -> Received |");
      Serial.print(temp);
      Serial.println("|");
      Serial.flush();
  }
}


////////********* INTERRUPT SERVICE ROUTINES     *********////////
////////*********                                *********////////

void ISR_motor_max() {
  // perhaps move the motors slightly so that it doesn't rest on the sensor permanently when testing
  motorRunning = false;
}

void ISR_motor_min() {
  // perhaps move the motors slightly so that it doesn't rest on the sensor permanently when testing
  motorStop();
  motorRaise(jogging);
  myDelay(50);
  motorStop();
}

void ISR_button_stop() {
  motorRunning = false;
}

void ISR_lid_sensor() {
  // insert code, may need another type of interrupt used instead of using the interrupt on CHANGE as declared
  motorRunning  = false;
}

void ISR_safety_switches() {
  motorRunning = false;
  motorStop();
  if (_usbDebug) {
    Serial.println("SAFETY SWITCH TRIP");
  }
}



////////*********       GENERAL FUNCTIONS     *********////////
////////*********                             *********////////

bool pollLimits() {
  limits = false;
  bool safetySwitch = digitalRead(switch_int);
  if (_usbDebug) {
    Serial.print("LD:");
    Serial.print(digitalRead(lid_switch));

    Serial.print(" MX:");
    Serial.print(digitalRead(motor_max));

    Serial.print(" MN:");
    Serial.print(digitalRead(motor_min));

    Serial.print(" ST:");
    Serial.print(digitalRead(button_start));

    Serial.print(" SP:");
    Serial.print(digitalRead(button_stop));

    Serial.print(" EN:");
    Serial.print(digitalRead(rotary_C));

    Serial.println('\r');
    Serial.flush();
    //Serial.println();
  }
  if (safetySwitch) {
    // do nothing
    // this is when the door is open
  }
  else {
    motorStop();
    lidSwitch = digitalRead(lid_switch);
    motorMinSwitch = digitalRead(motor_min);
    motorMaxSwitch = digitalRead(motor_max);

    if (lidSwitch) {
      Serial.print("lid: ");
      Serial.println(lidSwitch);
      errorScreen("Please Close the lid");
      unsigned long time = millis();
      while (millis() - time < 1000) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz
      }
      resetScreen();
      changeScreen(_currentScreen);

    }
    else if (motorMinSwitch) {
      Serial.print("Min:");
      Serial.println(motorMinSwitch);
      errorScreen("Reached Min Extension, Cannot go further");
      motorRaise(jogging);
      unsigned long time = millis();
      while (millis() - time < 300) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz

      }
      motorStop();
      time = millis();
      while (millis() - time < 1000) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz
      }
      resetScreen();
      changeScreen(_currentScreen);
    }
    else if (motorMaxSwitch) {
      Serial.print("Max:");
      Serial.println(motorMaxSwitch);
      errorScreen("Reached Max Extension, Cannot go further");
      motorLower(jogging);
      unsigned long time = millis();
      while (millis() - time < 300) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz
      }
      motorStop();
      time = millis();
      while (millis() - time < 1000) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz
      }
      resetScreen();
      changeScreen(_currentScreen);
    }

    motorRunning = false;
    limits = true;
  }


  return limits;
}


void rotary_encoder(void) { // handles the rotary encoder for user input

  // rotary_encoder variables
  static int currentKnobValue = 0;
  static int lastKnobValue = 0;
  static int currentRotaryC = 0;
  static int lastRotaryC = 1;

  // set the menu based on user input from the rotary encoder
  //limits = pollLimits();

  Update();

  //limits = pollLimits();
  currentKnobValue = (knob.read() / 4);
  currentRotaryC = digitalRead(rotary_C);

<<<<<<< Updated upstream
  if (currentKnobValue > lastKnobValue) {
    ms.next();
  }

  if (currentKnobValue < lastKnobValue) {
    ms.prev();
  }
=======
  currentKnobValue > lastKnobValue ? ms.next() : ms.prev();

  /*
    if (currentKnobValue > lastKnobValue) {
    ms.next();
    }
    if (currentKnobValue < lastKnobValue) {
    ms.prev();
    }
  */
>>>>>>> Stashed changes

  if (currentRotaryC > lastRotaryC) {
    ms.select();
    if (ms.get_current_menu()->get_name() == "SETTINGS") {
      changeScreen(SETTINGSCREEN);
    }
    if (ms.get_current_menu()->get_name() == "JOG") {
      changeScreen(JOGSCREEN);
    }
    if (ms.get_current_menu()->get_name() == "") {
      changeScreen(RUNSCREEN);
    }
  }
  // reset values for the next void main loop
  lastKnobValue = currentKnobValue;
  lastRotaryC = currentRotaryC;


} // end of rotary_encoder()

void myDelay(int num) { //general delay function that does not use an interrupt
  // the total time for this delay is approximately (2cycles/instruction)*(6instuctions/call)*(255call/16000000 cycles)~191.25 microcseconds
  // approx 522 times will give 100ms
  for ( int j = 0; j < (522 * num); j++) {
    for ( int i = 0; i < 255; i++) {
    }
  }
}// end of myDelay()


void jogScreen(void) {
  const int delayTime = 100;
  int currentKnobValue, lastKnobValue;
  motorStop();
  delayMicroseconds(10000); //10ms? a whole 100th second?!its a flash!

  // this is the jog menu item
  /*
    JOG screen
    CW:  RAISE MOTOR
    CCW: LOWER MOTOR
    CLICK TO GO BACK
  */

  // now let's put information on the user LCD
  resetScreen();
  _lcdscreen.print("JOG SCREEN");
  _lcdscreen.setCursor(2, 1); _lcdscreen.print("CW:  RAISE MOTOR");
  _lcdscreen.setCursor(2, 2); _lcdscreen.print("CCW: LOWER MOTOR");
  _lcdscreen.setCursor(0, 3); _lcdscreen.print("CLICK TO GO BACK");

  while (true) {
<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
    if (!digitalRead(rotary_C)) {
      ms.back();
      return;
    }
    motorMinSwitch = digitalRead(motor_min);
    currentKnobValue = (knob.read() / 4);
    if (not pollLimits()) {

      if (currentKnobValue > lastKnobValue) { // this is CCW
        if (!digitalRead(motor_max)) {
          motorLower(not jogging);
          delay(delayTime); // have it run for 1/10 second
          motorStop();
        }
        else {
          motorRaise(jogging);
          delay(2 * delayTime);
          motorStop();
        }
      }

      if (currentKnobValue < lastKnobValue) { // this is CW
        if (!digitalRead(motor_min)) {
          motorRaise(not jogging);
          delay(delayTime); // have it run for 1/10 second
          motorStop();
        }
        else {
          motorLower(jogging);
          delay(2 * delayTime);
          motorStop();
        }
      }

      lastKnobValue = currentKnobValue;
    }
    else {
      resetScreen();
      _lcdscreen.print("JOG SCREEN");
      _lcdscreen.setCursor(2, 1); _lcdscreen.print("CW:  RAISE MOTOR");
      _lcdscreen.setCursor(2, 2); _lcdscreen.print("CCW: LOWER MOTOR");
      _lcdscreen.setCursor(0, 3); _lcdscreen.print("CLICK TO GO BACK");
    }

  }

}// end of jogScreen()

void runScreen(void) {
  /*
    ____LBS______KGS____
    M:_xxxx.xx__xxxx.xx_
    C:_xxxx.xx__xxxx.xx_
    ONE LINE MENU
  */
  if (not _usbMode) {
    _lcdscreen.setCursor(0, 0); _lcdscreen.print("    LBS      KGS");
    _lcdscreen.setCursor(0, 1); _lcdscreen.print("M:");
    _lcdscreen.setCursor(0, 2); _lcdscreen.print("C:");
  }
  else {
    _lcdscreen.setCursor(0, 0); _lcdscreen.print("USB MODE ACTIVE");
  }

} // end of runScreen()

void resetScreen(void) {
  _lcdscreen.clear();
  _lcdscreen.home();

<<<<<<< Updated upstream
}// end of resetScreen()


void errorScreen(String _whatToPrint) {
=======
void errorScreen(String whatToPrint) {
>>>>>>> Stashed changes
  // TODO: make sure that length returns zero based, not one base
  resetScreen();
  for (int i = 0; i < _whatToPrint.length(); i++) {
    if (i == 20) {
      _lcdscreen.setCursor(0, 1);
    }
    if (i == 40) {
      _lcdscreen.setCursor(0, 2);
    }
    if (i == 60) {
      _lcdscreen.setCursor(0, 3);
    }
    _lcdscreen.print(_whatToPrint.charAt(i));
  }
}// end of errorScreen()


void userInterface_diagnostics(void) {

  int buttonsToTest[] = {button_stop, lid_switch, motor_max, motor_min, button_start, rotary_C};
  String buttonLabels[] = {"Stop Button", "Lid Switch", "Motor Max", "Motor Min", "Start Button", "Encoder Switch"};
  int buttonNormals[] = {0, 0, 0, 0, 0, 0};

  resetScreen();
  for (int i = 0; i < numOfButtons; i++) {
    errorScreen("Please press the    " + buttonLabels[i]);
    while (digitalRead(buttonsToTest[i]) != buttonNormals[i]) {
      delayMicroseconds(1000); // delays 1ms
    }
  }
  unsigned long time = millis();
  if (millis() - time < 1000) {
    getADCValue();
    sendVoltageOverUSB();
    delay(33); // update at 30Hz
  }


}// end of userInterface_diagnostics()

<<<<<<< Updated upstream

void changeScreen(int _whichScreen) {
  if (_usbDebug) {
=======
void changeScreen(int whichScreen) {
  if (usbDebug) {
>>>>>>> Stashed changes
    Serial.print("Chaning the screen from  ");
    Serial.print(_currentScreen);
    Serial.print("->");
    Serial.println(_whichScreen);
  }
  _currentScreen = _whichScreen;
  resetScreen();
  switch (_whichScreen) {
    case RUNSCREEN:
      runScreen();
      delay(100);
      break;
    case JOGSCREEN:
      jogScreen();
      delay(100);
      break;
    case SETTINGSCREEN:
      settingScreen();
      delay(100);
      break;
    default:
      errorScreen("I got an error on changing the screen");
  }
}// end of changeScreen()


int setSetting(int _whichSetting, int _value) {
  switch (_whichSetting) {
    case 0:
      _value = constrain(_value, 5, 10);
      _motorSpeed  = (int) map(_value, 1, 10, 0, 255);
      break;
    case 1:
      _value = constrain(_value, 5, 10);
      _jogSpeed = (int) map(_value, 1, 10, 0, 255);
      break;
    case 2:
      _value = constrain(_value, 0, 1);
      if (_value == 0) {
        _motorEnable = false;
      }
      else {
        _motorEnable = true;
      }
      break;
    case 3:
      _value = constrain(_value, 0, 1);
      if (_value == 0) {
        _usbMode = false;
      }
      else {
        _usbMode = true;
      }
      break;
    case 4:
      _value = constrain(_value, 0, 1);
      if (_value == 0) {
        _safetyOverride = false;
      }
      else {
        _safetyOverride = true;
      }
      break;
    case 5:
      _value = constrain(_value, 0, 1);
      if (_value == 0) {
        _usbDebug = false;
      }
      else {
        _usbDebug = true;
      }
      break;
    default:
      errorScreen("Tried to set a setting that was not 0-5");
      break;
  }

  return _value;
} // end of setSetting()


int getSetting(int _whichSetting) {
  switch (_whichSetting) {
    case 0:
      return _motorSpeed;
    case 1:
      return _jogSpeed;
    case 2:
      return _motorEnable;
    case 3:
      return _usbMode;
    case 4:
      return _safetyOverride;
    case 5:
      return _usbDebug;
    default:
      errorScreen("Tried to get a setting that was not 0-4");
  }
}


void settingScreen(void) {
  Menu const* cp_menu = ms.get_current_menu();
  MenuComponent const* cp_menu_sel = cp_menu->get_selected();
  String current_name = cp_menu_sel->get_name();

  int total_menus = cp_menu->get_num_menu_components(); // returns the number of the total menu items
  int current_menu;// =  cp_menu_sel->get_current_component_num(); // returns the number of the current menu item
  for (int i = 0; i <= total_menus; i++) {

    MenuComponent const* cp_m_comp = cp_menu->get_menu_component(i);
    String looping_name = cp_m_comp->get_name();
    if (looping_name == current_name) {
      current_menu = i;
    }
  }

  // TODO: Confirm zero base or one base!
  // TODO: decide how to deal with the current setting
  //Serial.print("Current Menu: ");Serial.print(current_menu);Serial.print(" || Total number: ");Serial.print(total_menus);Serial.print(" || Current name: ");Serial.println(cp_menu_sel->get_name());

  if (current_menu == 0) { // the first menu, Assuming one base
    _lcdscreen.setCursor(0, 0); _lcdscreen.print("->"); _lcdscreen.print( cp_menu->get_menu_component(current_menu)->get_name());
    _lcdscreen.setCursor(0, 1); _lcdscreen.print(cp_menu->get_menu_component(current_menu + 1)->get_name());
    _lcdscreen.setCursor(0, 2); _lcdscreen.print(cp_menu->get_menu_component(current_menu + 2)->get_name());
    _lcdscreen.setCursor(0, 3); _lcdscreen.print("more");
  }

  else {

    if ((current_menu + 3) < total_menus) {
      _lcdscreen.setCursor(0, 0); _lcdscreen.print(cp_menu->get_menu_component(current_menu - 1)->get_name());
      _lcdscreen.setCursor(0, 1); _lcdscreen.print("->"); _lcdscreen.print(cp_menu->get_menu_component(current_menu)->get_name());
      _lcdscreen.setCursor(0, 2); _lcdscreen.print(cp_menu->get_menu_component(current_menu + 1)->get_name());
      _lcdscreen.setCursor(0, 3); _lcdscreen.print("more");
    }
    else {
      _lcdscreen.setCursor(0, 0); _lcdscreen.print(cp_menu->get_menu_component(total_menus - 4)->get_name());
      _lcdscreen.setCursor(0, 1);
      if ((total_menus - current_menu ) == 3) {
        _lcdscreen.print("->");
      }
      _lcdscreen.print(cp_menu->get_menu_component(total_menus - 3)->get_name());
      _lcdscreen.setCursor(0, 2);
      if ((total_menus - current_menu) == 2) {
        _lcdscreen.print("->");
      }
      _lcdscreen.print(cp_menu->get_menu_component(total_menus - 2)->get_name());
      _lcdscreen.setCursor(0, 3);
      if ((total_menus - current_menu ) == 1) {
        _lcdscreen.print("->");
      }
      _lcdscreen.print(cp_menu->get_menu_component(total_menus - 1)->get_name());
    }

  }

}// end of settingScreen()


void Update(void) {
  static char local[8];
  switch (_currentScreen) {
    case RUNSCREEN:
      dtostrf(_maxInput * KGS2LBS, 4, 2, local);
      _lcdscreen.setCursor(3, 1); _lcdscreen.print(local); _lcdscreen.print("  ");
      dtostrf(_maxInput , 4, 2, local);
      _lcdscreen.setCursor(11, 1); _lcdscreen.print(local); _lcdscreen.print(" ");
      dtostrf(_currentInput * KGS2LBS, 4, 2, local);
      _lcdscreen.setCursor(3, 2); _lcdscreen.print(local); _lcdscreen.print("  ");
      dtostrf(_currentInput, 4, 2, local);
      _lcdscreen.setCursor(11, 2); _lcdscreen.print(local); _lcdscreen.print(" ");
      _lcdscreen.setCursor(0, 3); _lcdscreen.print(ms.get_current_menu()->get_selected()->get_name()); _lcdscreen.print("     ");
      //runScreen();
      break;
    case JOGSCREEN:
      jogScreen();
      break;
    case SETTINGSCREEN:
      settingScreen();
      break;
    default:
      errorScreen("Reached a default case in update");
      break;
  }
}// end of Update()


void motorRaise(bool joggingVariable) {

  if (_usbDebug) {
    Serial.println("Turning on the motor going up");
  }
  digitalWrite(motor_dir, HIGH);
  _motorDirection = HIGH;

  if (joggingVariable) {
    analogWrite(motor_pwm, getSetting(1));
  }
  else {
    analogWrite(motor_pwm, getSetting(0));
  }
}


void motorLower(bool joggingVariable) {
  if (_usbDebug) {
    Serial.println("Turning on the motor going down");
  }
  digitalWrite(motor_dir, LOW);
  _motorDirection = LOW;
  if (joggingVariable) {
    analogWrite(motor_pwm, getSetting(1));
  }
  else {
    analogWrite(motor_pwm, getSetting(0));
  }

}// end of motorLower()


void motorStop() {
  analogWrite(motor_pwm, 0);
  motorRunning = false;

}// end of motorStop()


void changeSettings(int whichSetting) {
  int tempValue = 0;
  //delay(10); // this will delay 10 milliseconds
  //_lcdscreen.setCursor(15, 3); _lcdscreen.print("->"); _lcdscreen.print();
  knob.write(getSetting(whichSetting));
  while (digitalRead(rotary_C)) {
    tempValue = setSetting(whichSetting, knob.read());
    knob.write(tempValue);
    _lcdscreen.setCursor(15, 3); _lcdscreen.print("->"); _lcdscreen.print(tempValue);
    if (tempValue < 10) {
      _lcdscreen.print(" ");
    }
  }

}// end of changeSettings()



////////*********       CUSTOM MENU ITEMS     *********////////
////////*********                             *********////////



void on_mi3_selected(MenuItem* p_menu_item) {
  // this is home menu item
  _maxInput = 0;
  motorRunning = true;
  motorLower(not jogging);
}

void on_mu2_mi1_selected(MenuItem* p_menu_item) {
  //   this is motor speed menu item
  changeSettings(0);
}


void on_mu2_mi2_selected(MenuItem* p_menu_item) {
  // this is jog speed menu item
  changeSettings(1);
}


void on_mu2_mi3_selected(MenuItem* p_menu_item) {
  // this is the usb menu item
  changeSettings(3);
}


void on_mu2_mi4_selected(MenuItem* p_menu_item) {
  // this is the safe override option
  changeSettings(4);
}


void on_mu2_mi5_selected(MenuItem* p_menu_item) {
  // this is the usb debug option
  changeSettings(5);
}


void on_mu2_mi6_selected(MenuItem* p_menu_item) {
  // this is the calibrate from the settings
  _currentInput = adc.readADC_SingleEnded(2); // this is the sensor value in V
  offset = _currentInput * VOLTS2KGS;
  userInterface_diagnostics();
}


void on_mu2_mi7_selected(MenuItem* p_menu_item) {
  // this is the back menu from the settings
  ms.back();
}


void on_mi1_selected(MenuItem* p_menu_item) {
  changeScreen(JOGSCREEN);
  if (_usbDebug) {
    Serial.println("TOLD IT TO CHANGE TO JOGSCREEN");
  }
}
