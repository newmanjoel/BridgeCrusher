#include "Screen_Testing.h"

extern RotaryEncoderJoel encoder;
extern LiquidCrystal_PCF8574 lcd;
extern Filter lpf;
extern ADS1246 sensor;

// Testing Screen
void testing_screen_enter() {
  String weight = String(lpf.output * sensor.sensitivity,3);
  String clicks = String(encoder.getPosition()+0.00000000001,1);
  lcd.setCursor(4, 0);
  lcd.print(weight+ " ");
  lcd.setCursor(4, 1);
  lcd.print(clicks+ " ");
}
void testing_screen_loop() {

}
void testing_screen_exit() {

}