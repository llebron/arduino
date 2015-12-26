#include "LedMatrix.h"

void LedMatrix::init() {
	matrix = Adafruit_8x8matrix();	
	matrix.begin(0x70);  // pass in the address
	matrix.fillScreen(LED_OFF); //start with screen off
	fillAmt = MAX_FILL_AMT; //start at max fill
	on = false;
	blinking = false;
	intensity = MAX_MATRIX_BRIGHTNESS;//initialize to max brightness
	fading = false;
	fadeMode = PingPong;
}


void LedMatrix::setBrightness(int brightness) {
	if (brightness == intensity) return; //if no change
	
	//cap it to the limits of the matrix
	if (brightness <= 0) {
		intensity = 0;
		matrix.clear();//can't set brightness to 0
		matrix.writeDisplay();
		return;
	} else if (brightness >= MAX_MATRIX_BRIGHTNESS) {
		intensity = MAX_MATRIX_BRIGHTNESS;
	} else intensity = brightness;
	matrix.setBrightness(intensity);
	matrix.writeDisplay();
}

void LedMatrix::setFill(int amt) {
	if (amt == fillAmt) return; //no change
	
	if (amt <= 0) {
		fillAmt = 0;
	} else if (amt >= MAX_FILL_AMT) {
		fillAmt = MAX_FILL_AMT;
	} else fillAmt = amt;
	
	if (on) turnOnPrivate(); //update the screen with the new fill amount
}

void LedMatrix::addFill(int amt) {
	setFill(fillAmt + amt);
}

void LedMatrix::subFill(int amt) {
	setFill(fillAmt - amt);
}

bool LedMatrix::isFull() {
	return fillAmt == MAX_FILL_AMT;
}

bool LedMatrix::isEmpty() {
	return fillAmt == 0;
}

//just a wrapper for public use
void LedMatrix::turnOn() {
	
	//cancel blinking & fading
	blinking = false;
	fading = false;
	turnOnPrivate();
}

//just a wrapper for public use
void LedMatrix::turnOff() {

	//cancel blinking & fading
	blinking = false;
	fading = false;
	turnOffPrivate();
}

void LedMatrix::turnOnPrivate() {
	on = true;
	matrix.clear();//start with a clean slate
	
	//don't bother drawing the pixels
	if (intensity == 0) {
		matrix.writeDisplay();
		return;
	}
	
	//update screen
	int x = 0;
	int y = 0;
	
	//update the leds being drawn
	for (int i = 0; i < fillAmt; i++) {
		if (x == WIDTH) {
			x = 0; //reached the end of a row
			y++;
		}
		matrix.drawPixel(x, y, LED_ON);
		x++;
	}
	matrix.writeDisplay();
}

void LedMatrix::turnOffPrivate() {
	matrix.clear();
	matrix.writeDisplay();
	on = false;
}

bool LedMatrix::isOn() {
	return on;
}

bool LedMatrix::isBlinking() {
	return blinking;
}

bool LedMatrix::isFading() {
	return fading;
}

void LedMatrix::blink(long length) {
	unevenBlink(length, length);
}

void LedMatrix::unevenBlink(long onLength, long offLength) {
	//initialize all blink tracking variables
	blinking = true;
	blinkOnLength = onLength;
	blinkOffLength = offLength;
		
	//start the blinking
	turnOnPrivate();
	blinkToggleTime = millis();
}

void LedMatrix::fade(int startBrightness, int endBrightness, long fadeDuration, FadeMode fm) {
	//Initialize the fade variables
	fading = true;
	
	//keep it within limits
	if (startBrightness <= 0) {
		fadeStartIntensity = 0;
	} else if (startBrightness >= MAX_MATRIX_BRIGHTNESS) {
		fadeStartIntensity = MAX_MATRIX_BRIGHTNESS;
	} else fadeStartIntensity = startBrightness;
	
	if (endBrightness <= 0) {
		fadeEndIntensity = 0;
	} else if (endBrightness >= MAX_MATRIX_BRIGHTNESS) {
		fadeEndIntensity = MAX_MATRIX_BRIGHTNESS;
	} else fadeEndIntensity = endBrightness;
	
	fadeLength = fadeDuration;
	fadeMode = fm;
	setBrightness(fadeStartIntensity);
	fadeStartTime = millis();
	updateFade(fadeStartTime);//start the fade
}

void LedMatrix::updateFade(long currTime) {
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
		int newIntensity = convertedAmtToAdd + fadeStartIntensity;
		
		setBrightness(newIntensity);
		
		//turn on the LedMatrix at the new level, if not blinking
		if (!blinking) turnOnPrivate();
	} else {
		handleFadeEnd(currTime);
	}
}

void LedMatrix::handleFadeEnd(long currTime) {

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

void LedMatrix::stopFade() {
	fading  = false;
}

void LedMatrix::updateBlink(long currTime) {
	
	//how long as blink been in current state?
	long timePassed = currTime - blinkToggleTime;
		
	if (on && timePassed >= blinkOnLength) {//turn off and reset timer
		turnOffPrivate();
		blinkToggleTime = millis();
	} else if (!on && timePassed >= blinkOffLength) {//turn on and reset timer
		turnOnPrivate();
		blinkToggleTime = millis();
	}
}


void LedMatrix::update() {
	long currTime = millis();
	if(blinking) updateBlink(currTime);
	if(fading) updateFade(currTime);
}
