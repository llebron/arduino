#include "NeoPixelRing.h"

NeoPixelRing::NeoPixelRing(int arg_size, uint8_t arg_numLightsPerCluster, uint8_t pin) {
	size = arg_size;
	numLightsPerCluster = arg_numLightsPerCluster;
	maxIndex = size-1;
	ring = new Adafruit_NeoPixel(size, pin, NEO_GRB + NEO_KHZ800);
	isRingIndicesChangedSinceLastUpdate = false;
	ringIndexActiveStatus = new bool[size]; 
	ringIndicesChangedSinceLastUpdate = new bool[size];
	blinkingPixels = new bool[size];
	pixels = new NeoPixel*[size];
	for (int i=0; i < size; i++) {
		ringIndexActiveStatus[i] = false; // will be set properly by the current state of the switches
		ringIndicesChangedSinceLastUpdate[i] = false;
		pixels[i] = new NeoPixel(i);
		blinkingPixels[i] = false;
	}
	ring->begin();
}

NeoPixelRing::~NeoPixelRing() {
	delete[] ringIndexActiveStatus;
	delete[] ringIndicesChangedSinceLastUpdate;
	for (int i=0; i < size; i++) {
		delete pixels[i];
	}
	delete[] pixels;
	delete ring;
}

void NeoPixelRing::printRingIndexActive() {
	for (int i=0; i < size; i++) {
		logger.log("ring active status for ", i, ": ", ringIndexActiveStatus[i]);
	}
}


void NeoPixelRing::update() {

	long currTime = millis();
	bool isRefreshRing = false;
	
	// process the spin offset first, so values will be processed accurately
	if (isSpinning) {
		// if offset changes, updateAll flag will be set true
		updateSpinOffset(currTime);
	}
	
	// process any lights that are actively blinking - if any lights toggle blink value
	// it'll update add its index to ringIndicesChangedSinceLastUpdate
	updateBlinkingPixels(currTime);
	
	// Need to refresh the ring if there are any changed lights
	isRefreshRing = updateAll || isRingIndicesChangedSinceLastUpdate;
	
	// if isRefreshRing, change the lights and call show()
	if (isRefreshRing) {
		 if (updateAll) {
		 	// if updateAll - go through all ring indices	
		 	for (int i=0; i<size; i++) {
		 		updateRingIndex(i);
		 	}
		 } else {
		 	// go through only the ringIndicesChangedSinceLastUpdate, updating the ring
		 	for (int index = 0; index < size; index++) {
		 		if (isRingIndexChangedSinceLastUpdate(index)) {
    				updateRingIndex(index);
    			}
  			}
		 }
		 
		 // draw the changes
		 ring->show();
		 
		// clear the change tracking set and updateAll flag
		clearRingIndexUpdateChangeTracking();
		updateAll = false;
	}
}

bool NeoPixelRing::isRingIndexChangedSinceLastUpdate(int ringIndex) {
	return ringIndicesChangedSinceLastUpdate[ringIndex];
}

void NeoPixelRing::flagRingIndexChangedSinceLastUpdateIfActive(int ringIndex) {
	if (ringIndexActiveStatus[ringIndex]) {
		ringIndicesChangedSinceLastUpdate[ringIndex] = true;
		/* set the global tracking flag true */
		isRingIndicesChangedSinceLastUpdate = true;
	}
}

void NeoPixelRing::clearRingIndexUpdateChangeTracking() {
	for (int i=0; i < size; i++) {
		ringIndicesChangedSinceLastUpdate[i] = false;
	}
	isRingIndicesChangedSinceLastUpdate = false;
}

void NeoPixelRing::updateBlinkingPixels(long currTime) {
	for (int i = 0; i < size; i++) {
		if (blinkingPixels[i]) {
			int blinkingPixelStartingIndex = i;
			NeoPixel* blinkingPixel = pixels[blinkingPixelStartingIndex];
			bool pixelBlinked = blinkingPixel->updateBlink(currTime);
		
			// if the pixel blinks this update, AND the ringIndex is active, add it to the tracking set
			if (pixelBlinked) {
				int ringIndexForPixel = getRingIndexFromStartingIndex(blinkingPixelStartingIndex);
				flagRingIndexChangedSinceLastUpdateIfActive(ringIndexForPixel);
			}
		}
  	}
}

void NeoPixelRing::updateRingIndex(int ringIndex) {	
	NeoPixel* pixel = getPixelAtRingIndex(ringIndex);
	
	// Is the light on  both in absolute terms and relative to its blink cycle?
	bool isOn = ringIndexActiveStatus[ringIndex] && pixel->isBlinkOn();
	
	//logger.log("Update ring index ", ringIndex, " ", isOn);
	
	uint8_t r;
	uint8_t g;
	uint8_t b;
	
	if (isOn) {
		// it's on - get the brightness-adjusted colors from the pixel
		r = pixel->getRed();
		g = pixel->getGreen();
		b = pixel->getBlue();
	} else {
		// it's off - either from the ring index or the pixel blinking
		r = 0;
		g = 0;
		b = 0;
	}

	ring->setPixelColor(ringIndex, r, g, b);
}

void NeoPixelRing::turnOnRingIndex(int index) {
	logger.log("ring index on ", index);
	// If already on, just return
	if (ringIndexActiveStatus[index]) {
		return;
	}
	ringIndexActiveStatus[index] = true;
	flagRingIndexChangedSinceLastUpdateIfActive(index);
}

void NeoPixelRing::turnOnLightCluster(int indices[]) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		turnOnRingIndex(indices[i]);
	}
}

void NeoPixelRing::turnOffRingIndex(int index) {
	logger.log("ring index off ", index);
	// If already off, just return
	if (!ringIndexActiveStatus[index]) {
		return;
	}
	// Must flag before making the indexActiveStatus false
	flagRingIndexChangedSinceLastUpdateIfActive(index);
	ringIndexActiveStatus[index] = false;
}

void NeoPixelRing::turnOffLightCluster(int indices[]) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		turnOffRingIndex(indices[i]);
	}
}

void NeoPixelRing::setRedRingIndex(int index, float redPercent) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setRedPercent(redPercent);	
	flagRingIndexChangedSinceLastUpdateIfActive(index);
	
}
void NeoPixelRing::setRedLightCluster(int indices[], float redPercent) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		setRedRingIndex(indices[i], redPercent);
	}
}

void NeoPixelRing::setGreenRingIndex(int index, float greenPercent) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setGreenPercent(greenPercent);	
	flagRingIndexChangedSinceLastUpdateIfActive(index);
}
void NeoPixelRing::setGreenLightCluster(int indices[], float greenPercent) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		setGreenRingIndex(indices[i], greenPercent);
	}
}

void NeoPixelRing::setBlueRingIndex(int index, float bluePercent) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setBluePercent(bluePercent);	
	flagRingIndexChangedSinceLastUpdateIfActive(index);
}
void NeoPixelRing::setBlueLightCluster(int indices[], float bluePercent) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		setBlueRingIndex(indices[i], bluePercent);
	}
}

void NeoPixelRing::setBrightnessPercentRingIndex(int index, float brightnessPercent) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setBrightnessPercent(brightnessPercent);
	flagRingIndexChangedSinceLastUpdateIfActive(index);
}
void NeoPixelRing::setBrightnessPercentLightCluster(int indices[], float brightnessPercent) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		setBrightnessPercentRingIndex(indices[i], brightnessPercent);
	}
}

void NeoPixelRing::blinkRingIndex(int index, long blinkLength) {
	int startingIndexForRingIndex = getStartingIndexFromRingIndex(index);
	NeoPixel* pixel = pixels[startingIndexForRingIndex];
	pixel->blink(blinkLength);	
	blinkingPixels[startingIndexForRingIndex] = true;
}
void NeoPixelRing::blinkLightCluster(int indices[], long blinkLength) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		blinkRingIndex(indices[i], blinkLength);
	}
}

void NeoPixelRing::stopBlinkRingIndex(int index) {
	int startingIndexForRingIndex = getStartingIndexFromRingIndex(index);
	NeoPixel* pixel = pixels[startingIndexForRingIndex];
	pixel->stopBlink();	
	blinkingPixels[startingIndexForRingIndex] = false;
	// manually flag the ring index as changed, since it'll no longer be processed by the blink update
	flagRingIndexChangedSinceLastUpdateIfActive(index);
}
void NeoPixelRing::stopBlinkLightCluster(int indices[]) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		stopBlinkRingIndex(indices[i]);
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

void NeoPixelRing::updateSpinOffset(long currTime) {
	bool incrementSpin = (lastSpinIncrementTime == INCREMENT_SPIN_AT_NEXT_UPDATE) ||
							(currTime - lastSpinIncrementTime) >= spinIncrementDuration;
	
	// if the lastSpinIncrementTime is the INCREMENT_SPIN_AT_NEXT_UPDATE flag, 
	// or if enough time has passed increment the spin offset
	if (incrementSpin) {
		if (isClockwiseSpin) {
			adjustSpinOffset(1);
		} else {
			adjustSpinOffset(-1);
		}
	}
}

/**
	Adjust the spin offset by amt
*/
void NeoPixelRing::adjustSpinOffset(int amt) {
	
	spinOffset += amt;
	// keep the spin offset within simple bounds (-maxIndex, maxIndex)
	if (abs(spinOffset) == size) {
		spinOffset = 0;
	}
	lastSpinIncrementTime = millis();
	
	// set flag that everything now needs to be updated
	updateAll = true;
}



void NeoPixelRing::randomize(int blinkDieRollSides, long maxBlinkDuration, int spinDieRollSides, long maxSpinIncrementDuration) {
	// set per-pixel values
	for (int i = 0; i < size; i++) {
		// random color
		setRedRingIndex		(i, random(101) / 100.0);
		setGreenRingIndex	(i, random(101) / 100.0);
		setBlueRingIndex	(i, random(101) / 100.0);	
		
		// random brightness
		setBrightnessPercentRingIndex(i, random(101) / 100.0);
		
		// random blink
		int blinkRoll = random(blinkDieRollSides);
		int isBlink = blinkRoll== 0;
		if (isBlink) {
			long blinkLength = random(maxBlinkDuration+1);
			blinkRingIndex(i, blinkLength);
		} else {
			stopBlinkRingIndex(i);
		}
	}
	
	// random spin
	int spinRoll = random(spinDieRollSides);
	int isSpin = spinRoll == 0;
	if (isSpin) {
		long spinIncrementDuration = random(maxSpinIncrementDuration+1);
		bool isClockwise = random(2) == 0;
		spin(spinIncrementDuration, isClockwise);
	} else {
		isSpinning = false;
	}
}

void NeoPixelRing::rainbow() {
	// hard-coded to 24 index ring - 
	// general solution to the rainbow color transition needed for other sizes
	if (size != 24) {
		return;
	}
	
	for (int ringIndex=0, colorIndex=0; ringIndex < size; ringIndex++, colorIndex+=3) {
		float r = rainbowColors[colorIndex]/100.0;;
		float g = rainbowColors[colorIndex+1]/100.0;
		float b = rainbowColors[colorIndex+2]/100.0;
		
		setRedRingIndex(ringIndex, r);
		setGreenRingIndex(ringIndex, g);
		setBlueRingIndex(ringIndex, b);
		
		setBrightnessPercentRingIndex(ringIndex,1);
		stopBlinkRingIndex(ringIndex);
	}
}

NeoPixel* NeoPixelRing::getPixelAtRingIndex(int index) {
	int startingIndexForRingIndex = getStartingIndexFromRingIndex(index);
	return pixels[startingIndexForRingIndex];
}

int NeoPixelRing::getRingIndexFromStartingIndex(int index) {
	return getWrappedIndex(index + spinOffset);
}

int NeoPixelRing::getStartingIndexFromRingIndex(int index) {
	return getWrappedIndex(index - spinOffset);
}

/**
	Accepts any index value and calculates the proper value by "wrapping" it 
	within the allowable bounds for the ring indices (0, size-1)
*/
int NeoPixelRing::getWrappedIndex(int index) {
	
	// accounts for negative values
	if (index < 0)
        index += size * ((-index) / size + 1);

    return index % size;
}
