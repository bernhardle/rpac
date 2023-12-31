#include <EEPROM.h>
//
const int pulsePin = 10, sensorPin = A0, loggerPin = 15, buttonPin = 16, pulseVars = 5 ;
const unsigned long waitForCmd = 5000, logRes = 4, loopDura = 50, loggingFadeOut = 5000 ;
const unsigned long pulseOnDura [pulseVars] = {500, 1000, 2000, 3000, 4500} ;
const unsigned long pulseOffDura [pulseVars] = {9500, 6000, 4000, 3000, 2000} ;
const unsigned int pulseProgress [pulseVars] = {12, 12, 17, 21, 53} ;
const bool debug = true, logFlag = true ;
//
volatile unsigned long buttonPressedTime = 0 ;
unsigned int loopCount = 0, pulseMode = 0, pulseProgressCount = 0 ;
volatile unsigned long pulseChangeTime = 0 ;
volatile bool pulseEnabled = true, loggingEnabled = false ;
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
//  Handles the falling edge interrupt caused by switch closing contacts
//
void handler () {
  //
  buttonPressedTime = millis () ;
  //
}
//
void pulseControlSetup (volatile unsigned long & lastChange) {
  //
  pulseMode = 0 ;
  pulseProgressCount = 0 ;
  lastChange = millis () ;
  digitalWrite (pulsePin, LOW) ;
  digitalWrite (LED_BUILTIN, LOW) ;
  //
}
//
void dataLoggerSetup () {
  //
  bool logCmd = true ;
  //
  buttonPressedTime = 0 ;
  //
  Serial.println ("You may enter 'n' or press button within 5 sec. to disable logging.") ;
  //
  for (unsigned long mytime = millis () ; mytime + waitForCmd > millis () ; delay (200)) {
    //
    digitalWrite (LED_BUILTIN, HIGH) ;
    //
    if (buttonPressedTime > 0) {
      //
      Serial.println ("Button has been pressed ...") ;
      //
      logCmd = false ;
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
        logCmd = false ;
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
  if (logCmd && logFlag) {
    //
    //  Digital pin #15 powers the data logger;
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
      loggingEnabled = true ;
      //
      Serial.println ("Data logging is enabled.") ;
      //
    } else {
      //
      Serial.println ("Enabling of data logging failed: " + msg) ;
      //
      digitalWrite (loggerPin, LOW) ;
      //
      Serial.println ("Data logging is disabled.") ;
      //
      Serial1.end () ;
      //
    }
    //
  } else {
    //
    Serial.println ("Data logging is disabled.") ;
    //
  }
  //
}
//
// the setup function runs once when you press reset or power the board
//
void setup() {
  //
  pinMode (LED_BUILTIN, OUTPUT) ;
  pinMode (sensorPin, INPUT) ;
  pinMode (buttonPin, INPUT_PULLUP) ;
  pinMode (pulsePin, OUTPUT) ;
  pinMode (loggerPin, OUTPUT) ;
  //
  digitalWrite (LED_BUILTIN, HIGH) ;
  digitalWrite (pulsePin, LOW) ;
  digitalWrite (loggerPin, LOW) ;
  //
  delay (1500) ;
  //
  digitalWrite (LED_BUILTIN, LOW) ;
  //
  Serial.begin (115200) ;
  //
  attachInterrupt(digitalPinToInterrupt(buttonPin), handler, FALLING) ;
  //
  delay (500) ;
  //
  dataLoggerSetup () ;
  //
  pulseControlSetup (pulseChangeTime) ;
  //
}
//
static void logDataLoop (String && message) {
  //
  Serial1.print (String(millis (), DEC)) ;
  Serial1.print (";") ;
  Serial1.print (digitalRead (pulsePin)) ;
  Serial1.print (";") ;
  Serial1.print (String (digit2hcPa (analogRead(sensorPin)), 2)) ;
  Serial1.print (";") ;
  Serial1.println (message) ;
    //
}
//
void pulseControlLoop (volatile unsigned long & lastChange, unsigned int & lastCount) {
  //
  unsigned long myTime = millis () ;
  //
  if (digitalRead (pulsePin)) {
    //
    if (myTime > lastChange + pulseOnDura [pulseMode]) {
      //
      digitalWrite (pulsePin, LOW) ;
      digitalWrite (LED_BUILTIN, LOW) ;
      lastChange = myTime ;
      //
      if (++ lastCount > pulseProgress [pulseMode]) {
        //
        if (pulseMode < pulseVars - 1) {
          //
          pulseMode ++ ;
          lastCount = 0 ;
          //
          Serial.print ("Switching pulse mode to level ") ;
          Serial.print (String(pulseMode + 1, DEC)) ;
          Serial.print ("/") ;
          Serial.print (String(pulseVars, DEC)) ;
          Serial.print (" ... ON=") ;
          Serial.print (String(pulseOnDura [pulseMode], DEC)) ;
          Serial.print ("ms, OFF=") ;
          Serial.print (String(pulseOffDura [pulseMode], DEC)) ;
          Serial.println ("ms.") ;
          //
        } else {
          //
          pulseEnabled = false ;
          Serial.print ("Disabeling pulses.") ;
          //
        }
        //
      }
      //
    }
  } else {
    //
    if (myTime > lastChange + pulseOffDura [pulseMode]) {
      //
      digitalWrite (pulsePin, HIGH) ;
      digitalWrite (LED_BUILTIN, HIGH) ;
      lastChange = myTime ;
      //
    }
    //
  }
  //
}
//
// the loop function runs over and over again forever
//
void loop() {
  //
  unsigned long loopBegin = millis () ;
  //
  if (buttonPressedTime) {
    //
    noInterrupts () ;
    //
    Serial.print ("Button was pressed @ ") ;
    Serial.print (String (buttonPressedTime)) ;
    Serial.println (" ms from start.") ;
    //
    buttonPressedTime = 0 ;
    //
    interrupts () ;
    //
  }
  //
  if (pulseEnabled) pulseControlLoop (pulseChangeTime, pulseProgressCount) ;
  //
  if (loggingEnabled && (0 == loopCount % logRes) && (pulseEnabled || loopBegin < pulseChangeTime + loggingFadeOut)) logDataLoop ("-/-") ;
  //
  loopCount ++ ;
  //
  delay (loopDura - (millis () - loopBegin)) ;
  //
}
//