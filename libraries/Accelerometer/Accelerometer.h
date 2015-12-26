/**
Accelerometer.h
Larry LeBron

A simple library for an analog accelerometer connected to three analog pins, 5V and ground

Specifically designed for this unit, from moderndevice.com: 
http://shop.moderndevice.com/products/mma7260qt-3-axis-accelerometer

*/

#include "Arduino.h"

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

//how long to initialize accelerometer rest values
const int CALIBRATION_DURATION = 2000;

class Accelerometer {
public:
	
	/*
		Initializes & calibrates the Accelerometer
		xP, yP, zP: the respective analog pins for each axis
	*/
	Accelerometer(int xP, int yP, int zP);
	
	//Returns a sum of the differences between each axis' rest value most recent update value
	int getMovementIntensity();
	
	//update the Accelerometer, must be called every loop for best accuracy
	void update();	
	
	//!!MUST CALIBRATE AT REST PRIOR TO USE FOR ACCURATE INTENSITY MEASUREMENTS!!
	void calibrate();
	
private:
		
	//update the movement intensity -- described below
	void updateMovementIntensity();

	/*
		an overall measure of movement intensity from the most recent update
		Calculated by adding up the differences between each axis' rest value most recent update value
	*/
	int movementIntensity;
	
	//the axis pins
	int xPin;
	int yPin;
	int zPin;
	
	//the values when the accelerometer is at rest -- set through the calibration
	int xRest;
	int yRest;
	int zRest;
};

#endif
