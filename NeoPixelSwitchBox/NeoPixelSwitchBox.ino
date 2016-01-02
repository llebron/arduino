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

/* 
  initialize the ring
    What pin is it on?
    Function to turn on/off a group of indices together - could be handled by NeoPixelLightCluster
    randomAll()
    rainbow()
    spin() - set spin speed - (-100, +100), % of max - needs to operate on the individual light level
    update() - poll and update all lights
  
  Make my "lights" (NeoPixelLight)
    What indices does it control?
    Specify indices 
    brightness()
    blink()
    toggleOnOff()
    setColor()
    // For my "random" and "rainbow" functionality, how do I interface with the light clusters?

   
  initialize the switches/buttons - do I want to add a notion of "momentary" to the switch class for handling buttons?
  
  SPIN
  Set up knob to have guarantee it's possible to stop the spin in the center of the knob - maybe with a detent?
  Need to initialize the spin speed knob, pulling the current value. If it's non-center, should probably still start "stopped". Might need to alter the neopixelring constructor to support these inits..

*/

// ring size 24, on pin 7
NeoPixelRing ring(24, 7);
  
void setup() {


}

void loop() {
  // first, update all inputs - switches, knobs, etc - 
  updateComponents();
  
  // Finally, update the ring itself, which should now have all current state, and will be able to determine if it needs to refresh
  ring.update();

}

void updateComponents() {
  // update spin pot - ideally have a library for this, like switch - which tells me if it's changed in a meaningful way - if it has, call spin()
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


