#ifndef HardwareConfig_h
#define HardwareConfig_h

#include "Pin.h"
#include "RJ45.h"

//#define Mtr_CS A0 // A0, Pin 5
#define Dist_VAL A1 // A1, Pin 6
//#define Mtr_SLP A2 // A2, Pin 7
//#define Adc_RST A4 //A4, Pin 9
//#define Adc_CS A5 // A5, Pin 10
//#define SCK 11 // Pin 11
//#define MOSI 12 // Pin 12
//#define MISO 13 // Pin 13
//#define Min_NC 2 // D2, Pin 14
//#define Lid_NC 3 // D3, Pin 18
//#define Max_NC 4 // D4, Pin 19
#define Adc_RDY 0 // RX_D0, Pin 20
#define Adc_STR 1 // TX_D1, Pin 21
//#define I2C_SDA 22 // SDA, Pin 22
//#define I2C_SCL 23 // SCL, Pin 23
//#define Mtr_PWM 5 // D5, Pin 24
//#define Mtr_DIR 7 // D7, Pin 25
//#define Enc_SW_NO 9 // D9, Pin 26
//#define Enc_B 10 // D10, Pin 27
//#define Enc_A 11 // D11, Pin 28
//#define Stop_NC 12 // D12, Pin 29
//#define Start_NO 13 // D13, Pin 30


struct SPIPins
{
  Pin* MOSI;
  Pin* MISO;
  Pin* SCK;
};
struct I2CPins
{
  Pin* SDA;
  Pin* SCL;
};
struct EncoderPins
{
  Pin* A;
  Pin* B;
  Pin* SW_NO;
};
struct MotorPins
{
  Pin* PWM;
  Pin* DIR;
  Pin* SLP;
  Pin* CS;
};
struct NCSwitch
{
  Pin* NC;
};
struct NOSwitch
{
  Pin* NO;
};
struct ADCPins
{
  Pin* RST;
  Pin* STR;
  Pin* CS;
  Pin* RDY;
};

class HardwareConfig
{
  public:
    // ------ variables ------
    RJ45 Jacks[10];
    Pin Pins[86];
    I2CPins I2CBus;
    SPIPins SPIBus;
    EncoderPins EncoderPin;
    MotorPins MotorPin;
    NCSwitch Min;
    NCSwitch Max;
    NCSwitch Lid;
    NCSwitch Stop;
    NOSwitch Start;
    ADCPins ADCPin;
    
    // ----- Constructors -----
    HardwareConfig();
    
    // starting points
    void setup();


  private:
    
    int setupJacks(int pinStartingID);
    int setupEncoder(int jackID, int pinStartingID);
    int setupLid(int jackID, int pinStartingID);
    int setupStop(int jackID, int pinStartingID);
    int setupStart(int jackID, int pinStartingID);
    int setupMax(int jackID, int pinStartingID);
    int setupMin(int jackID, int pinStartingID);
    int setupScreen(int jackID, int pinStartingID);
    int setupMotor(int jackID, int pinStartingID);
    int setupDistance(int jackID, int pinStartingID);
    int setupSensor(int jackID, int pinStartingID);
    void setupPins();
};

#endif //HardwareConfig_h