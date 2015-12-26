#include "Light.h"

Light::Light(int lightPin) {
	pin = lightPin;
	pinMode(lightPin, OUTPUT);
}

//just a wrapper for public use
void Light::turnOn(int brightness) {
	
	//cancel blinking & fading
	blinking = false;
	fading = false;
	turnOnPrivate(brightness);
}

//just a wrapper for public use
void Light::turnOff() {
	//cancel blinking & fading
	blinking = false;
	fading = false;
	turnOffPrivate();
}

void Light::turnOnPrivate(int brightness) {
	if (brightness >= MAX_PWM_VAL) {
		digitalWrite(pin, HIGH);
		on = true;
	} else if (brightness <= 0) {
		turnOffPrivate();
	//pwm-able pins on the uno
	} else if ( pin == 3 || pin == 5 || pin == 6 || pin == 9 || pin == 10 || pin == 11 ) {
		analogWrite(pin, brightness);
		on = true;
	}
}

void Light::turnOffPrivate() {
	digitalWrite(pin, LOW);
	on = false;
}

bool Light::isOn() {
	return on;
}

bool Light::isBlinking() {
	return blinking;
}

bool Light::isFading() {
	return fading;
}

void Light::blink(long length, int brightness) {
	unevenBlink(length, length, brightness);
}

void Light::unevenBlink(long onLength, long offLength, int brightness) {
	//initialize all blink tracking variables
	blinking = true;
	blinkOnLength = onLength;
	blinkOffLength = offLength;
	intensity = brightness;
		
	//start the blinking
	turnOnPrivate(intensity);
	blinkToggleTime = millis();
}

void Light::fade(int startBrightness, int endBrightness, long fadeDuration, FadeMode fm) {
	//Initialize the fade variables
	fading = true;
	fadeStartIntensity = startBrightness;
	fadeEndIntensity = endBrightness;
	fadeLength = fadeDuration;
	fadeMode = fm;
	intensity = startBrightness;
	fadeStartTime = millis();
	updateFade(fadeStartTime);//start the fade
}

void Light::updateFade(long currTime) {
	//how much time has passed since the start of the fade?
	long timePassed = currTime - fadeStartTime;
	
	//fade is still going -- set the appropriate intensity relative to the duration
	if (timePassed < fadeLength) {
		//how far into fade length are we?
		double fadePercent = (double) timePassed/fadeLength;
		
		// what is the overall fade difference
		int fadeDiff = fadeEndIntensity - fadeStartIntensity; 
		
		//how much intensity to add right now?
		double amtToAdd = fadePercent * fadeDiff; 
		
		//round and convert to int
		int convertedAmtToAdd = (amtToAdd > 0.0) ? (amtToAdd + .5) : (amtToAdd - .5);
		
		//add amt to start intensity	
		intensity = convertedAmtToAdd + fadeStartIntensity;
		
		//turn on the light at the new level, if not blinking
		if (!blinking) turnOnPrivate(intensity);
	} else {
		handleFadeEnd(currTime);
	}
}

void Light::handleFadeEnd(long currTime) {

	//loop back to start
	if (fadeMode == Loop) {
	
		//reset intensity to start
		intensity = fadeStartIntensity;
		//reset timer to current time
		fadeStartTime = currTime;
		
	} else if (fadeMode == PingPong) {
		//start the fade in the opposite direction
		fade(fadeEndIntensity, fadeStartIntensity, fadeLength, fadeMode);
	} else if (fadeMode == OnceThenOn) {
		stopFade();
	} else {
		turnOff();
	}
}

void Light::stopFade() {
	//turnOn(intensity);
	fading = false;
}

void Light::updateBlink(long currTime) {
	
	//how long as blink been in current state?
	long timePassed = currTime - blinkToggleTime;
		
	if (on && timePassed >= blinkOnLength) {//turn off and reset timer
		turnOffPrivate();
		blinkToggleTime = millis();
	} else if (!on && timePassed >= blinkOffLength) {//turn on and reset timer
		turnOnPrivate(intensity);
		blinkToggleTime = millis();
	}
}

void Light::update() {
	long currTime = millis();
	if(blinking) updateBlink(currTime);
	if(fading) updateFade(currTime);
}
