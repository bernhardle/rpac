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
const unsigned long pulserOnDura [pulserVars] = {0, 2000, 3000, 4000, 5500, 7000} ;
const unsigned long pulserOffDura [pulserVars] = {10000, 6000, 5000, 4000, 3000, 2000} ;
const unsigned int pulserProgress [pulserVars] = {1, 6, 8, 14, 20, 25} ;
#endif
//
static unsigned long pulserChangeTime = 0 ;
static unsigned int pulserMode = 0, pulserProgressCount = 0 ;
static bool pulserAuto = true, pulserState = false ;
static uint8_t pulserPin ;
//
static uint8_t pulserControlCB (uint8_t) {
  //
  if (pulserAuto) {
    //
    pulserAuto = false ;
#ifdef __DEBUG__PULSER__
    Serial.println ("[INFO] pulserControlCB () disabled autopulse") ;
#endif
    //
    return 0U ;
    //
  } else {
    //
    pulserAuto = true ;
#ifdef __DEBUG__PULSER__
    Serial.println ("[INFO] pulserControlCB () enabled autopulse") ;
#endif
    //
    return 1U ;
    //
  }
  //
  return 0 ;
  //
}

static unsigned long int pulserDataCB (void) {
  //
  return pulserState ;
  //
}
//
void pulserSetup (rpacPin_t pin, controlCBs_t & ccbs, loggerCBs_t & lcbs) {
  //
  pinMode ((pulserPin = static_cast <uint8_t> (pin)), OUTPUT) ;
  digitalWrite (pulserPin, LOW) ;
  //
  lcbs.add (& pulserDataCB, "pulse") ;
  ccbs.add (& pulserControlCB, 2) ;
  //
  pulserMode = 0 ;
  pulserState = false ;
  pulserProgressCount = 0 ;
  pulserChangeTime = millis () ;
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
  if (pulserAuto) {
    //
    unsigned long myTime = millis () ;
    //
    if (pulserState) {
      //
      if (myTime > pulserChangeTime + pulserOnDura [pulserMode]) {
        //
        digitalWrite (pulserPin, LOW) ;
        //
        pulserChangeTime = myTime ;
        pulserState = false ;
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
            pulserAuto = false ;
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
        pulserState = true ;
        //
      }
      //
    }
    //
  } else {
    //
    if (pulserTrigger != pulserState) {
      //
      digitalWrite (pulserPin, (pulserState = pulserTrigger) ? HIGH : LOW) ;
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
  return pulserState ;
  //
}