//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __global_h_included__
#define __global_h_included__
//
#define __DEBUG__RPAC__
//
#ifdef __DEBUG__RPAC__
#define __DEBUG__BUTTON__
#define __DEBUG_CONTROL__
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
#include <array>
template <typename A, int n> using Array = std::array <A, n> ;
enum struct rpacPin {logger = 15, button = 27, flow = 33, pressure = A0, pulser = 10, relais = LED_RED, signal = LED_BLUE} ;
#else
#include <Array.h>
enum struct rpacPin {logger = 15, button= 16, flow = 7, pressure = A0, pulser = 10, relais = 2, signal = LED_BUILTIN} ;
#endif
//
typedef rpacPin rpacPin_t ;
//
#endif
