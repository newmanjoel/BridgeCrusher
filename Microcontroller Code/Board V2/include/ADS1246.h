#ifndef ADS1246_H
#define ADS1246_H

#include "Arduino.h"
#include <Wire.h>
#include "ArduinoJson.h"
#include "Settings.h"

namespace ADS1246COMMANDS{
    const byte WAKEUP = 0x01;
    const byte SLEEP = 0x02;
    const byte SYNC1 = 0x04;
    const byte SYNC2 = 0x04;
    const byte RESET = 0x06;
    const byte NOP = 0xFF;
    const byte RDATA = 0x12;
    const byte RDATAC = 0x14;
    const byte SDATAC = 0x16;
    const byte RREG = 0x20;
    const byte WREG = 0x40;
    const byte SYSOCAL = 0x60;
    const byte SYSGCAL = 0x61;
    const byte SELFOCAL = 0x62;

    const byte PGA1 = 0x00 << 4;
    const byte PGA2 = 0x01 << 4;
    const byte PGA4 = 0x02 << 4;
    const byte PGA8 = 0x03 << 4;
    const byte PGA16 = 0x04 << 4;
    const byte PGA32 = 0x05 << 4;
    const byte PGA64 = 0x06 << 4;
    const byte PGA128 = 0x07 << 4;
    const byte GAIN1 = 1;
    const byte GAIN2 = 2;
    const byte GAIN4 = 4;
    const byte GAIN8 = 8;
    const byte GAIN16 = 16;
    const byte GAIN32 = 32;
    const byte GAIN64 = 64;
    const byte GAIN128 = 128;

    const byte SPS5 = 0x00;
    const byte SPS10 = 0x01;
    const byte SPS20 = 0x02;
    const byte SPS40 = 0x03;
    const byte SPS80 = 0x04;
    const byte SPS160 = 0x05;
    const byte SPS320 = 0x06;
    const byte SPS640 = 0x07;
    const byte SPS1000 = 0x08;
    const byte SPS2000 = 0x09;
    const int SPS[10] = {5, 10, 20, 40, 80, 160, 320, 640, 1000, 2000};
    const byte BCS = 0x00;
    const byte VBIAS = 0x01;
    const byte MUX1 = 0x02;
    const byte SYS0 = 0x03;
    const byte OFC0 = 0x04;
    const byte OFC1 = 0x05;
    const byte OFC2 = 0x06;
    const byte FSC0 = 0x07;
    const byte FSC1 = 0x08;
    const byte FSC2 = 0x09;
    const byte ID = 0x0A;
};

using namespace ADS1246COMMANDS;


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
    double FS1 = voltage / GAIN1;
    double FS2 = voltage / GAIN2;
    double FS4 = voltage / GAIN4;
    double FS8 = voltage / GAIN8;
    double FS16 = voltage / GAIN16;
    double FS32 = voltage / GAIN32;
    double FS64 = voltage / GAIN64;
    double FS128 = voltage / GAIN128;


  public:
    ADS1246();

    void toJson(JsonObject inputJsonObject);
    void jsonConfig(JsonObject inputJsonObject);

    void begin(int inputCsPin, int inputStartPin, int inputResetPin);
    void startTrans();
    void stopTrans();
    void setSensorReady();
    void setupADC();
    void autoRange();
    double readADC();
    double getConvertedOutput();
    signed long ConvertTwosComplementByteToInteger(unsigned long rawValue);
    double dmap(double x, double in_min, double in_max, double out_min, double out_max);
};


#endif