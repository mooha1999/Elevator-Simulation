#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include "Floor.h"
using namespace std;
class UI
{
	int mode;
	string displayIDs(Queue<int> ids) {
		string ret = "";
		while (!ids.IsEmpty())
		{
			ret += to_string(ids.Pop());
			ret += ",";
		}
		if (ret.length() > 0)
			ret.pop_back(); // remove trailing coma
		return ret;
	}
	void displayLine(int count, string name, Queue<int> ups, Queue<int> downs) {
		cout << count << " Waiting " << name << " : ";
		cout << "UP[" << displayIDs(ups) << "] ";
		cout << "DOWN[" << displayIDs(downs) << "]\n";
	}
	void displayElevators(Queue<Elevator*> elevators) {
		cout << "Elevators: ";
		if (elevators.IsEmpty())
			cout << "None\n";
		else
		{
			string dis = "";
			while (!elevators.IsEmpty())
			{
				dis += displayElevatorData(elevators.Pop());
			}
			dis.pop_back(); // remove trailing space
			dis.pop_back(); // remove trailing coma
			cout << dis << endl;
		}
	}
	string displayElevatorData(Elevator* elevator)
	{
		string ret = "";
		string direction = elevator->IsUp() ? "up" : elevator->IsDown() ? "down" : "";
		string states[] = { "IDLE", "MOV", "UNLOAD", "AVAIL", "OUT" };
		ret += elevator->getType();
		ret += "[" + direction + ", ";
		ret += states[elevator->getState()] + ", ";
		ret += to_string(elevator->getCapacity()) + ", ";
		ret += to_string(elevator->getCurrentLoad()) + "], ";
		return ret;
	}
	int inServiceCount(Queue<Elevator*> elevators)
	{
		int count = 0;
		while (!elevators.IsEmpty())
			count += elevators.Pop()->getCurrentLoad();
		return count;
	}
public:
	void getMode() {
		cout << "Choose the mode:\n[1]Interactive\n[2]Step-By-Step\n[3]Silent\n";
		cin >> mode;
		string m;
		switch (mode)
		{
		case 1:
			m = "Interactive";
			break;
		case 2:
			m = "Step-By-Step";
			break;
		case 3:
			m = "Silent";
			break;
		default:
			cout << "Invalid input\n";
			assert(mode > 0 && mode < 4);
			break;
		}
		cout << m << " Mode\n" << "Simulation Starts...\n";
	}
	void showInfo(int timestep, LinkedList<Floor*> floors, int completedNum, int stairsNum) {
		if (mode == 3)
			return;
		int totalWaitingPickables = 0;
		int totalInService = stairsNum;
		cout << "Current timestep: " << timestep << endl;
		for (int i = floors.Size() - 1; i >= 0; i--) {
			Floor* f = floors[i];
			//Total waiting for each floor
			totalWaitingPickables += f->TotalWaitingCount();
			//Total in-service for each floor
			totalInService += inServiceCount(f->ElevatorsData());
			//display patients
			displayLine(f->WaitingPatientsCount(), "patients"
				, f->UpPatientsIDs(), f->DownPatientsIDs());
			//display cargo
			displayLine(f->WaitingCargoCount(), "cargo"
				, f->UpCargoIDs(), f->DownCargoIDs());
			//display visitors
			displayLine(f->WaitingVisitorsCount(), "visitors"
				, f->UpVisitorsIDs(), f->DownVisitorsIDs());
			displayElevators(floors[i]->ElevatorsData());
			cout << "--------------------- ";
			cout << (i == 0 ? "Ground" : "Floor " + to_string(i));
			cout << " ---------------------\n";
		}
		cout << totalWaitingPickables << " total waiting pass/cargo\n";
		cout << totalInService << " total in-service pass/cargo ( ";
		cout << stairsNum << " visitors by stairs)\n";
		cout << completedNum << " total completed pass/cargo\n\n\n";
		if (mode == 2) {
			Sleep(1000);
		}
		else { // Interactive
			cout << "Enter any character to continue\n";
			string c;
			cin >> c;
		}
	}
	void showEnd() {
		cout << "Simulation ended, Output file created\n";
	}
};
