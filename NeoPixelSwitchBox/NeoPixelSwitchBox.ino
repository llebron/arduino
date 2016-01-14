#include <Potentiometer.h>

#include <Switch.h>

// have to include libraries referenced within libraries in the sketch as well!
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <NeoPixel.h>
#include <StandardCplusplus.h>
#include <set>
#include <iterator>


#include <NeoPixelRing.h>

bool seededRand = false;

//button on pin 0, debounce 30 ms
Switch randomButton(0, 30);

// test pot
Potentiometer pot(A0);



/* 
  
  initialize the switches/buttons 
  initialize pots
  - guess I should just set everything based on the hardware to start
  or - start with defaults and then update state once there's a change - might be trickier than it's worth
  
  SPIN
  Set up knob to have guarantee it's possible to stop the spin in the center of the knob - maybe with a detent?
  Need to initialize the spin speed knob, pulling the current value. If it's non-center, should probably still start "stopped". Might need to alter the neopixelring constructor to support these inits..

*/

// ring size 24, on pin 7
NeoPixelRing ring(24, 7);
  
void setup() {
  Serial.begin(9600);  
  Serial.println("--- Start Serial Monitor ");
  // stand-in for initialization
  
  ring.randomize();
  //ring.blinkRingIndex(0, 100);
  ring.spin(1000, false);
  /*for (int i =0; i < 24; i++) {
   // if (i!=23 && i != 0 && i!=1 && i!=2 ) {
   if (i != 0) {
      ring.setBrightnessPercentRingIndex(i, 0);
    }
  }*/
  
  std::set<uint16_t> offLights;
  offLights.insert(0);
  offLights.insert(1);
  offLights.insert(2);
  //ring.turnOffRingIndices(offLights);
}

void loop() {
  
  //delay(1000);
  //ring.randomize();
  // first, update all inputs - switches, knobs, etc - 
  updateComponents();
  
  // Finally, update the ring itself, which should now have all current state, and will be able to determine if it needs to refresh
  ring.update();

}

void updateComponents() {
  pot.update();
  if (pot.valChangedThisUpdate()) {
    Serial.print("pot changed to: "); Serial.println(pot.getPercentVal());
  }
  // update spin pot - if it has changed, call spin()
  // update light knobs
  // update blink and brightness sliders
  // update switches and other buttons
  
  // update the status of all switches/buttons - might be worth putting them in an array to make it an easy loop
  updateRandomButton();
}

void updateRandomButton() {
   randomButton.update();
   if (randomButton.closedThisUpdate()) {
     // if this is the first time the random button is closed, seed the random number generator with the time the program's been running - will vary from run to run
     if (!seededRand) {
       randomSeed(millis());
       seededRand = true;
     }
     ring.randomize();
   }
}


