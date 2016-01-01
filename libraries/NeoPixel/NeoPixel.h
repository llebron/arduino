/**
NeoPixel.h
Larry LeBron

A library with custom functionality for a pixel on a NeoPixel ring
*/

#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include "Arduino.h"
#include <math.h> 

//max color value
const int MAX_COLOR_VAL = 255;

class NeoPixel {
public:
	
	/**
		update the pixel, must be called every loop for best accuracy with blink tracking
		N.B. NEED TO ALTER TO RETURN A BOOL TO INDICATE IF THE VALUE HAS CHANGED SINCE LAST UPDATE
	*/
	void update();	
	
	/**
		Is the light on in its blink cycle?
	*/
	bool isOn();
	
	// blink(onOffDuration)
	
	// stopBlink() - should set light to on - absolute light on/off is managed by the ring class
	
	// set brightness percent from 0 <-> 1
	void setBrightnessPercent(float arg_brightnessPercent);
	
	// get the color value - takes current brightness into account
	uint8_t getRed();
	uint8_t getGreen();
	uint8_t getBlue();
	
	
private:
	// is the light on in its current blink cycle
	bool _isOn = false;
	
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
