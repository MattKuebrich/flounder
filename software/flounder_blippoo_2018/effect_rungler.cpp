// rungler
// adapted from:
// https://github.com/bastl-instruments/one-chip-modules/blob/e3cfc6c9cfab16f935b851e349dfa706e0ca5aec/RUNGLER/RUNGLER.ino

// block a = signal
// block b = trigger

//rate variables not used

#include <Arduino.h>
#include "effect_rungler.h"

void AudioEffectRungler::update(void)
{

  audio_block_t *blocka, *blockb;
  int16_t *pa, *pb, *end;
  static int16_t audio_in, prev_trig, trig;
  static short change;
  uint16_t runglerOut;

  //runglerByte=random(255);
  blocka = receiveWritable(0);
  blockb = receiveReadOnly(1);

  if (!blocka) {
    return;
  }

  pa = (int16_t *)(blocka->data);
  pb = (int16_t *)(blockb->data);

  end = (pa + AUDIO_BLOCK_SAMPLES);

  while (pa < end) {

    //prev_audio_in = audio_in;
    prev_trig = trig;
    trig = *pb++;

    if (prev_trig >= 0 && trig < 0) {
      change = 1;
    }

    else {
      change = 0;
    }


    if (change == 1 && audio_in >= 0) { // if clock and audio are both high...
      runglerByte = runglerByte << 1; //we shift bit...
      
      if (loop_enable) {
        bitWrite(runglerByte, 0, bitRead(runglerByte, 7)); //loop enable, take last bit and connect it to first
      } else {
        bitWrite(runglerByte, 0, 1); //then write new 1 bit to runglerByte
      }
    }

    if (change == 1 && audio_in < 0) { //if clock if high, but audio is not
      runglerByte = runglerByte << 1; //shift bit..

      if (loop_enable) {
        bitWrite(runglerByte, 0, bitRead(runglerByte, 7)); //loop enable, take last bit and connect it to first
      } else {
        bitWrite(runglerByte, 0, 0); //then write new 0 bit to runglerByte
      }

    }


    // this prints out every bit from runglerByte, for debugging.
    //for ( int8_t i = 7; i >= 0; i-- )
    //  Serial.print( bitRead( runglerByte, i ) );
    

    runglerOut = 0; // ?
    bitWrite(runglerOut, 0, bitRead(runglerByte, 5));
    bitWrite(runglerOut, 1, bitRead(runglerByte, 6));
    bitWrite(runglerOut, 2, bitRead(runglerByte, 7));

    runglerOut = map(runglerOut, 0, 7, 0, 32768); // 0 to 1v
    
    audio_in = *pa; // was from s&h
    *pa++ = runglerOut;
  }

  transmit(blocka);
  release(blocka);
  release(blockb);

}
