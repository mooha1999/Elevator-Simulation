#pragma once
#include "Event.h"
#include "Pickable.h"

class ArrivalEvent : public Event
{
	Pickable* pickable;
public:
	ArrivalEvent(int timestep, int ID, Pickable* pickable) : 
				Event(timestep, ID){
		this->pickable = pickable;
		pickable->setID(ID);
	}
	virtual void Execute(LinkedList<Floor *> floors) {
		//Check the direction of the pickable
		if(pickable->IsUp())
			floors[pickable->getSourceFloor()]->AddUpPickable(pickable);
		else
			floors[pickable->getSourceFloor()]->AddDownPickable(pickable);
	}
};

