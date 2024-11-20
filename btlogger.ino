//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#if defined(ARDUINO_Seeed_XIAO_nRF52840)
//
#include <bluefruit.h>
#include "btlogger.h"
//
bool rpac::BTLogger::initialized {false} ;
rpac::BTLogger * rpac::BTLogger::instance {nullptr} ;
//
rpac::BTLogger::BTLogger (loggerCBs_t & cbs) : Logger (cbs) {
  //

  //
}
//
void rpac::BTLogger::setup (loggerCBs_t & cbs) {
  //
  Bluefruit.begin ();
  Bluefruit.setTxPower (4) ;    // Check bluefruit.h for supported values
  //
  Bluefruit.Periph.setConnectCallback ([](uint16_t) -> void {}) ;
  Bluefruit.Periph.setDisconnectCallback ([](uint16_t, uint8_t r) -> void {
#ifdef __DEBUG_LOGGER__
    Serial.print ("[Debug] BLE connection lost for reason ") ;
    Serial.println (String (r, DEC)) ;
#endif
  }) ;
  // Set connection secured callback, invoked when connection is encrypted
  Bluefruit.Security.setSecuredCallback ([](uint16_t) -> void {}) ;
  //
  Bluefruit.Advertising.addFlags (BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE) ;
  Bluefruit.Advertising.addTxPower () ;
  Bluefruit.Advertising.addName () ;
  //
  Bluefruit.Advertising.setStopCallback ([](void) -> void {}) ;
  Bluefruit.Advertising.restartOnDisconnect (true) ;
  Bluefruit.Advertising.setInterval (32, 244) ;    // in units of 0.625 ms
  Bluefruit.Advertising.setFastTimeout (30) ;      // number of seconds in fast mode
  Bluefruit.Advertising.start (0) ;                // Infinitely run advertising 
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