#include <SwitchArray.h>
#include <Switch.h>

SwitchArray switchTest();

void setup() {
  Serial.begin(9600);  
  Serial.println("--- Start Serial Monitor ");
  // put your setup code here, to run once:
  switchTest.print();

}

void loop() {
  // put your main code here, to run repeatedly:

}
