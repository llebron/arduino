/**
SoundPlayerTimer1.h
Larry LeBron

A library for sound output
Uses the built-in Arduino tone functionality (uses timer1 and disables pwm on 3 & 11)
*/



#ifndef SOUND_PLAYER_TIMER_1_H
#define SOUND_PLAYER_TIMER_1_H

#include "Arduino.h"
#include "../GameSongs/GameSongs.h"

class SoundPlayerTimer1{
public:

	/**
		Play Modes -- accessible via SoundPlayer::modeName, i.e. SoundPlayer::Once
		Once: Play the song once and end
		Loop: Repeat the song until stopped
	*/
	enum PlayMode { Once, Loop };
	
	/*
		Initializes the SoundPlayer with the speaker pin
	*/
	SoundPlayerTimer1(int outputPin);
	
	/*
		Play the song at tempo -- tempo sets the length of one beat in ms
		Song is in the format of {Note, length, Note, length, ... END}
		Set playmode for Once or Loop
	*/
	void playSong(const float* song, float tempo, PlayMode pm=Loop);
	
	//returns true if currently playing a song
	bool playingSong();
	
	//stop playback and reset all parameters
	void stop();
	
	//update the SoundPlayer, must be called every loop for best accuracy
	void update();	
	
private:

	//Play the frequency for duration in ms
	void playNote(int frequency, int duration);
	
	int pin; //the speaker pin
	const float* currSong; //a pointer to the current song, if one is being played
	float currTempo; //the current tempo
	int currIndex; //the index of the next note to play
	long noteStartTime; //time when the most recent note started playing
	long currNoteDuration; //how long the current note should be played for
	PlayMode playMode; //what is the current play mode?
	bool playing_Song;
};

#endif
