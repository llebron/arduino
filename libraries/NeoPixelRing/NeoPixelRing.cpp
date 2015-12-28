#include "NeoPixelRing.h"

/*
TODO:
Put to sleep if no activity over time
Turn absolute on/off
Set blink rate current
Set color current
Set brightness current
Update loop
stop spin

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
	// process the spin offset first, so values will be processed accurately
	if (isSpinning) updateSpin(currTime);
}

void NeoPixelRing::spin(long arg_spinIncrementDuration, boolean arg_isClockwiseSpin) {
	if (!isSpinning) {
		// if it's a fresh start, set spin increment tracking time to current time
		lastSpinIncrementTime = millis();
		isSpinning = true;
	}
	spinIncrementDuration = arg_spinIncrementDuration;
	isClockwiseSpin = arg_isClockwiseSpin;	
}

void NeoPixelRing::updateSpin(long currTime) {
	//how long since spin offset was last incremented
	long timePassed = currTime - lastSpinIncrementTime;
		
	// if enough time has passed, increment the spin offset
	if (timePassed >= spinIncrementDuration) {
		if (isClockwiseSpin) {
			spinOffset++;
		} else {
			spinOffset--;
		}
		// modulo size to keep the spin offset within simple bounds (-maxIndex, maxIndex)
		spinOffset = spinOffset % size;
		lastSpinIncrementTime = millis();
	}
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
