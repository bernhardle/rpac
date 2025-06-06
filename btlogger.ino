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
uint16_t rpac::BTLogger::connections {0x0000} ;
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
            instance->wrmode (mode_t::LOG) ;
            //
#ifdef __DEBUG__LOGGER__
            Serial.println ("RPAC protocol 'Notify' enabled") ;
#endif
            //
        } else {
            //
            instance->wrmode (mode_t::INVENTORY) ;
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
rpac::BTLogger::BTLogger (loggerCBs_t & cbs, uint16_t cyc, uint16_t adj) : Logger (cbs), sampleInterval (cyc), sampleAdjust (adj) {
  //
  //
}
//
void rpac::BTLogger::setup (loggerCBs_t & cbs, uint16_t cyc, uint16_t adj) {
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
        connections ++ ;
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
        instance->wrmode (mode_t::INVENTORY) ;
        //
      }) ;
      //
      Bluefruit.Periph.setDisconnectCallback ([](uint16_t con, uint8_t rea) -> void {
        //
        (void) rea ;
        //
        connections -- ;
        //
#if defined(__INFO__LOGGER__) || defined(__DEBUG__LOGGER__)
        Serial.print ("[INFO] Connection 0x") ;
        Serial.print (con, HEX) ;
        Serial.print (" lost for reason 0x") ;
        Serial.println (String (rea, HEX)) ;
#endif
        instance->wrmode (mode_t::INVENTORY) ;
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
        uint16_t dur {0} ;
        //
        dur += static_cast <uint16_t> (dat [0]) << 8 ;
        dur += static_cast <uint16_t> (dat [1]) ;
        //
        (void) con ;
        (void) chr ;
        //
        rpac::Pulser <rpacPin_t::pulser>::remotePulse (dur) ;
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
        rpac::Pulser <rpacPin_t::pulser>::remoteDuty (static_cast <float> (dat [0]) < 101 ? static_cast <uint8_t> (dat [0]) : 100) ;
        //
      }) ;
      //
      pDuty.begin () ;
      //
      pClock.setProperties (CHR_PROPS_READ | CHR_PROPS_WRITE) ;
      pClock.setPermission (SECMODE_OPEN, SECMODE_OPEN) ;
      pClock.setMaxLen (sizeof (uint8_t)) ;
      pClock.write8 (42) ;
      pClock.setWriteCallback ([](uint16_t con, BLECharacteristic* chr, uint8_t * dat, uint16_t len) -> void {
        //
        (void) con ;
        (void) chr ;
        (void) dat ;
        (void) len ;
        //
      });
      //
      pClock.begin () ;
      //
      initialized = true ;
      //
      instance->wrmode (mode_t::ADV_ONLY) ;
      //
#if defined(__INFO__LOGGER__) || defined(__DEBUG__LOGGER__)
      Serial.println ("[INFO] Data logging via BLE server.") ;
#endif
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
bool rpac::BTLogger::loop (uint32_t mytime) {
  //
  switch (mode) {
    //
    case mode_t::ADV_LOG :
      //
      if (connections < maxConnectionsBLE) {
        Bluefruit.autoConnLed (true) ;
        Bluefruit.Advertising.start (30) ;  // 30 seconds advertising
#if defined(__INFO__LOGGER__) || defined(__DEBUG__LOGGER__)
        Serial.print ("[INFO] BTLogger::loop () Advertising while logging started.") ;
      } else {
        Serial.print ("[INFO] BTLogger::loop () Advertising skipped.") ;
#endif
      }      //
      mode = mode_t::LOG ;
      //
      [[fallthrough]];
      //
    case mode_t::LOG :
      //
      while (mytime > nextSampleTime) nextSampleTime += sampleInterval ;
      //
      if (mytime < nextSampleTime - sampleAdjust) { return false ; }
      //
      delay (nextSampleTime - mytime) ;
      //
      nextSampleTime += underSample * sampleInterval ;
      //
      {
        //
        const char * const line {Logger::dataLine ()} ;
        uint16_t notified {0u} ;
        //
        for (auto i = 0 ; i < maxConnectionsBLE ; i ++) {
          //
          if (Bluefruit.connected(i) && pLineUpd.notifyEnabled(i)) notified += pLineUpd.notify (i, line) ? 1 : 0 ;
          //
        }
        //
        if (notified) {
          //
  #ifdef __DEBUG__LOGGER__
          Serial.print ("[DEBUG] BTLogger::loop () Notified '") ;
          Serial.print (line) ;
          Serial.print ("' to ") ;
          Serial.print (notified) ;
          Serial.println (" clients.") ;
  #endif
          return true ;
          //
        }
        //
      }
      //
      return false ;
      //
    case mode_t::ADV_ONLY :
      //
      if (connections < maxConnectionsBLE) {
        Bluefruit.autoConnLed (true) ;
        Bluefruit.Advertising.start (30) ;  // 30 seconds advertising
#if defined(__INFO__LOGGER__) || defined(__DEBUG__LOGGER__)
        Serial.print ("[INFO] BTLogger::loop () Advertising only started.") ;
      } else {
        Serial.print ("[INFO] BTLogger::loop () Advertising skipped.") ;
#endif
      }
      //
      mode = mode_t::IDLE;
      //
      return false ;
      //
    case mode_t::SHUTDOWN :
      //
#if defined(__INFO__LOGGER__) || defined(__DEBUG__LOGGER__)
      Serial.print ("[INFO] BTLogger::loop () Shutdown.") ;
#endif
      for (auto i = 0 ; i < maxConnectionsBLE ; i++) {
        //
        if (Bluefruit.connected (i)) Bluefruit.disconnect (i) ;
        //
      }
      //
      [[fallthrough]] ;
      //
    case mode_t::INVENTORY :
      //
#if defined(__INFO__LOGGER__) || defined(__DEBUG__LOGGER__)
      Serial.print ("[INFO] BTLogger::loop () Take inventory.") ;
#endif
      {
        //
        uint8_t cons {0}, logs {0} ;
        const char * const line {instance->Logger::headLine ()} ;
        //
        for (auto i = 0 ; i < maxConnectionsBLE ; i ++) {
          //
          if (Bluefruit.connected (i)) {
            //
            cons ++ ;
            //
            if (pLineUpd.notifyEnabled (i)) {
              //
              logs ++ ;
              //
              pLineUpd.notify (i, line) ? 1 : 0 ;
              //
            }
            //
          }
          //
        }
        //
#if defined(__INFO__LOGGER__) || defined(__DEBUG__LOGGER__)
        Serial.print ("[INFO] BTLogger::loop () connections: ") ;
        Serial.print (cons) ;
        Serial.print (", logging: ") ;
        Serial.print (logs) ;
        Serial.println (".") ;
#endif
        rpac::Pulser <rpacPin_t::pulser>::remoteOperation (cons > 0) ;
        //
        instance->wrmode (logs > 0 ? mode_t::LOG : mode_t::IDLE) ;
        //
      }
      //
      [[fallthrough]] ;
      //
    case mode_t::IDLE :
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