#include "LedMatrix.h"
#include <Wire.h>
#include "SoundPlayerNoTimer.h"
#include "Accelerometer.h"
#include "RFSend.h" //timer 1
#include "GameIRTimer2.h" //timer 2
#include "LinearHallSensor.h"
#include "Switch.h"
#include "Light.h"

//!! This thief's id -- must be changed for each thief !!
const int ID = 0;

//thief is fully visible
const int MAX_VISIBILITY = 64;

//how many shots to gain a unit of visibility
const int ATTACKS_PER_UNIT_VISIBILITY =1;

//the highest intensity that can be considered "still"
const int MAX_STILL_MOVEMENT_INTENSITY = 50;

//how many rounds do you need to be still to gain a unit of energy back?
const int STILL_ROUNDS_PER_RECHARGE = 50;

//how much movement gains a movement unit?
const int MOVEMENT_INTENSITY_RATIO = 15;

//how much movement gains a visibility?
const int MOVEMENT_PER_UNIT_VISIBILITY = 15;

const int TEMPO_VISIBILITY_NUMERATOR = 1000;

const int MIN_SOUND_VISIBILITY = 5;

//30 ms delay to allow attack song to keep playing
const long UNDER_ATTACK_WAIT_TIME = 100;

//the highest visibility that'll allow terminal scanning
const int MAX_SCAN_VISIBILITY = 2;

//x, y, z
Accelerometer accel(A3, A2, A1);

//IR Receiver
GameIRTimer2 ir(8);

//sound player -- blocks the loop during note durations
SoundPlayerNoTimer sound(2);

//pin 4, thief id
RFSend rf(4, ID);

//switch on pin 6, debounce 30 ms
Switch sw(6, 30); 

//Single wrist LED
Light lt(5);

//the led matrix -- shows visibility
LedMatrix mat;

//hall sensor on A0
LinearHallSensor hall(A0);

//how visible is the thief?
int visibility;

enum ThiefState {
  Hunting, WithData, InPrison, GameWon};

ThiefState state;

//how many attacks have you received?
int numAttacks;

//how many rounds have you been still consecutively?
int stillRounds;

//the last movement intensity
int lastMvmtIntensity;

//How much are you moving?
int mvmtAmt;

//when were you last attacked?
long lastAttackTime;

//are you under attack?
bool underAttack;

void setup() {
  Serial.begin(9600);
  rf.init();
  mat.init();
  state = Hunting;
  numAttacks = 0;
  visibility = 0;
  mvmtAmt = 0;
  stillRounds = 0;
  lastMvmtIntensity = 0;
  lastAttackTime = 0;
  underAttack = false;
  mat.turnOn(); //turn on the matrix  
  accel.calibrate();//calibrate the accelerometer at startup
}

void loop() {
  updateComponents();

  if (state == Hunting) {
    handleHunting();
  } 
  else if (state == WithData) {
    handleWithData();
  } 
  else if (state == GameWon) {
    //yippee!
    mat.setFill(MAX_VISIBILITY);
    if (!mat.isBlinking()) mat.blink(50);
    if (!mat.isFading()) mat.fade(1, 15, 500);
  } else if (state == InPrison) {
    handleInPrison();
  }  
  mat.setFill(visibility);
  if (underAttack && millis() - lastAttackTime >= UNDER_ATTACK_WAIT_TIME) {
    underAttack = false;
    sound.stop(); //stop playing the attack sound
    mat.turnOn();
  }

  if (!underAttack && state != InPrison) {
   // Serial.println("not under");
    if (!sound.playingSong() && visibility >= MIN_SOUND_VISIBILITY) sound.playSong(thiefMoving, TEMPO_VISIBILITY_NUMERATOR/visibility);
    if (visibility < MIN_SOUND_VISIBILITY) sound.stop();
    else sound.setTempo(TEMPO_VISIBILITY_NUMERATOR / visibility );
  }
}

//update all the sensors, etc.
void updateComponents() {
  //don't need to update these if in prison
  if (state != InPrison && state != GameWon) {
    accel.update();
    //hall.update();    
    lt.update();
    if (state == Hunting) {
      sw.update();
    }
  }
  sound.update();
  mat.update();

}

//handle the thief in hunting state
void handleHunting() {
  //are you under attack?
  if (ir.checkMessages() && ir.getLastMessage() == IRRayGun) {
    handleAttack();
  } else mat.turnOn();
  
  //handle visibility relative to movement
  handleMovement();
  
  //are you at the terminal or the prison?
  //handleHallSensor();
}

void handleAttack() {
  //just came under attack
  if (!underAttack) {
    sound.playSong(thiefAttacked, 200);
    mat.blink(10);
  }
  underAttack = true;
  lastAttackTime = millis();
  
  numAttacks++;
  if (numAttacks >= ATTACKS_PER_UNIT_VISIBILITY) {

    //You're done for!
    if (visibility == MAX_VISIBILITY) {
      if (state == WithData) {
        rf.send(RFCapturedWithData);
        lt.turnOff(); //no longer have the data
      }
      state = InPrison;
      sound.stop();
      return;
    }
    visibility++;
    numAttacks = 0;//reset counter
  }
}

//handle visibility affected by movement
void handleMovement() {
  
  //how much are you moving?
  int mvmt = accel.getMovementIntensity();
  /*
  Serial.print("movement: ");
  Serial.println(mvmt);
  Serial.println(abs(mvmt - lastMvmtIntensity));*/
  //Is this round 
  if ( abs(mvmt - lastMvmtIntensity) >= MAX_STILL_MOVEMENT_INTENSITY) {
    stillRounds = 0; //
    //add to mvmtAmt according to ratio
    mvmtAmt += mvmt/MOVEMENT_INTENSITY_RATIO;
    
    //you've accumulated enough movement
    if (mvmtAmt >= MOVEMENT_PER_UNIT_VISIBILITY) {
      if (visibility < MAX_VISIBILITY) visibility++;
      mvmtAmt = 0;
    }
  } else { //you're standing still!
    mvmtAmt = 0;
    stillRounds++;
    if (stillRounds >= STILL_ROUNDS_PER_RECHARGE) {
      if (visibility > 0) visibility--;
      stillRounds = 0;
    }
    if (visibility < MAX_SCAN_VISIBILITY && sw.closedThisUpdate()) {
      //you're still and invisibile, so you can scan
      rf.send(RFThiefScan, 5);
      visibility = MAX_VISIBILITY;
    }
  }
  lastMvmtIntensity = mvmt; 
}

void handleHallSensor() {
  if (hall.atNorth()) rf.send(RFJailbreak, 20);//at the prison -- jailbreak!
  
  if (state != WithData && hall.atSouth()) {
    rf.send(RFGotData);
    state = WithData; //at the data -- got it!
  }
}

//the thief has the data!
void handleWithData() {
    
  //data indicator
  if ( !lt.isFading() ) lt.fade(1, 255, 500); 
  
  long irMsg;
  if ( ir.checkMessages() ) {
    irMsg = ir.getLastMessage();
    if (irMsg == IRRayGun) {
      handleAttack();
    } else if (irMsg == IRAirlock) {
      rf.send(RFThievesWin);
      state = GameWon; //you made it!
    }
  } else mat.turnOn();
  
  //handle visibility relative to movement
  handleMovement();
  
  //are you at the prison? Wanna save your buddies?
  //handleHallSensor();
}

void handleInPrison() {
  //free yet?
  
  if ( ir.checkMessages() && ir.getLastMessage() == IRJailbreak) {
    state = Hunting;
  }
  if(sound.playingSong()) sound.stop();
  if(mat.isBlinking()) mat.turnOn();
  if (!mat.isFading()) mat.fade(1, 15, 5000);
}

