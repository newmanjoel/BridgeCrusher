#ifndef HardwareConfig_h
#define HardwareConfig_h

#include "Pin.h"
#include "RJ45.h"


class HardwareConfig
{
  public:
    // ------ variables ------
    RJ45 Jacks[10];
    Pin Pins[80];
    
    // ----- Constructors -----
    HardwareConfig();
    
    // starting points
    void setup();


  private:
    void setupPins();
    void setupJacks();
};

#endif //HardwareConfig_h