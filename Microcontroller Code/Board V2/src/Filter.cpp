#include "Filter.h"

Filter::Filter(){
  b0 =0;
  b1 = 0;
  b2 = 0;
  a1 = 0;
  a2 = 0;
}

void Filter::begin(double ib0, double ib1, double ib2, double ia1, double ia2, Filter* nextFilter){
  begin(ib0,ib1,ib2,ia1,ia2);
  setNextState(nextFilter);
}

void Filter::begin(double ib0, double ib1, double ib2, double ia1, double ia2){
  // setup for the numerator
  b0 = ib0;
  b1 = ib1;
  b2 = ib2;

  // setup for the denumerator
  a1 = ia1;
  a2 = ia2;

  // make sure that the inputs are set to zero and that they dont have any weird values
  x[0] = 0;
  x[1] = 0;
  x[2] = 0;

  output = 0;
  nextStageSet = false;
}

double Filter::next(double nextInput){
  // this is the primary thing that will be called

  // increment the inputs
  rollx();
  x[0] = nextInput;
  rollw();
  w[0] = x[0] + w[1]*a1 + w[2]*a2;

  output = w[0]*b0 + w[1]*b1 + w[2]*b2;

  if (nextStageSet){
    output =  nextStage->next(output);
  }
  return output;

}

void Filter::setNextState(Filter* nextFilter){
  nextStage  = nextFilter;
  nextStageSet = true;
}

void Filter::rollx(){
  //
  x[2] = x[1];
  x[1] = x[0];
}
void Filter::rollw(){
  //
  w[2] = w[1];
  w[1] = w[0];
}
