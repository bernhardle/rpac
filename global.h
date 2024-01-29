#ifndef __global_h_included__
#define __global_h_included__
//
#define __DEBUG__RPAC__
//
#ifdef __DEBUG__RPAC__
// #define __DEBUG__BUTTON__
// #define __DEBUG_CONTROL__
#define __DEBUG__FLOW__
#define __DEBUG__LOGGER__
#define __DEBUG__PRESSURE__
#define __DEBUG__PULSER__
#define __DEBUG__RELAIS__
#define __DEBUG__RTC__
#define __DEBUG__SIGNAL__
#endif
//
typedef class loggerCBs loggerCBs_t ;
//
#ifdef ARDUINO_UBLOX_NINA_W10
#define LED_BUILTIN LED_BLUE
typedef uint8_t pin_size_t ;
#endif
//
#endif
