  #include <Log.h>
  #include <Potentiometer.h>
  #include <Switch.h>
  
  /*
  Tasks 
  
  Test fixes for:
    updateSpinKnob() should probably have a delta to allow for "close to center" to stop it
    
    BUG (believe I fixed it): when calling stopBlinking, if the light is off, won't turn on because it won't be queried during updateBlinkingLights. 
    Or, something else may be going on. Seems like it sometimes gets one more update. 
    
   Functionality test:
    increment/decrement buttons
    rainbow
    random
        
    Concerned that I may have now run it out of memory again...
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
  
  // Give a bit of wiggle room to treat a pot as off
  const float POTENTIOMETER_OFF_MAX_PERCENT = .01;
  
  // Range for the potentiometer to be considered "at middle"
  const float POTENTIOMETER_LOWER_MIDDLE_PERCENT = .49;
  const float POTENTIOMETER_UPPER_MIDDLE_PERCENT = .51;
  // absolute middle value for potentiometer
  const float POTENTIOMETER_MIDDLE_PERCENT = .5;
 
  /* spin increment constants */
  const long FASTEST_SPIN_INCREMENT_DURATION_MS = 10;
  const long SLOWEST_SPIN_INCREMENT_DURATION_MS = 800;
  const long SPIN_INCREMENT_DURATION_RANGE_MS = SLOWEST_SPIN_INCREMENT_DURATION_MS - FASTEST_SPIN_INCREMENT_DURATION_MS;
  
  /* Blink constants */
  const long FASTEST_BLINK_DURATION_MS = 10;
  const long SLOWEST_BLINK_DURATION_MS = 1000;
  const long BLINK_DURATION_RANGE_MS = SLOWEST_BLINK_DURATION_MS - FASTEST_BLINK_DURATION_MS;
  
  /* Random constants */
  const int BLINK_DIE_ROLL_SIDES = 6;
  const int SPIN_DIE_ROLL_SIDES = 3;
  
  /* The ring indices to edit with "local" operations like brightness, blink and rgb */
  int RING_INDICES_TO_EDIT[] = {0,1,2};
  
  Log logger;
  
  /* has the random number generator been seeded? Set true the first time random is used */
  bool seededRand = false;
  
  /* Increment/decrement spin buttons on pins 0/1 - need to disable if want serial communication */
  Switch incrementSpinButton(0, DEBOUNCE_TIME);
  Switch decrementSpinButton(1, DEBOUNCE_TIME);
  
   /* Place light switches on 2 - 9, because pins 0 and 1 are used for serial communication */
  const int FIRST_LIGHT_SWITCH_PIN = 2;
  
  /* The 8 light switches and the sets of lights they control*/
  Switch* lightSwitches[NUM_LIGHT_SWITCHES];
  int lightsForSwitches[NUM_LIGHT_SWITCHES][NUM_LIGHTS_PER_SWITCH];
  
  // ring size NUM_LIGHTS, NUM_LIGHTS_PER_SWITCH assigned for each switch, on pin 13
  NeoPixelRing ring(NUM_LIGHTS, NUM_LIGHTS_PER_SWITCH, 13);
  
  //button on pin 10, debounce for DEBOUNCE_TIME
  Switch randomButton(10, DEBOUNCE_TIME);
  
  //button on pin 11, debounce for DEBOUNCE_TIME
  Switch rainbowButton(11, DEBOUNCE_TIME);
  
  // button on pin 12 - toggle spin on/off
  Switch toggleSpinButton(12, DEBOUNCE_TIME);
  
  /* Potentiometers on analog inupts*/
  // potentiometer to control ring spin speed
  Potentiometer spinKnob(A0);
  // Control blink rate for lights being edited
  Potentiometer blinkKnob(A1);
  // Control brightness rate for lights being edited
  Potentiometer brightnessKnob(A2);
  // Control RGB for lights being edited
  Potentiometer redKnob(A3);
  Potentiometer greenKnob(A4);
  Potentiometer blueKnob(A5);
  

  
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
    
    /* Initialize the pots to set their initial values */
    initializePotentiometers();    
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
    
    // update the spin knob before the spin toggle, just in case they both change on the same update. Want the button to override */
    updateSpinKnob();
    updateToggleSpinButton();
    
    // update the increment/decrement buttons
    updateIncrementButtons();
    
    // update the other pots
    updateBlinkKnob();
    updateBrightnessKnob();
    updateRGBKnobs();
    
    // update the status of buttons
    updateRandomButton();
    updateRainbowButton();
  }
  
  boolean potentiometerPercentAtMiddle(float percent) {
    return POTENTIOMETER_LOWER_MIDDLE_PERCENT <= percent && percent <= POTENTIOMETER_UPPER_MIDDLE_PERCENT;
  }
  
  void updateSpinKnob() {
    spinKnob.update();
    if (spinKnob.valChangedThisUpdate()) {
      handleSendSpinState();
    }
  }
  
  /* Broke this out into a separate method so it can be transmitted at startup */
  void handleSendSpinState() {
    float spinKnobPercent = spinKnob.getPercentVal();
    logger.log("spin knob changed to: ", spinKnobPercent);
    
    /* If the knob is centered, send the special flag to the ring to stop spinning, and return */
    if (potentiometerPercentAtMiddle(spinKnobPercent)) {
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
  
   void updateBlinkKnob() {
    blinkKnob.update();
    if (blinkKnob.valChangedThisUpdate()) {
      float blinkKnobPercent = blinkKnob.getPercentVal();
      logger.log("blink knob changed to: ", blinkKnobPercent);
      
      /* If the knob is "off" (with some wiggle room), stop blinking and return
      */
      if (blinkKnobPercent <= POTENTIOMETER_OFF_MAX_PERCENT) {
        logger.log("Stop blinking");
        ring.stopBlinkLightCluster(RING_INDICES_TO_EDIT);
        return;
      }
      
      /* How much of the increment time range to use (from min - max) is the "opposite" of the blink knob percent amt, since more is less time */
      float percentOfDurationTimeRange = 1 - blinkKnobPercent;
      logger.log("Blink knob percentOfIncrementTimeRange", percentOfDurationTimeRange);
      
      /* Use this percent to determine the actual increment time*/
      long blinkDuration = FASTEST_BLINK_DURATION_MS + (BLINK_DURATION_RANGE_MS * percentOfDurationTimeRange);
      logger.log("Blink knob duration", blinkDuration);
      
      ring.blinkLightCluster(RING_INDICES_TO_EDIT, blinkDuration);
    }
  }
  
  void updateBrightnessKnob() {
    brightnessKnob.update();
    if (brightnessKnob.valChangedThisUpdate()) {
      float brightnessPercent = brightnessKnob.getPercentVal();
      logger.log("brightness knob changed to: ", brightnessPercent);
      ring.setBrightnessPercentLightCluster(RING_INDICES_TO_EDIT, brightnessPercent);
    }
  }
  
  void updateRGBKnobs() {
    redKnob.update();
    greenKnob.update();
    blueKnob.update();
    /* If any color changes, update all, so that the settings are current */
    if (redKnob.valChangedThisUpdate() || greenKnob.valChangedThisUpdate() || blueKnob.valChangedThisUpdate()) {
      float redKnobPercent = redKnob.getPercentVal();
      ring.setRedLightCluster(RING_INDICES_TO_EDIT, redKnobPercent);
      float greenKnobPercent = greenKnob.getPercentVal();
      ring.setGreenLightCluster(RING_INDICES_TO_EDIT, greenKnobPercent);
      float blueKnobPercent = blueKnob.getPercentVal();
      ring.setBlueLightCluster(RING_INDICES_TO_EDIT, blueKnobPercent);
    }
  }
  
  void updateToggleSpinButton() {
     toggleSpinButton.update();
     if (toggleSpinButton.closedThisUpdate()) {
       logger.log("toggle spin button closed ");
       ring.toggleSpin();
     }
  }
  
  void updateIncrementButtons() {
    
    // spin increment button adjusts the spin 1 clockwise
    incrementSpinButton.update();
    if (incrementSpinButton.closedThisUpdate()) {
      ring.adjustSpinOffset(1);
    }
    
    // spin decrement button adjusts the spin 1 counter-clockwise
    decrementSpinButton.update();
    if (incrementSpinButton.closedThisUpdate()) {
      ring.adjustSpinOffset(-1);
    }
  }
  
  void updateRainbowButton() {
     rainbowButton.update();
     if (rainbowButton.closedThisUpdate()) {
       ring.rainbow();
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
       ring.randomize(BLINK_DIE_ROLL_SIDES, SLOWEST_BLINK_DURATION_MS, SPIN_DIE_ROLL_SIDES, SLOWEST_SPIN_INCREMENT_DURATION_MS);
     }
  }
 
  void initializeLightSwitches() {
    // track the light ring indices to assign for the switch
    int lightRingIndex = 0;
    
    for (int i = FIRST_LIGHT_SWITCH_PIN; i < NUM_LIGHT_SWITCHES; i++) {
      
      lightSwitches[i] = new Switch(i, DEBOUNCE_TIME); 
      
      for (int j = 0; j < NUM_LIGHTS_PER_SWITCH ; j++) {
        logger.log("lightsForSwitch ", i, " ", lightRingIndex);
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
  
  /* Update the knobs so that their values are set internally. 
    This way, we won't register changes until the knob is moved by the user
  */
  void initializePotentiometers() {
    // For the spin knob - if the spin knob isn't centered at start, actually send the state to the ring, and then stop spinning at init
    // This will allow the spin toggle button to function without having touched the spin knob first
    spinKnob.update();
    if (!potentiometerPercentAtMiddle(spinKnob.getPercentVal())) {
      handleSendSpinState();
      ring.toggleSpin();
    }
    
    blinkKnob.update();
    brightnessKnob.update();
    redKnob.update();
    greenKnob.update();
    blueKnob.update();
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
