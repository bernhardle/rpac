#include "global.h"
#include "pulser.h"
//
const int pulserVars = 1 ;
const unsigned long pulserOnDura [pulserVars] = {0} ;
const unsigned long pulserOffDura [pulserVars] = {10000} ;
const unsigned int pulserProgress [pulserVars] = {1} ;
/*
const int pulserVars = 1 ;
const unsigned long pulserOnDura [pulserVars] = {0, 500, 1000, 2000, 3000, 4500} ;
const unsigned long pulserOffDura [pulserVars] = {10000, 9500, 6000, 4000, 3000, 2000} ;
const unsigned int pulserProgress [pulserVars] = {1, 12, 12, 17, 21, 53} ;
*/
//
unsigned long pulserChangeTime = 0 ;
unsigned int pulserMode = 0, pulserProgressCount = 0 ;
static bool pulserEnabled = true, lastPulseState = false ;
//
void pulserSetup (unsigned long & lastChange) {
  //
  pinMode (pulserPin, OUTPUT) ;
  digitalWrite (pulserPin, LOW) ;
  //
  pulserMode = 0 ;
  pulserProgressCount = 0 ;
  lastChange = millis () ;
  lastPulseState = false ;
  //
#ifdef __DEBUG__RPAC__
  Serial.println ("INFO] Pulse pattern variants:") ;
  //
  for (int i = 0 ; i < pulserVars ; i++) {
    //
    Serial.print ("\tMode ") ;
    Serial.print (String (i, DEC)) ;
    Serial.print (", ON = ") ;
    Serial.print (String (pulserOnDura [i], DEC)) ;
    Serial.print (", OFF = ") ;
    Serial.print (String (pulserOffDura [i], DEC)) ;
    Serial.print (", CYCLES = ") ;
    Serial.print (String (pulserProgress [i], DEC)) ;
    Serial.println (".") ;
    //
  }
#endif
}
//
bool pulserLoop (unsigned long & lastChange, unsigned int & lastCount, bool pulserTrigger) {
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
        //
        lastChange = myTime ;
        lastPulseState = false ;
        //
        if (++ lastCount > pulserProgress [pulserMode]) {
          //
          if (pulserMode < pulserVars - 1) {
            //
            pulserMode ++ ;
            lastCount = 0 ;
            //
#ifdef __DEBUG__RPAC__
            Serial.print ("[INFO] Switching auto pulse to mode ") ;
            Serial.print (String(pulserMode, DEC)) ;
            Serial.print ("/") ;
            Serial.print (String(pulserVars - 1, DEC)) ;
            Serial.print (" ... ON=") ;
            Serial.print (String(pulserOnDura [pulserMode], DEC)) ;
            Serial.print ("ms, OFF=") ;
            Serial.print (String(pulserOffDura [pulserMode], DEC)) ;
            Serial.println ("ms.") ;
#endif
            //
          } else {
            //
            pulserEnabled = false ;
            //
#ifdef __DEBUG__RPAC__
            Serial.print ("[INFO] Disabeling auto pulse.") ;
#endif
            //
          }
          //
        }
        //
      }
      //
    } else {
      //
      if (myTime > lastChange + pulserOffDura [pulserMode]) {
        //
        digitalWrite (pulserPin, HIGH) ;
        //
        lastChange = myTime ;
        lastPulseState = true ;
        //
      }
      //
    }
    //
  } else {
    //
    if (pulserTrigger != lastPulseState) {
      //
      digitalWrite (pulserPin, (lastPulseState = pulserTrigger) ? HIGH : LOW) ;
      //
#ifdef __DEBUG__RPAC__
      Serial.println (pulserTrigger ? "[INFO] Pulse manually started." : "[INFO] Pulse manually stopped.") ;
#endif
      lastChange = millis () ;
      //
    }
    //
  }
  //
  return digitalRead (pulserPin) ;
  //
}