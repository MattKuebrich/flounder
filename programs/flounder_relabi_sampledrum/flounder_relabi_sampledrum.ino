/*

  flounder

  program: relabi_sampledrum 

  description: this is inspired by ideas from John Berndt's essay on Relabi (https://www.johnberndt.org/relabi/Relabi_essay.htm).

  this isn't super refined and a bit freeform.
  the basic idea is that 3 sine wave LFOs that are mixed to create a rhythmic but non-repeating “relabi wave”.
  you can set different thresholds in that wave to trigger the different drum samples.
  and each of the LFOs can modulate each other.
  reverb and distortion are added for fun.
  right now this just uses 909 drum samples.

  knob layout:

  LFO1_FREQ    LFO2_FREQ    LFO3_FREQ                RELABI_MIX
  LFO1_THRESH  LFO2_THRESH  LFO3_THRESH/REVERB_TIME  RELABI_THRESH / BITCRUSH_BITS
  LFO1->LFO2   LFO2->LFO3   LFO3->LFO1/REVERB_MIX    BITCRUSH_RATE


*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "effect_platervbstereo.h"

#include "AudioSample909clap.h"
#include "AudioSample909kick.h"
#include "AudioSample909snare.h"
#include "AudioSample909rimsh.h"

#define POT01 14
#define POT02 15
#define POT03 16
#define POT04 17
#define POT05 18
#define POT06 19
#define POT07 22
#define POT08 23
#define POT09 24
#define POT10 25
#define POT11 26
#define POT12 27

float LFOdivs[7] = {0, 0.25, 0.5, 1, 2, 4, 8};
int divSelect1;
int divSelect2;
int divSelect3;

float peak1val;
float peak1val_prev;

float peak2val;
float peak2val_prev;

float peak3val;
float peak3val_prev;

float peak4val;
float peak4val_prev;

float peak5val;
float peak5val_prev;

uint8_t pot[12] = {POT01, POT02, POT03, POT04, POT05, POT06, POT07, POT08, POT09, POT10, POT11, POT12};

#define POT_BIT_RES         10 // 10 works, 7-16 is valid
#define POT_NUM_READS       32 // 32 works

// GUItool: begin automatically generated code
AudioSynthWaveformModulated LFO1;           //xy=331.71429443359375,429.8571557998657
AudioSynthWaveformModulated LFO2;           //xy=340.42857360839844,478.99999237060547
AudioSynthWaveformModulated LFO3;           //xy=341.4285888671875,523.5714244842529
AudioPlayMemory          playMem2;       //xy=343.71429443359375,813.8571557998657
AudioPlayMemory          playMem1;       //xy=346.71429443359375,769.8571557998657
AudioPlayMemory          playMem3;       //xy=350.71429443359375,860.8571557998657
AudioPlayMemory          playMem4;       //xy=371.71429443359375,894.8571557998657
AudioAmplifier           amp1;           //xy=465.71429443359375,424.8571557998657
AudioAmplifier           amp2;           //xy=466.71429443359375,470.8571557998657
AudioAmplifier           amp3;           //xy=497.8571662902832,530.1428699493408
AudioMixer4              relabiwave;     //xy=569.7142944335938,707.8571557998657
AudioMixer4              drummixer;      //xy=570.7142944335938,832.8571557998657
AudioAnalyzePeak         peak1;          //xy=642.7142944335938,402.8571557998657
AudioAnalyzePeak         peak2;          //xy=650.7142944335938,445.8571557998657
AudioAnalyzePeak         peak3;          //xy=652.7142944335938,489.8571557998657
AudioEffectBitcrusher    bitcrusher1;    //xy=737.1428108215332,898.5715093612671
AudioAnalyzePeak         peak5;          //xy=792.7142944335938,618.8571557998657
AudioMixer4              mixer1;         //xy=849.4285850524902,788.571475982666
AudioEffectPlateReverb   platereverb1;   //xy=1018.5713958740234,744.2857246398926
AudioMixer4              mixerL;         //xy=1209.9997825622559,825.7143135070801
AudioMixer4              mixerR;         //xy=1215.7141952514648,905.7143135070801
AudioOutputPT8211        pt8211_1;       //xy=1416.571403503418,858.2857360839844
AudioConnection          patchCord1(LFO1, amp1);
AudioConnection          patchCord2(LFO1, peak1);
AudioConnection          patchCord3(LFO1, 0, relabiwave, 0);
AudioConnection          patchCord4(LFO2, amp2);
AudioConnection          patchCord5(LFO2, peak2);
AudioConnection          patchCord6(LFO2, 0, relabiwave, 1);
AudioConnection          patchCord7(LFO3, amp3);
AudioConnection          patchCord8(LFO3, peak3);
AudioConnection          patchCord9(LFO3, 0, relabiwave, 2);
AudioConnection          patchCord10(playMem2, 0, drummixer, 1);
AudioConnection          patchCord11(playMem1, 0, drummixer, 0);
AudioConnection          patchCord12(playMem3, 0, drummixer, 2);
AudioConnection          patchCord13(playMem4, 0, drummixer, 3);
AudioConnection          patchCord14(amp1, 0, LFO2, 0);
AudioConnection          patchCord15(amp2, 0, LFO3, 0);
AudioConnection          patchCord16(amp3, 0, LFO1, 0);
AudioConnection          patchCord17(relabiwave, 0, mixer1, 1);
AudioConnection          patchCord18(relabiwave, peak5);
AudioConnection          patchCord19(drummixer, bitcrusher1);
AudioConnection          patchCord20(bitcrusher1, 0, mixer1, 0);
AudioConnection          patchCord21(mixer1, 0, platereverb1, 0);
AudioConnection          patchCord22(mixer1, 0, platereverb1, 1);
AudioConnection          patchCord23(mixer1, 0, mixerL, 1);
AudioConnection          patchCord24(mixer1, 0, mixerR, 1);
AudioConnection          patchCord25(platereverb1, 0, mixerL, 0);
AudioConnection          patchCord26(platereverb1, 1, mixerR, 0);
AudioConnection          patchCord28(mixerL, 0, pt8211_1, 0);
AudioConnection          patchCord30(mixerR, 0, pt8211_1, 1);

//AudioOutputUSB           usb1;           //xy=1417.0000457763672,809.4285764694214
//AudioConnection          patchCord27(mixerL, 0, usb1, 0);
//AudioConnection          patchCord29(mixerR, 0, usb1, 1);

// GUItool: end automatically generated code

void setup() {

  Serial.begin(9600);
  analogReadResolution(POT_BIT_RES ); // set the analog read resolution to 7 bits (a range of 0 - 127)
  analogReadAveraging(POT_NUM_READS); // average the analog value by averaging 16 readings
  AudioMemory(20);

  //waveformMod1.begin(1.0f, 50.f, WAVEFORM_TRIANGLE);

  //print max audiomemory
  //int mem = AudioMemoryUsageMax();
  // Serial.println(mem);

  LFO1.begin(1.0f, 100.f, WAVEFORM_SINE);
  LFO2.begin(1.0f, 100.f, WAVEFORM_SINE);
  LFO3.begin(1.0f, 100.f, WAVEFORM_SINE);

  mixer1.gain(0, 1); //drums
  mixer1.gain(1, 0); //realabi wave (mixed lfos)

  drummixer.gain(0, 0.25);
  drummixer.gain(1, 0.25);
  drummixer.gain(2, 0.25);
  drummixer.gain(3, 0.25);

  platereverb1.size(0.5);
  platereverb1.diffusion(1.0);
  platereverb1.lowpass(0.3);  // sets the reverb master lowpass filter
  platereverb1.lodamp(0.1);   // amount of low end loss in the reverb tail
  platereverb1.hidamp(0.2);   // amount of treble loss in the reverb tail

  //AudioInterrupts();


}

void loop() {

  if (peak1.available()) {
    peak1val = peak1.read();
  }

  if (peak2.available()) {
    peak2val = peak2.read();
  }


  if (peak3.available()) {
    peak3val = peak3.read();
  }


  if (peak5.available()) {
    peak5val = peak5.read();
  }


  //Serial.print("peak1=");
  //Serial.println(peak1val);

  divSelect1 = map(analogRead(POT01), 0, 1023, 0, 6);
  divSelect2 = map(analogRead(POT02), 0, 1023, 0, 6);
  divSelect3 = map(analogRead(POT03), 0, 1023, 0, 6);

  LFO1.frequency(LFOdivs[divSelect1]);
  LFO2.frequency(LFOdivs[divSelect2]);
  LFO3.frequency(LFOdivs[divSelect3]);

  //modulate each lfo by each other
  float lfomod1 = mapf(analogRead(POT09), 0, 1023, 0.0, 1.0);
  float lfomod2 = mapf(analogRead(POT10), 0, 1023, 0.0, 1.0);
  float lfomod3 = mapf(analogRead(POT11), 0, 1023, 0.0, 1.0);

  amp1.gain(lfomod1);
  amp2.gain(lfomod2);
  amp3.gain(lfomod3);

  bitcrusher1.bits(map(analogRead(POT08), 0, 1023, 16, 4)); //4 not to get TOO loud
  bitcrusher1.sampleRate(map(analogRead(POT12), 0, 1023, 44100, 0));

  relabiwave.gain(0, mapf(analogRead(POT04), 0, 1023, 0.0, 1.0));
  relabiwave.gain(1, mapf(analogRead(POT04), 0, 1023, 0.0, 1.0));
  relabiwave.gain(2, mapf(analogRead(POT04), 0, 1023, 0.0, 1.0));
  relabiwave.gain(3, mapf(analogRead(POT04), 0, 1023, 0.0, 1.0));

  float thresh1 = mapf(analogRead(POT05), 0, 1023, 0.1, 0.9);
  float thresh2 = mapf(analogRead(POT06), 0, 1023, 0.1, 0.9);
  float thresh3 = mapf(analogRead(POT07), 0, 1023, 0.1, 0.9);

  //float thresh4 = mapf(analogRead(POT08), 0, 1023, 0.1, 0.9);
  //float thresh_relabi = mapf(analogRead(POT08), 0, 1023, 0.1, 0.9);

  // reverb mixer
  float reverbtime = mapf(analogRead(POT07), 0, 1023, 0.0, 1.0);
  float reverbmix = mapf(analogRead(POT11), 0, 1023, 0.0, 0.3); // 1.0 is too much

  mixerL.gain(0, reverbmix); //wet, pot 11
  mixerL.gain(1, 1.0); //dry

  mixerR.gain(0, reverbmix); //wet, pot 11
  mixerR.gain(1, 1.0); //dry

  platereverb1.size(reverbtime); // pot 7

  if ((peak1val >= thresh1 - 0.001) && (peak1val_prev <= thresh1 + 0.001) && (peak1val != peak1val_prev)) { // works!
    //drum1.noteOn();
    Serial.println("drum1");
    playMem1.play(AudioSample909kick);
  }


  if ((peak2val >= thresh2 - 0.001) && (peak2val_prev <= thresh2 + 0.001) && (peak2val != peak2val_prev)) { // works!
    //drum2.noteOn();
    Serial.println("drum2");
    playMem2.play(AudioSample909clap);
  }

  if ((peak3val >= thresh3 - 0.001) && (peak3val_prev <= thresh3 + 0.001) && (peak3val != peak3val_prev)) { // works!
    //drum3.noteOn();

    Serial.println("drum3");
    playMem3.play(AudioSample909rimsh);
  }


  //relabi wave stuff

  if ((peak5val >= thresh1 - 0.001) && (peak5val_prev <= thresh1 + 0.001) && (peak5val != peak5val_prev)) { // works!
    playMem2.stop();
    playMem3.stop();
    playMem4.stop();

    playMem1.play(AudioSample909rimsh);

  }

  if ((peak5val >= thresh2 - 0.001) && (peak5val_prev <= thresh2 + 0.001) && (peak5val != peak5val_prev)) { // works!
    playMem1.stop();
    playMem3.stop();
    playMem4.stop();

    playMem2.play(AudioSample909snare);
  }

  if ((peak5val >= thresh3 - 0.001) && (peak5val_prev <= thresh3 + 0.001) && (peak5val != peak5val_prev)) { // works!
    playMem1.stop();
    playMem2.stop();
    playMem4.stop();

    playMem3.play(AudioSample909kick);
  }

  peak1val_prev = peak1val;
  peak2val_prev = peak2val;
  peak3val_prev = peak3val;
  peak5val_prev = peak5val;


} //end of loop


// floating point map function taken from https://forum.arduino.cc/index.php?topic=361046.0
double mapf(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
