/**
Prison Slave Unit
Timer0: Tone
Timer1: Tone
Timer2: IR Leds
*/

#include "GameIRTimer2.h"
//#include "Tone.h" //need to modify tone library to remove all timer2 references

#include "Switch.h"

//Tone mainTone; //main speaker
//Tone hackTone; //hacking game tone

//IR Leds -- no receiver, so initialized to a bunk value
GameIRTimer2 ir(-1);
Switch sw(9, 30);

void setup() {
  Serial.begin(9600);
  //initialize the piezos
  //mainTone.begin(10);
  //hackTone.begin(12);
}

void loop() {
  sw.update();
  if (sw.isClosed()) {
    Serial.println("closed");
    ir.send(IRJailbreak);
  } else ir.send(IRPrison);
}
