#pragma once
#include "Pickable.h"
#include "Container.h"

class Patient : public Pickable
{
public:
	Patient(int arrivalTime, int srcFloor, int trgFloor, int emgLevel)
		: Pickable(arrivalTime, srcFloor, trgFloor) 
	{
		this->emgLevel = emgLevel;
		type = 'P';
	};

	int getEmergencyLevel() { return emgLevel; }
};