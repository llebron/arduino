#include "NeoPixelRing.h"

NeoPixelRing::NeoPixelRing(uint16_t arg_size, uint8_t pin) {
	size = arg_size;
	maxIndex = size-1;
	ring = new Adafruit_NeoPixel(size, pin, NEO_GRB + NEO_KHZ800);
	ringIndexActiveStatus = new bool[size]; 
	pixels = new NeoPixel*[size];
	for (int i=0; i < size; i++) {
		ringIndexActiveStatus[i] = true;
		pixels[i] = new NeoPixel(i);
	}
	ring->begin();
}

NeoPixelRing::~NeoPixelRing() {
	delete[] ringIndexActiveStatus;
	for (int i=0; i < size; i++) {
		delete pixels[i];
	}
	delete[] pixels;
	delete ring;
}

void NeoPixelRing::demo() {
	/*for(int i=0;i<size;i++){

		// pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
		ring->setPixelColor(i, 0,75,0); // Moderately bright green color.

		ring->show(); // This sends the updated pixel color to the hardware.

		delay(500); // Delay for a period of time (in milliseconds).
	}*/
	randomize();
	//printRingIndexActive();
	update();
	//printRingIndexActive();
}

void NeoPixelRing::printRingIndexActive() {
	for (int i=0; i < size; i++) {
		Serial.print("ring active status for "); Serial.print(i); Serial.print(": "); Serial.println(ringIndexActiveStatus[i]);
	}
}


void NeoPixelRing::update() {
	long currTime = millis();
	bool isRefreshRing = false;
	bool spinOffsetChanged = false;
	// process the spin offset first, so values will be processed accurately
	if (isSpinning) {
		spinOffsetChanged = updateSpinOffset(currTime);
	}
	
	// process any lights that are actively blinking - if any lights toggle blink value
	// it'll update add its index to ringIndicesChangedSinceLastUpdate
	updateBlinkingPixels(currTime);
	
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
		 		uint16_t i = *it;
    			updateRingIndex(*it);
  			}
		 }
		 
		 // draw the changes
		 ring->show();
		 
		// clear the change tracking set
		ringIndicesChangedSinceLastUpdate.clear();
	}
}

void NeoPixelRing::updateBlinkingPixels(long currTime) {
	for (std::set<uint16_t>::iterator it=blinkingPixels.begin(); it!=blinkingPixels.end(); it++) {
    	uint16_t blinkingPixelStartingIndex = *it;
    	NeoPixel* blinkingPixel = pixels[blinkingPixelStartingIndex];
    	bool pixelBlinked = blinkingPixel->updateBlink(currTime);
    	
    	// if the pixel blinks this update, AND the ringIndex is active, add it to the tracking set
    	if (pixelBlinked) {
    		uint16_t ringIndexForPixel = getRingIndexFromStartingIndex(blinkingPixelStartingIndex);
    		if (ringIndexActiveStatus[ringIndexForPixel]) {
    			ringIndicesChangedSinceLastUpdate.insert(ringIndexForPixel);
    		}
    	}
  	}
}

void NeoPixelRing::updateRingIndex(uint16_t ringIndex) {	
	NeoPixel* pixel = getPixelAtRingIndex(ringIndex);
	
	// Is the light on  both in absolute terms and relative to its blink cycle?
	bool isOn = ringIndexActiveStatus[ringIndex] && pixel->isBlinkOn();
	
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

void NeoPixelRing::turnOnRingIndex(uint16_t index) {
	// If already on, just return
	if (ringIndexActiveStatus[index]) {
		return;
	}
	ringIndexActiveStatus[index] = true;
	ringIndicesChangedSinceLastUpdate.insert(index);
}

void NeoPixelRing::turnOnRingIndices(std::set<uint16_t> indices) {
	for (std::set<uint16_t>::iterator it=indices.begin(); it!=indices.end(); it++) {
		turnOnRingIndex(*it);
	}
}

void NeoPixelRing::turnOffRingIndex(uint16_t index) {
	// If already off, just return
	if (!ringIndexActiveStatus[index]) {
		return;
	}
	ringIndexActiveStatus[index] = false;
	ringIndicesChangedSinceLastUpdate.insert(index);
}
void NeoPixelRing::turnOffRingIndices(std::set<uint16_t> indices) {
	for (std::set<uint16_t>::iterator it=indices.begin(); it!=indices.end(); it++) {
		turnOffRingIndex(*it);
	}
}

void NeoPixelRing::setRedRingIndex(uint16_t index, uint8_t red) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setRed(red);	
	ringIndicesChangedSinceLastUpdate.insert(index);
}
void NeoPixelRing::setRedRingIndices(std::set<uint16_t> indices, uint8_t red) {
	for (std::set<uint16_t>::iterator it=indices.begin(); it!=indices.end(); it++) {
		setRedRingIndex(*it, red);
	}
}

void NeoPixelRing::setGreenRingIndex(uint16_t index, uint8_t green) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setGreen(green);	
	ringIndicesChangedSinceLastUpdate.insert(index);
}
void NeoPixelRing::setGreenRingIndices(std::set<uint16_t> indices, uint8_t green) {
	for (std::set<uint16_t>::iterator it=indices.begin(); it!=indices.end(); it++) {
		setGreenRingIndex(*it, green);
	}
}

void NeoPixelRing::setBlueRingIndex(uint16_t index, uint8_t blue) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setBlue(blue);	
	ringIndicesChangedSinceLastUpdate.insert(index);
}
void NeoPixelRing::setBlueRingIndices(std::set<uint16_t> indices, uint8_t blue) {
	for (std::set<uint16_t>::iterator it=indices.begin(); it!=indices.end(); it++) {
		setBlueRingIndex(*it, blue);
	}
}

void NeoPixelRing::setBrightnessPercentRingIndex(uint16_t index, float brightnessPercent) {
	NeoPixel* pixel = getPixelAtRingIndex(index);
	pixel->setBrightnessPercent(brightnessPercent);
	ringIndicesChangedSinceLastUpdate.insert(index);
}
void NeoPixelRing::setBrightnessPercentRingIndices(std::set<uint16_t> indices, float brightnessPercent) {
	for (std::set<uint16_t>::iterator it=indices.begin(); it!=indices.end(); it++) {
		setBrightnessPercentRingIndex(*it, brightnessPercent);
	}
}

void NeoPixelRing::blinkRingIndex(uint16_t index, long blinkLength) {
	uint16_t startingIndexForRingIndex = getStartingIndexFromRingIndex(index);
	NeoPixel* pixel = pixels[startingIndexForRingIndex];
	pixel->blink(blinkLength);	
	blinkingPixels.insert(startingIndexForRingIndex);
}
void NeoPixelRing::blinkRingIndices(std::set<uint16_t> indices, long blinkLength) {
	for (std::set<uint16_t>::iterator it=indices.begin(); it!=indices.end(); it++) {
		blinkRingIndex(*it, blinkLength);
	}
}

void NeoPixelRing::stopBlinkRingIndex(uint16_t index) {
	uint16_t startingIndexForRingIndex = getStartingIndexFromRingIndex(index);
	NeoPixel* pixel = pixels[startingIndexForRingIndex];
	pixel->stopBlink();	
	blinkingPixels.erase(startingIndexForRingIndex);
}
void NeoPixelRing::stopBlinkRingIndices(std::set<uint16_t> indices) {
	for (std::set<uint16_t>::iterator it=indices.begin(); it!=indices.end(); it++) {
		stopBlinkRingIndex(*it);
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

void NeoPixelRing::randomize() {
	// set per-pixel values
	for (uint16_t i = 0; i < size; i++) {
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

NeoPixel* NeoPixelRing::getPixelAtRingIndex(uint16_t index) {
	uint16_t startingIndexForRingIndex = getStartingIndexFromRingIndex(index);
	return pixels[startingIndexForRingIndex];
}

uint16_t NeoPixelRing::getRingIndexFromStartingIndex(uint16_t index) {
	return getWrappedIndex(index + spinOffset);
}

uint16_t NeoPixelRing::getStartingIndexFromRingIndex(uint16_t index) {
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
