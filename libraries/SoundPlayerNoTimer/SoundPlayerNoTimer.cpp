#include "SoundPlayerNoTimer.h"

SoundPlayerNoTimer::SoundPlayerNoTimer(int outputPin) {
	pin = outputPin;
	currSong = 0;
	currTempo = 0;
	currIndex = 0;
	noteStartTime = 0;
	currNoteDuration = 0;
	playing_Song = false;
	playMode = Loop;//default to loop
}

void SoundPlayerNoTimer::playNote(int frequency, int duration) {
	noteStartTime = millis(); //update the note start time
	freqout(frequency, duration);
}

void SoundPlayerNoTimer::playSong(const float* song, float tempo, PlayMode pm) {
	playing_Song = true;
	currSong = song;
	currIndex = 0;
	currTempo = tempo;
	playMode = pm;
	
	//play first note & initialize variables
	currNoteDuration = currTempo * currSong[currIndex+1];
	playNote(currSong[currIndex], currNoteDuration);
}

void SoundPlayerNoTimer::stop() {
	playing_Song = false;
	currTempo = 0;
	currSong = 0;
	currIndex = 0;
	currNoteDuration = 0;
	noteStartTime = 0;
}

bool SoundPlayerNoTimer::playingSong() {
	return playing_Song;
}

void SoundPlayerNoTimer::setTempo(int tempo) {
	if(playing_Song) currTempo = tempo;
}

void SoundPlayerNoTimer::update() {
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

//Paul Badger's Algorithm
void SoundPlayerNoTimer::freqout(int freq, int t)  // freq in hz, t in ms
{
  int hperiod;                               //calculate 1/2 period in us
  long cycles, i;
  pinMode(pin, OUTPUT);

  hperiod = (500000 / freq) - 7;             // subtract 7 us to make up for digitalWrite overhead

  cycles = ((long)freq * (long)t) / 1000;    // calculate cycles
 // Serial.print(freq);
 // Serial.print((char)9);                   // ascii 9 is tab - you have to coerce it to a char to work 
 // Serial.print(hperiod);
 // Serial.print((char)9);
 // Serial.println(cycles);

  for (i=0; i<= cycles; i++){              // play note for t ms 
    digitalWrite(pin, HIGH); 
    delayMicroseconds(hperiod);
    digitalWrite(pin, LOW); 
    delayMicroseconds(hperiod - 1);     // - 1 to make up for digitaWrite overhead
  }
  
  pinMode(pin, INPUT); //deactivate the pin to prevent random noise

}