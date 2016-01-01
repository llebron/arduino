#include "NeoPixel.h"

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

bool NeoPixel::isBlinkOn() {
	return _isBlinkOn;
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
	float redBrightness = roundf(red*brightnessPercent);
	return static_cast <uint8_t> (redBrightness);
}


uint8_t NeoPixel::getGreen() {
	float greenBrightness = roundf(green*brightnessPercent);
	return static_cast <uint8_t> (greenBrightness);
}

uint8_t NeoPixel::getBlue() {
	float blueBrightness = roundf(blue*brightnessPercent);
	return static_cast <uint8_t> (blueBrightness);
}
