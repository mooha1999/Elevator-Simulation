#pragma once
#include "Pickable.h"
#include "Container.h"

class Visitor : public Pickable
{
public:
	Visitor(int arrivalTime, int srcFloor, int trgFloor)
		: Pickable(arrivalTime, srcFloor, trgFloor) 
	{
		type = 'V';
	};
};