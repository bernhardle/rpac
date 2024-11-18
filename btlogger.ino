//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#if defined(ARDUINO_Seeed_XIAO_nRF52840)
//
#include "btlogger.h"
//
bool rpac::BTLogger::initialized {false} ;
rpac::BTLogger * rpac::BTLogger::instance {nullptr} ;
//
void rpac::BTLogger::setup (loggerCBs_t & cbs) {
  //
  if (!initialized) {
    //
    instance = new BTLogger (cbs) ;
    //
    if (*instance) {
      //
      initialized = true ;
      //
      Serial.println ("[INFO] Data logging to BLE server.") ;
      //
    } else {
      //
      Serial.println ("[ERROR] BTLogger::setup() failed.") ;
      //
    }
    //
  }
  //
}
//
bool rpac::BTLogger::loop (unsigned long int myTime) {
  //
  return false ;
  //
}
//
#endif