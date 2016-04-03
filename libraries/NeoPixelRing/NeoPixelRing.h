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
init - seems like pin vals aren't reliable until after setup - maybe just not until after setup?
forward/back for spin

Nice to have: accel/decel for spin
*/

#ifndef NEOPIXELRING_H
#define NEOPIXELRING_H

#include <limits.h>
#include "Arduino.h"

#include "../Adafruit_NeoPixel/Adafruit_NeoPixel.h"
#include "../NeoPixel/NeoPixel.h"
#include "../Log/log.h"

// spinIncrementDuration flag used to stop the spin
const long STOP_SPIN_INCREMENT_DURATION = -1;
// lastSpinIncrementTime flag used to guarantee the next update will increment the offset
const long INCREMENT_SPIN_AT_NEXT_UPDATE = LONG_MIN;

// fixed array for range for Red->Orange->Yellow->Green->Blue->Indigo->Violet
// hard-coded for a 24-index ring
const uint8_t rainbowColors[] = 
{
	255,	0,		0,
	255,	41,		0,
	255,	83,		0,
	255,	124,	0,
	255,	165,	0,
	255,	195,	0,
	255,	225,	0,
	255,	255,	0,
	191,	223,	0,
	128,	192,	0,
	64,		160,	0,
	0,		128,	0,
	0,		96,		64,
	0,		64,		128,
	0,		32,		191,
	0,		0,		255,
	19,		0,		224,
	38,		0,		193,
	56,		0,		161,
	75,		0,		130,
	116,	33,		157,
	157,	66,		184,
	197,	98,		211,
	238,	130,	238
};

class NeoPixelRing {
public:

	/**
		Initializes the ring with size and pin
	*/
	NeoPixelRing(int size, uint8_t numLightsPerCluster, uint8_t pin);
	~NeoPixelRing();
	
	/**
		update the ring, must be called every loop for best accuracy
		N.B. Call after updating any ring state - e.g. on/off, blink, brightness, color, spin
	*/
	void update();	
	
	/**
		Turn on the this ring index or indices (iterates through numLightsPerCluster)
	*/
	void turnOnRingIndex(int index);
	void turnOnLightCluster(int indices[]);
	
	/**
		Turn off this ring index or indices (iterates through numLightsPerCluster)
	*/
	void turnOffRingIndex(int index);
	void turnOffLightCluster(int indices[]);

	// Set a color for a ring index or indices
	void setRedRingIndex(int index, float redPercent);
	void setRedLightCluster(int indices[], float redPercent);
	void setGreenRingIndex(int index, float greenPercent);
	void setGreenLightCluster(int indices[], float greenPercent);
	void setBlueRingIndex(int index, float bluePercent);
	void setBlueLightCluster(int indices[], float bluePercent);
	
	// Set a brightness percent for a ring index or indices (iterates through numLightsPerCluster)
	void setBrightnessPercentRingIndex(int index, float brightnessPercent);
	void setBrightnessPercentLightCluster(int indices[], float brightnessPercent);
	
	/**
		blink the pixel(s) at this ring index or indices with this blinkLength (iterates through numLightsPerCluster)
	*/
	void blinkRingIndex(int index, long blinkLength);
	void blinkLightCluster(int indices[], long blinkLength);
	
	/**
		stop blinking the pixel(s) at this ring index or indices (iterates through numLightsPerCluster)
	*/
	void stopBlinkRingIndex(int index);
	void stopBlinkLightCluster(int indices[]);	
		
	/**
		spin the wheel
		arg_spinIncrementDuration: how long to wait between spin increments in ms
		arg_isClockwiseSpin: true if spinning clockwise, otherwise counter clockwise
		If this is called with STOP_SPIN_INCREMENT_DURATION (-1), will stop the spin
	*/
	void spin(long arg_spinIncrementDuration, boolean arg_isClockwiseSpin);
	
	/** 
		spin the wheel by amt
		positive is clockwise
		negative is counter clockwise
	*/
	
	/**
		start/stop the spin, using the current spinIncrementDuration and direction
		Will not start the spin if the increment time is set to STOP_SPIN_INCREMENT_DURATION
	*/
	void toggleSpin();	
	
	/**
		Set all pixels to random colors, brightness and blink rate
		Set ring to random spin
		DOES NOT affect the ring index active statuses, which are controlled switches 
		likelihood of blinking/spinning is determined by a random "die roll"
		e.g. chance of 1 in 6 if a D6 is stated
	*/
	void randomize(int blinkDieRollSides, long maxBlinkDuration, int spinDieRollSides, long maxSpinIncrementDuration);
	
	/** 
		Set the ring colors to a rainbow, max brightness and stops all blinking
	*/
	void rainbow();
	
	/**
		Adjust the spin offset by amt
		Positive is clockwise
		Negative is counter clockwise
	*/
	void adjustSpinOffset(int amt);
	
	
private:
	// the neopixel ring - library defined by Adafruit
	Adafruit_NeoPixel* ring;	
	// array of custom NeoPixel- one for each in ring - allows reference to pixels by starting index
	NeoPixel** pixels;
	// the size of the ring
	int size;
	// the max index of the ring
	int maxIndex;
	
	// the number of lights in a cluster. Used for assigning a single switch to a group of lights
	uint8_t numLightsPerCluster;

	// array of on/off flags matching the ring indices
	bool* ringIndexActiveStatus;
	
	// flag and tracking array for ring indices changed since last update
	bool isRingIndicesChangedSinceLastUpdate;
	bool* ringIndicesChangedSinceLastUpdate;
	void flagRingIndexChangedSinceLastUpdateIfActive(int ringIndex);
	/**
		Did this ring index change since last update?
	*/
	bool isRingIndexChangedSinceLastUpdate(int ringIndex);
	void clearRingIndexUpdateChangeTracking();
	
	// initializing this to true to force an initial update
	bool updateAll = true;
	
	
	// tracking array for blinking pixels - true if the pixel's starting index is blinking
	bool* blinkingPixels;
	// update the currently blinking pixels
	void updateBlinkingPixels(long currTime);
	
	/**
		update the ring pixel value for this absolute index.
		Uses the ring index value for on/off and the information for 
		the NeoPixel object currently at this index, compensating for spin offset
	*/
	void updateRingIndex(int ringIndex);
	

	
	// diagnostic method to print the current state of the ring indices
	void printRingIndexActive();
	

	/* 
		tracks the current offset of the lights from their absolute indices, 
		altered as the ring spins
	*/
	int spinOffset = 0;
	void updateSpinOffset(long currTime);

	// is the ring spinning?
	bool isSpinning = false;
	// how long between spin increments - initialized to the "null" duration
	long spinIncrementDuration = STOP_SPIN_INCREMENT_DURATION;
	// is the ring spinning clockwise or counter clockwise?
	bool isClockwiseSpin = true;
	// tracking last time spin was incremented
	long lastSpinIncrementTime = INCREMENT_SPIN_AT_NEXT_UPDATE;
	
	// utilities for converting between ring (current) and starting indices
	NeoPixel* getPixelAtRingIndex(int index);
	int getRingIndexFromStartingIndex(int index);
	int getStartingIndexFromRingIndex(int index);
	int getWrappedIndex(int index);
	
	Log logger;
	
};

#endif
