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

//max color value
// const int MAX_COLOR_VAL = 255;
// toning down overall brightness by setting color to be half max
const int MAX_COLOR_VAL = 77;

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
	
	void setRed(uint8_t r);
	void setGreen(uint8_t g);
	void setBlue(uint8_t b);
	
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
	
	// RGB (0-255)
	uint8_t red = MAX_COLOR_VAL;
	uint8_t green = MAX_COLOR_VAL;
	uint8_t blue = MAX_COLOR_VAL;
	// cap the colorVal from 0 <-> MAX_COLOR_VAL
	uint8_t getCappedColorValue(uint8_t colorVal);
	
	// blinking logic - whether or not light is blinking is tracked by the NeoPixelRing
	// when did blink last toggle
	long blinkToggleTime = millis();
	// how long is blink on/off
	long blinkLength = 0; 
};

#endif
