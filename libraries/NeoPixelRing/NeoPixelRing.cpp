#include "NeoPixelRing.h"

NeoPixelRing::NeoPixelRing(int arg_size, uint8_t arg_numLightsPerCluster, uint8_t pin) {
	size = arg_size;
	numLightsPerCluster = arg_numLightsPerCluster;
	maxIndex = size-1;
	ring = new Adafruit_NeoPixel(size, pin, NEO_GRB + NEO_KHZ800);
	isRingIndicesChangedSinceLastUpdate = false;
	ringIndexActiveStatus = new bool[size]; 
	ringIndicesChangedSinceLastUpdate = new bool[size];
	pixels = new NeoPixel*[size];
	for (int i=0; i < size; i++) {
		ringIndexActiveStatus[i] = false; // will be set properly by the current state of the switches
		ringIndicesChangedSinceLastUpdate[i] = false;
		pixels[i] = new NeoPixel(i);
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
		Serial.print("ring active status for "); Serial.print(i); Serial.print(": "); Serial.println(ringIndexActiveStatus[i]);
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

void NeoPixelRing::flagRingIndexChangedSinceLastUpdate(int ringIndex) {
	ringIndicesChangedSinceLastUpdate[ringIndex] = true;
	/* set the global tracking flag true */
	isRingIndicesChangedSinceLastUpdate = true;
}

void NeoPixelRing::clearRingIndexUpdateChangeTracking() {
	for (int i=0; i < size; i++) {
		ringIndicesChangedSinceLastUpdate[i] = false;
	}
	isRingIndicesChangedSinceLastUpdate = false;
}

void NeoPixelRing::updateBlinkingPixels(long currTime) {
	for (std::set<int>::iterator it=blinkingPixels.begin(); it!=blinkingPixels.end(); it++) {
    	int blinkingPixelStartingIndex = *it;
    	NeoPixel* blinkingPixel = pixels[blinkingPixelStartingIndex];
    	bool pixelBlinked = blinkingPixel->updateBlink(currTime);
    	
    	// if the pixel blinks this update, AND the ringIndex is active, add it to the tracking set
    	if (pixelBlinked) {
    		int ringIndexForPixel = getRingIndexFromStartingIndex(blinkingPixelStartingIndex);
    		if (ringIndexActiveStatus[ringIndexForPixel]) {
    			flagRingIndexChangedSinceLastUpdate(ringIndexForPixel);
    		}
    	}
  	}
}

void NeoPixelRing::updateRingIndex(int ringIndex) {	
	NeoPixel* pixel = getPixelAtRingIndex(ringIndex);
	
	// Is the light on  both in absolute terms and relative to its blink cycle?
	bool isOn = ringIndexActiveStatus[ringIndex] && pixel->isBlinkOn();
	Serial.print("Update ring index "); Serial.print(ringIndex);Serial.print(" ");Serial.println(isOn);
	
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
	Serial.print("ring index on "); Serial.println(index);
	// If already on, just return
	if (ringIndexActiveStatus[index]) {
		return;
	}
	ringIndexActiveStatus[index] = true;
	flagRingIndexChangedSinceLastUpdate(index);
}

void NeoPixelRing::turnOnLightCluster(int indices[]) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		turnOnRingIndex(indices[i]);
	}
}

void NeoPixelRing::turnOffRingIndex(int index) {
	Serial.print("ring index off "); Serial.println(index);
	// If already off, just return
	if (!ringIndexActiveStatus[index]) {
		return;
	}
	ringIndexActiveStatus[index] = false;
	flagRingIndexChangedSinceLastUpdate(index);
}

void NeoPixelRing::turnOffLightCluster(int indices[]) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		turnOffRingIndex(indices[i]);
	}
}

void NeoPixelRing::setRedRingIndex(int index, uint8_t red) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setRed(red);	
	flagRingIndexChangedSinceLastUpdate(index);
}
void NeoPixelRing::setRedLightCluster(int indices[], uint8_t red) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		setRedRingIndex(indices[i], red);
	}
}

void NeoPixelRing::setGreenRingIndex(int index, uint8_t green) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setGreen(green);	
	flagRingIndexChangedSinceLastUpdate(index);
}
void NeoPixelRing::setGreenLightCluster(int indices[], uint8_t green) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		setGreenRingIndex(indices[i], green);
	}
}

void NeoPixelRing::setBlueRingIndex(int index, uint8_t blue) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setBlue(blue);	
	flagRingIndexChangedSinceLastUpdate(index);
}
void NeoPixelRing::setBlueLightCluster(int indices[], uint8_t blue) {
	for (int i = 0; i < numLightsPerCluster; i++) {
		setBlueRingIndex(indices[i], blue);
	}
}

void NeoPixelRing::setBrightnessPercentRingIndex(int index, float brightnessPercent) {
	//Serial.print("before set brightness for: "); Serial.println(index);
	NeoPixel* pixel = getPixelAtRingIndex(index);
	//Serial.print("pixel is "); Serial.print(!pixel);Serial.print(" bright: ");
	//	Serial.println(pixel->getBrightnessPercent());
	pixel->setBrightnessPercent(brightnessPercent);
	flagRingIndexChangedSinceLastUpdate(index);
	//Serial.print("after set brightness for: "); Serial.println(index);
	/*Serial.print("pixel is "); Serial.print(!pixel);Serial.print(" bright: ");
		Serial.println(pixel->getBrightnessPercent());
		Serial.println();*/
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
	blinkingPixels.insert(startingIndexForRingIndex);
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
	blinkingPixels.erase(startingIndexForRingIndex);
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



void NeoPixelRing::randomize() {
	// set per-pixel values
	for (int i = 0; i < size; i++) {
		// random color
		uint8_t r = random(MAX_COLOR_VAL+1);
		uint8_t g = random(MAX_COLOR_VAL+1);
		uint8_t b = random(MAX_COLOR_VAL+1);
		setRedRingIndex(i, r);
		setGreenRingIndex(i, g);
		setBlueRingIndex(i, b);		
		
		// random brightness
		float brightnessPercent = random(101) / 100.0;
		setBrightnessPercentRingIndex(i, brightnessPercent);
		
		// random blink
		int blinkChance = random(1, 6);
		int isBlink = random(blinkChance) == 0;
		if (isBlink) {
			long blinkLength = random(MAX_RANDOM_BLINK_LENGTH+1);
			blinkRingIndex(i, blinkLength);
		} else {
			stopBlinkRingIndex(i);
		}
	}
	
	// random spin
	int isSpin = random(2) == 0;
	if (isSpin) {
		long spinIncrementDuration = random(MAX_RANDOM_SPIN_INCREMENT_DURATION+1);
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
	
	for (int i=0; i < size; i++) {
		uint8_t r = rainbowColors[i*3];
		uint8_t g = rainbowColors[i*3+1];
		uint8_t b = rainbowColors[i*3+2];
		
		setRedRingIndex(i, r);
		setGreenRingIndex(i, g);
		setBlueRingIndex(i, b);
		
		setBrightnessPercentRingIndex(i,1);
		stopBlinkRingIndex(i);
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
