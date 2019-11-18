// -----
// RotaryEncoder.cpp - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// 17.06.2015 minor updates.
// -----

#include "Arduino.h"
#include "RotaryEncoderJoel.h"


// The array holds the values �1 for the entries where a position was decremented,
// a 1 for the entries where the position was incremented
// and 0 in all the other (no change or not valid) cases.

const int8_t KNOBDIR[] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
  -1,  0,  0,  1,
  0,  1, -1,  0
};


// positions: [3] 1 0 2 [3] 1 0 2 [3]
// [3] is the positions where my rotary switch detends
// ==> right, count up
// <== left,  count down


// ----- Initialization and Default Values -----
RotaryEncoderJoel::RotaryEncoderJoel(int pin1, int pin2, int pin3) {
  begin3(pin1, pin2, pin3);
}
RotaryEncoderJoel::RotaryEncoderJoel(int pin1, int pin2) {
  begin2(pin1, pin2);
}
void RotaryEncoderJoel::begin() {
  // when not started in motion, the current state of the encoder should be 3
  _oldState = 3;

  // start with position 0;
  _position = 0;
  _positionExt = 0;
  _positionExtPrev = 0;
  _pin1 = -1;
  _pin2 = -2;
  _pin3 = -3;
}
void RotaryEncoderJoel::begin2(int pin1, int pin2) {
  begin();
  // Remember Hardware Setup
  _pin1 = pin1;
  _pin2 = pin2;

  // Setup the input pins
  pinMode(pin1, INPUT);
  digitalWrite(pin1, HIGH);   // turn on pullup resistor

  pinMode(pin2, INPUT);
  digitalWrite(pin2, HIGH);   // turn on pullup resistor

}

void RotaryEncoderJoel::begin3(int pin1, int pin2, int pin3) {
  begin();
  begin2(pin1, pin2);
  _pin3 = pin3;
  // Setup the input pin
  pinMode(_pin3, INPUT);
  digitalWrite(_pin3, HIGH);   // turn on pullup resistor
}



long  RotaryEncoderJoel::getPosition() {
  return _positionExt;
} // getPosition()


int8_t  RotaryEncoderJoel::getDirection() {

  int8_t ret = 0;

  if ( _positionExtPrev > _positionExt )
  {
    ret = -1;
    _positionExtPrev = _positionExt;
  }
  else if ( _positionExtPrev < _positionExt )
  {
    ret = 1;
    _positionExtPrev = _positionExt;
  }
  else
  {
    ret = 0;
    _positionExtPrev = _positionExt;
  }

  return ret;
}



void RotaryEncoderJoel::setPosition(long newPosition) {
  // only adjust the external part of the position.
  _position = ((newPosition << 2) | (_position & 0x03L));
  _positionExt = newPosition;
  _positionExtPrev = newPosition;
} // setPosition()


void RotaryEncoderJoel::tick(void) {
  static int sig1 = 0;
  static int sig2 = 0;
  static int8_t thisState = 0;
  sig1 = digitalRead(_pin1);
  sig2 = digitalRead(_pin2);
  thisState = sig1 | (sig2 << 1);

  if (_oldState != thisState) {
    _position += KNOBDIR[thisState | (_oldState << 2)];

    if (thisState == LATCHSTATE)
      _positionExt = _position >> 2;

    _oldState = thisState;
  } // if
} // tick()

int8_t RotaryEncoderJoel::getSwitch(void) {
  return (int8_t)digitalRead(_pin3);
} // getSwitch

// End
