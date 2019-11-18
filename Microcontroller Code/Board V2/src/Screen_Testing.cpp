#include "Screen_Testing.h"

extern RotaryEncoderJoel encoder;
extern LiquidCrystal_PCF8574 lcd;
extern Filter lpf;
extern ADS1246 sensor;
extern HydraulicMotor jack;
extern Switch encoderSw;

// Testing Screen
void testing_screen_enter() {
  
  jack.Setpoint = encoder.getPosition();
  jack.Setpoint /= 10;
  jack.Input = lpf.output * sensor.sensitivity;
  //String weight = String(lpf.output * sensor.sensitivity,3);
  String clicks = String(encoder.getPosition()+0.00000000001,1);
  String encSw = String(encoderSw.NO + 0.00000001,1);
  lcd.setCursor(4, 0);
  lcd.print(String(jack.Setpoint,2)+ " ");
  lcd.setCursor(4, 1);
  lcd.print(String(jack.Input,2)+ " ");
  lcd.setCursor(4, 2);
  lcd.print(String(jack.Output,2)+ " ");
  lcd.setCursor(4, 3);
  lcd.print(encSw+ " ");
}
void testing_screen_loop() {

}
void testing_screen_exit() {

}