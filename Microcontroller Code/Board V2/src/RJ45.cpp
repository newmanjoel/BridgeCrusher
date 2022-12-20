 #include "RJ45.h"
 
 RJ45::RJ45(){}
 RJ45::RJ45(String i_JackLabel)
 {
     JackLabel = i_JackLabel;
 }

    // starting points
void RJ45::setup(String i_JackLabel){}
void RJ45::setPins(Pin* i_Pin1, Pin* i_Pin2, Pin* i_Pin3, Pin* i_Pin4, Pin* i_Pin5, Pin* i_Pin6, Pin* i_Pin7, Pin* i_Pin8)
{
    Pin1 = i_Pin1;
    Pin2 = i_Pin2;
    Pin3 = i_Pin3;
    Pin4 = i_Pin4;
    Pin5 = i_Pin5;
    Pin6 = i_Pin6;
    Pin7 = i_Pin7;
    Pin8 = i_Pin8;
}