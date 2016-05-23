/**
  The guard station
  Sends a guard recharge IR signal when button is pushed
*/

#include "Switch.h"
#include "Light.h"
#include "GameIRTimer2.h"

Switch sw(9, 30);
Light lt(5);
GameIRTimer2 ir(-1); //ir send only -- sends on pin 3

void setup() {
}

void loop() {
  //update light and switch
  sw.update();
  lt.update();
  
  if (sw.isClosed()) {
    if (lt.isFading()) lt.stopFade();
    if (!lt.isBlinking()) lt.blink(20);
    ir.send(IRGuardStation);
    
  } else {
    if (lt.isBlinking()) lt.turnOn();//stop blinking if coming from send
    if (!lt.isFading()) lt.fade(1, 255, 2000);
  }
}
