//
//  (c) Bernhard Schupp, Frankfurt (2024-2025)
//
#if defined(ARDUINO_Seeed_XIAO_nRF52840)
//
#include <bluefruit.h>
#include "Adafruit_TinyUSB.h"
#include "global.h"
#include "btlogger.h"
#include "pulser.h"
//
static BLEService pService (BLEUuid ("cc133984-dc6c-444c-8b50-b2434eb7592f")) ;               // cc133984dc6c444c8b50b2434eb7592f
static BLECharacteristic pLineUpd (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f45")) ;        // 380157bffc564440a5be34a660d16f45
static BLECharacteristic pPulse (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f46"), 2, true) ; // 
static BLECharacteristic pDuty (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f47"), 1, true) ;  //
static BLECharacteristic pClock (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f48"), 1, true) ; //
//
bool rpac::BTLogger::initialized {false} ;
rpac::BTLogger * rpac::BTLogger::instance {nullptr} ;
//
static void callback (uint16_t hdl, class BLECharacteristic * chr, uint16_t cccd) {
    //
#ifdef __DEBUG__LOGGER__
    Serial.print ("CCCD Updated: ") ;
    Serial.print (cccd) ;
    Serial.println ("") ;
#endif
    //
    // Check the characteristic this CCCD update is associated with in case
    // this handler is used for multiple CCCD records.
    //
    if (chr->uuid == pLineUpd.uuid) {
        //
        if (chr->notifyEnabled (hdl)) {
            //
#ifdef __DEBUG__LOGGER__
            Serial.println ("RPAC protocol 'Notify' enabled") ;
#endif
            //
            // pLineUpd.notify (instance->Logger::headLine ()) ;
            //
        } else {
            //
#ifdef __DEBUG__LOGGER__
            Serial.println ("RPAC protocol 'Notify' disabled") ;
#endif
            //
        }
    }
    //
}
//
rpac::BTLogger::BTLogger (loggerCBs_t & cbs, unsigned int cyc, unsigned int adj) : Logger (cbs), sampleInterval (cyc), sampleAdjust (adj) {
  //
  //
}
//
void rpac::BTLogger::setup (loggerCBs_t & cbs, unsigned int cyc, unsigned int adj) {
  //
  if (!initialized) {
    //
    instance = new BTLogger (cbs, cyc, adj) ;
    //
    if (*instance) {
      //
      Bluefruit.begin (maxConnectionsBLE, 0) ;
      Bluefruit.setTxPower (8) ;    // Check bluefruit.h for supported values
      //
      Bluefruit.Periph.setConnectCallback ([](uint16_t con) -> void {
        //
        (void) con ;
        //
        rpac::Pulser <rpacPin_t::pulser>::toggle (rpac::Pulser <rpacPin_t::pulser>::mode_t::mBLE) ;
        //
#ifdef __DEBUG__LOGGER__
        char deviceName [32] {0x0} ;
        Bluefruit.Connection(con)->getPeerName (deviceName, sizeof (deviceName)) ;
        Serial.print ("[Debug] Connection 0x") ;
        Serial.print (String (con, HEX)) ;
        Serial.print (" establisehed with device ") ;
        Serial.println (deviceName) ;
#endif
        instance->wrmode (0u) ;
        //
      }) ;
      //
      Bluefruit.Periph.setDisconnectCallback ([](uint16_t con, uint8_t rea) -> void {
        //
        (void) con ;
        (void) rea ;
        //
        rpac::Pulser <rpacPin_t::pulser>::toggle (rpac::Pulser <rpacPin_t::pulser>::mode_t::mBase) ;
        //
#ifdef __DEBUG__LOGGER__
        Serial.print ("[Debug] Connection 0x") ;
        Serial.print (con, HEX) ;
        Serial.print (" lost for reason 0x") ;
        Serial.println (String (rea, HEX)) ;
#endif
        instance->wrmode (4u) ;
        //
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
      Bluefruit.Advertising.setInterval (800, 800) ;      // in units of 0.625 ms
      Bluefruit.Advertising.setFastTimeout (30) ;         // number of seconds in fast mode
      Bluefruit.Advertising.start (0) ;                   // Infinitely run advertising
      //
      pService.begin () ;
      //
      //  typedef void (*write_cb_t)       (uint16_t, BLECharacteristic* , uint8_t* , uint16_t) ;
      //  typedef void (*write_cccd_cb_t)  (uint16_t, BLECharacteristic* , uint16_t) ;
      //
      pLineUpd.setProperties (CHR_PROPS_NOTIFY) ;
      pLineUpd.setPermission (SECMODE_OPEN, SECMODE_NO_ACCESS) ;
      pLineUpd.setMaxLen (maxLoggerLineLength) ;
      pLineUpd.write ("") ;
      pLineUpd.setCccdWriteCallback (callback) ;
      pLineUpd.begin () ;
      //
      pPulse.setProperties (CHR_PROPS_WRITE) ;
      pPulse.setPermission (SECMODE_OPEN, SECMODE_OPEN) ;
      pPulse.setWriteCallback([](uint16_t con, BLECharacteristic* chr, uint8_t * dat, uint16_t len) -> void {
        //
        uint16_t duration {0} ;
        //
        duration += static_cast <uint16_t> (dat [0]) << 8 ;
        duration += static_cast <uint16_t> (dat [1]) ;
        //
        (void) con ;
        (void) chr ;
        //
        rpac::Pulser <rpacPin_t::pulser>::remotePulse (duration) ;
        //
        return ;
        //
      }) ;
      //
      pPulse.begin () ;
      //
      pDuty.setProperties (CHR_PROPS_WRITE) ;
      pDuty.setPermission (SECMODE_OPEN, SECMODE_OPEN) ;
      pDuty.setUserDescriptor ("Pulse duty factor: 1...100% (write only)") ;
      pDuty.setWriteCallback([](uint16_t con, BLECharacteristic* chr, uint8_t * dat, uint16_t len) -> void {
        //
        uint8_t dutyCycle {80} ;  // integral percentage %
        //
        dutyCycle = static_cast <uint8_t> (dat [0]) < 101 ? static_cast <uint8_t> (dat [0]) : 100 ;
        //
        return ;
        //
      }) ;
      //
      pDuty.begin () ;
      //
      pClock.setProperties (CHR_PROPS_READ | CHR_PROPS_WRITE) ;
      pClock.setPermission (SECMODE_OPEN, SECMODE_OPEN) ;
      pClock.setWriteCallback([](uint16_t con, BLECharacteristic* chr, uint8_t * dat, uint16_t len) -> void {
        //
        (void) chr ;
        //
      });
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
      while (mytime > nextSampleTime) nextSampleTime += sampleInterval ;
      //
      if (mytime < nextSampleTime - sampleAdjust) { return false ; }
      //
      delay (nextSampleTime - mytime) ;
      //
      nextSampleTime += sampleInterval ;
      //
      if (Bluefruit.connected ()) {
        //
        const char * line = Logger::dataLine () ;
        //
        if (pLineUpd.notify (line)) {
          //
#ifdef __DEBUG__LOGGER__
          Serial.print ("[Debug] Notified '") ;
          Serial.print (line) ;
          Serial.println ("'") ;
#endif
          return true ;
          //
        } 
        //
#ifdef __DEBUG__LOGGER__
          Serial.println ("[Debug] Notifications not fully set in all CCCD.") ;
#endif
        //
      }
      //
      return false ;
      //
    case 4 :
      //
      return false ;
      //
    case 5 :
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