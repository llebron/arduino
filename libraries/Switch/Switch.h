/**
Switch.h
Larry LeBron

A simple library for a switch connected between a digital pin and ground

Set the debounce delay to compensate for your switch's bounce factor

Does not block the loop while waiting for your switch to settle
*/

#include "Arduino.h"

#ifndef SWITCH_H
#define SWITCH_H

class Switch {
public:
	/*
		Initializes the switch and activates the internal pullup resistor
		Also sets the debounce delay. This isn't the entire debounce duration. 
		Instead, this is the amount of time by which the reading is delayed every time
		it toggles
	*/
	Switch(uint8_t newSwitchPin, uint8_t debDelay);
	
	//update the switch, must be called every loop for best accuracy
	void update();
	
	//returns true if switch is closed
	bool isClosed();
	
	//returns true if switch was closed this update
	bool closedThisUpdate();
	
	//returns true if switch was opened this update
	bool openedThisUpdate();
	
	
private:
	void handleDebouncing();//MUST BE DEBOUNCING TO CALL. Helper fn to resolve debouncing and state changes	
	
	uint8_t switchPin; //the switch pin
	long toggleTime; //the time when the switch state toggled
	bool debouncing; //are we debouncing?
	bool switchState; //open is false, closed is true
	bool lastReading; //last switch reading
	bool justClosed; //switch was closed this update
	bool justOpened; //switch was opened this update
	//time to wait for switch to stop bouncing. Increases wait by this amount every time switch value toggles
	uint8_t debounceDelay; 
};



#endif
