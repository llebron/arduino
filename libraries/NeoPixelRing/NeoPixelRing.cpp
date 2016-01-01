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
	
	// process any lights that are actively blinking
	
	// Need to refresh the ring if there are any changed lights
	isRefreshRing |= !lightsChangedSinceLastUpdate.empty();
	
	// series of isRefreshRing |= methodThatIndicatesRefreshRing (e.g. blink, switches, etc)
	
	// process any lights that are actively blinking
	
	// if isRefreshRing, change the lights and call show()
	if (isRefreshRing) {
		 // incrementing spin means updating every light regardless of other changes
		 
		 // otherwise, go through the lightsChangedSinceLastUpdate, updating the ring

		 // on/off & blink: just the lights that are actually turning from on <-> off
		 // first check absolute on/off... then, if it's on, check the blink status
		 
		 // color: just the lights with colors changing
		 // brightness: just the lights with the brightness changing
	}
	
	// clear the change tracking set
	lightsChangedSinceLastUpdate.clear();
}

void NeoPixelRing::turnOnAbsolute(uint16_t index) {
	// If already on, just return
	if (lightActiveStatusAbsolute[index]) {
		return;
	}
	lightActiveStatusAbsolute[index] = true;
	lightsChangedSinceLastUpdate.insert(index);
}

void NeoPixelRing::turnOffAbsolute(uint16_t index) {
	// If already off, just return
	if (!lightActiveStatusAbsolute[index]) {
		return;
	}
	lightActiveStatusAbsolute[index] = false;
	lightsChangedSinceLastUpdate.insert(index);
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

uint16_t NeoPixelRing::getCurrentIndexFromAbsoluteIndex(uint16_t index) {
	return getWrappedIndex(index + spinOffset);
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
