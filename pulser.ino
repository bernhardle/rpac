#include "global.h"
#include "pulser.h"
#include "logger.h"
//
#if 0
const int pulserVars = 1 ;
const unsigned long pulserOnDura [pulserVars] = {0} ;
const unsigned long pulserOffDura [pulserVars] = {10000} ;
const unsigned int pulserProgress [pulserVars] = {1} ;
#else
const int pulserVars = 6 ;
const unsigned long pulserOnDura [pulserVars] = {0, 1500, 2500, 4000, 5500, 7000} ;
const unsigned long pulserOffDura [pulserVars] = {10000, 6000, 5000, 4000, 3000, 2000} ;
const unsigned int pulserProgress [pulserVars] = {1, 12, 12, 17, 21, 25} ;
#endif
//
static unsigned long pulserChangeTime = 0 ;
static unsigned int pulserMode = 0, pulserProgressCount = 0 ;
static bool pulserEnabled = true, lastPulseState = false ;
static pin_size_t pulserPin ;
//
static uint8_t pulserControlCB (void) {
  //
  if (pulserEnabled) {
    //
    pulserEnabled = false ;
#ifdef __DEBUG__PULSER__
    Serial.println ("[INFO] pulserControlCB () disabled autopulse") ;
#endif
    //
  } else {
    //
    pulserEnabled = true ;
#ifdef __DEBUG__PULSER__
    Serial.println ("[INFO] pulserControlCB () enabled autopulse") ;
#endif
    //
  }
  //
  return 0 ;
  //
}

static unsigned long int pulserDataCB (void) {
  //
  return digitalRead (pulserPin) ;
  //
}
//
void pulserSetup (int pin, controlCBs_t & ccbs, loggerCBs_t & lcbs) {
  //
  pinMode ((pulserPin = pin), OUTPUT) ;
  digitalWrite (pulserPin, LOW) ;
  //
  lcbs.add (& pulserDataCB, "pulse") ;
  ccbs.add (& pulserControlCB, 2) ;
  //
  pulserMode = 0 ;
  pulserProgressCount = 0 ;
  pulserChangeTime = millis () ;
  lastPulseState = false ;
  //
#ifdef __DEBUG__PULSER__
  Serial.println ("[INFO] Pulse pattern variants:") ;
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
bool pulserLoop (bool pulserTrigger) {
  //
  if (pulserEnabled) {
    //
    unsigned long myTime = millis () ;
    //
    if (digitalRead (pulserPin)) {
      //
      if (myTime > pulserChangeTime + pulserOnDura [pulserMode]) {
        //
        digitalWrite (pulserPin, LOW) ;
        //
        pulserChangeTime = myTime ;
        lastPulseState = false ;
        //
        if (++ pulserProgressCount > pulserProgress [pulserMode]) {
          //
          if (pulserMode < pulserVars - 1) {
            //
            pulserMode ++ ;
            pulserProgressCount = 0 ;
            //
#ifdef __DEBUG__PULSER__
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
            pulserMode = 0 ;
            //
#ifdef __DEBUG__PULSER__
            Serial.println ("[INFO] Disabeling auto pulse.") ;
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
      if (myTime > pulserChangeTime + pulserOffDura [pulserMode]) {
        //
        digitalWrite (pulserPin, HIGH) ;
        //
        pulserChangeTime = myTime ;
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
#ifdef __DEBUG__PULSER__
      Serial.println (pulserTrigger ? "[INFO] Pulse manually started." : "[INFO] Pulse manually stopped.") ;
#endif
      pulserChangeTime = millis () ;
      //
    }
    //
  }
  //
  return digitalRead (pulserPin) ;
  //
}