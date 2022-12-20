#include <Arduino.h>
#include "Configuration.h"

HardwareConfiguration hardware;

void setup() {
  // put your setup code here, to run once:
  hardware.init();

  hardware.Jacks[1].pin1->function = "Something new";

}

void loop() {
  // put your main code here, to run repeatedly:
}