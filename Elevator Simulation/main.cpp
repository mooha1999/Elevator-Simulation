#include "Hospital.h"
int main() {
	Hospital* hospital = new Hospital("input.txt");
	hospital->simulate();
	return 0;
}