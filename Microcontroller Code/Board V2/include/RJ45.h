#ifndef RJ45_h
#define RJ45_h
#include "Arduino.h"
#include "Pin.h"

class RJ45
{
  public:
    // ------ variables ------
    // Label on the board
    String JackLabel;

    Pin* Pin1;
    Pin* Pin2;
    Pin* Pin3;
    Pin* Pin4;
    Pin* Pin5;
    Pin* Pin6;
    Pin* Pin7;
    Pin* Pin8;
    
    
    // ----- Constructor -----
    RJ45();

    // starting points
    void setup(String i_JackLabel);

  private:
    
};
#endif //RJ45_h