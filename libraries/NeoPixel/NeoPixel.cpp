#include "NeoPixel.h"

void NeoPixel::update() {
}

bool NeoPixel::isOn() {
	return _isOn;
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
