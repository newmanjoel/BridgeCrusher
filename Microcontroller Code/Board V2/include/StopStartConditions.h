#ifndef StopStartCondition_h
#define StopStartCondition_h
#include "Wire.h"
#include "Sensors.h"
#include "UserInterface.h"
#include "JsonInterface.h"


class StartStop : public JsonInterface
{
  public:
    Sensors sensors;
    Switches switches;

  public:
    bool running;
    bool shouldStop();
    bool canStart();

};

class VerticleMovement : public StartStop
{
  public:
    bool safeToMoveUp();
    bool safeToMoveDown();
};

class JackSafety: public VerticleMovement
{
  public:
    void begin()
    {
      sensors = *Sensors::instance();
      switches = *Switches::instance();
    };

    bool canStart()
    {
      if(switches.start.NO_contact)
      {
        return true;
      }
      return false;
    };

    bool safeToMoveUp()
    {
      if(switches.max.NC_contact)
      {
        return false;
      }
      return true;
    };

    bool safeToMoveDown()
    {
      if(switches.min.value)
      {
        return true;
      }
      return false;
    };

    bool shouldStop()
    {
      if( switches.min.NC_contact || 
          switches.max.NC_contact || 
          switches.lid.NC_contact ||
          switches.stop.NC_contact)
          {
            return true;
          }
          return false;
    };

};

#endif
