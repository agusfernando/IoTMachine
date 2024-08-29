// #define DEBUG 1


// // #define USELITTLEFS 0
// #if DEBUG == 0
// #define debug(x) Serial.print(x)
// #define debugln(x) Serial.println(x)
// #else
// #define debug(x)
// #define debugln(x)
// #endif

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "variables.h";
#include "keypad.h";

#include "wifiConfig.h";



void setup() {
  // #if DEBUG == 1
  Serial.begin(115200);  //Initialising if(DEBUG)Serial Monitor
// #endif


setupKeypad();
  setupWifi();
  delay(3000);

}

void loop() {
  // put your main code here, to run repeatedly:
loopKeypad();
}
