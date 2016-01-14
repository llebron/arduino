#include "Potentiometer.h"

Potentiometer::Potentiometer(int arg_pin) {
	potPin = arg_pin;
	changedThisUpdate = false;
	// initialize to the current value
	lastReading = -1; // guarantees it'll update to current value during first update
}

void Potentiometer::update() {
	int newReading = analogRead(potPin);
	
	int readingChange = lastReading - newReading;
	
	changedThisUpdate = abs(readingChange) >= MIN_CHANGE_DELTA;
	
	/*Serial.print("last: "); Serial.println(lastReading);
	Serial.print("newReading: "); Serial.println(newReading);
	Serial.print("changed: "); Serial.println(changedThisUpdate);*/
	
	/*
		only update lastReading if the change was significant.
		prevents the pot from losing actual changes which happen slowly, 
		below the MIN_CHANGE_DELTA
	*/
	if (changedThisUpdate) {
		lastReading = newReading;
	}
}

bool Potentiometer::valChangedThisUpdate() {
	return changedThisUpdate;
}

float Potentiometer::getPercentVal() {
	return (float)lastReading/(float)MAX_POT_VALUE;
}
