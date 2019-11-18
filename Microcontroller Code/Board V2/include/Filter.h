#ifndef filter_h
#define filter_h

class Filter {
  public:
    // define all of the variables
    volatile double x[3];
    volatile double output;
    volatile double b0;
    volatile double b1;
    volatile double b2;
    volatile double a1;
    volatile double a2;
    Filter* nextStage;
  public:
    // define all of the functions
    Filter();

    void begin(double ib0, double ib1, double ib2, double ia1, double ia2);
    void begin(double ib0, double ib1, double ib2, double ia1, double ia2, Filter* nextFilter);

    double next(double nextInput);
    void setNextState(Filter* nextFilter);

  private:
    volatile bool nextStageSet;
    volatile double w[3];
    void rollx();
    void rollw();



};


#endif
