  #include <Potentiometer.h>
  #include <Switch.h>
  
  // have to include libraries referenced within libraries in the sketch as well!
  #include <Adafruit_NeoPixel.h>
  #include <math.h>
  #include <NeoPixel.h>  
  #include <NeoPixelRing.h>
  #include <MemoryFree.h>
  
  const int NUM_LIGHTS = 24;
  const int NUM_LIGHT_SWITCHES = 8;
  const int NUM_LIGHTS_PER_SWITCH = 3;
  /* stand-in until I come up with individualized settings */
  const int DEBOUNCE_TIME = 30;
  
  /* has the random number generator been seeded? Set true the first time random is used */
  bool seededRand = false;
  
  /* For prototyping, use this offset to place light switches from pins 2 - 9, instead of 0-7, because pins 0 and 1 are used for serial communication */
  const int LIGHT_SWITCH_PIN_OFFSET = 2;
  
  /* The 8 light switches and the sets of lights they control*/
  Switch* lightSwitches[NUM_LIGHT_SWITCHES];
  int lightsForSwitches[NUM_LIGHT_SWITCHES][NUM_LIGHTS_PER_SWITCH];
  
  // ring size NUM_LIGHTS, NUM_LIGHTS_PER_SWITCH assigned for each switch, on pin 13
  NeoPixelRing ring(NUM_LIGHTS, NUM_LIGHTS_PER_SWITCH, 13);
    
  // test pot
  Potentiometer pot(A0);
  
  //button on pin 12, debounce for DEBOUNCE_TIME
  Switch randomButton(12, DEBOUNCE_TIME);
  
  /* 
    
    initialize the switches/buttons 
    initialize pots
    - guess I should just set everything based on the hardware to start
    or - start with defaults and then update state once there's a change - might be trickier than it's worth
    
    SPIN
    Set up knob to have guarantee it's possible to stop the spin in the center of the knob - maybe with a detent?
    Need to initialize the spin speed knob, pulling the current value. If it's non-center, should probably still start "stopped". Might need to alter the neopixelring constructor to support these inits..
  
  */  
  
  void setup() {
    Serial.begin(9600);  
    Serial.println("--- Start Serial Monitor ");
    
    /* Initialize the light switches */
    initializeLightSwitches();
    
    /* randomize experiment*/
    //ring.randomize();
    //ring.blinkRingIndex(0, 10);
    //ring.spin(1000, true);
    
    // testing to max out memory
    for (int i = 0; i < NUM_LIGHTS; i++) {
      ring.blinkRingIndex(i, 100);
    }
    
    // Test to try to cap out memory
    for (int i = 0; i < NUM_LIGHTS; i++) {
      ring.setBrightnessPercentRingIndex(i, .2);
    }
        
    printFreeMemory("");
    
  }
  
  void loop() {
    //printFreeMemory("1");
    
    // first, update all inputs - switches, knobs, etc - 
    updateComponents();
    
    //printFreeMemory("2");
    
    // Finally, update the ring itself, which should now have all current state, and will be able to determine if it needs to refresh
    ring.update();
    
    //printFreeMemory("3");
  
  }
  
  void updateComponents() {
    updateLightSwitches();
    pot.update();
    if (pot.valChangedThisUpdate()) {
      //Serial.print("pot changed to: "); Serial.println(pot.getPercentVal());
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
 
  void initializeLightSwitches() {
    // track the light ring indices to assign for the switch
    int lightRingIndex = 0;
    
    for (int i = 0; i < NUM_LIGHT_SWITCHES; i++) {
      
      // For prototyping, using offset so I retain use of serial communication, which requires pins 0 and 1
      int switchPin = i+LIGHT_SWITCH_PIN_OFFSET;
      lightSwitches[i] = new Switch(switchPin, DEBOUNCE_TIME); 
      
      for (int j = 0; j < NUM_LIGHTS_PER_SWITCH ; j++) {
        //Serial.print("lightsForSwitch ");Serial.print(i);Serial.print(" ");Serial.println(lightRingIndex);
        lightsForSwitches[i][j] = lightRingIndex;
        lightRingIndex++;
      }
      
      Serial.print("init light switch "); Serial.print(i);
      /* Set lights on/off based on initial state */
      if (lightSwitches[i]->isClosed()) {
         Serial.println(" closed");
         ring.turnOnLightCluster(lightsForSwitches[i]);
      } else {
        Serial.println(" open");
        ring.turnOffLightCluster(lightsForSwitches[i]);
      }
    }
  }
  
  void updateLightSwitches() {
    for (int i = 0; i < NUM_LIGHT_SWITCHES; i++) {
      lightSwitches[i]->update();
      
      if (lightSwitches[i]->closedThisUpdate()) {
        Serial.print("light switch closed "); Serial.println(i);
        ring.turnOnLightCluster(lightsForSwitches[i]);
      } else if (lightSwitches[i]->openedThisUpdate()) {
        Serial.print("light switch opened "); Serial.println(i);
        ring.turnOffLightCluster(lightsForSwitches[i]);
      }
    }
  }
  
  //Look closely at how memory is allocated in the various classes... seems like I might have a mem leak or misallocation somewhere?
  void printFreeMemory(String prefix) {
    Serial.print(prefix);Serial.print(" free mem: ");
    Serial.print(availableMemory()); Serial.print(", ");Serial.println(freeMemory());
  }
  
  int availableMemory() {
    int size = 2048; // Use 2048 with ATmega328
    byte *buf;
  
    while ((buf = (byte *) malloc(--size)) == NULL)
      ;
  
    free(buf);
  
    return size;
  }
