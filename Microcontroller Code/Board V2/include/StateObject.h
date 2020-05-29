#ifndef StateObject_H
#define StateObject_H
#include "JsonInterface.h"



class StateObject: public JsonInterface
{
    private:
        /* data */
    public:
        StateObject(String i_name);
        ~StateObject();
        void Enter();
        void Loop();
        void Exit();
};

StateObject::StateObject(String i_name)
{
    name = i_name;
}

StateObject::~StateObject()
{
}

void StateObject::Enter(){};
void StateObject::Loop(){};
void StateObject::Exit(){};




#endif