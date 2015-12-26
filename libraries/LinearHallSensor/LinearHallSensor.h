/**
LinearHallSensor.h
Larry LeBron

A simple library for a LinearHallSensor connected to 5V, ground and an analogPin
The sensor is available here: http://shop.moderndevice.com/products/a1324-hall-effect-sensor

*/

#include "Arduino.h"

#ifndef LINEAR_HALL_SENSOR_H
#define LINEAR_HALL_SENSOR_H

//voltage read values when at a north or south magnetic pole
const int MAX_SOUTH = 10;
const int MIN_NORTH = 1000;

class LinearHallSensor {
public:
	//Initializes the LinearHallSensor
	LinearHallSensor(int sensorPin);
	
	//Returns true if sensor is at a north or south pole
	bool atNorth();
	bool atSouth();
	
	//update the sensor, must be called every loop for best accuracy
	void update();	
	
private:
	int pin; //the sensor pin
	
	//Is the sensor detecting a north or south pole?
	bool north;
	bool south;
};

#endif
