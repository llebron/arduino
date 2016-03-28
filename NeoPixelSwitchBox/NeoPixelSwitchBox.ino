  #include <Log.h>
  #include <Potentiometer.h>
  #include <Switch.h>
  
  /*
  Tasks 
  
  updateSpinKnob() should probably have a delta to allow for "close to center" to stop it
  */
  
 
  
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
  
  /* The fastest the spin will increment */
  const long FASTEST_SPIN_INCREMENT_DURATION_MS = 10;
  const long SLOWEST_SPIN_INCREMENT_DURATION_MS = 800;
  const long SPIN_INCREMENT_DURATION_RANGE_MS = SLOWEST_SPIN_INCREMENT_DURATION_MS - FASTEST_SPIN_INCREMENT_DURATION_MS;
  const float POTENTIOMETER_MIDDLE_PERCENT = .5;
  
  Log logger;
  
  /* has the random number generator been seeded? Set true the first time random is used */
  bool seededRand = false;
  
  /* For prototyping, use this offset to place light switches from pins 2 - 9, instead of 0-7, because pins 0 and 1 are used for serial communication */
  const int LIGHT_SWITCH_PIN_OFFSET = 2;
  
  /* The 8 light switches and the sets of lights they control*/
  Switch* lightSwitches[NUM_LIGHT_SWITCHES];
  int lightsForSwitches[NUM_LIGHT_SWITCHES][NUM_LIGHTS_PER_SWITCH];
  
  // ring size NUM_LIGHTS, NUM_LIGHTS_PER_SWITCH assigned for each switch, on pin 13
  NeoPixelRing ring(NUM_LIGHTS, NUM_LIGHTS_PER_SWITCH, 13);
    
  // potentiometer to control ring spin speed
  Potentiometer spinKnob(A0);
  
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
    logger.log("--- Start Serial Monitor ");
    
    /* 
      Use external analog reference, so that potentiometers work relative to the voltage being supplied by the battery 
      N.B. Must be sure this is configured before the first analogRead(). For this sketch, that shouldn't be uitil the first update call for potentiometers
    */
    analogReference(EXTERNAL);
    
    /* Initialize the light switches */
    initializeLightSwitches();
    
    /* randomize experiment*/
    //ring.randomize();
    //ring.blinkRingIndex(0, 10);
    //ring.spin(1000, true);
    
    // testing to max out memory
    /*for (int i = 0; i < NUM_LIGHTS; i++) {
      ring.blinkRingIndex(i, 10);
    }*/
    
    // Test to try to cap out memory
    for (int i = 0; i < NUM_LIGHTS; i++) {
      ring.setBrightnessPercentRingIndex(i, .2);
    }
    
    ring.rainbow();
        
    printFreeMemory();
    
  }
  
  void loop() {
    //printFreeMemory();
    
    // first, update all inputs - switches, knobs, etc - 
    updateComponents();
    
    //printFreeMemory();
    
    // Finally, update the ring itself, which should now have all current state, and will be able to determine if it needs to refresh
    ring.update();
    
    //printFreeMemory();
  
  }
  
  void updateComponents() {
    updateLightSwitches();
    updateSpinKnob();

    // update light knobs
    // update blink and brightness sliders
    // update switches and other buttons
    
    // update the status of all switches/buttons - might be worth putting them in an array to make it an easy loop
    updateRandomButton();
  }
  
  void updateSpinKnob() {
    spinKnob.update();
    if (spinKnob.valChangedThisUpdate()) {
      float spinKnobPercent = spinKnob.getPercentVal();
      logger.log("spin knob changed to: ", spinKnobPercent);
      
      /* If the knob is centered, send the special flag to the ring to stop spinning, and return 
        TODO: probably want a delta here to give a bit of wiggle room for getting "close to center"
      */
      if (spinKnobPercent == POTENTIOMETER_MIDDLE_PERCENT) {
        logger.log("Spin knob at center");
        ring.spin(-1, true);
        return;
      }
      
      /* 
        Clockwise if spin pot is >= midpoint 
        Determine "raw" amount spin knob is turned from center (.5 max since it's from 0-1)
      */
      boolean isClockwise;
      float rawSpinKnobAmtTurnedFromCenter;
      if (spinKnobPercent >= POTENTIOMETER_MIDDLE_PERCENT) {
        isClockwise = true;
        rawSpinKnobAmtTurnedFromCenter = spinKnobPercent - .5;
        logger.log("Spin knob clockwise", rawSpinKnobAmtTurnedFromCenter);
      } else {
        isClockwise = false;
        rawSpinKnobAmtTurnedFromCenter = .5 - spinKnobPercent;
        logger.log("Spin knob counter clockwise", rawSpinKnobAmtTurnedFromCenter);
      }
      
      /* Percent the spin knob is away from center (0-1) */
      float percentSpinKnobAmtTurnedFromCenter = rawSpinKnobAmtTurnedFromCenter / .5;
      logger.log("Spin knob percentSpinKnobAmtTurnedFromCenter", percentSpinKnobAmtTurnedFromCenter);
      
      /* How much of the increment time range to use (from min - max) is the "opposite" of the spin knob percent amt, since more is less time */
      float percentOfIncrementTimeRange = 1 - percentSpinKnobAmtTurnedFromCenter;
      logger.log("Spin knob percentOfIncrementTimeRange", percentOfIncrementTimeRange);
      
      /* Use this percent to determine the actual increment time*/
      long incrementDuration = FASTEST_SPIN_INCREMENT_DURATION_MS + (SPIN_INCREMENT_DURATION_RANGE_MS * percentOfIncrementTimeRange);
      logger.log("Spin knob incrementDuration", incrementDuration);
      
      ring.spin(incrementDuration, isClockwise);
      
    }
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
        //logger.log("lightsForSwitch ", i, " ", lightRingIndex);
        lightsForSwitches[i][j] = lightRingIndex;
        lightRingIndex++;
      }
      
      logger.log("init light switch ", i);
      /* Set lights on/off based on initial state */
      if (lightSwitches[i]->isClosed()) {
         logger.log(" closed");
         ring.turnOnLightCluster(lightsForSwitches[i]);
      } else {
        logger.log(" open");
        ring.turnOffLightCluster(lightsForSwitches[i]);
      }
    }
  }
  
  void updateLightSwitches() {
    for (int i = 0; i < NUM_LIGHT_SWITCHES; i++) {
      lightSwitches[i]->update();
      
      if (lightSwitches[i]->closedThisUpdate()) {
        logger.log("light switch closed ", i);
        ring.turnOnLightCluster(lightsForSwitches[i]);
      } else if (lightSwitches[i]->openedThisUpdate()) {
        logger.log("light switch opened ", i);
        ring.turnOffLightCluster(lightsForSwitches[i]);
      }
    }
  }
  
  //Look closely at how memory is allocated in the various classes... seems like I might have a mem leak or misallocation somewhere?
  void printFreeMemory() {
    logger.log(" free mem: ", availableMemory(), ", ", freeMemory());
  }
  
  int availableMemory() {
    int size = 2048; // Use 2048 with ATmega328
    byte *buf;
  
    while ((buf = (byte *) malloc(--size)) == NULL)
      ;
  
    free(buf);
  
    return size;
  }
