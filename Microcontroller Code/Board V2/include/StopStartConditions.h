#ifndef StopStartCondition_h
#define StopStartCondition_h

class StopStartCondition {
    // Note this should probably be a singleton but because its just me then I feel like I can get away with stuff like this.

  public:
    // define all of the variables
    volatile bool Stop;
    volatile bool Start;
    volatile bool Max ;
    volatile bool Min;
    volatile bool Lid;

    volatile bool StopDown;
    volatile bool StopUp;
    volatile bool StartUp;
    volatile bool StartDown;

  public:
    // define all of the functions
    StopStartCondition();
    void stop_logic();

   

  private:

};


#endif
