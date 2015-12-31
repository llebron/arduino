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

#include <limits.h>

#include "../Adafruit_NeoPixel/Adafruit_NeoPixel.h"
#include "../NeoPixel/NeoPixel.h"
#include "Arduino.h"

//max color value
const int MAX_RING_SPEED = 255;

// spinIncrementDuration flag used to stop the spin
const long STOP_SPIN_INCREMENT_DURATION = -1;
// lastSpinIncrementTime flag used to guarantee the next update will increment the offset
const long INCREMENT_SPIN_AT_NEXT_UPDATE = LONG_MIN;

class NeoPixelRing {
public:

	/**
		Initializes the ring with size and pin
	*/
	NeoPixelRing(uint16_t size, uint8_t pin);
	~NeoPixelRing();
	
	/**
		spin the wheel
		arg_spinIncrementDuration: how long to wait between spin increments in ms
		arg_isClockwiseSpin: true if spinning clockwise, otherwise counter clockwise
		If this is called with STOP_SPIN_INCREMENT_DURATION (-1), will stop the spin
	*/
	void spin(long arg_spinIncrementDuration, boolean arg_isClockwiseSpin);
	
	/**
		start/stop the spin, using the current spinIncrementDuration and direction
		Will not start the spin if the increment time is set to STOP_SPIN_INCREMENT_DURATION
	*/
	void toggleSpin();	
	
	/**
		update the ring, must be called every loop for best accuracy
		N.B. Call after updating any ring state - e.g. on/off, blink, brightness, color, spin
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
	// update the spin, returns true if spin offset was actually altered, meaning the ring needs to be refreshed
	bool updateSpinOffset(long currTime);
	uint16_t spinOffset = 0;
	// is the ring spinning?
	bool isSpinning = false;
	// how long between spin increments - initialized to the "null" duration
	long spinIncrementDuration = STOP_SPIN_INCREMENT_DURATION;
	// is the ring spinning clockwise or counter clockwise?
	bool isClockwiseSpin = true;
	// tracking last time spin was incremented
	long lastSpinIncrementTime = INCREMENT_SPIN_AT_NEXT_UPDATE;
	
	// utilities for converting between current and absolute indices
	uint16_t getCurrentIndexFromAbsoluteIndex(uint16_t index);
	uint16_t getAbsoluteIndexFromCurrentIndex(uint16_t index);
	uint16_t getWrappedIndex(uint16_t index);
};

#endif
