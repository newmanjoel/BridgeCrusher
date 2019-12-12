#include "StopStartConditions.h"

StopStartCondition::StopStartCondition(Switch *i_stop, Switch *i_start, Switch *i_max, Switch *i_min, Switch *i_lid): Stop(i_stop), Start(i_start), Max(i_max), Min(i_min), Lid(i_lid)
{

    StopDown = true;
    StopUp = true;
    StartUp = true;
    StartDown = true;
}

void StopStartCondition::stop_logic() 
{
  StopDown = Stop->NC && Lid->NC && Max->NC;
  StopUp = Stop->NC && Lid->NC && Min->NC;
  StartUp = Start->NO && !StopUp;
  StartDown = Start->NO && !StopDown;
}