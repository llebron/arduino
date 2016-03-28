#include "Potentiometer.h"

Potentiometer::Potentiometer(int arg_pin) {
	potPin = arg_pin;
	changedThisUpdate = false;
	// initialize to the current value
	lastAcknowledgedReading = -1; // guarantees it'll update to current value during first update
}

void Potentiometer::update() {
	int newReading = analogRead(potPin);
	
	int readingChange = lastAcknowledgedReading - newReading;
	
	changedThisUpdate = abs(readingChange) >= MIN_CHANGE_DELTA;
	
	/*
		only update lastAcknowledgedReading if the change was over the delta.
		prevents the pot from losing actual changes which happen slowly, 
		below the MIN_CHANGE_DELTA
	*/
	if (changedThisUpdate) {
		lastAcknowledgedReading = newReading;
	}
}

bool Potentiometer::valChangedThisUpdate() {
	return changedThisUpdate;
}

float Potentiometer::getPercentVal() {
	return (float)lastAcknowledgedReading/(float)MAX_POT_VALUE;
}
