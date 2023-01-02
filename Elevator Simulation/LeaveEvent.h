#pragma once
#include "Event.h"
class LeaveEvent : public Event
{
public:
    LeaveEvent(int timestep, int ID) : Event(timestep, ID){}
    
    virtual void Execute(LinkedList<Floor *> floors) {
        Pickable * p = floors[0]->LeaveVisitor(ID);
        //REmove from the system
        delete p;
        p = NULL;
    }
};