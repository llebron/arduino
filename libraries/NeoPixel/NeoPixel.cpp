#include "NeoPixel.h"

NeoPixel::NeoPixel(uint8_t arg_startingIndex) {
	startingIndex = arg_startingIndex;
}

void NeoPixel::setRedPercent(float r) {
	red = getColorValueForPercent(r);
}

void NeoPixel::setGreenPercent(float g) {
	green = getColorValueForPercent(g);
}

void NeoPixel::setBluePercent(float b) {
	blue = getColorValueForPercent(b);
}

uint8_t NeoPixel::getColorValueForPercent(float colorValPercent) {
	if (colorValPercent < 0) {
		return 0;
	} else if (colorValPercent > 1) {
		return MAX_COLOR_VAL;
	}
	return colorValPercent * MAX_COLOR_VAL;
}

bool NeoPixel::updateBlink(long currTime) {

	//how long as blink been in current state?
	long timePassed = currTime - blinkToggleTime;
	
	// if enough time has passed - toggle _isOn, set the blinkToggleTime,
	// and return true
	if (timePassed >= blinkLength) {
		_isBlinkOn = !_isBlinkOn;
		blinkToggleTime = currTime;
		return true;
	}
	return false;
}

// blink the pixel - it'll toggle between on/off after blinkLength (ms)
// DOES NOT alter blinkToggleTime so that blink rate can be fluidly increased/decreased without losing past state
// management of tracking which lights are blinking is handled in NeoPixelRing
void NeoPixel::blink(long arg_blinkLength) {
	blinkLength = arg_blinkLength;
}

// stop the blinking - sets _isBlinkOn to true, 
// so "absolute" on/off can be managed by NeoPixelRing
// management of tracking which lights are blinking is handled in NeoPixelRing
void NeoPixel::stopBlink() {
	_isBlinkOn = true;
}

bool NeoPixel::isBlinkOn() {
	return _isBlinkOn;
}

float NeoPixel::getBrightnessPercent() {
	return brightnessPercent;
}

void NeoPixel::setBrightnessPercent(float arg_brightnessPercent) {
	if (arg_brightnessPercent < 0) {
		arg_brightnessPercent = 0;
	} else if (arg_brightnessPercent > 1) {
		arg_brightnessPercent = 1;
	}
	brightnessPercent = arg_brightnessPercent;
}

uint8_t NeoPixel::getRed() {
	float redBrightness = roundf(red*brightnessPercent*BRIGHTNESS_SCALING_FACTOR);
	return static_cast <uint8_t> (redBrightness);
}


uint8_t NeoPixel::getGreen() {
	float greenBrightness = roundf(green*brightnessPercent*BRIGHTNESS_SCALING_FACTOR);
	return static_cast <uint8_t> (greenBrightness);
}

uint8_t NeoPixel::getBlue() {
	float blueBrightness = roundf(blue*brightnessPercent*BRIGHTNESS_SCALING_FACTOR);
	return static_cast <uint8_t> (blueBrightness);
}
