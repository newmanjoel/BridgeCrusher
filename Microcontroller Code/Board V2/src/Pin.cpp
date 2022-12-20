#include "Pin.h"

Pin::Pin(){}
Pin::Pin(int i_JackPin, int i_ChipID,int i_ChipPin, String i_Label)
{
    // TODO: Set the type to Actual
}
Pin::Pin(int i_JackPin, String i_Label)
{
    //TODO: Set the type to virtal 
}

// starting points
void Pin::setup(int i_ID, int i_ChipID, PinType i_PinType, String i_Label){}
void Pin::setup(int i_ID, PinType i_PinType, String i_Label){}