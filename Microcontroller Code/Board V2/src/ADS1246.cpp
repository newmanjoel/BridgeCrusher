#ifndef ADS1246_H
#define ADS1246_H

#include "Arduino.h"
#include <SPI.h>

class ADS1246 {
  public:
    volatile bool newData;

    int cs;
    int str;
    int rst;
    unsigned long data;
    signed long sdata;
    byte MSB, MID, LSB;
    byte currentPGA;
    byte currentSPS;
    const double voltage = 4.91;
    const double vref = 2.508;
    double fullScale = 0;
    double lsb = 0;
    double corrected = 0;
    const long top = pow(2, 23);
    const long bottom = -1 * pow(2, 23);
    const long decimals = 10000;
    const long topCorrected = decimals * voltage;
    double sensitivity = 0; // v/kg (5/(500*2))
    double gain = 1;
    double offset = 0;
    double corrected_gain = 1;



    static const byte WAKEUP = 0x01;
    static const byte SLEEP = 0x02;
    static const byte SYNC1 = 0x04;
    static const byte SYNC2 = 0x04;
    static const byte RESET = 0x06;
    static const byte NOP = 0xFF;
    static const byte RDATA = 0x12;
    static const byte RDATAC = 0x14;
    static const byte SDATAC = 0x16;
    static const byte RREG = 0x20;
    static const byte WREG = 0x40;
    static const byte SYSOCAL = 0x60;
    static const byte SYSGCAL = 0x61;
    static const byte SELFOCAL = 0x62;

    static const byte PGA1 = 0x00 << 4;
    static const byte PGA2 = 0x01 << 4;
    static const byte PGA4 = 0x02 << 4;
    static const byte PGA8 = 0x03 << 4;
    static const byte PGA16 = 0x04 << 4;
    static const byte PGA32 = 0x05 << 4;
    static const byte PGA64 = 0x06 << 4;
    static const byte PGA128 = 0x07 << 4;

    static const byte GAIN1 = 1;
    static const byte GAIN2 = 2;
    static const byte GAIN4 = 4;
    static const byte GAIN8 = 8;
    static const byte GAIN16 = 16;
    static const byte GAIN32 = 32;
    static const byte GAIN64 = 64;
    static const byte GAIN128 = 128;

    const double FS1 = voltage / GAIN1;
    const double FS2 = voltage / GAIN2;
    const double FS4 = voltage / GAIN4;
    const double FS8 = voltage / GAIN8;
    const double FS16 = voltage / GAIN16;
    const double FS32 = voltage / GAIN32;
    const double FS64 = voltage / GAIN64;
    const double FS128 = voltage / GAIN128;

    static const byte SPS5 = 0x00;
    static const byte SPS10 = 0x01;
    static const byte SPS20 = 0x02;
    static const byte SPS40 = 0x03;
    static const byte SPS80 = 0x04;
    static const byte SPS160 = 0x05;
    static const byte SPS320 = 0x06;
    static const byte SPS640 = 0x07;
    static const byte SPS1000 = 0x08;
    static const byte SPS2000 = 0x09;

    const int SPS[10] = {5, 10, 20, 40, 80, 160, 320, 640, 1000, 2000};

    static const byte BCS = 0x00;
    static const byte VBIAS = 0x01;
    static const byte MUX1 = 0x02;
    static const byte SYS0 = 0x03;
    static const byte OFC0 = 0x04;
    static const byte OFC1 = 0x05;
    static const byte OFC2 = 0x06;
    static const byte FSC0 = 0x07;
    static const byte FSC1 = 0x08;
    static const byte FSC2 = 0x09;
    static const byte ID = 0x0A;


  public:
    ADS1246() {}

    void begin(int inputCsPin, int inputStartPin, int inputResetPin) {
      cs = inputCsPin;
      str = inputStartPin;
      rst = inputResetPin;

      pinMode(cs, OUTPUT);
      pinMode(str, OUTPUT);
      pinMode(rst, OUTPUT);
      digitalWrite(str, HIGH);
      digitalWrite(rst, HIGH);
      newData = false;

      //currentPGA = PGA128;
      //gain = 4.572793841362E-09; // 128 gain
      currentSPS = SPS160;
      //currentPGA = PGA64;
      //gain = 9.145587682724E-08; // 64 gain
      currentPGA = PGA128;
      gain = GAIN128;
      fullScale = voltage / gain;
      lsb = ((2 * vref) / gain) / pow(2, 24);

    }

    void startTrans() {
      digitalWrite(cs, LOW);
    }

    void stopTrans() {
      digitalWrite(cs, HIGH);
    }

    void setSensorReady() {
      newData = true;
    }


    void setupADC() {
      //sensitivity = (500) / voltage; // The units are Kg/v
      sensitivity = 36613.04;

      delay(10);
      startTrans();
      SPI.transfer(RESET); // reset everything
      stopTrans();

      delay(10);
      startTrans();
      SPI.transfer(WREG | SYS0); // write to a register | SYS0
      SPI.transfer(0x00 | 0x03); // writing 2 bytes ( PGA | DR )
      SPI.transfer(currentSPS | currentPGA); // set the data rate 320 SPS and PGR to 128
      stopTrans();


      /*
        delay(10);
        startTrans();
        SPI.transfer(WREG | VBIAS); // write to a register | VBIAS
        SPI.transfer(0x00); // writing one bit (write + 1)
        SPI.transfer(0x00 | 0x03); // apply a voltage offset to the pins
        stopTrans();
      */

      delay(10);
      // Dont send SYSOCAL as it requires shorting input to the inputs
      //startTrans();
      //SPI.transfer(SYSOCAL); // system offset calibration
      //stopTrans();
      /*
        delay(10);
        startTrans();
        SPI.transfer(SELFOCAL); // self offset calibration
        stopTrans();
        // the longest calibration time is 3.2 seconds, just more buffer
        delay(4000);
      */
      //delay(10);
      //startTrans();
      //SPI.transfer(0x20 | 0x03); // read to a register | SYS0
      //SPI.transfer(0x03); // reading two bytes (read-1)
      //int gain = SPI.transfer(0xFF);
      //stopTrans();;

      delay(10);
      startTrans();
      SPI.transfer(RDATAC); // read continous data mode
      stopTrans();

      delay(10);

    }

    void autoRange() {
      static bool changed = false;
      //Serial.print("FS:");
      if (abs(corrected) > FS2 * 0.8) {
        //Serial.print(FS2*0.8);
        changed = currentPGA == PGA1 ? false : true;
        currentPGA = PGA1;
        gain = GAIN1;
      }
      else if (abs(corrected) > FS4 * 0.8) {
        //Serial.print(FS4*0.8);
        changed = currentPGA == PGA2 ? false : true;
        currentPGA = PGA2;
        gain = GAIN2;
      }
      else if (abs(corrected) > FS8 * 0.8) {
        //Serial.print(FS8*0.8);
        changed = currentPGA == PGA4 ? false : true;
        currentPGA = PGA4;
        gain = GAIN4;
      }
      else if (abs(corrected) > FS16 * 0.8) {
        //Serial.print(FS16*0.8);
        changed = currentPGA == PGA8 ? false : true;
        currentPGA = PGA8;
        gain = GAIN8;
      }
      else if (abs(corrected) > FS32 * 0.8) {
        //Serial.print(FS32*0.8);
        changed = currentPGA == PGA16 ? false : true;
        currentPGA = PGA16;
        gain = GAIN16;
      }
      else if (abs(corrected) > FS64 * 0.8) {
        //Serial.print(FS64*0.8);
        changed = currentPGA == PGA32 ? false : true;
        currentPGA = PGA32;
        gain = GAIN32;
      }
      else if (abs(corrected) > FS128 * 0.8) {
        //Serial.print(FS128*0.8);
        changed = currentPGA == PGA64 ? false : true;
        currentPGA = PGA64;
        gain = GAIN64;
      }
      else if (abs(corrected) < FS128 * 0.8) {
        //Serial.print(FS128*0.8);
        changed = currentPGA == PGA128 ? false : true;
        currentPGA = PGA128;
        gain = GAIN128;
      }
      /*
        Serial.print(", PGA:");
        Serial.print(currentPGA >> 4);
        Serial.print(", VOL:");
        Serial.print(abs(corrected),6);
        Serial.print(", FS:");
        Serial.println(voltage/gain,6);
      */

      if (changed) {
        startTrans();
        SPI.transfer(WREG | SYS0); // write to a register | SYS0
        SPI.transfer(0x00 | 0x03); // writing 2 bytes ( PGA | DR )
        SPI.transfer(currentSPS | currentPGA); // set the data rate 320 SPS and PGR to 128
        stopTrans();
        fullScale = voltage / gain;
        lsb = ((2 * vref) / gain) / pow(2, 24);
      }
    }
    double readADC() {
      // Returns the voltage on the ADC, returns in Volts
      noInterrupts();
      startTrans();
      MSB = SPI.transfer(0xFF);
      MID = SPI.transfer(0xFF);
      LSB = SPI.transfer(0xFF);
      stopTrans();
      interrupts();

      newData = false;
      data = MSB;
      data = (data << 8) | MID;
      data = (data << 8) | LSB;

      sdata = ConvertTwosComplementByteToInteger(data);
      corrected = dmap(sdata, bottom, top, -fullScale, (fullScale) - lsb);

      //autoRange();

      return corrected;
    }

    double getConvertedOutput() {
      double temp = readADC();
      return  (temp+offset)* sensitivity*corrected_gain;
    }

    signed long ConvertTwosComplementByteToInteger(unsigned long rawValue) {
      static signed long localvar = 0;

      localvar = rawValue;
      if ((rawValue >> 21 & 0x1) == 1) {
        // Otherwise perform the 2's complement math on the value
        localvar = ~rawValue;
        localvar += 1;
        localvar = localvar & 0x7FFFFF;
        localvar *= -1;
      }


      return localvar;
    }

    double dmap(double x, double in_min, double in_max, double out_min, double out_max) {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

};


#endif
