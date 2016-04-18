#include "Switch.h"

Switch::Switch(uint8_t newSwitchPin, uint8_t debDelay) {
	switchPin = newSwitchPin;
	pinMode(switchPin, INPUT_PULLUP);//activate pullup resistor
	toggleTime = -1;
	debounceDelay = debDelay;
	
	lastReading = digitalRead(switchPin); // read current state
	switchState = lastReading == LOW; //LOW means switch is closed
	justClosed = switchState;
	justOpened =  !switchState; 
	
}

void Switch::update() {
	bool newReading = digitalRead(switchPin);
	
	//reset just push/release booleans;
	justClosed = false;
	justOpened = false;
	
	//switch has been toggled or is bouncing
	if (newReading != lastReading) {
		toggleTime = millis();//set time toggled
		debouncing = true;
	}
		
	//update lastReading
	lastReading = newReading;
	
	if (debouncing) handleDebouncing();
}

void Switch::handleDebouncing() {
	if (!debouncing) return;//if improperly called
	
	//we've been debouncing longer than the debounceDelay time
	if (millis() - toggleTime >= debounceDelay ) {
		debouncing = false;
		
		//switch is closed
		if (lastReading == LOW) {
			if (!switchState) justClosed = true; //switch was just pushed
			switchState = true;
			
		} else { //switch is open
			if (switchState) justOpened = true; //switch was just released
			switchState = false;
		}
	}
}

bool Switch::isClosed() {
	return switchState;
}

bool Switch::closedThisUpdate() {
	return justClosed;
}

bool Switch::openedThisUpdate() {
	return justOpened;
}
