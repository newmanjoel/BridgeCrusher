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
    RJ45(String i_JackLabel);

    // starting points
    void setup(String i_JackLabel);
    void setPins(Pin* i_Pin1, Pin* i_Pin2, Pin* i_Pin3, Pin* i_Pin4, Pin* i_Pin5, Pin* i_Pin6, Pin* i_Pin7, Pin* i_Pin8);

  private:
    
};
#endif //RJ45_h