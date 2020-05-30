#include "ADS1246.h"
#include "Arduino.h"
#include <SPI.h>


using namespace ADS1246COMMANDS;


void ADS1246::toJson(JsonObject inputJsonObject)
{
    JsonObject adsJson = inputJsonObject.createNestedObject(name);
    adsJson["STATUS"] = "Untested";
    offset.toJson(adsJson);
    corrected_gain.toJson(adsJson);
    //inputJsonObject["offset"] = offset;
    adsJson["SPS_options"] = SPS;
    adsJson["gain"] = gain;
    adsJson["SPS"] = SPS[currentSPS];
    adsJson["sensitivity"] = sensitivity;
    adsJson["KG"] = getConvertedOutput();
}

void ADS1246::fromJson(JsonObject inputJsonObject)
{
  if (inputJsonObject.containsKey(offset.name))
  {
    offset.fromJson(inputJsonObject[offset.name].as<JsonObject>());
    //offset = inputJsonObject["offset"].as<double>();
  }
  if (inputJsonObject.containsKey(corrected_gain.name))
  {
    corrected_gain.fromJson(inputJsonObject[corrected_gain.name].as<JsonObject>());
    //corrected_gain = inputJsonObject["gain"].as<double>();
  }
  if (inputJsonObject.containsKey("SPS"))
  {
    //offset = inputJsonObject["SPS"].as<double>();
  }
  if (inputJsonObject.containsKey("sensitivity"))
  {
    sensitivity = inputJsonObject["sensitivity"].as<double>();
  }
}

void ADS1246::begin(int inputCsPin, int inputStartPin, int inputResetPin)
{
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

  corrected_gain.setup("corrected_gain", &_corrected_gain, 1, 0, 10, 0.1);
  offset.setup("offset", &_offset, 0, -5, 5, 0.001);
}

void ADS1246::startTrans()
{
  digitalWrite(cs, LOW);
}

void ADS1246::stopTrans()
{
  digitalWrite(cs, HIGH);
}

void ADS1246::setupADC()
{
  //sensitivity = (500) / voltage; // The units are Kg/v
  sensitivity = 36613.04;

  delay(10);
  startTrans();
  SPI.transfer(RESET); // reset everything
  stopTrans();

  delay(10);
  startTrans();
  SPI.transfer(WREG | SYS0);             // write to a register | SYS0
  SPI.transfer(0x00 | 0x03);             // writing 2 bytes ( PGA | DR )
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

void ADS1246::autoRange()
{
  static bool changed = false;
  //Serial.print("FS:");
  if (abs(corrected) > FS2 * 0.8)
  {
    //Serial.print(FS2*0.8);
    changed = currentPGA == PGA1 ? false : true;
    currentPGA = PGA1;
    gain = GAIN1;
  }
  else if (abs(corrected) > FS4 * 0.8)
  {
    //Serial.print(FS4*0.8);
    changed = currentPGA == PGA2 ? false : true;
    currentPGA = PGA2;
    gain = GAIN2;
  }
  else if (abs(corrected) > FS8 * 0.8)
  {
    //Serial.print(FS8*0.8);
    changed = currentPGA == PGA4 ? false : true;
    currentPGA = PGA4;
    gain = GAIN4;
  }
  else if (abs(corrected) > FS16 * 0.8)
  {
    //Serial.print(FS16*0.8);
    changed = currentPGA == PGA8 ? false : true;
    currentPGA = PGA8;
    gain = GAIN8;
  }
  else if (abs(corrected) > FS32 * 0.8)
  {
    //Serial.print(FS32*0.8);
    changed = currentPGA == PGA16 ? false : true;
    currentPGA = PGA16;
    gain = GAIN16;
  }
  else if (abs(corrected) > FS64 * 0.8)
  {
    //Serial.print(FS64*0.8);
    changed = currentPGA == PGA32 ? false : true;
    currentPGA = PGA32;
    gain = GAIN32;
  }
  else if (abs(corrected) > FS128 * 0.8)
  {
    //Serial.print(FS128*0.8);
    changed = currentPGA == PGA64 ? false : true;
    currentPGA = PGA64;
    gain = GAIN64;
  }
  else if (abs(corrected) < FS128 * 0.8)
  {
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

  if (changed)
  {
    startTrans();
    SPI.transfer(WREG | SYS0);             // write to a register | SYS0
    SPI.transfer(0x00 | 0x03);             // writing 2 bytes ( PGA | DR )
    SPI.transfer(currentSPS | currentPGA); // set the data rate 320 SPS and PGR to 128
    stopTrans();
    fullScale = voltage / gain;
    lsb = ((2 * vref) / gain) / pow(2, 24);
  }
}

double ADS1246::readADC()
{
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
  corrected = dmap(sdata, bottom, top, -fullScale, (fullScale)-lsb);

  //autoRange();

  return corrected;
}

double ADS1246::getConvertedOutput()
{
  double temp = readADC();
  return temp * sensitivity * (*corrected_gain.value) + (*offset.value);
}

signed long ADS1246::ConvertTwosComplementByteToInteger(unsigned long rawValue)
{
  static signed long localvar = 0;

  localvar = rawValue;
  if ((rawValue >> 21 & 0x1) == 1)
  {
    // Otherwise perform the 2's complement math on the value
    localvar = ~rawValue;
    localvar += 1;
    localvar = localvar & 0x7FFFFF;
    localvar *= -1;
  }

  return localvar;
}

void ADS1246::setSensorReady()
{
  newData = true;
}

double ADS1246::dmap(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}