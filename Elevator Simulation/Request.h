#pragma once
struct Request
{
	int floor;
	bool isUp;
	char type;
	Request(int floor, bool isUp, char type) {
		this->floor = floor;
		this->isUp = isUp;
		this->type = type;
	}
};

