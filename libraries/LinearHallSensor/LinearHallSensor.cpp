#include "LinearHallSensor.h"

LinearHallSensor::LinearHallSensor(int sensorPin) {
	pin = sensorPin;
	north = false;
	south = false;
}

bool LinearHallSensor::atNorth() {
	return north;
}

bool LinearHallSensor::atSouth() {
	return south;
}

void LinearHallSensor::update() {

	//read the sensor, and set north or south if it's detecting one of those fields
	int val = analogRead(pin);
	if (val <= MAX_SOUTH) {
		south = true;
		north = false;
	} else if (val >= MIN_NORTH) {
		north  = true;
		south = false;
	} else {
		north = false;
		south = false;
	}
}
