/**
SoundPlayerNoTimer.h
Larry LeBron

Uses Paul Badger's Freqout function for output -- blocks the main loop during note durations
http://arduino.cc/playground/Main/Freqout

Will continue with main loop during rests and after note durations are completed
Uses an update function, so may not be perfectly accurate if your loop is doing a lot of processing

*/

#ifndef SOUND_PLAYER_NO_TIMER_H
#define SOUND_PLAYER_NO_TIMER_H

#include "Arduino.h"
#include "../GameSongs/GameSongs.h"

class SoundPlayerNoTimer {
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
	SoundPlayerNoTimer(int outputPin);
	
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
	
	//set the current tempo
	void setTempo(int tempo);
	
private:

	//Play the frequency for duration in ms
	void playNote(int frequency, int duration);
	
	//Play frequency freq for t ms - blocks the loop during the duration
	void freqout(int freq, int t);
	
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
