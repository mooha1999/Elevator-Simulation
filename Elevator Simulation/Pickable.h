#pragma once
class Pickable
{
	int arrivalTime,
		srcFloor,
		trgFloor, ID;
	int waitingTime, pickTime, serviceTime, targetTime;
	bool isUp;
protected:
	char type;
	int emgLevel = -1;
public:
	Pickable(int arrivalTime,
		int srcFloor, int trgFloor) {
		this->arrivalTime = arrivalTime;
		this->srcFloor = srcFloor;
		this->trgFloor = trgFloor;
		isUp = trgFloor - srcFloor > 0;
		ID = waitingTime = pickTime = serviceTime = targetTime = 0;
		type = ' ';
	}
	// --- Getters ---
	char getType() { return type; }
	int getID() { return ID; }
	int getArrivalTime() { return arrivalTime; }
	int getSourceFloor() { return srcFloor; }
	int getTargetFloor() { return trgFloor; }
	int getWaitingTime() { return waitingTime; }
	int getPickTime() { return pickTime; }
	int getServiceTime() { return serviceTime; }
	int getEmergencyLevel() { return emgLevel; }
	int getTargetTime() {
		return max(arrivalTime + waitingTime + serviceTime, targetTime);
	}
	bool IsUp() { return isUp; }
	// --- Setters ---
	void setPickTime(int pickTime) { this->pickTime = pickTime; }
	void setID(int ID) { this->ID = ID; }
	void setTargetTime(int targetTime) { this->targetTime = targetTime; }
	// --- Incrementals ---
	void IncrementWaitingTime() { waitingTime++; }
	void IncrementServiceTime() { serviceTime++; }
};
