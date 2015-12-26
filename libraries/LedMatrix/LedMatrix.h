/**
LedMatrix.h
Larry LeBron
larrylebron@gmail.com
11/26/2012

A wrapper for Adafruit's 8x8 LED matrix with specific functionality for data thieves
Matrix is connected via an I2C backpack and wired to analog pins 4 and 5
*/



#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "Arduino.h"
#include "Adafruit_LEDBackpack.h"

const int MAX_MATRIX_BRIGHTNESS = 15;
const int MAX_FILL_AMT = 64; //number of leds on the screen
const int WIDTH = 8;
const int HEIGHT = 8;

class LedMatrix {
public:
	
	/**
		fade modes -- accessible via LedMatrix::modeName, i.e. LedMatrix::Once
		OnceThenOn: do the fade and stay on at end intensity
		OnceThenOff: do the fade and turn off
		Loop: Repeat the fade until stopped
		PingPong: Fade up and down repeatedly
	*/
	enum FadeMode { OnceThenOn, OnceThenOff, Loop, PingPong };
	
	//MUST CALL TO INITIALIZE MATRIX!!!
	void init();
	
	//update the matrix, must be called every loop for best accuracy with fade and blink
	void update();	
	
	/**
		set the brightness of the matrix (from 0 - 15)
		0 will clear the screen though it will still be technically on and treated as such
	*/	
	void setBrightness(int brightness);
	
	/**
		set the number of leds to activate
		fills in rows, starting from the top left
		range is 0 - 64
		Won't turn on the screen if it's off
	*/
	void setFill(int amt);
	
	//add or subtract fill from the matrix -- defaults to add/sub 1
	void addFill(int amt=1);
	void subFill(int amt=1);
	
	//returns true if screen is full/empty
	bool isFull();
	bool isEmpty();
	
	/* 	Turns the matrix on to its current brightness and fill level
		Cancels prior blinking & fading
	*/
	void turnOn();
	
	/*
		turns the LedMatrix off - does not affect brightness or fill level
		cancels blinking & fading
	*/
	void turnOff();
		
	//returns true if the LedMatrix is on
	bool isOn();
	
	//returns true if blinking
	bool isBlinking();
	
	//returns true if fading
	bool isFading();
	
	
	
	/*
		blink the LedMatrix at current fill level and brightness
		This is non-blocking, though probably not perfectly precise
		length: how long it should blink (on/off time is equal)
	*/
	void blink(long length);
	
	/** 
		blink the LedMatrix unevenly -- non-blocking, though not perfectly precise
		onLength: how long it should stay on (in milliseconds)
		offLength: how long it should stay off (in milliseconds)
		Blinks at current fill level and brightness
	*/
	void unevenBlink(long onLength, long offLength);
	
	/**
		Fade LedMatrix evenly from startBrightness to endBrightness over the course of fadeDuration (milliseconds)
		Brightness val must be between 0-15 (anything over/under will be capped) -- 0 turns off the screen and isn't super smooth looking
		Non-blocking, so not perfectly accurate or smooth
		**Will work in conjunction with blinking**
		FadeMode dictates the function. Choose from Once, Loop, or PingPong (defaults to pingPong)
	*/
	void fade(int startBrightness, int endBrightness, long fadeDuration, FadeMode fm=PingPong);
	
	//stop the fade, and remain at the current brightness
	void stopFade();
	
private:
	/*
		turn LedMatrix on/off -- description in public functions above
		brightness: how bright from 0-15, defaults to current brightness
	*/
	void turnOnPrivate();
	void turnOffPrivate(); 
	
	//update the blink status
	void updateBlink(long currTime);
	
	//update the fade status
	void updateFade(long currTime);
	
	//handles the end of the fade when its time is up
	void handleFadeEnd(long currTime);
	
	Adafruit_8x8matrix matrix;
	int fillAmt; //how many leds should turn on -- fills from top left, row by row
	bool on; //is the LedMatrix on?
	
	//blink tracking
	bool blinking; //is the LedMatrix blinking?
	long blinkToggleTime; //how long has blink been in its current state
	int intensity; //how bright is the LedMatrix?
	long blinkOnLength; //how long LedMatrix should stay on in millis
	long blinkOffLength; //how long LedMatrix should stay off in millis
	
	//fade tracking
	bool fading; //is the LedMatrix fading?
	int fadeStartIntensity; //the fade start intensity
	int fadeEndIntensity; //the fade end intensity
	long fadeLength; //the fade length
	long fadeStartTime; //the last time the fade was updated
	FadeMode fadeMode;//the fadeMode
};

#endif
