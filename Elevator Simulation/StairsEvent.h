#pragma once
#include "Event.h"
class StairsEvent :
    public Event
{
public:
    StairsEvent(int timestep, int ID) : Event(timestep, ID) {}

    virtual void Execute(LinkedList<Floor *> floors) {
        for(int i = 0; i<7; i++)
            if(floors[i]->AddToStairs(ID))
                return;
    }
};

