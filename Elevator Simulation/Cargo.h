#pragma once
#include "Pickable.h"
#include "Container.h"

class Cargo : public Pickable
{
public:
	Cargo(int arrivalTime, int srcFloor, int trgFloor)
		: Pickable(arrivalTime, srcFloor, trgFloor) 
	{
		type = 'C';
	};
	
};