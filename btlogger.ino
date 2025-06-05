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
static BLEService pService (BLEUuid ("cc133984-dc6c-444c-8b50-b2434eb7592f")) ;               // CC133984-DC6C-444C-8B50-B2434EB7592F
//
static BLECharacteristic pLineUpd (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f45")) ;        // 380157BF-FC56-4440-A5BE-34A660D16F45
static BLECharacteristic pPulse (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f46")) ;          // 380157BF-FC56-4440-A5BE-34A660D16F46
static BLECharacteristic pDuty (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f47")) ;           // 380157BF-FC56-4440-A5BE-34A660D16F47
static BLECharacteristic pClock (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f48")) ;          // 380157BF-FC56-4440-A5BE-34A660D16F48
//
struct rpac::BTLogger::hx rpac::BTLogger::handles [] {{.valid = false, .value = 0x0}} ;
//
bool rpac::BTLogger::initialized {false} ;
rpac::BTLogger * rpac::BTLogger::instance {nullptr} ;
//
void rpac::BTLogger::callback (uint16_t hdl, BLECharacteristic * chr, uint16_t cccd) {
    //
#ifdef __DEBUG__LOGGER__
    Serial.print ("CCCD Updated: ") ;
    Serial.print (cccd) ;
    Serial.println ("") ;
#endif
    //
    // Check the characteristic this CCCD update is associated 
    // with in case this handler is used for multiple CCCD records.
    //
    if (chr->uuid == pLineUpd.uuid) {
        //
        if (chr->notifyEnabled (hdl)) {
            //
            instance->wrmode (0u) ;
            //
#ifdef __DEBUG__LOGGER__
            Serial.println ("RPAC protocol 'Notify' enabled") ;
#endif
            //
        } else {
            //
            instance->wrmode (4u) ;
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
      Bluefruit.setTxPower (8) ;
      //
      Bluefruit.Periph.setConnectCallback ([](uint16_t con) -> void {
        //
        for (auto i = 0 ; i < maxConnectionsBLE ; i++) {
          //
          if (! handles [i].valid) {
            //
            handles [i] = {.valid = true, .value = con} ;
            //
            break ;
            //
          }
          //
        }
        //
#ifdef __DEBUG__LOGGER__
        char deviceName [32] {0x0} ;
        Bluefruit.Connection(con)->getPeerName (deviceName, sizeof (deviceName)) ;
        Serial.print ("[Debug] Connection 0x") ;
        Serial.print (String (con, HEX)) ;
        Serial.print (" establisehed with device ") ;
        Serial.println (deviceName) ;
#endif
        //
        instance->wrmode (4u) ;
        //
      }) ;
      //
      Bluefruit.Periph.setDisconnectCallback ([](uint16_t con, uint8_t rea) -> void {
        //
        (void) rea ;
        //
        for (auto i = 0 ; i < maxConnectionsBLE ; i++) {
          //
          if (handles [i].value == con) {
            //
            handles [i] = {.valid = false, .value = 0x0} ;
            //
          }
          //
        }
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
      Bluefruit.Advertising.restartOnDisconnect (false) ;
      Bluefruit.Advertising.setInterval (800, 800) ;      // in units of 0.625 ms
      Bluefruit.Advertising.setFastTimeout (30) ;         // number of seconds in fast mode
      //
      pService.begin () ;
      //
      pLineUpd.setProperties (CHR_PROPS_NOTIFY) ;
      pLineUpd.setPermission (SECMODE_OPEN, SECMODE_NO_ACCESS) ;
      pLineUpd.setMaxLen (maxLoggerLineLength) ;
      pLineUpd.write ("") ;
      pLineUpd.setCccdWriteCallback (callback) ;
      pLineUpd.setUserDescriptor ("Protocol line human readable") ;
      pLineUpd.begin () ;
      //
      pPulse.setProperties (CHR_PROPS_WRITE) ;
      pPulse.setPermission (SECMODE_OPEN, SECMODE_OPEN) ;
      pPulse.setFixedLen (sizeof (uint16_t)) ;
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
      pDuty.setProperties (CHR_PROPS_READ | CHR_PROPS_WRITE) ;
      pDuty.setPermission (SECMODE_OPEN, SECMODE_OPEN) ;
      pDuty.setFixedLen (sizeof (uint8_t)) ;
      pDuty.write16 (80) ;
      pDuty.setUserDescriptor ("Pulse duty factor: 1...100% (write only)") ;
      pDuty.setWriteCallback ([](uint16_t, BLECharacteristic *, uint8_t * dat, uint16_t) -> void {
        //
        uint8_t dutyCycle {80} ;  // integral percentage %
        //
        dutyCycle = static_cast <uint8_t> (dat [0]) < 101 ? static_cast <uint8_t> (dat [0]) : 100 ;
        //
        rpac::Pulser <rpacPin_t::pulser>::remoteDuty (dutyCycle) ;
        //
        return ;
        //
      }) ;
      //
      pDuty.begin () ;
      //
      pClock.setProperties (CHR_PROPS_READ | CHR_PROPS_WRITE) ;
      pClock.setPermission (SECMODE_OPEN, SECMODE_OPEN) ;
      pClock.setMaxLen (sizeof (uint8_t)) ;
      pClock.write8 (42) ;
      pClock.setWriteCallback([](uint16_t con, BLECharacteristic* chr, uint8_t * dat, uint16_t len) -> void {
        //
        (void) chr ;
        //
      });
      //
      pClock.begin () ;
      //
      initialized = true ;
      //
      instance->wrmode (1u) ;
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logging via BLE server.") ;
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
    case 10u :
      //
      Bluefruit.Advertising.start (30) ;  // 30 seconds advertising
      //
      mode = 0u ;
      //
      [[fallthrough]];
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
      {
        //
        const char * const line {Logger::dataLine ()} ;
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
      }
      //
      return false ;
      //
    case 1u :
      //
      Bluefruit.Advertising.start (30) ;  // 30 seconds advertising
      //
      mode = 2u ;
      //
      [[fallthrough]];
      //
    case 2u :
      //
      return false ;
      //
    case 3u :
      //
      for (auto i = 0 ; i < maxConnectionsBLE ; i++) {
        //
        if (handles [i].valid) Bluefruit.disconnect (handles [i].value) ;
        //
        handles [i] = {.valid = false, .value = 0x0} ;
        //
      }
      //
      [[fallthrough]] ;
      //
    case 4u :
      //
      {
        //
        bool cons {false}, logs {false} ;
        //
        for (auto i = 0 ; i < maxConnectionsBLE ; i++) {
          //
          if (handles [i].valid) {
            //
            cons = true ;
            //
            if (pLineUpd.notifyEnabled (handles [i].value)) {
              //
              pLineUpd.notify (instance->Logger::headLine ()) ;
              //
              instance->wrmode (0u) ;
              //
              logs = true ;
              //
              break ;
              //
            }
            //
          }
          //
        }
        //
        rpac::Pulser <rpacPin_t::pulser>::toggle (cons ? rpac::Pulser <rpacPin_t::pulser>::mode_t::mBLE : rpac::Pulser <rpacPin_t::pulser>::mode_t::mBase) ;
        //
        instance->wrmode (logs ? 0u : 5u) ;
        //
      }
      //
      [[fallthrough]] ;
      //
    case 5u :
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