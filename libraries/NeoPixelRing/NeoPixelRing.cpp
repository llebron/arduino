#include "NeoPixelRing.h"

/*
TODO:
Put to sleep if no activity over time
Turn absolute on/off
Set blink rate current
Set color current
Set brightness current
Update loop

Nice to have: accel/decel for spin
*/

NeoPixelRing::NeoPixelRing(uint16_t arg_size, uint8_t pin) {
	size = arg_size;
	maxIndex = size-1;
	ring.updateLength(size);
	ring.setPin(pin);
	lightActiveStatusAbsolute = new bool[size]{true}; 
	pixels = new NeoPixel[size];
	// should iterate through and turn everything on - maybe start with random?
}

NeoPixelRing::~NeoPixelRing() {
	delete[] lightActiveStatusAbsolute;
	delete[] pixels;
}

void NeoPixelRing::update() {
	long currTime = millis();
	bool isRefreshRing = false;
	
	// process the spin offset first, so values will be processed accurately
	if (isSpinning) {
		isRefreshRing = updateSpinOffset(currTime);
	}
	
	// series of isRefreshRing |= methodThatIndicatesRefreshRing (e.g. blink, switches, etc)
	// if isRefreshRing, change the lights and call show()
	if (isRefreshRing) {
		 // updating spin means updating every light

		 // on/off & blink: just the lights that are actually turning from on <-> off
		 // color: just the lights with colors changing
		 // brightness: just the lights with the brightness changing
	}
}

void NeoPixelRing::spin(long arg_spinIncrementDuration, boolean arg_isClockwiseSpin) {
	spinIncrementDuration = arg_spinIncrementDuration;
	isClockwiseSpin = arg_isClockwiseSpin;	
	
	// If STOP_SPIN_INCREMENT_DURATION is passed in, stop the spin and return immediately
	if (arg_spinIncrementDuration == STOP_SPIN_INCREMENT_DURATION) {
		isSpinning = false;
		return;
	}
	
	// if it's a fresh start, set spin increment tracking time to increment at the next update
	if (!isSpinning) {
		lastSpinIncrementTime = INCREMENT_SPIN_AT_NEXT_UPDATE;
		isSpinning = true;
	}
}

void NeoPixelRing::toggleSpin() {
	
	if (!isSpinning) {
		// just call spin with the current settings
		spin(spinIncrementDuration, isClockwiseSpin);
	} else {
		isSpinning = false;
	}
}

bool NeoPixelRing::updateSpinOffset(long currTime) {
	bool incrementSpin = (lastSpinIncrementTime == INCREMENT_SPIN_AT_NEXT_UPDATE) ||
							(currTime - lastSpinIncrementTime) >= spinIncrementDuration;
	
	// if the lastSpinIncrementTime is the INCREMENT_SPIN_AT_NEXT_UPDATE flag, 
	// or if enough time has passed increment the spin offset
	if (incrementSpin) {
		if (isClockwiseSpin) {
			spinOffset++;
		} else {
			spinOffset--;
		}
		// modulo size to keep the spin offset within simple bounds (-maxIndex, maxIndex)
		spinOffset = spinOffset % size;
		lastSpinIncrementTime = millis();
	}
	return incrementSpin;
}

uint16_t NeoPixelRing::getAbsoluteIndexFromCurrentIndex(uint16_t index) {
	return getWrappedIndex(index - spinOffset);
}

/**
	Accepts any index value and calculates the proper value by "wrapping" it 
	within the allowable bounds for the ring indices (0, size-1)
*/
uint16_t NeoPixelRing::getWrappedIndex(uint16_t index)
{
	// accounts for negative values
	if (index < 0)
        index += size * ((-index) / size + 1);

    return index % size;
}
