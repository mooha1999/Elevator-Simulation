#pragma once

#include "UI.h"
// --- Events ---
#include "ArrivalEvent.h"
#include "LeaveEvent.h"
#include "StairsEvent.h"
// --- Pickables ---
#include "Patient.h"
#include "Cargo.h"
#include "Visitor.h"
// --- DSs ---
#include "Queue.h"
#include "LinkedList.h"
// --- STLs ---
#include <string>
#include <fstream>
#include "Request.h"

class Hospital
{
private:
	// --- Members ---
	ifstream* inputfFile;
	ofstream* outputFile;
	UI ui;
	int currentTimestep;
	int floorsNum;
	int elevatorsSpeeds[3];
	int elevatorsCapacity[3];
	int stairsSpeed, autoS;
	Elevator* patientElevator,
		* visitorElevator, * cargoElevator;
	// ---DSs---
	Queue<Event*> events;
	LinkedList<Floor*> floors;
	PriorityQueue<Pickable*> stairs;
	PriorityQueue<Pickable*> completedPickables;
	// ---- Private Methods ----
	//Read data from the input file
	void readFile(string path) {
		*inputfFile >> floorsNum;

		for (int i = 0; i < 3; i++)
			*inputfFile >> elevatorsSpeeds[i];

		for (int i = 0; i < 3; i++)
			*inputfFile >> elevatorsCapacity[i];

		*inputfFile >> stairsSpeed >> autoS;

		//Add floors
		//Actual
		for (int i = 0; i < floorsNum; i++)
			floors.InsertEnd(new Floor(i, autoS));

		addElevatorstoGroundFloor();

		int eventsNum;
		*inputfFile >> eventsNum;
		for (int i = 0; i < eventsNum; i++)
		{
			char eventType;
			*inputfFile >> eventType;
			switch (eventType)
			{
			case 'A':
				readArrivalEvent();
				break;
			case 'L':
				readLeaveEvent();
				break;
			case 'S':
				readStairsEvent();
				break;
			default:
				break;
			}
		}
		inputfFile->close();
	}
	//Read the Arrival event from the file
	void readArrivalEvent() {
		Event* temp;
		char type;
		int timestep, ID, srcFloor, trgFloor, emgLevel = -1;

		*inputfFile >> type >> timestep >> ID >> srcFloor >> trgFloor;

		if (type == 'P')
		{
			*inputfFile >> emgLevel;
			temp = new ArrivalEvent(timestep, ID,
				new Patient(timestep, srcFloor, trgFloor, emgLevel));
		}
		else if (type == 'V')
		{
			temp = new ArrivalEvent(timestep, ID,
				new Visitor(timestep, srcFloor, trgFloor));
		}
		else
		{
			temp = new ArrivalEvent(timestep, ID,
				new Cargo(timestep, srcFloor, trgFloor));
		}

		events.Push(temp);
	}
	//Read the Leave event from the file
	void readLeaveEvent() {
		int timestep, ID;
		*inputfFile >> timestep >> ID;
		events.Push(new LeaveEvent(timestep, ID));
	}
	//Read the Stairs event from the file
	void readStairsEvent() {
		int timestep, ID;
		*inputfFile >> timestep >> ID;
		events.Push(new StairsEvent(timestep, ID));
	}
	//Add elevators to the ground floor
	void addElevatorstoGroundFloor() {
		visitorElevator = new Elevator('V', elevatorsSpeeds[0],
			elevatorsCapacity[0]);

		cargoElevator = new Elevator('C', elevatorsSpeeds[1],
			elevatorsCapacity[1]);

		patientElevator = new Elevator('P', elevatorsSpeeds[2],
			elevatorsCapacity[2]);

		floors[0]->AddElevator(visitorElevator);
		floors[0]->AddElevator(cargoElevator);
		floors[0]->AddElevator(patientElevator);
	}

	void IncrementTimes() {
		patientElevator->IncrementTimes();
		cargoElevator->IncrementTimes();
		visitorElevator->IncrementTimes();
		PriorityQueue<Pickable*> temp;
		while (!stairs.IsEmpty())
		{
			auto p = stairs.Top();
			p->IncrementServiceTime();
			bool completed = p->getArrivalTime() + p->getServiceTime() + p->getWaitingTime() == p->getTargetTime();
			if (completed)
				completedPickables.Insert(stairs.Pop(), p->getTargetTime() * -1);
			else
				temp.Insert(stairs.Pop(), p->getTargetTime() * -1);
		}
		stairs = temp;
	}

	void MoveStairs() {
		for (int i = 0; i < floorsNum; i++)
		{
			auto& st = floors[i]->GetStairs();
			while (!st.IsEmpty())
			{
				auto p = st.Pop();
				int diff = abs(p->getTargetFloor() - p->getSourceFloor());
				p->setTargetTime(p->getWaitingTime() + diff * stairsSpeed);
				stairs.Insert(p, p->getTargetTime() * -1);
			}
		}
	}
	//Move Elevators
	void Move() {
		MoveElevator(patientElevator);
		MoveElevator(cargoElevator);
		MoveElevator(visitorElevator);
		MoveStairs();
	}
	void MoveElevator(Elevator* elevator) {
		if (elevator->Move())
		{
			floors[elevator->getCurrentFloor()]->RemoveElevator(elevator->getType());
			int newFloor;
			if (elevator->IsUp()) {
				newFloor = elevator->getCurrentFloor() + 1;
				floors[newFloor]->AddElevator(elevator);
			}
			else if (elevator->IsDown()) {
				newFloor = elevator->getCurrentFloor() - 1;
				floors[newFloor]->AddElevator(elevator);
			}
		}
	}
	//Unload pickabels reaching their target floor
	void Unload() {
		UnloadElevator(patientElevator);
		UnloadElevator(cargoElevator);
		UnloadElevator(visitorElevator);
	}
	void UnloadElevator(Elevator* elevator)
	{
		if (*elevator == Elevator::UNLOAD)
		{
			if (!elevator->IsUnLoading())
				elevator->setIsUnloading(true);
			else
			{
				Pickable* p = elevator->Unload();
				while (p)
				{
					completedPickables.Insert(p, p->getTargetTime() * -1);
					p = elevator->Unload();
				}
				elevator->setState(Elevator::IDLE);
				elevator->setIsUnloading(false);
			}
		}
	}
	bool isAllElevatorsEmpty() {
		return isElevatorEmpty(patientElevator)
			&& isElevatorEmpty(cargoElevator)
			&& isElevatorEmpty(visitorElevator);
	}
	bool isElevatorEmpty(Elevator* e)
	{
		return e->getCurrentLoad() == 0;
	}
	void Requests() {
		for (int i = 0; i < floorsNum; i++) {
			Floor* f = floors[i];
			if (f->IsUpPatientRequest())
				HandlePatientRequest(i, true);
			if (f->IsDownPatientRequest())
				HandlePatientRequest(i, false);
			if (f->IsUpVisitorRequest())
				HandleVisitorRequest(i, true);
			if (f->IsDownVisitorRequest())
				HandleVisitorRequest(i, false);
			if (f->IsUpCargoRequest())
				HandleCargoRequest(i, true);
			if (f->IsDownCargoRequest())
				HandleCargoRequest(i, false);
		}
	}
	void HandlePatientRequest(int floor, bool isUp)
	{
		if (isUp)
		{
			bool reqPElevator = patientElevator->IsUp() && patientElevator->getCurrentFloor() < floor;
			bool reqVElevator = visitorElevator->IsUp() && visitorElevator->getCurrentFloor() < floor;
			bool reqCElevator = cargoElevator->IsUp() && cargoElevator->getCurrentFloor() < floor;

			if (!reqPElevator && !reqVElevator && !reqCElevator)
			{
				CheckElevator(patientElevator, floor) || CheckElevator(visitorElevator, floor)
					|| CheckElevator(cargoElevator, floor);
			}
		}
		else if (!isUp)
		{
			bool reqPElevator = patientElevator->IsDown() && patientElevator->getCurrentFloor() > floor;
			bool reqVElevator = visitorElevator->IsDown() && visitorElevator->getCurrentFloor() > floor;
			bool reqCElevator = cargoElevator->IsDown() && cargoElevator->getCurrentFloor() > floor;

			if (!reqPElevator && !reqVElevator && !reqCElevator)
			{
				CheckElevator(patientElevator, floor) || CheckElevator(visitorElevator, floor)
					|| CheckElevator(cargoElevator, floor);
			}
		}
	}
	void HandleVisitorRequest(int floor, bool isUp) {
		if (isUp)
		{
			bool reqPElevator = patientElevator->IsUp() && patientElevator->getCurrentFloor() < floor;
			bool reqVElevator = visitorElevator->IsUp() && visitorElevator->getCurrentFloor() < floor;
			if (!reqPElevator && !reqVElevator)
			{
				CheckElevator(visitorElevator, floor) || CheckElevator(patientElevator, floor);
			}
		}
		else if (!isUp)
		{
			bool reqPElevator = patientElevator->IsDown() && patientElevator->getCurrentFloor() > floor;
			bool reqVElevator = visitorElevator->IsDown() && visitorElevator->getCurrentFloor() > floor;

			if (!reqPElevator && !reqVElevator)
			{
				CheckElevator(visitorElevator, floor) || CheckElevator(patientElevator, floor);
			}
		}
	}
	void HandleCargoRequest(int floor, bool isUp) {
		if (isUp)
		{
			bool reqCElevator = cargoElevator->IsUp() && cargoElevator->getCurrentFloor() < floor;

			if (!reqCElevator)
			{
				CheckElevator(cargoElevator, floor);
			}
		}
		else if (!isUp)
		{
			bool reqCElevator = cargoElevator->IsDown() && cargoElevator->getCurrentFloor() > floor;

			if (!reqCElevator)
			{
				CheckElevator(cargoElevator, floor);
			}
		}
	}
	bool CheckElevator(Elevator* elevator, int floor)
	{
		if (*elevator == Elevator::IDLE && elevator->HasNoDirection()
			&& floors[elevator->getCurrentFloor()]->IsAllWaitingListsEmpty())
		{
			bool up = elevator->getCurrentFloor() < floor;
			elevator->setTargetFloor(floor);
			elevator->setIsUp(up);
			elevator->setIsDown(up);
			elevator->setState(Elevator::MOV);
			return true;
		}
		return false;
	}
	//Generate output file
	void generateOutputFile() {
		outputFile = new ofstream("Output.txt");
		*outputFile << "TT\tID\tAT\tWT\tST\n";
		int patients = 0, cargos = 0, visitors = 0;
		float totalWaitingTime = 0;
		while (!completedPickables.IsEmpty()) {
			Pickable* pickable = completedPickables.Pop();
			*outputFile << pickable->getTargetTime() << '\t';
			*outputFile << pickable->getID() << '\t';
			*outputFile << pickable->getArrivalTime() << '\t';
			*outputFile << pickable->getWaitingTime() << '\t';
			*outputFile << pickable->getServiceTime() << '\n';
			totalWaitingTime += pickable->getWaitingTime();
			switch (pickable->getType())
			{
			case 'P':
				patients++;
				break;
			case 'C':
				cargos++;
				break;
			case 'V':
				visitors++;
				break;
			default:
				break;
			}
		}
		*outputFile << "....................\n....................\n";
		*outputFile << "passengers: " << patients + visitors << "  ";
		*outputFile << "[V: " << visitors << ",P: " << patients << "]\n";
		*outputFile << "cargos: " << cargos << '\n';
		*outputFile << "Avg Wait = " << totalWaitingTime / (float)currentTimestep;
		outputFile->close();
	}
public:
	Hospital(string path) {
		currentTimestep = 1;
		floorsNum = stairsSpeed = autoS = 0;
		inputfFile = nullptr;
		outputFile = nullptr;
		patientElevator = visitorElevator = cargoElevator = nullptr;
		inputfFile = new ifstream(path);

		readFile(path);
	}

	void simulate() {
		ui.getMode();
		//Start simulation
		bool isAllWaitingListsEmpty = true;
		while (true) {
			//Check if multiple events should execute
			while (!events.IsEmpty() && events.Peek()->getTimeStep() == currentTimestep) {
				events.Pop()->Execute(floors);
			}
			//Move Elevators
			Move();
			//Load Elevators
			for (int i = 0; i < floors.Size(); i++) {
				floors[i]->LoadElevators();
			}
			//Unload Elevators
			Unload();
			//Increment AVAIL, IDLE and Service times for all elevators
			IncrementTimes();
			//Elevator Requests
			//Requests();
			//Increment the waiting time of every unpicked pickable
			for (int i = 0; i < floors.Size(); i++) {
				floors[i]->IncrementAllWaitingLists();
			}
			//Display timestep's information
			ui.showInfo(currentTimestep, floors, completedPickables.Size(), stairs.Size());
			//Check emptiness of all waiting lists
			isAllWaitingListsEmpty = true;
			for (int i = 0; i < floors.Size(); i++) {
				isAllWaitingListsEmpty =
					isAllWaitingListsEmpty && floors[i]->IsAllWaitingListsEmpty();
			}
			//if all waiting lists are empty and all events are executed the break out of the loop
			if (isAllWaitingListsEmpty && events.IsEmpty()
				&& isAllElevatorsEmpty() && stairs.IsEmpty())
				break;
			//Increment the timestep by 1
			currentTimestep++;
		}
		generateOutputFile();
		ui.showEnd();
	}
};