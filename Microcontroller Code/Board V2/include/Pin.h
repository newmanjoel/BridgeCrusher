#ifndef Pin_h
#define Pin_h
#include "Arduino.h"

enum PinType { Actual, Virtual};

class Pin
{
  public:
    // ------ variables ------
    // Label on the board
    String Label;
    // Ids on board and on microcontroller
    int JackPin, ChipID, ChipPin;
    PinType Type;
    
    // ----- object ----- (has a relationships)
    
    // ----- Constructors -----
    Pin();
    Pin(int i_JackPin, int i_ChipID,int i_ChipPin, String i_Label);
    Pin(int i_JackPin, String i_Label);

    // starting points
    void setup(int i_JackPin, int i_mcuID, PinType i_PinType, String i_Label);
    void setup(int i_JackPin, PinType i_PinType, String i_Label);
    // primary looping point

    
  private:
    
};
#endif //Pin_h