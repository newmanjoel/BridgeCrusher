#ifndef Pin_h
#define Pin_h
#include "Arduino.h"

enum PinType { Actual, Logical};

class Pin
{
  public:
    // ------ variables ------
    // Label on the board
    String Label;
    // Ids on board and on microcontroller
    int JackID, MicrocontrollerID;
    PinType Type;
    // ----- object ----- (has a relationships)
    
    // ----- Constructors -----
    Pin();
    Pin(int i_ID, int i_mcuID, PinType i_PinType, String i_Label);
    Pin(int i_ID, PinType i_PinType, String i_Label);

    // starting points
    void setup(int i_ID, int i_mcuID, PinType i_PinType, String i_Label);
    void setup(int i_ID, PinType i_PinType, String i_Label);
    // primary looping point

    
  private:
    
};
#endif //Pin_h