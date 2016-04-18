/**
NeoPixel.h
Larry LeBron

A library with custom functionality for a pixel on a NeoPixelRing
Designed to be accessed via the API provided by NeoPixelRing
*/

#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include "Arduino.h"
#include <math.h> 

const int MAX_COLOR_VAL = 255;

// Toning down overall brightness by scaling down by this factor
const float BRIGHTNESS_SCALING_FACTOR = .1f;

class NeoPixel {
public:

	NeoPixel(uint8_t arg_startingIndex);
	
	/**
		update the pixel, must be called every loop for best accuracy with blink tracking
		returns true if the value of isBlinkOn() changes
	*/
	bool updateBlink(long currTime);	
	
	/**
		Is the light on in its blink cycle?
	*/
	bool isBlinkOn();
	
	// blink the pixel - it'll toggle between on/off after blinkLength (ms)
	void blink(long arg_blinkLength);
	
	// stop the blinking - sets _isBlinkOn to true, 
	// so "absolute" on/off can be managed by NeoPixelRing
	void stopBlink();
	
	float getBrightnessPercent();
	
	// set brightness percent from 0 <-> 1
	void setBrightnessPercent(float arg_brightnessPercent);
	
	/* Set the color percentage */
	void setRedPercent(float r);
	void setGreenPercent(float g);
	void setBluePercent(float b);
	
	// get the color value - takes current brightness into account
	uint8_t getRed();
	uint8_t getGreen();
	uint8_t getBlue();
	
	
private:

	uint8_t startingIndex;
		
	// is the light on in its current blink cycle
	bool _isBlinkOn = true;
	
	// brightness 0 - 1 - multiplies the RGB values when setting color
	float brightnessPercent = 1;
	
	// RGB
	uint8_t red = MAX_COLOR_VAL;
	uint8_t green = MAX_COLOR_VAL;
	uint8_t blue = MAX_COLOR_VAL;
	
	// get the colorVal for the percent, from 0 <-> MAX_COLOR_VAL
	uint8_t getColorValueForPercent(float colorVal);
	
	// blinking logic - whether or not light is blinking is tracked by the NeoPixelRing
	// when did blink last toggle
	long blinkToggleTime = millis();
	// how long is blink on/off
	int blinkLength = 0; 
};

#endif
