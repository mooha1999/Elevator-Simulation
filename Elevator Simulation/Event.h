#pragma once
#include "Floor.h"
#include "LinkedList.h"
class Event
{
protected:
	int timestep, ID;

public:
	Event(int timestep, int ID) {
		this->ID = ID;
		this->timestep = timestep;
	}

	int getTimeStep() { return timestep; }

	int getID() { return ID; }

	virtual void Execute(LinkedList<Floor *> floors) = 0;
};

