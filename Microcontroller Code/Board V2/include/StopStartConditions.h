#ifndef StopStartCondition_h
#define StopStartCondition_h
#include "UserInterface.h"

class StopStartCondition {
    // Note this should probably be a singleton but because its just me then I feel like I can get away with stuff like this.

  public:
    // define all of the variables
    Switch Stop;
    Switch Start;
    Switch Max ;
    Switch Min;
    Switch Lid;

    bool StopDown;
    bool StopUp;
    bool StartUp;
    bool StartDown;

  public:
    // define all of the functions
    
    StopStartCondition(Switch* i_stop, Switch* i_start, Switch* i_max,
      Switch* i_min, Switch* i_lid);
    void stop_logic();

   

  private:

};


#endif
