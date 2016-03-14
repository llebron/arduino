/**
Log.h
Larry LeBron

A simple library for logging, wrapped around Serial.print
*/

#include "Arduino.h"

#ifndef LOG_H
#define LOG_H

class Log {
public:
	void log(String msg) {
		if (isLogging) {
			Serial.println(msg);
		}
	}
	void log(String msg, int i) {
		if (isLogging) {
			Serial.print(msg); Serial.println(i);
		}
	}
	void log(String msg, int i, String msg2) {
		if (isLogging) {
			Serial.print(msg); Serial.print(i); Serial.println(msg2);
		}
	}
	void log(String msg, int i, String msg2, int i2) {
		if (isLogging) {
			Serial.print(msg); Serial.print(i); Serial.print(msg2); Serial.println(i2);
		}
	}
	void log(String msg, int i, String msg2, int i2, String msg3) {
		if (isLogging) {
			Serial.print(msg); Serial.print(i); Serial.print(msg2); Serial.print(i2); Serial.println(msg3);
		}
	}
	void log(String msg, int i, String msg2, int i2, String msg3, int i3) {
		if (isLogging) {
			Serial.print(msg); Serial.print(i); Serial.print(msg2); Serial.print(i2); Serial.print(msg3); Serial.println(i3);
		}
	}
	

	
private:
	const bool isLogging = false;
};

#endif


