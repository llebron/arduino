/**
NeoPixel.h
Larry LeBron

A library with custom functionality for a pixel on a NeoPixel ring
*/

#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include "Arduino.h"

//max color value
const int MAX_COLOR_VAL = 255;

class NeoPixel {
public:
	
	/**
		update the pixel, must be called every loop for best accuracy
	*/
	void update();	
	
	
private:
	// is the light on?
	bool isOn = false;
	
	// brightness 0 - 1 - multiplies the RGB values when setting color
	float brightnessPercent = 1;
	
	// RGB (0-255)
	uint8_t red = MAX_COLOR_VAL;
	uint8_t green = MAX_COLOR_VAL;
	uint8_t blue = MAX_COLOR_VAL;
	
	// blinking logic
	bool isBlinking = false;
	long blinkToggleTime = 0; //how long has blink been in its current state
	long blinkLength = 0; // how long is on/off
};

#endif
