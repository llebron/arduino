/**
Light.h
Larry LeBron

A simple library for a light connected between a digital pin and ground

*/

#ifndef LIGHT_H
#define LIGHT_H

#include "Arduino.h"

//max pwm value
const int MAX_PWM_VAL = 255;

class Light {
public:

	/**
		fade modes -- accessible via LIGHT::modeName, i.e. Light::Once
		OnceThenOn: do the fade and stay on at end intensity
		OnceThenOff: do the fade and turn off
		Loop: Repeat the fade until stopped
		PingPong: Fade up and down repeatedly
	*/
	enum FadeMode { OnceThenOn, OnceThenOff, Loop, PingPong };

	//Initializes the light
	Light(int lightPin);
	
	//update the light, must be called every loop for best accuracy with fade and blink
	void update();	
	
	/* 	Turns the light on via digitalWrite. 
		Cancels prior blinking & fading
		If on a pwm pin, can enter an brightness between 0 - 255 (defaults to HIGH) 
		If a brightness is entered on a non-pwm pin, will not function
	*/
	void turnOn(int brightness = MAX_PWM_VAL);
	
	/*
		turns the light off
		cancels blinking & fading
	*/
	void turnOff();
		
	//returns true if the light is on
	bool isOn();
	
	//returns true if blinking
	bool isBlinking();
	
	//returns true if fading
	bool isFading();
	
	/*
		blink the light -- non-blocking, though probably not perfectly precise
		length: how long it should blink (on/off time is equal)
		brightness: how bright, from 0-255(only on pwm pins)
	*/
	void blink(long length, int brightness = MAX_PWM_VAL);
	
	/** 
		blink the light unevenly -- non-blocking, though not perfectly precise
		onLength: how long it should stay on (in milliseconds)
		offLength: how long it should stay off (in milliseconds)
		brightness: how bright, from 0-255 (only on pwm pins)
	*/
	void unevenBlink(long onLength, long offLength, int brightness=MAX_PWM_VAL);
	
	/**
		Fade light evenly from startBrightness to endBrightness over the course of fadeDuration (milliseconds)
		Only works on pwm pins, val must be between 0-255 (anything over/under will be capped)
		Non-blocking, so not perfectly accurate or smooth
		**Will work in conjunction with blinking**
		FadeMode dictates the function. Choose from Once, Loop, or PingPong (defaults to pingPong)
	*/
	void fade(int startBrightness, int endBrightness, long fadeDuration, FadeMode fm=PingPong);
	
	//stop the fade, and remain at the current brightness
	void stopFade();
	
	
private:

	/*
		turn light on/off -- description in public functions above
		brightness: how bright from 0-255 on pwm pins
	*/
	void turnOnPrivate(int brightness = MAX_PWM_VAL);
	void turnOffPrivate(); 
	
	//update the blink status
	void updateBlink(long currTime);
	
	//update the fade status
	void updateFade(long currTime);
	
	//handles the end of the fade when its time is up
	void handleFadeEnd(long currTime);
	
	int pin; //the light pin
	bool on; //is the light on?
	
	//blink tracking
	bool blinking; //is the light blinking?
	long blinkToggleTime; //how long has blink been in its current state
	int intensity; //how bright is the light?
	long blinkOnLength; //how long light should stay on in millis
	long blinkOffLength; //how long light should stay off in millis
	
	//fade tracking
	bool fading; //is the light fading?
	int fadeStartIntensity; //the fade start intensity
	int fadeEndIntensity; //the fade end intensity
	long fadeLength; //the fade length
	long fadeStartTime; //the last time the fade was updated
	FadeMode fadeMode;//the fadeMode
	
};

#endif
