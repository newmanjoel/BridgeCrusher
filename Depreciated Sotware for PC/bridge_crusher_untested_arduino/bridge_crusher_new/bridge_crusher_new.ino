#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#include <Arduino.h>
#include <LiquidTWI.h>
#include <Wire.h>
#include <Encoder.h>
#include <Adafruit_ADS1015.h>

#include "hydraulic_jack.h"
#include "settings.h"



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


#define stop_led 27 // was on pin 7
#define start_led 25 // was on pin 8

#define numOfButtons  6
#define adcPort 3

//************************** INSERT COMMENTS **************************//


volatile boolean stopButton = 0;
volatile boolean motorRunning = 0;

//************************** INSERT COMMENTS **************************//
volatile float currentInput;
volatile float maxInput;
volatile float offset;
double voltageRise, lastReadVoltage;
unsigned long lastReadTime;

volatile int currentScreen;   // this is the current screen that I am on
volatile int motorSpeed;      // will go from 50-100, default = 100
volatile int motorDirection;  // will go from 0-1, default = 0
volatile int jogSpeed;        // will go from 50-100, default = 70
volatile bool motorEnable;    // will go from on-off, default = on
volatile bool usbMode;        // will go from on-off, default = off
volatile bool safetyOverride; // will go from on-off, default = off
volatile bool usbDebug;       // will go from on-off, default = off
volatile bool lidSwitch = false;
volatile bool motorMinSwitch = false;
volatile bool motorMaxSwitch = false;
volatile bool limits = false;
volatile const bool jogging = true;





//************************** DECLARING OBJECTS FOR PERIPHERALS **************************//
Encoder knob(rotary_A, rotary_B);   // rotary encoder for user control
LiquidTWI lcdscreen(0);            // lcd screen for user display
Adafruit_ADS1115 adc;               // 16-bit ADC to communicate with the LCD and force sensor (s-sensor)

Settings status = Settings();

ThreadController controll = ThreadController();
Thread adcThread = Thread();
Thread updateScreen = Thread();
Thread motorMove = Thread();

//************************** PERIPHERAL CONTROL FUNCTIONS **************************//
//void motorRaise(bool joggingVariable);
//void motorLower(bool joggingVariable);
void motorStop();
void rotary_encoder(void);
void getADCValue(void);

void ISR_button_stop(void);
void ISR_safety_sensor(void);


void setup() {



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



  Serial.println("Before the lcd");
  lcdscreen.begin(20, 4);
  lcdscreen.setBacklight(HIGH);
  Serial.println("after the lcd");
  currentInput = adc.readADC_SingleEnded(adcPort); // this is the sensor value in V
  offset = currentInput * VOLTS2KGS;
  Serial.println("Setup is all done");

  adcThread.onRun(getADCValue);
  adcThread.setInterval(10); // update at 100Hz
  adcThread.enabled = true;

  updateScreen.onRun(runScreen);
  updateScreen.setInterval(50); // update at 20Hz
  updateScreen.enabled = true;


  controll.add(&adcThread);
  controll.add(&updateScreen);

  interrupts();

}// end of void setup()

bool startConditions() {
  return (!digitalRead(button_start));
}
bool homeConditions() {
  return (!digitalRead(rotary_C));
}

void getADCValue() {

  currentInput = adc.readADC_SingleEnded(adcPort); // this is the sensor value in V
  

  sendVoltageOverUSB();
}

void sendVoltageOverUSB() {
  Serial.print("V:");
  Serial.println(currentInput);
  //Serial.print(" I:");
  //Serial.println(analogRead(motor_amp) - 5);
  Serial.flush();
}

void loop() {

}

void serialEvent() {
  char temp = Serial.read();
  switch (temp) {
    case 'U':
      motorRunning = true;
      //myPID.SetMode(AUTOMATIC);
      //motorRaise(not jogging);
      break;
    case 'D':
      motorRunning = true;
      //myPID.SetMode(MANUAL);
      //motorLower(not jogging);
      break;
    case 'S':
      //motorStop();
      motorRunning = false;
      break;
    default:
      Serial.print("Arduino -> Received |");
      Serial.print(temp);
      Serial.println("|");
      Serial.flush();
  }
}


void ISR_button_stop() {
  motorRunning = false;
}

void ISR_safety_switches() {
  pollLimits();
}


void motorUpdate() {
  if (startConditions()) {
  }

  if (homeConditions()) {

  }
}

bool pollLimits() {
  limits = false;
  bool safetySwitch = digitalRead(switch_int);
  if (usbDebug) {
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

      unsigned long time = millis();
      while (millis() - time < 1000) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz
      }


    }
    else if (motorMinSwitch) {
      Serial.print("Min:");
      Serial.println(motorMinSwitch);
      //motorRaise(jogging);
      unsigned long time = millis();
      while (millis() - time < 300) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz

      }
      //motorStop();
      time = millis();
      while (millis() - time < 1000) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz
      }

    }
    else if (motorMaxSwitch) {
      Serial.print("Max:");
      Serial.println(motorMaxSwitch);

      //motorLower(jogging);
      unsigned long time = millis();
      while (millis() - time < 300) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz
      }
      //motorStop();
      time = millis();
      while (millis() - time < 1000) {
        getADCValue();
        sendVoltageOverUSB();
        delay(33); // update at 30Hz
      }

    }

    motorRunning = false;
    limits = true;
  }

  return limits;
}

void runScreen(void) {
  /*
    ____LBS______KGS____
    M:_xxxx.xx__xxxx.xx_
    C:_xxxx.xx__xxxx.xx_
    ONE LINE MENU
  */
  char local[7];
  //local = {'','','','','','',''};
  String Max_lbs = dtostrf(maxInput * KGS2LBS, 4, 2, local);
  //local = {'','','','','','',''};
  String Max_kgs = dtostrf(maxInput , 4, 2, local);
  //local = {'','','','','','',''};
  String Cur_lbs = dtostrf(currentInput * KGS2LBS, 4, 2, local);
  //local = {'','','','','','',''};
  String Cur_kgs = dtostrf(currentInput, 4, 2, local);

  lcdscreen.setCursor(0, 0); lcdscreen.print("    LBS      KGS");
  lcdscreen.setCursor(0, 1); lcdscreen.print("M: "); lcdscreen.print(Max_lbs); lcdscreen.print("  "); lcdscreen.print(Max_kgs);
  lcdscreen.setCursor(0, 2); lcdscreen.print("C: "); lcdscreen.print(Cur_lbs); lcdscreen.print("  "); lcdscreen.print(Cur_kgs);
}

void motorStop() {
  analogWrite(motor_pwm, 0);
  motorRunning = false;
}




