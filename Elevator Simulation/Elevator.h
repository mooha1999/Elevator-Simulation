#pragma once
#include "Pickable.h"
#include "PriorityQueue.h"
class Elevator
{
public:
	enum State
	{
		IDLE, MOV, UNLOAD, AVAIL, OuT
	};
private:
	int speed, capacity;
	int checkupPeriod;
	int currentLoad;
	int idleTime, movingTime, availTime;
	int currentFloor, targetFloor;
	char type;
	bool isUp, isDown, isUnloading;
	State state;
	PriorityQueue<Pickable*> pickables;
	void IncrementServiceTime() {
		auto temp = pickables;
		while (!temp.IsEmpty())
			temp.Pop()->IncrementServiceTime();
	}

public:
	Elevator(char type, int speed, int capacity) {
		this->type = type;
		this->speed = speed - 1;
		this->capacity = capacity;
		idleTime = movingTime = availTime = 0;
		currentLoad = checkupPeriod = idleTime = 0;
		currentFloor = 0;
		targetFloor = INT_MAX;
		isUp = isDown = isUnloading = false;
		state = IDLE;
	}
#pragma region GETTERS

	char getType() { return type; }
	int getSpeed() { return speed; }
	int getCapacity() { return capacity; }
	int getCurrentLoad() { return currentLoad; }
	int getCurrentFloor() { return currentFloor; }
	int getTargetFloor() { return targetFloor; }
	int getMovingTime() { return movingTime; }
	bool IsUp() { return isUp; }
	bool IsDown() { return isDown; }
	bool IsUnLoading() { return isUnloading; }
	bool HasNoDirection() { return !isUp && !isDown; }
	int getState() { return state; }
	Pickable* getPickable() { return pickables.IsEmpty() ? nullptr : pickables.Top(); }
#pragma endregion

#pragma region SETTERS

	void setState(State state) {
		if (state == IDLE && pickables.IsEmpty()) {
			isUp = isDown = false;
		}
		idleTime = 0;
		availTime = 0;
		this->state = state;
	}
	void setCurrentFloor(int floor) {
		
		if (floor == targetFloor)
		{
			targetFloor = pickables.IsEmpty() ? targetFloor : pickables.Top()->getTargetFloor();
			State s = pickables.IsEmpty() ? IDLE : UNLOAD;
			setState(s);
		}
		currentFloor = floor; 
	}
	void setTargetFloor(int floor) {
		if (floor > targetFloor)
			isUp = true;
		else
			isDown = true;
		targetFloor = floor;
	}
	void setIsUp(bool isUp) { this->isUp = isUp; }
	void setIsDown(bool isDown) { this->isDown = isDown; }
	void setIsUnloading(bool isUnloading) { this->isUnloading = isUnloading; }
#pragma endregion
	Pickable* RemovePickable() {
		if (!pickables.IsEmpty())
		{
			Pickable* p = pickables.Pop();
			currentLoad--;
			if (pickables.IsEmpty())
			{
				isDown = isUp = false;
				setTargetFloor(currentFloor);
			}
			else {
				setTargetFloor(pickables.Top()->getTargetFloor());
			}
			return p;
		}
		return nullptr;
	}
	// ----- Methods -------
	void AddPickable(Pickable* pickable) {
		int targetFloor = pickable->getTargetFloor();
		if (isUp){
			pickables.Insert(pickable,targetFloor * -1);
			if (targetFloor < this->targetFloor || currentFloor == this->targetFloor)
			{
				this->targetFloor = targetFloor;
			}
		}else if (isDown){
			pickables.Insert(pickable, targetFloor);
			if (targetFloor > this->targetFloor || currentFloor == this->targetFloor)
			{
				this->targetFloor = targetFloor;
			}
		}
		currentLoad++;
	}
	//Return true if the elevator reaches its next floor
	bool Move() {
		if (state != MOV)
			return false;
		if (movingTime == speed)
		{
			movingTime = 0;
			return true;
		}
		else
			movingTime++;
		return false;
	}
	/*
	* Increment IDLE and AVAIL times for each elevator.
	* Increment Service time for all pickables 
	* */
	void IncrementTimes() {
		switch (state)
		{
		case IDLE:
			if (++idleTime == 3)
				setState(AVAIL);
			break;
		case AVAIL:
			if (!pickables.IsEmpty())
				setState(MOV);
			else if (++availTime == 7)
				setState(IDLE);
			break;
		default:
			break;
		}
		IncrementServiceTime();
	}
	Pickable* Unload() {
		if (pickables.IsEmpty())
			return nullptr;

		return (pickables.Top()->getTargetFloor() == currentFloor) ?
			RemovePickable() : nullptr;
	}

	bool operator ==(State state) {
		return this->state == state;
	}
};

