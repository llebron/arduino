#include "SoundPlayerTimer1.h"

SoundPlayerTimer1::SoundPlayerTimer1(int outputPin) {
	pin = outputPin;
	currSong = 0;
	currTempo = 0;
	currIndex = 0;
	noteStartTime = 0;
	currNoteDuration = 0;
	playing_Song = false;
	playMode = Loop;//default to loop
}

void SoundPlayerTimer1::playNote(int frequency, int duration) {
	noteStartTime = millis(); //update the note start time
	tone(pin, frequency, duration);
}

void SoundPlayerTimer1::playSong(const float* song, float tempo, PlayMode pm) {
	playing_Song = true;
	currSong = song;
	currIndex = 0;
	currTempo = tempo;
	playMode = pm;
	
	//play first note & initialize variables
	currNoteDuration = currTempo * currSong[currIndex+1];
	playNote(currSong[currIndex], currNoteDuration);
}

void SoundPlayerTimer1::stop() {
	playing_Song = false;
	currTempo = 0;
	currSong = 0;
	currIndex = 0;
	currNoteDuration = 0;
	noteStartTime = 0;
}

bool SoundPlayerTimer1::playingSong() {
	return playing_Song;
}

void SoundPlayerTimer1::update() {
	//not playing, or not time for next note yet
	if (!playing_Song || millis() - noteStartTime < currNoteDuration) return;
	
	//go to next note
	currIndex += 2;
	
	//End of song?
	if (currSong[currIndex] == END) {
		if (playMode == Loop) {
			currIndex = 0;//back to start
		} else {
			stop();
			return; //break out without playing			
		}
	}
	//set current note duration
	currNoteDuration = currTempo * currSong[currIndex+1];
	//play the note if it's not a rest
	if (!currSong[currIndex] == REST) playNote(currSong[currIndex], currNoteDuration);
}