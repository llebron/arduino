/**
NeoPixelRing.h
Larry LeBron

A library wrapping Adafruit's Neopixel library, with some functionality specific to their ring
e.g. spinning, blinking, setting clusters of lights
References to the absolute indices of the pixels are tagged with absolute
References to the current indices of the pixels (relative to the current spin offset) are tagged with current
*/

#ifndef NEOPIXELRING_H
#define NEOPIXELRING_H

#include "../Adafruit_NeoPixel/Adafruit_NeoPixel.h"
#include "../NeoPixel/NeoPixel.h"
#include "Arduino.h"

//max color value
const int MAX_RING_SPEED = 255;

class NeoPixelRing {
public:

	/**
		Initializes the ring with size and pin
	*/
	NeoPixelRing(uint16_t size, uint8_t pin);
	~NeoPixelRing();	
	
	/**
		update the ring, must be called every loop for best accuracy
	*/
	void update();	
	
	
private:
	// the neopixel ring - library defined by Adafruit
	Adafruit_NeoPixel ring;	
	// the size of the ring
	uint16_t size;
	// the max index of the ring
	uint16_t maxIndex;
	// array of on/off flags matching the absolute index for each pixel
	bool* lightActiveStatusAbsolute;
	// array of custom NeoPixel- one for each in ring
	NeoPixel* pixels;
	
	/* 
		tracks the current offset of the lights from their absolute indices, 
		altered as the ring spins
	*/
	void spin(long arg_spinIncrementDuration, boolean arg_isClockwiseSpin);
	void updateSpin(long currTime);
	uint16_t spinOffset = 0;
	// is the ring spinning?
	bool isSpinning = false;
	// how long between spin increments
	long spinIncrementDuration = 0;
	// is the ring spinning clockwise or counter clockwise?
	bool isClockwiseSpin = false;
	// tracking last time spin was incremented
	long lastSpinIncrementTime = 0;
	
	// utilities for converting a current index to an absolute
	uint16_t getAbsoluteIndexFromCurrentIndex(uint16_t index);
	uint16_t getWrappedIndex(uint16_t index);
};

#endif
