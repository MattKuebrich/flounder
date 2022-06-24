#ifndef effect_sampleandhold_h_
#define effect_effect_sampleandhold_h_
#include <Arduino.h>
#include "AudioStream.h"

class AudioEffectSampleAndHold : public AudioStream
{
public:
	
	AudioEffectSampleAndHold() : AudioStream(2, inputQueueArray) { }

	virtual void update(void);

  
private:
	audio_block_t *inputQueueArray[2];
};

#endif
