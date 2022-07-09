// sample and hold

// adapted from:
// https://forum.pjrc.com/threads/29227-Rate-Reduction-effect-for-audio-h-code-and-questions?p=78290&viewfull=1#post78290

// block a = signal to sample
// block b = trigger

//rate variables removed from this

#include <Arduino.h>
#include "effect_samplehold.h"

void AudioEffectSampleAndHold::update(void)
{

  audio_block_t *blocka, *blockb;
  int16_t *pa, *pb, *end;
  static int16_t prev_audio_in, audio_in, prev_trig, trig;
  //static int16_t rate_cnt, rate_cnt_max;
  static short change;

  blocka = receiveWritable(0);
  blockb = receiveReadOnly(1);

  if (!blocka) {
    return;
  }

  pa = (int16_t *)(blocka->data);
  pb = (int16_t *)(blockb->data);

  end = (pa + AUDIO_BLOCK_SAMPLES);

  while (pa < end) {

    prev_audio_in = audio_in;
    prev_trig = trig;
    trig = *pb++;

    if (prev_trig >= 0 && trig < 0) {
      change = 1;
      //rate_cnt_max = rate_cnt;
      //Serial.print(rate_cnt);
      //rate_cnt = 0;

    }

    else {
      change = 0;
      //rate_cnt++;
    }

    if (change == 1) {
      audio_in = *pa;
    }

    if (change == 0) {
      audio_in = prev_audio_in;
    }
    *pa++ = audio_in;
  }

  transmit(blocka);
  release(blocka);
  release(blockb);

}
