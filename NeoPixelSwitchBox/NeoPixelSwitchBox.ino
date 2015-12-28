// have to include libraries referenced within libraries in the sketch as well!
#include <Adafruit_NeoPixel.h>
#include <NeoPixel.h>

#include <NeoPixelRing.h>


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

*/

// ring size 24, on pin 7
NeoPixelRing ring(24, 7);
  
void setup() {


}

void loop() {
  // put your main code here, to run repeatedly:

}
