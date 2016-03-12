#include "SwitchArray.h"

SwitchArray::SwitchArray(){}

void SwitchArray::init(int startingPin, int endingPin) {
	numSwitches = endingPin - startingPin + 1;
	
	switches = new Switch*[numSwitches];
	
	int pin = startingPin;
	for (int i=0; i < numSwitches; i++, pin++) {
		Serial.print(i);Serial.print(": switch at pin ");Serial.println(pin);
		switches[i] = new Switch(pin, 30);
	}
}

Switch SwitchArray::get(int switchIndex) {
	return *switches[switchIndex];
}

void SwitchArray::print() {
	for (int i=0; i < numSwitches; i++) {
		Serial.print("switch at ");Serial.println(i);
		//switches[i] = new Switch(pin, 30);
	}
}