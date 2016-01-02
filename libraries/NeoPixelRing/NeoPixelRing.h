/**
NeoPixelRing.h
Larry LeBron

A library wrapping Adafruit's Neopixel library, with some functionality specific to their NeoPixel ring
e.g. spinning, blinking, setting clusters of lights
References to the starting indices of the pixels are tagged with 'starting'
References to the ring indices of the pixels (relative to the current spin offset) are tagged with 'ringIndex'
*/

/*
TODO:
Put to sleep if no activity over time
Set brightness

Random
Rainbow
Arrays or Vectors of indices

Nice to have: accel/decel for spin
*/

#ifndef NEOPIXELRING_H
#define NEOPIXELRING_H

#include <limits.h>
#include "Arduino.h"

#include "../Adafruit_NeoPixel/Adafruit_NeoPixel.h"
#include "../NeoPixel/NeoPixel.h"

#include "../StandardCplusplus-master/StandardCplusplus.h"
#include "../StandardCplusplus-master/set"
#include "../StandardCplusplus-master/iterator"

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
		Turn on the this ring index
	*/
	void turnOnRingIndex(uint16_t index);
	
	/**
		Turn off this ring index
	*/
	void turnOffRingIndex(uint16_t index);
	
	/**
		blink the pixel at this ring index with this blinkLength
	*/
	void blinkRingIndex(uint16_t index, long blinkLength);
	
	/**
		stop blinking the pixel at this ring index
	*/
	void stopBlinkRingIndex(uint16_t index);
	
	// Set a color for a ring index
	void setRedRingIndex(uint16_t index, uint8_t red);
	void setGreenRingIndex(uint16_t index, uint8_t green);
	void setBlueRingIndex(uint16_t index, uint8_t blue);
	
	// Set a brightness percent for a ring index
	void setBrightnessPercentRingIndex(uint16_t index, float brightnessPercent);
	
	/**
		update the ring, must be called every loop for best accuracy
		N.B. Call after updating any ring state - e.g. on/off, blink, brightness, color, spin
	*/
	void update();	
	
	
private:
	// the neopixel ring - library defined by Adafruit
	Adafruit_NeoPixel ring;	
	// array of custom NeoPixel- one for each in ring - allows reference to pixels by starting index
	NeoPixel* pixels;
	// the size of the ring
	uint16_t size;
	// the max index of the ring
	uint16_t maxIndex;

	// array of on/off flags matching the ring indices
	bool* ringIndexActiveStatus;
	// tracking set for ring indices changed 
	std::set<uint16_t> ringIndicesChangedSinceLastUpdate;
	
	
	// tracking set for blinking pixels - stores the pixel's starting index
	std::set<uint16_t> blinkingPixels;
	// update the currently blinking pixels
	void updateBlinkingPixels(long currTime);
	
	/**
		update the ring pixel value for this absolute index.
		Uses the ring index value for on/off and the information for 
		the NeoPixel object currently at this index, compensating for spin offset
	*/
	void updateRingIndex(uint16_t ringIndex); //Unsure of which way to go so far on this
	
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
	
	// utilities for converting between ring (current) and starting indices
	NeoPixel getPixelAtRingIndex(uint16_t index);
	uint16_t getRingIndexFromStartingIndex(uint16_t index);
	uint16_t getStartingIndexFromRingIndex(uint16_t index);
	uint16_t getWrappedIndex(uint16_t index);
	
};

#endif
