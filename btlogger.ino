//
//  (c) Bernhard Schupp, Frankfurt (2024-2025)
//
#if defined(ARDUINO_Seeed_XIAO_nRF52840)
//
#include <bluefruit.h>
#include "btlogger.h"
//
BLEService pService (BLEUuid ("cc133984-dc6c-444c-8b50-b2434eb7592f")) ;
BLECharacteristic pLineUpd (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f45")) ;
// BLECharacteristic pFlowUpd (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f46"), 4, true) ;
// BLECharacteristic pCntlUpd (BLEUuid ("380157bf-fc56-4440-a5be-34a660d16f47"), 1, true) ;
BLECharacteristic pPrgNum (BLEUuid ("5203b040-4076-472c-8949-d3039bd3b370")) ;
BLECharacteristic pPrgDef (BLEUuid ("5203b040-4076-472c-8949-d3039bd3b371")) ;
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
      Bluefruit.begin () ;
      Bluefruit.setTxPower (8) ;    // Check bluefruit.h for supported values
      //
      Bluefruit.Periph.setConnectCallback ([](uint16_t con) -> void {
        //
        (void) con ;
        pLineUpd.notify (instance->Logger::headLine ()) ;
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
        (void) con ;
        (void) rea ;
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
      Bluefruit.Advertising.setInterval (32, 244) ;       // in units of 0.625 ms
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
#ifdef __DEBUG__LOGGER__
      pLineUpd.setCccdWriteCallback (callback) ;
#endif
      pLineUpd.begin () ;
      //
      pPrgNum.setProperties (CHR_PROPS_READ) ;
      pPrgNum.setPermission (SECMODE_OPEN, SECMODE_NO_ACCESS) ;
      pPrgNum.setMaxLen (sizeof (uint8_t)) ;
      pPrgNum.write8 (static_cast <uint8_t> (maxUserProgramsNumber)) ;
      //
      pPrgNum.begin () ;
      //
      pPrgDef.setProperties (CHR_PROPS_READ || CHR_PROPS_WRITE) ;
      pPrgDef.setPermission (SECMODE_OPEN, SECMODE_OPEN) ;
      pPrgDef.setMaxLen (512) ;
      pPrgDef.setWriteCallback([](uint16_t con, BLECharacteristic* chr, uint8_t * dat, uint16_t len) -> void {
        (void) con ;
        (void) chr ;
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Program definition write callback.") ;
#endif        
        return ;
      }) ;
      //
      pPrgDef.begin () ;
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