#pragma once
#include "Elevator.h"
#include "Queue.h"
class PureFunctions
{
public:
#pragma region PureFunctions
	//------------------- Pure functions -----------------
//Functions that DO NOT use any class members

//Get the IDs of a given waiting list
	static Queue<int> IDs(Queue<Pickable*> pickables) {
		Queue<int> ids;
		while (!pickables.IsEmpty()) {
			ids.Push(pickables.Pop()->getID());
		}
		return ids;
	}
	//Get the IDs of a given waiting list
	static Queue<int> IDs(PriorityQueue<Pickable*> pickables) {
		Queue<int> ids;
		while (!pickables.IsEmpty()) {
			ids.Push(pickables.Pop()->getID());
		}
		return ids;
	}
	//increment waiting time by 1 to all pickables in a given list
	static void IncrementQueue(Queue<Pickable*> pickables) {
		while (!pickables.IsEmpty())
		{
			pickables.Pop()->IncrementWaitingTime();
		}
	}
	//increment waiting time by 1 to all pickables in a given list
	static void IncrementQueue(PriorityQueue<Pickable*> pickables) {
		while (!pickables.IsEmpty())
		{
			pickables.Pop()->IncrementWaitingTime();
		}
	}
	//Load pickables into elevator
	//P.S: pickables list is passed by refrence
	static void LoadPickables(Elevator* elevator, PriorityQueue<Pickable*>& pickabels) {
		while (!pickabels.IsEmpty() &&
			elevator->getCurrentLoad() < elevator->getCapacity())
		{
			elevator->AddPickable(pickabels.Pop());
		}

	}
	//Load pickables into elevator
	//P.S: pickables list is passed by refrence
	static void LoadPickables(Elevator* elevator, Queue<Pickable*>& pickabels) {
		while (!pickabels.IsEmpty() &&
			elevator->getCurrentLoad() < elevator->getCapacity())
		{
			elevator->AddPickable(pickabels.Pop());
		}

	}
#pragma endregion
};

