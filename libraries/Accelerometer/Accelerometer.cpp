#include "Accelerometer.h"

Accelerometer::Accelerometer(int xP, int yP, int zP) {
	xPin = xP;
	yPin = yP;
	zPin = zP;
	movementIntensity = 0;
}

void Accelerometer::calibrate() {
	
	//for tracking the average values
	long numLoops = 0;
 	long xTotal = 0;
  	long yTotal = 0;
  	long zTotal = 0;
 
  	long startTime = millis();
  	
  	//Take a running total and number of iterations
  	while (millis() - startTime < CALIBRATION_DURATION) {
  		analogRead(A0);//hack to compensate for hall sensor
    	xTotal += analogRead(xPin);
    	yTotal += analogRead(yPin);
    	zTotal += analogRead(zPin);
    	numLoops++;
  	}
  	
  	/* Did I need to use longs?
  	//Calculate average readings for rest values
  	long xAvg = xTotal/numLoops;
  	long yAvg = yTotal/numLoops;
  	long zAvg = zTotal/numLoops;
  	
  	xRest = xAvg;
  	yRest = yAvg;
  	zRest = zAvg;
  	*/
  	
  	//Calculate average readings for rest values
  	xRest = xTotal/numLoops;
  	yRest = yTotal/numLoops;
  	zRest = zTotal/numLoops;
  	
  
  	/*Debug*/
  	Serial.println("totals: ");
  	Serial.println(xTotal);
  	Serial.println(yTotal);
  	Serial.println(zTotal);
  	
  	Serial.println("numLoops: ");
  	Serial.println(numLoops);
  	
   	Serial.println("restVals: ");
  	Serial.println(xRest);
  	Serial.println(yRest);
  	Serial.println(zRest);
}

void Accelerometer::updateMovementIntensity() {
  int newX = analogRead(xPin);
  int newY = analogRead(yPin);
  int newZ = analogRead(zPin);
  
  movementIntensity = abs(newX - xRest) + abs(newY - yRest) + abs(newZ - zRest);
}

void Accelerometer::update() {
	updateMovementIntensity();
}

int Accelerometer::getMovementIntensity() {
	return movementIntensity;
}
