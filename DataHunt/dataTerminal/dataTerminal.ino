#include "RFRec.h"
#include "ToneTimer2.h";
#include "Light.h"

//The rf receiver on 7
RFRec rf(7);

//the lights
Light blue(6);
Light green(12);
Light yellow(5);
Light red (9);

ToneTimer2 sound;

const int MAX_VISIBILITY = 10;

//stores the last message received
uint8_t lastMsg[BYTES_PER_MESSAGE] = {-1, -1, -1};

//how visible?
int visibility;



void setup() {
  Serial.begin(9600);
  rf.init();
  sound.begin(8);
  visibility = 4;
}

void loop() {
  red.update();
  blue.update();
  green.update();
  yellow.update();
  
  //check for transmissions
  handleRF();
  
  

}

void handleRF() {
  uint8_t* msg; //to store message
  if(rf.checkMessages()) {
    
    /*
    Serial.println("got msg: ");
    for (int i = 0 ; i < 3; i++) {
      Serial.print(msg[i]);
      Serial.print(" ");
    }Serial.println();*/
    
    msg = rf.getLastMessage();
 
    //fresh message -- different sender or different messageID
    if (msg[0] != lastMsg[0] || msg[2] != lastMsg[2]) {
      
      if (msg[1] == RFThiefScan) {
        //scanned
        if(visibility < MAX_VISIBILITY) visibility++;
      } else if (msg[1] == RFThievesWin) {
        visibility = MAX_VISIBILITY; //go to max at end
      }
      
      //set appropriate visibility
      handleVisibility();
      
      //update last message
      for (int i = 0; i < BYTES_PER_MESSAGE; i++) {
        lastMsg[i] = msg[i];
      }
    }
  }
}

void handleVisibility() {
  if (visibility == 1) {
    yellow.turnOn(100);
  } else if (visibility == 2) {
    blue.turnOn(100);
  } else if (visibility == 3) {
   green.turnOn(100);
  } else if (visibility == 4) {
    red.turnOn(100);
  } else if (visibility == 5) {
    red.turnOn(255);
    yellow.turnOn(255);
    green.turnOn(255);
    blue.turnOn(255);
  } else if (visibility == 6) {
    if (!blue.isBlinking()) blue.blink(100);
  } else if (visibility == 7) {
    sound.play(440);
  } else if (visibility == 8) {
    if (!yellow.isBlinking()) yellow.blink(100);
  } else if (visibility == 9) {
      if (!green.isBlinking()) green.blink(50);
  } else if (visibility == 10) {
    sound.play(1000);
    if (!red.isBlinking()) red.blink(50);
  }
}
