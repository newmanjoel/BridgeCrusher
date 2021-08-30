#include "Pin.h"

Pin::Pin(){}
Pin::Pin(int i_ID, int i_mcuID, PinType i_PinType, String i_Label){}
Pin::Pin(int i_ID, PinType i_PinType, String i_Label){}

// starting points
void Pin::setup(int i_ID, int i_mcuID, PinType i_PinType, String i_Label){}
void Pin::setup(int i_ID, PinType i_PinType, String i_Label){}