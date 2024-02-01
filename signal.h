#ifndef __signal_h_included__
#define __signal_h_included__
//
#include "global.h"
//
typedef struct signalSequence {
  //
  short unsigned rep = 0 ;
  short unsigned len = 0 ;
  const short unsigned (*seq)[2] = nullptr ;
  signalSequence (short unsigned a, short unsigned b, const short unsigned (*c)[2]) : rep (a), len (b), seq(c) {}
  //
} signalSequence_t ;
//
const short unsigned seqHB [2][2] {{2,5},{2,10}} ;
const signalSequence_t heartBeat (0,2, seqHB) ;
const signalSequence_t blankSeq (0, 0, nullptr) ;
//
extern void signalSetup (rpacPin_t) ;
extern void signalLoop (bool, const signalSequence_t & = heartBeat) ;
//
#endif