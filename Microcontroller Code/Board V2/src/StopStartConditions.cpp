#include "StopStartConditions.h"
StopStartCondition::StopStartCondition(){
    Stop = true;
    Start = true;
    Max = true;
    Min = true;
    Lid = true;

    StopDown = true;
    StopUp = true;
    StartUp = true;
    StartDown = true;
}

void StopStartCondition::stop_logic() {
  StopDown = !Stop && !Lid && !Max;
  StopUp = !Stop && !Lid && !Min;
  StartUp = Start && !StopUp;
  StartDown = Start && !StopDown;
}