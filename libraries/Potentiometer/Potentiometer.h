/**
Potentiometer.h
Larry LeBron

A simple library for a potentiometer connected between 5V, ground and an an analog input
*/

#include "Arduino.h"

#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

const int MAX_POT_VALUE = 1023;

// only considered a value change if the jump is at least of this delta
// prevents noise from random fluctuation
const int MIN_CHANGE_DELTA = 11;

class Potentiometer {
public:
	Potentiometer(uint8_t potPin);
	
	//update the pot, must be called every loop for best accuracy
	void update();
	
	//returns true if the pot changed this update
	bool valChangedThisUpdate();
	
	// returns the percent of the pot, from 0 to 1 (0, MAX_POT_VALUE)
	float getPercentVal();
	
	
private:
	uint8_t potPin; //the pot pin
	int lastAcknowledgedReading; // the last pot reading acknowledged as actual input
	bool changedThisUpdate = false; // did the pot change this update?
};



#endif
