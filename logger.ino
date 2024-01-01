#include "global.h"
#include "sensor.h"
#include "logger.h"
//
const unsigned long waitForCmd = 5000, loggerFadeOut = 60000, loggerInterval = 50 ;
//
unsigned long loggerLastWrite = 0 ;
unsigned int lastSampleLogged = 0 ;
bool loggerEnabled = false ;
//
static inline float digit2hcPa (int dgs) {
  //  
  //  Voltage divider is 100 k / 147 k
  //  in order to scale max. out < 3.3V
  //  Sensor zero offset is ~ 33 digits raw
  //  MXP555 sensitvity is 9.0 mV / kPa
  //  ADC conversion rate is 5 / 1.024 mV / digit 
  //
  static const float sca = float (1.47 * 5.0/1024 / 0.9) ;
  //
  return float (max (dgs - 33, 0)) * sca ;
  //
}
//
void loggerSetup (void) {
  //
  bool loggerCmd = true ;
  //
  pinMode (loggerPin, OUTPUT) ;
  digitalWrite (loggerPin, LOW) ;
  //
  buttonPressedTime = 0 ;
  //
#ifdef __DEBUG__RPAC__
  Serial.print ("You may enter 'n' or press button within ") ;
  Serial.print (String (waitForCmd / 1000, DEC)) ;
  Serial.println (" sec. to disable logging.") ;
#endif
  //
  for (unsigned long mytime = millis () ; mytime + waitForCmd > millis () ; delay (200)) {
    //
    digitalWrite (LED_BUILTIN, HIGH) ;
    //
    if (buttonPressedTime > 0) {
      //
#ifdef __DEBUG__RPAC__
      Serial.println ("Button has been pressed ...") ;
#endif
      //
      loggerCmd = false ;
      //
      buttonPressedTime = 0 ;
      //
      break ;
      //
    }
    //
    if (Serial.available () > 0) {
      //
      int msg = Serial.read () ;
      //
      if (msg == 'n') {
        //
        for (int i = 0; i < 20; i++) {
          //
          digitalWrite (LED_BUILTIN, HIGH);
          delay (10) ;
          digitalWrite (LED_BUILTIN, LOW);
          delay (50) ;
          //
        }
        //
        loggerCmd = false ;
        //
        break ;
        //
      }
      //
    }
    //
    delay (200) ;
    //
    digitalWrite (LED_BUILTIN, LOW) ;
    //
  }
  //
  if (loggerCmd && loggerFlag) {
    //
    //  Digital pin #__ powers the data logger;
    //  Allow the logger to boot before opening
    //  serial connection and awaiting response.
    //
    Serial1.begin (57600) ;
    //
    delay (100) ;
    //
    digitalWrite (loggerPin, HIGH) ;
    //
    while (Serial1.available () == 0) ;
    //
    String msg = Serial1.readString () ;
    //
    msg.trim () ;
    //
    if (msg.compareTo ("12<") == 0) {
      //
      loggerEnabled = true ;
      //
#ifdef __DEBUG__RPAC__
      Serial.println ("Data logging is enabled.") ;
#endif
      //
    } else {
      //
#ifdef __DEBUG__RPAC__
      Serial.println ("Enabling of data logging failed: " + msg) ;
#endif
      //
      digitalWrite (loggerPin, LOW) ;
      //
#ifdef __DEBUG__RPAC__
      Serial.println ("Data logging is disabled.") ;
#endif
      //
      Serial1.end () ;
      //
    }
    //
  } else {
    //
#ifdef __DEBUG__RPAC__
    Serial.println ("Data logging is disabled.") ;
#endif
    //
  }
  //
}
//
void loggerLoop (const sensorData_t & data, String && message) {
  //
  unsigned long myTime = millis () ;
  //
  if (! loggerEnabled) return ;
  //
  if (data.sample > lastSampleLogged) {
    //
    if (myTime < loggerLastWrite + loggerInterval) {
      //
#ifdef __DEBUG__RPAC__
      Serial.println ("[WARNING] Logging frequency limit exceeded: Sample skipped.") ;
#endif
      //
      return ;
      //
    }
    //
    Serial1.print (String(data.sample, DEC)) ;
    Serial1.print (";") ;
    Serial1.print (String(data.time, DEC)) ;
    Serial1.print (";") ;
    Serial1.print (data.pulseOn) ;
    Serial1.print (";") ;
    Serial1.print (String (digit2hcPa (data.pressure), 2)) ;
    Serial1.print (";") ;
    Serial1.println (message) ;
    //
    loggerLastWrite = myTime ;
    lastSampleLogged = data.sample ;
    //
  } else if (myTime > data.time + loggerFadeOut) {
    //
    digitalWrite (loggerPin, LOW) ;
    //
#ifdef __DEBUG__RPAC__
    Serial.println ("Data logging is disabled.") ;
#endif
    //
    Serial1.end () ;
    //
    loggerEnabled = false ;
    //
  }
  //
}