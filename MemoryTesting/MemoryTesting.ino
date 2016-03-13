  #include <SwitchArray.h>
  
  
  #include <Adafruit_NeoPixel.h>
  #include <MemoryFree.h>
  #include <NeoPixelRing.h>
  #include <NeoPixel.h>
  #include <StandardCplusplus.h>
  #include <Switch.h>
  
  const int NUM_LIGHTS = 24;
  const int NUM_LIGHT_SWITCHES = 8;
  const int NUM_LIGHTS_PER_SWITCH = 3;
  
  //NeoPixelRing ring(NUM_LIGHTS, 13);
  //SwitchArray lightSwitches;
  //std::set<int> lightsForSwitchesSets[NUM_LIGHT_SWITCHES];
  int lightsForSwitchesArrays[NUM_LIGHT_SWITCHES][NUM_LIGHTS_PER_SWITCH];
  
  
  void setup() {
    Serial.begin(9600);  
    Serial.println("--- Start Serial Monitor ");
    printFreeMemory("");
    //lightSwitches.init(2, 9);
    //initializeLightSwitchesSets();
    initializeLightSwitchesArrays();
    printFreeMemory("");
    
    /*for (int i = 0; i < NUM_LIGHTS; i++) {
      //Serial.print("Before Set brightness "); Serial.print(i);
      //printFreeMemory("");
      ring.setBrightnessPercentRingIndex(i, .2);
      //Serial.print("After Set brightness "); Serial.print(i);
      //printFreeMemory("");
    }
    
    Serial.print("After Set brightness ");printFreeMemory("");
    
    ring.update();
    printFreeMemory("");*/
  
  }
  
  
  
    /*void initializeLightSwitchesSets() {
      for (int i = 0; i < NUM_LIGHT_SWITCHES; i++) {
        
        int firstLightIndexForSwitch = i * NUM_LIGHTS_PER_SWITCH;
        int lastLightIndexForSwitch = firstLightIndexForSwitch + NUM_LIGHTS_PER_SWITCH - 1;
        
        for (int lightRingIndex = firstLightIndexForSwitch; lightRingIndex <= lastLightIndexForSwitch ; lightRingIndex++) {
          Serial.print("lightsForSwitch ");Serial.print(i);Serial.print(" ");Serial.println(lightRingIndex);
          lightsForSwitchesSets[i].insert(lightRingIndex);
        }
      }
    }*/
    
    void initializeLightSwitchesArrays() {
      int lightRingIndex = 0;
      
      for (int i = 0; i < NUM_LIGHT_SWITCHES; i++) {
        for (int j = 0; j < NUM_LIGHTS_PER_SWITCH ; j++) {
          Serial.print("lightsForSwitch ");Serial.print(i);Serial.print(" ");Serial.println(lightRingIndex);
          lightsForSwitchesArrays[i][j] = lightRingIndex;
          lightRingIndex++;
        }
      }
    }
  
  void loop() {
    // put your main code here, to run repeatedly:
  
  }
  
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
