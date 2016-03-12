#ifndef SWITCH_ARRAY_H
#define SWITCH_ARRAY_H

#include "Arduino.h"
#include "../Switch/Switch.h"

class SwitchArray {

public:
	/* Build the array of switches linearly from starting pin to ending pin */
	SwitchArray();
	
	void init(int startingPin, int endingPin);
	
	Switch get(int switchIndex);
	
	void print();
	
private: 
	Switch** switches;
	int numSwitches;
};

#endif