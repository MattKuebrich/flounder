#include <Arduino.h>
#include "AudioStream.h"

class AudioEffectComparator : public AudioStream
{
public:

	AudioEffectComparator() : AudioStream(2, inputQueueArray) { }

	virtual void update(void);
  
private:
	audio_block_t *inputQueueArray[2];
  //int threshold=0 ;  
  
};
