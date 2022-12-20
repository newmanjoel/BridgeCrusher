#ifndef Behaviour_h
#define Behaviour_h

#include "Fsm.h"

enum class ScrollableTriggers
{
    C = 0,
    N = 1,
    P = 2
};
enum class ButtonTriggers
{
    Rising = 0,
    Falling = 1,
    Changed = 2,
    Low = 3,
    High = 4
};

template <class T>
class Behavior
{
public:
    // the machine that will run
    Fsm *machine;

    Behavior(){};
    void sendTrigger(T trigger)
    {
        machine->trigger(trigger);
    };
};

class B_Scrollable : public Behavior<ScrollableTriggers>
{
public:
    void onClick()
    {
        sendTrigger(ScrollableTriggers::C);
    };
    void next()
    {
        sendTrigger(ScrollableTriggers::N);
    };
    void prev()
    {
        sendTrigger(ScrollableTriggers::P);
    };
};

class B_Button : public Behavior<ButtonTriggers>
{
public:
    void onRising()
    {
        sendTrigger(ButtonTriggers::Rising);
    };
    void onFalling()
    {
        sendTrigger(ButtonTriggers::Falling);
    };
    void onChange()
    {
        sendTrigger(ButtonTriggers::Changed);
    };
    void onLow()
    {
        sendTrigger(ButtonTriggers::Low);
    };
    void onHigh()
    {
        sendTrigger(ButtonTriggers::High);
    };
};

#endif