//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#if defined(ARDUINO_Seeed_XIAO_nRF52840)
//
#include <bluefruit.h>
#include "btlogger.h"
//
BLEService pService (BLEUuid ("cc133984-dc6c-444c-8b50-b2434eb7592f")) ;
BLECharacteristic pLineUpd (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f45")) ;
//
//BLEService rpacs = BLEService ({0xcc, 0x13, 0x39, 0x84,/**/0xdc, 0x6c, /**/0x44, 0x4c, /**/ 0x8b, 0x50, /**/0xb2, 0x43, 0x4e, 0xb7, 0x59, 0x2f}) ;
//BLECharacteristic rpacp = BLECharacteristic(UUID16_CHR_HEART_RATE_MEASUREMENT);
//
bool rpac::BTLogger::initialized {false} ;
rpac::BTLogger * rpac::BTLogger::instance {nullptr} ;
//
#ifdef __DEBUG__LOGGER__
//
static void callback (uint16_t hdl, class BLECharacteristic * chr, uint16_t cccd) {
    //
    // Display the raw request packet
    //
    Serial.print ("CCCD Updated: ") ;
    Serial.print (cccd) ;
    Serial.println ("") ;
    //
    // Check the characteristic this CCCD update is associated with in case
    // this handler is used for multiple CCCD records.
    //
    if (chr->uuid == pLineUpd.uuid) {
        //
        if (chr->notifyEnabled (hdl)) {
            //
            Serial.println ("RPAC protocol 'Notify' enabled") ;
            //
        } else {
            //
            Serial.println ("RPAC protocol 'Notify' disabled") ;
            //
        }
    }
    //
}
//
#endif
//
rpac::BTLogger::BTLogger (loggerCBs_t & cbs) : Logger (cbs) {
  //
  pService.begin () ;
  pLineUpd.setProperties (CHR_PROPS_NOTIFY) ;
  pLineUpd.setPermission (SECMODE_OPEN, SECMODE_NO_ACCESS) ;
  pLineUpd.setFixedLen (4) ;
  pLineUpd.write32 (0x00000000) ;
#ifdef __DEBUG__LOGGER__
  pLineUpd.setCccdWriteCallback (callback) ;
#endif
  pLineUpd.begin () ;
  //
}
//
void rpac::BTLogger::setup (loggerCBs_t & cbs) {
  //
  Bluefruit.begin ();
  Bluefruit.setTxPower (4) ;    // Check bluefruit.h for supported values
  //
  Bluefruit.Periph.setConnectCallback ([](uint16_t con) -> void {
    (void) con ;
#ifdef __DEBUG__LOGGER__
    char dev [32] {0} ;
    Bluefruit.Connection(con)->getPeerName (dev, sizeof(dev)) ;
    Serial.print ("[Debug] Connection establisehed with ") ;
    Serial.println (dev) ;
#endif    
  }) ;
  //
  Bluefruit.Periph.setDisconnectCallback ([](uint16_t con, uint8_t rea) -> void {
    (void) con ;
    (void) rea ;
#ifdef __DEBUG__LOGGER__
    char dev [32] {0} ;
    Bluefruit.Connection(con)->getPeerName (dev, sizeof(dev)) ;
    Serial.print ("[Debug] Connection with ") ;
    Serial.print (dev) ;
    Serial.print (" lost for reason ") ;
    Serial.println (String (rea, DEC)) ;
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
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logging to BLE server.") ;
      //
    } else {
      //
      Serial.println ("[ERROR] BTLogger::setup() failed.") ;
#endif
      //
    }
    //
  }
  //
}
//
bool rpac::BTLogger::loop (unsigned long int mytime) {
  //
  switch (mode) {
    //
    case 0 :
      //
      while (mytime > loggerNextSampleTime) loggerNextSampleTime += loggerSampleInterval ;
      //
      if (mytime < loggerNextSampleTime - loggerSampleAdjust) { return false ; }
      //
      delay (loggerNextSampleTime - mytime) ;
      //
      loggerNextSampleTime += loggerSampleInterval ;
      //
      (void) Logger::dataLine () ;
      //
      //
      if (Bluefruit.connected ()) {
        //
        if (pLineUpd.notify (&mytime, sizeof(mytime))) {
          //
#ifdef __DEBUG__LOGGER__
          Serial.print ("Time updated to: ") ;
          Serial.println (mytime) ;
#endif
          return true ;
          //
        } else {
          //
#ifdef __DEBUG__LOGGER__
          Serial.println ("ERROR: Time not set in the CCCD or not connected!") ;
          //
#endif
        }
        //
      }
      //
      return false ;
      //
    default :
      //
      return false ;
      //
    //
  }
  //
}
//
#endif