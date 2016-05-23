#include "RFRec.h"
#include "GameIRTimer2.h"
#include "Light.h"

//RF receiver on pin 8
RFRec rf(8);

//The green leds
Light lt(6);

//IR receiver on pin 
GameIRTimer2 ir(2);

//stores the last message received
uint8_t lastMsg[BYTES_PER_MESSAGE] = {-1, -1, -1};

void setup(){
  Serial.begin(9600);
  rf.init();
}
void loop(){
  if (!lt.isFading()) lt.fade(1, 255, 2000);
  updateComponents();
  
  handleRF();
}

void updateComponents() {
  lt.update();
}

//handle incoming RF messages
void handleRF() {
  uint8_t* msg; //to store message
  if(rf.checkMessages()) {
    msg = rf.getLastMessage();
 
    //fresh message -- different sender or different messageID
    if (msg[0] != lastMsg[0] || msg[2] != lastMsg[2]) {
      
      if (msg[1] == RFJailbreak) {
        Serial.write(1);//tell slave to send jailbreak
        if (!lt.isBlinking()) lt.blink(30);
      }        
      
      //update last message
      for (int i = 0; i < BYTES_PER_MESSAGE; i++) {
        lastMsg[i] = msg[i];
      }
    }
  }
}

