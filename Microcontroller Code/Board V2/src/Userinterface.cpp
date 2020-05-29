#include "UserInterface.h"


Userinterface::Userinterface()
{
    name = "UI";
}

void Userinterface::setup()
{
}

void Userinterface::begin()
{
    lcd = *LCD::instance();
}

void Userinterface::poll(){
    
}