// comparator

// block a = signal 1
// block b = signal 2


#include <Arduino.h>
#include "effect_comparator.h"

void AudioEffectComparator::update(void)
{
  audio_block_t *blocka, *blockb;
  uint32_t *pa, *pb, *end;
  uint32_t a12, a34;
  uint32_t b12, b34;

  blocka = receiveWritable(0);
  blockb = receiveReadOnly(1);

  if (!blocka) {
    if (blockb) release(blockb);
    return;
  }
  if (!blockb) {
    release(blocka);
    return;
  }
  pa = (uint32_t *)(blocka->data);
  pb = (uint32_t *)(blockb->data);
  end = pa + AUDIO_BLOCK_SAMPLES / 2;

  while (pa < end) {
    a12 = *pa;
    a34 = *(pa + 1);
    b12 = *pb++;
    b34 = *pb++;


if (a12 > b12){
  a12 = 32768;
} else {
  a12 = 0;
}

if (a34 > b34){
  a34 = 32768;
} else {
  a34 = 0;
}
    *pa++ = a12;
    *pa++ = a34;
  }
  transmit(blocka);
  release(blocka);
  release(blockb);
}
