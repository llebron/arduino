/**
  A guard for data hunt
*/

#include "GameIRTimer1.h"
#include "Switch.h"
#include "LedMatrix.h"
#include <Wire.h>//Needs to be included for LedMatrix to work
#include "ToneTimer2.h"

//maximum raygun energy
const int MAX_ENERGY = 64;

//how many sends to deplete one energy
const int SHOTS_PER_UNIT_ENERGY = 10;

//how many recharge pulses does it take to refill an energy unit?
const int RECHARGES_PER_UNIT_ENERGY = 10;

/*
  IR send attached to pin 9
  IR rec attached to pin 11
  uses timer1 to allow tone() to work
*/
GameIRTimer1 ir(11);
Switch sw(8, 30);//switch pin 10, debounce delay 30ms
LedMatrix mat;//the led matrix -- must be initialized and updated every loop
ToneTimer2 sound;//the speaker - non-blocking
int energy; //amt of energy
int numShots;//counts how many shots you've taken this energy unit
int numRecharges; //counts how many recharge pulses you've received

void setup() {
  Serial.begin(9600);
  mat.init();//initialize matrix
  sound.begin(6);//initialize speaker
  energy = MAX_ENERGY;
  numShots = 0;  
  numRecharges = 0;
}

void loop() {
  //update everything
  updateComponents();
  
  if (sw.isClosed()) {
    //energy is available
    if (energy > 0) fire();
  } else if (ir.checkMessages() && ir.getLastMessage() == IRGuardStation) {
      //recharge
      if (energy < MAX_ENERGY) recharge();
  } else {
      mat.setBrightness(MAX_MATRIX_BRIGHTNESS);
      mat.turnOn();
      sound.stop();
  }    
}

void updateComponents() {
  sw.update();//switch update
  mat.update();//matrix update
}

//handle firing the gun
void fire() {
  ir.send(IRRayGun);
  if (!mat.isBlinking())mat.blink(20);
  sound.play(440);//test tone - play an A on 440 - replace with a song
  numShots++;//increment shot counter
  
  //A unit of energy has been depleted
  if (numShots >= SHOTS_PER_UNIT_ENERGY) {
    energy--;
    mat.subFill();
    numShots = 0;//reset the count
  }
  if (energy == 0) sound.stop();
}

void recharge() {
  if (!mat.isFading())mat.fade(1, 15, 1000);
      sound.play(523);//test tone - play a C
      numRecharges++;
      
      if (numRecharges >= RECHARGES_PER_UNIT_ENERGY) {
        energy++;
        mat.addFill();
        numRecharges = 0;
      }
      if (energy == MAX_ENERGY) sound.stop();
}

