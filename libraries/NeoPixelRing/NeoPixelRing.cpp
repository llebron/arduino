#include "NeoPixelRing.h"

/*
TODO:
Put to sleep if no activity over time
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
	ringIndexActiveStatus = new bool[size]{true}; 
	pixels = new NeoPixel[size];

	// prepares the arduino output to the ring for communication
	ring.begin();
	
	// should iterate through and turn everything on - maybe start with random?
	// "draw" any changes set here
	ring.show();
}

NeoPixelRing::~NeoPixelRing() {
	delete[] ringIndexActiveStatus;
	delete[] pixels;
}

void NeoPixelRing::update() {
	long currTime = millis();
	bool isRefreshRing = false;
	bool spinOffsetChanged = false;
	
	// process the spin offset first, so values will be processed accurately
	if (isSpinning) {
		spinOffsetChanged = updateSpinOffset(currTime);
	}
	
	// process any lights that are actively blinking
	
	// Need to refresh the ring if there are any changed lights
	isRefreshRing = spinOffsetChanged || !ringIndicesChangedSinceLastUpdate.empty();
	
	// if isRefreshRing, change the lights and call show()
	if (isRefreshRing) {
		 if (spinOffsetChanged) {
		 	// if spin offset updated - go through all ring indices	
		 	for (uint16_t i=0; i<size; i++) {
		 		updateRingIndex(i);
		 	}
		 } else {
		 	// go through only the ringIndicesChangedSinceLastUpdate, updating the ring
		 	for (std::set<uint16_t>::iterator it=ringIndicesChangedSinceLastUpdate.begin(); it!=ringIndicesChangedSinceLastUpdate.end(); it++) {
    			updateRingIndex(*it);
  			}
		 }
		 
		 // draw the changes
		 ring.show();
		 
		// clear the change tracking set
		ringIndicesChangedSinceLastUpdate.clear();
	}
}

void NeoPixelRing::updateRingIndex(uint16_t ringIndex) {
	// get the starting index of the pixel currently at this index to access its state
	uint16_t startingIndexOfCurrentPixel = getStartingIndexFromCurrentIndex(ringIndex);
	NeoPixel pixel = pixels[startingIndexOfCurrentPixel];
	
	// Is the light on  both in absolute terms and relative to its blink cycle?
	bool isOn = ringIndexActiveStatus[ringIndex] && pixel.isOn();
	
	uint8_t r;
	uint8_t g;
	uint8_t b;
	
	if (isOn) {
		// it's on - get the brightness-adjusted colors from the pixel
		r = pixel.getRed();
		g = pixel.getGreen();
		b = pixel.getBlue();
	} else {
		// it's off - either from the ring index or the pixel blinking
		r = 0;
		g = 0;
		b = 0;
	}
	ring.setPixelColor(ringIndex, r, g, b);
}

void NeoPixelRing::turnOnRingIndex(uint16_t index) {
	// If already on, just return
	if (ringIndexActiveStatus[index]) {
		return;
	}
	ringIndexActiveStatus[index] = true;
	ringIndicesChangedSinceLastUpdate.insert(index);
}

void NeoPixelRing::turnOffRingIndex(uint16_t index) {
	// If already off, just return
	if (!ringIndexActiveStatus[index]) {
		return;
	}
	ringIndexActiveStatus[index] = false;
	ringIndicesChangedSinceLastUpdate.insert(index);
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

uint16_t NeoPixelRing::getCurrentIndexFromStartingIndex(uint16_t index) {
	return getWrappedIndex(index + spinOffset);
}

uint16_t NeoPixelRing::getStartingIndexFromCurrentIndex(uint16_t index) {
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
