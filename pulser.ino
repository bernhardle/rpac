#include "global.h"
#include "pulser.h"
//
const int pulserVars = 6 ;
const unsigned long pulserOnDura [pulserVars] = {0, 500, 1000, 2000, 3000, 4500} ;
const unsigned long pulserOffDura [pulserVars] = {10000, 9500, 6000, 4000, 3000, 2000} ;
const unsigned int pulserProgress [pulserVars] = {1, 12, 12, 17, 21, 53} ;
//
volatile unsigned long pulserChangeTime = 0 ;
volatile bool pulserEnabled = true ;
unsigned int pulserMode = 0, pulserProgressCount = 0 ;
//
void pulserSetup (volatile unsigned long & lastChange) {
  //
  pinMode (pulserPin, OUTPUT) ;
  digitalWrite (pulserPin, LOW) ;
  //
  pulserMode = 0 ;
  pulserProgressCount = 0 ;
  lastChange = millis () ;
  //
}
//
void pulserLoop (volatile unsigned long & lastChange, unsigned int & lastCount) {
  //
  if (pulserEnabled) {
    //
    unsigned long myTime = millis () ;
    //
    if (digitalRead (pulserPin)) {
      //
      if (myTime > lastChange + pulserOnDura [pulserMode]) {
        //
        digitalWrite (pulserPin, LOW) ;
        digitalWrite (LED_BUILTIN, LOW) ;
        lastChange = myTime ;
        //
        if (++ lastCount > pulserProgress [pulserMode]) {
          //
          if (pulserMode < pulserVars - 1) {
            //
            pulserMode ++ ;
            lastCount = 0 ;
            //
            if (verbose || debug) {
              //
              Serial.print ("Switching pulser mode to level ") ;
              Serial.print (String(pulserMode, DEC)) ;
              Serial.print ("/") ;
              Serial.print (String(pulserVars - 1, DEC)) ;
              Serial.print (" ... ON=") ;
              Serial.print (String(pulserOnDura [pulserMode], DEC)) ;
              Serial.print ("ms, OFF=") ;
              Serial.print (String(pulserOffDura [pulserMode], DEC)) ;
              Serial.println ("ms.") ;
              //
            }
            //
          } else {
            //
            pulserEnabled = false ;
            //
            if (verbose || debug) Serial.print ("[INFO] Disabeling pulses.") ;
            //
          }
          //
        }
        //
      }
    } else {
      //
      if (myTime > lastChange + pulserOffDura [pulserMode]) {
        //
        digitalWrite (pulserPin, HIGH) ;
        digitalWrite (LED_BUILTIN, HIGH) ;
        lastChange = myTime ;
        //
      }
      //
    }
    //
  }
}