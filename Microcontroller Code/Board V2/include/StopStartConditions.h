#ifndef StopStartCondition_h
#define StopStartCondition_h
#include "UserInterface.h"

class StartStop
{
  public:
    StartStop();

    virtual bool shouldStop();
    virtual bool canStart();
    virtual void toJson(JsonObject inputJsonObject); 
    virtual void json(JsonObject inputJsonObject);

};

class VerticleMovement : public StartStop
{
  public:
    virtual bool safeToMoveUp();
    virtual bool safeToMoveDown();
};

class StopStartCondition 
{
    // Note this should probably be a singleton but because its just me then I feel like I can get away with stuff like this.

  public:
    // define all of the variables
    Switch* Stop;
    Switch* Start;
    Switch* Max;
    Switch* Min;
    Switch* Lid;

    bool StopDown;
    bool StopUp;
    bool StartUp;
    bool StartDown;

    bool last_StopDown;
    bool last_StopUp;
    bool last_StartUp;
    bool last_StartDown;

    bool test;

  public:
    // define all of the functions
    
    StopStartCondition(Switch *i_stop, Switch *i_start, Switch *i_max,
      Switch* i_min, Switch* i_lid);
    void stop_logic();

   

  private:

};


#endif
