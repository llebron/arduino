#include <Light.h>

#include <StandardCplusplus.h>
#include <set>
#include <iterator>

using namespace std;

Light testLightA(0);
Light testLightB(1);
Light testLightC(2);
set<int> blinkingLights;
Light lights[3] = {testLightA, testLightB, testLightC};

void setup(void)
{
  Serial.begin(57600);
  
  blinkingLights.insert(0);
  blinkingLights.insert(1);
  blinkingLights.insert(2);
  
  /* If any lights are blinking, go through blinking set, updating any blinking lights */
  for (set<int>::iterator it=blinkingLights.begin(); it!=blinkingLights.end(); it++) {
    int x = 33;
  }
}

void loop(void)
{
}
