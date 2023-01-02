#pragma once
#include "PriorityQueue.h"
#include "Queue.h"
#include "LinkedList.h"

#include "Pickable.h"
#include "Elevator.h"

#include "Cargo.h"
#include "Patient.h"
#include "Visitor.h"
class Floor
{
	int floorNumber, autoS;
	Elevator* visitorElevator, * cargoElevator, * patientElevator;
	PriorityQueue<Pickable*> upPatients, downPatients;
	Queue<Pickable*> upCargo, downCargo;
	Queue<Pickable*> upVisitors, downVisitors;
	Queue<Pickable*> stairs;

	void Load(Elevator* elevator, Queue<Pickable*>& upList, Queue<Pickable*>& downList) {
		bool upRequest = !upPatients.IsEmpty() || !upList.IsEmpty();
		bool downRequest = !downPatients.IsEmpty() || !downList.IsEmpty();
		bool isRequest = upRequest || downRequest;
		if (*elevator == Elevator::AVAIL && isRequest)
		{
			if (elevator->IsUp() && upRequest)
			{
				LoadPickables(elevator, upPatients);
				LoadPickables(elevator, upList);
			}
			else if (elevator->IsDown() && downRequest)
			{
				LoadPickables(elevator, downPatients);
				LoadPickables(elevator, downList);
			}
			else if (elevator->HasNoDirection())
			{
				SetDirection(elevator, upList, downList);
				Load(elevator, upList, downList);
			}
			elevator->setState(Elevator::MOV);
		}
		else if (*elevator == Elevator::MOV && elevator->getMovingTime() == 0
			&& elevator->getCapacity() > elevator->getCurrentLoad())
		{
			bool isUpRequest = !upPatients.IsEmpty() || !upList.IsEmpty();
			bool isDownRequest = !downPatients.IsEmpty() || !downList.IsEmpty();
			if (elevator->IsUp() && isUpRequest)
			{
				elevator->setState(Elevator::IDLE);
			}
			else if (elevator->IsDown() && isDownRequest)
			{
				elevator->setState(Elevator::IDLE);
			}
		}
	}
	void SetDirection(Elevator* elevator, Queue<Pickable*> upList, Queue<Pickable*> downList) {
		//Check patients lists for emptiness
		if (!upPatients.IsEmpty() && downPatients.IsEmpty())
			elevator->setIsUp(true);

		else if (upPatients.IsEmpty() && !downPatients.IsEmpty())
			elevator->setIsDown(true);

		else if (!upPatients.IsEmpty() && !downPatients.IsEmpty())
		{
			int up = upPatients.Top()->getEmergencyLevel();
			int down = downPatients.Top()->getEmergencyLevel();
			if (up > down)
				elevator->setIsUp(true);
			else
				elevator->setIsDown(true);
		}
		else if (!upList.IsEmpty() && downList.IsEmpty())
			elevator->setIsUp(true);
		else if (upList.IsEmpty() && !downList.IsEmpty())
			elevator->setIsDown(true);
		else if (!upList.IsEmpty() && !downList.IsEmpty())
		{
			int up = upList.Peek()->getArrivalTime();
			int down = downList.Peek()->getArrivalTime();
			if (up < down)
				elevator->setIsUp(true);
			else
				elevator->setIsDown(true);
		}
	}
public:
	Floor(int number, int autoS) {
		this->floorNumber = number;
		this->autoS = autoS;
		visitorElevator = cargoElevator = patientElevator = nullptr;
	}
	//Add an elevator to the current floor
	void AddElevator(Elevator* elevator) {
		elevator->setCurrentFloor(floorNumber);
		switch (elevator->getType())
		{
		case 'P':
			patientElevator = elevator;
			break;
		case 'V':
			visitorElevator = elevator;
			break;
		case 'C':
			cargoElevator = elevator;
			break;
		default:
			break;
		}
	}
	//remove an elevator from the current floor
	void RemoveElevator(char elevatorType) {
		switch (elevatorType)
		{
		case 'P':
			patientElevator = nullptr;
			break;
		case 'V':
			visitorElevator = nullptr;
			break;
		case 'C':
			cargoElevator = nullptr;
			break;
		default:
			break;
		}
	}
	//Increment all waiting times for all pickables in the floor by 1
	void IncrementAllWaitingLists() {
		IncrementQueue(upCargo);
		IncrementQueue(downCargo);
		IncrementQueue(upVisitors);
		IncrementQueue(downVisitors);
		IncrementQueue(upPatients);
		IncrementQueue(downPatients);
		AddToStairs(upVisitors, stairs);
		AddToStairs(downVisitors, stairs);
	}
	Queue<Pickable*>& GetStairs() {
		return stairs;
	}
	//Add a pickable to its corresponding up waiting list
	void AddUpPickable(Pickable* pickable) {
		switch (pickable->getType())
		{
		case'P':
			//Reasonable weight is yet to be implemented
			upPatients.Insert(pickable, pickable->getEmergencyLevel());
			break;
		case'V':
			upVisitors.Push(pickable);
			break;
		case'C':
			upCargo.Push(pickable);
		default:
			break;
		}
	}
	//Add a pickable to its corresponding down waiting list
	void AddDownPickable(Pickable* pickable) {
		switch (pickable->getType())
		{
		case'P':
			//Reasonable weight is yet to be implemented
			downPatients.Insert(pickable, pickable->getEmergencyLevel());
			break;
		case'V':
			downVisitors.Push(pickable);
			break;
		case'C':
			downCargo.Push(pickable);
		default:
			break;
		}
	}
	//Load incoming elevators
	void LoadElevators() {
		bool isVisitor = (!upVisitors.IsEmpty() || !downVisitors.IsEmpty())
			&& (upPatients.IsEmpty() && downPatients.IsEmpty());
		//If there are visitors and no patients, check the V-elevator first
		if (isVisitor)
			swap(patientElevator, visitorElevator);
		if (patientElevator)
			Load(patientElevator, upVisitors, downVisitors);
		if (visitorElevator)
			Load(visitorElevator, upVisitors, downVisitors);
		if (cargoElevator)
			Load(cargoElevator, upCargo, downCargo);
		if (isVisitor)
			swap(patientElevator, visitorElevator);
	}
	//Add visitor to stairs list
	bool AddToStairs(int id) {
		bool ret = false;
		Queue<Pickable*> temp;

		while (!upVisitors.IsEmpty())
		{
			if (upVisitors.Peek()->getID() != id)
			{
				temp.Push(upVisitors.Pop());
			}
			else
			{
				stairs.Push(upVisitors.Pop());
				ret = true;
			}
		}
		upVisitors = temp;
		if (ret)
			return ret;
		ret = false;
		temp = Queue<Pickable*>();
		while (!downVisitors.IsEmpty())
		{
			if (downVisitors.Peek()->getID() != id)
			{
				temp.Push(downVisitors.Pop());
			}
			else
			{
				stairs.Push(downVisitors.Pop());
				ret = true;
			}
		}
		downVisitors = temp;
		return ret;
	}
	//For the LEAVE event, remove the visitor with the given ID if found
	Pickable* LeaveVisitor(int id) {
		if (floorNumber > 0)
			return nullptr;
		Queue<Pickable*> temp;
		Pickable* ret = nullptr;
		//check up list
		while (!upVisitors.IsEmpty()) {
			Pickable* top = upVisitors.Pop();
			if (top->getID() != id)
				temp.Push(top);
			else
				ret = top;
		}
		upVisitors = temp;

		return ret;

		////check down list;
		//temp = Queue<Pickable*>();
		//while (!downVisitors.IsEmpty()) {
		//	Pickable* top = downVisitors.Pop();
		//	if (top->getID() != id)
		//		temp.Push(top);
		//	else
		//		ret = top;
		//}
		//downVisitors = temp;
		//return ret;
	}

#pragma region Getters

	int WaitingPatientsCount() {
		return upPatients.Size() + downPatients.Size();
	}
	int WaitingCargoCount() {
		return upCargo.Size() + downCargo.Size();;
	}
	int WaitingVisitorsCount() {
		return upVisitors.Size() + downVisitors.Size();
	}
	int TotalWaitingCount() {
		return WaitingPatientsCount() +
			WaitingCargoCount() + WaitingVisitorsCount();
	}

	Queue<int> UpPatientsIDs() { return IDs(upPatients); }
	Queue<int> DownPatientsIDs() { return IDs(downPatients); }

	Queue<int> UpCargoIDs() { return IDs(upCargo); }
	Queue<int> DownCargoIDs() { return IDs(downCargo); }

	Queue<int> UpVisitorsIDs() { return IDs(upVisitors); }
	Queue<int> DownVisitorsIDs() { return IDs(downVisitors); }

	bool IsAllWaitingListsEmpty() {
		return upPatients.IsEmpty() && downPatients.IsEmpty()
			&& upCargo.IsEmpty() && downCargo.IsEmpty()
			&& upVisitors.IsEmpty() && downVisitors.IsEmpty();
	}

	Queue<Elevator*> ElevatorsData() {
		Queue<Elevator*> elevators;
		if (patientElevator) elevators.Push(patientElevator);
		if (cargoElevator) elevators.Push(cargoElevator);
		if (visitorElevator) elevators.Push(visitorElevator);
		return elevators;
	}

	// ---- Requests ----

	bool IsUpPatientRequest() {
		return !upPatients.IsEmpty() &&
			!patientElevator && !visitorElevator && !cargoElevator;
	}
	bool IsDownPatientRequest() {
		return !downPatients.IsEmpty() &&
			!patientElevator && !visitorElevator && !cargoElevator;
	}
	bool IsUpVisitorRequest() {
		return !upVisitors.IsEmpty() &&
			!patientElevator && !visitorElevator;
	}
	bool IsDownVisitorRequest() {
		return !downVisitors.IsEmpty() &&
			!patientElevator && !visitorElevator;
	}
	bool IsUpCargoRequest() {
		return !upCargo.IsEmpty() && !cargoElevator;
	}
	bool IsDownCargoRequest() {
		return !downCargo.IsEmpty() && !cargoElevator;
	}

#pragma endregion

private:
#pragma region PureFunctions
	//------------------- Pure functions -----------------
//Functions that DO NOT use any class members

//Get the IDs of a given waiting list
	Queue<int> IDs(Queue<Pickable*> pickables) {
		Queue<int> ids;
		while (!pickables.IsEmpty()) {
			ids.Push(pickables.Pop()->getID());
		}
		return ids;
	}
	//Get the IDs of a given waiting list
	Queue<int> IDs(PriorityQueue<Pickable*> pickables) {
		Queue<int> ids;
		while (!pickables.IsEmpty()) {
			ids.Push(pickables.Pop()->getID());
		}
		return ids;
	}
	//increment waiting time by 1 to all pickables in a given list
	void IncrementQueue(Queue<Pickable*> pickables) {
		while (!pickables.IsEmpty())
		{
			pickables.Pop()->IncrementWaitingTime();
		}
	}
	//increment waiting time by 1 to all pickables in a given list
	void IncrementQueue(PriorityQueue<Pickable*> pickables) {
		while (!pickables.IsEmpty())
		{
			pickables.Pop()->IncrementWaitingTime();
		}
	}
	//increment service time by 1 to all pickables in a given list
	void IncrementServiceQueue(Queue<Pickable*> pickables) {
		while (!pickables.IsEmpty())
		{
			pickables.Pop()->IncrementServiceTime();
		}
	}
	//Load pickables into elevator
	//P.S: pickables list is passed by refrence
	void LoadPickables(Elevator* elevator, PriorityQueue<Pickable*>& pickabels) {
		if (LoadCargo1(elevator))
			return;
		while (!pickabels.IsEmpty() &&
			elevator->getCurrentLoad() < elevator->getCapacity())
		{
			elevator->AddPickable(pickabels.Pop());
		}
	}
	//Load pickables into elevator
	//P.S: pickables list is passed by refrence
	void LoadPickables(Elevator* elevator, Queue<Pickable*>& pickabels) {
		if (LoadCargo2(elevator))
			return;
		while (!pickabels.IsEmpty() &&
			elevator->getCurrentLoad() < elevator->getCapacity())
		{
			elevator->AddPickable(pickabels.Pop());
		}
	}
	void AddToStairs(Queue<Pickable*>& visitors, Queue<Pickable*>& stairs)
	{
		while (!visitors.IsEmpty() && visitors.Peek()->getWaitingTime() == autoS)
		{
			stairs.Push(visitors.Pop());
		}
	}
	bool LoadCargo1(Elevator* elevator)
	{
		if (elevator->getType() != 'C')
			return false;
		return elevator->getCurrentLoad() > 0 &&
			elevator->getPickable()->getType() == 'C';
	}
	bool LoadCargo2(Elevator* elevator)
	{
		if (elevator->getType() != 'C')
			return false;
		return elevator->getCurrentLoad() > 0 &&
			elevator->getPickable()->getType() == 'P';
	}
#pragma endregion
};
