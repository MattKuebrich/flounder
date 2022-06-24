#include <Arduino.h>
#include "AudioStream.h"

class AudioEffectRungler : public AudioStream
{
public:

	AudioEffectRungler() : AudioStream(2, inputQueueArray) { }

	virtual void update(void);

  void loopEnable(short sa) {
    loop_enable=sa;
  }
  
private:
	audio_block_t *inputQueueArray[2];
  uint8_t runglerByte; //8bits in a byte
  short loop_enable=0;  
};
