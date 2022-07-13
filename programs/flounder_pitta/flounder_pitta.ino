/*

  flounder

  program: pitta

  description: a polysynth

  notes: use Teensyduino and make sure USB type is set to "Serial + MIDI + Audio"

  knob layout:

  ATTACK         DECAY          SUSTAIN                 RELEASE
  TRI/SAW MIX    CUTOFF         RES                     NOTHING
  LFO SPEED      LFO AMOUNT     REVERB MIX              REVERB TIME

  thanks to:
  https://github.com/andr-ew/bleached
  https://www.criusdigitalstudio.com/projects-english/cius-touch-midi-keyboard
  http://little-scale.blogspot.com/2021/05/xiao-mx-midi-device.html
  https://www.benjaminpoilve.com/projects/synthetica.html
  https://www.pjrc.com/teensy/td_midi.html
  https://llllllll.co/t/four-n-128-sized-knob-controller/24
  https://github.com/GadgetReboot/ADSR_Envelope_Demo/blob/master/ADSR_Envelope_Demo.ino
  https://forum.arduino.cc/t/debouncing-capacitive-sensor/421300/3
  https://forum.pjrc.com/threads/65688-Stereo-Plate-Reverb-for-Teensy4-x
  https://github.com/quarterturn/teensy3-ensemble-chorus

*/

//set to 1 for use headphone output, 0 for line out
int headphoneout = 0;

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "effect_platervbstereo.h"
#include "effect_ensemble.h"

#include <FastTouch.h>

int notes[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66}; //13 midi notes (octave)
int transpose = 0;
//0-13 are the keys, 28-33 are the pads
int touchkeys[] = {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 28, 29, 30, 31, 32, 33};
int touches[19];
int prevtouches[19];

int volumefactor = 1;

// for cc pots
uint16_t prev_pot_val[12];

#define CC01  102
#define CC02  103
#define CC03  104
#define CC04  105
#define CC05  106
#define CC06  107
#define CC07  108
#define CC08  109
#define CC09  110
#define CC10  111
#define CC11  112
#define CC12  113

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

// store pot readings into ADSR parameter settings
int   attackParam;
int   decayParam;
float sustainParam;
int   releaseParam;

// store pot readings into pitch ADSR parameter settings
int   attackParam_pitch;
int   decayParam_pitch;
float sustainParam_pitch;
int   releaseParam_pitch;
float pitchenvamt;

float LFOamt;
float LFOfreq;

float cutoffParam;
float resonanceParam;

// limit maximum timing for parameters set by pots
#define attackMax  2000
#define decayMax   2000
#define releaseMax 2000

uint8_t cc[12] = {CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12};
uint8_t pot[12] = {POT01, POT02, POT03, POT04, POT05, POT06, POT07, POT08, POT09, POT10, POT11, POT12};

// Use this MIDI channel.
#define MIDI_CHANNEL 1

#define POT_BIT_RES         10 // 10 works, 7-16 is valid
#define POT_NUM_READS       32 // 32 works

// Prevent jitter when reading the potentiometers.
// Higher value is less chance of jitter but also less precision.
const uint8_t nbrhd = 5;

// using arrays with teensy design tool
// https://forum.pjrc.com/threads/61270-Array-handling-in-the-audio-tool
// https://forum.pjrc.com/threads/58158-Accessing-Audio-Objects-from-an-Array

// in order to use teensy design tool:
// copy below chunk into audio tool, but NOT the pointer arrays below
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformModulated waveformModB1;  //xy=750.6666831970215,147.66668510437012
AudioSynthWaveformModulated waveformModA1;  //xy=754.6666831970215,111.66668510437012
AudioSynthWaveformModulated waveformModA2;  //xy=757.6666831970215,217.66668510437012
AudioSynthWaveformModulated waveformModB2;  //xy=757.6666831970215,257.6666851043701
AudioSynthWaveformModulated waveformModA3;  //xy=760.6666831970215,312.6666851043701
AudioSynthWaveformModulated waveformModA6;  //xy=759.6666831970215,609.6666851043701
AudioSynthWaveformModulated waveformModB3;  //xy=761.6666831970215,345.6666851043701
AudioSynthWaveformModulated waveformModA5;  //xy=762.6666831970215,502.6666851043701
AudioSynthWaveformModulated waveformModB6;  //xy=762.6666831970215,639.6666851043701
AudioSynthWaveformModulated waveformModB5;  //xy=763.6666831970215,538.6666851043701
AudioSynthWaveformModulated waveformModB4;  //xy=764.6666831970215,432.6666851043701
AudioSynthWaveformModulated waveformModA8;  //xy=763.6666831970215,802.6666851043701
AudioSynthWaveformModulated waveformModA4;  //xy=765.6666831970215,398.6666851043701
AudioSynthWaveformModulated waveformModB9;  //xy=763.6666831970215,955.6666851043701
AudioSynthWaveformModulated waveformModA9;  //xy=766.6666831970215,919.6666851043701
AudioSynthWaveformModulated waveformModA7;  //xy=767.6666831970215,699.6666851043701
AudioSynthWaveformModulated waveformModB8;  //xy=767.6666831970215,839.6666851043701
AudioSynthWaveformModulated waveformModA10; //xy=767.6666831970215,1054.6666851043701
AudioSynthWaveformModulated waveformModB7;  //xy=776.6666831970215,733.6666851043701
AudioSynthWaveformModulated waveformModB10; //xy=787.6666831970215,1093.6666851043701
AudioSynthWaveformModulated waveformModA11; //xy=795.6666831970215,1154.6666851043701
AudioSynthWaveformModulated waveformModB11; //xy=796.6666831970215,1189.6666851043701
AudioSynthWaveformModulated waveformModB12; //xy=801.6666831970215,1293.6666851043701
AudioSynthWaveformModulated waveformModA12; //xy=802.6666831970215,1261.6666851043701
AudioSynthWaveformModulated waveformModA13; //xy=812.6666831970215,1357.6666851043701
AudioSynthWaveformModulated waveformModB13; //xy=815.6666831970215,1397.6666851043701
AudioMixer4              oscmixer3;      //xy=921.6666831970215,329.6666851043701
AudioMixer4              oscmixer2;      //xy=923.6666831970215,229.66668510437012
AudioMixer4              oscmixer1;      //xy=924.6666831970215,125.66668510437012
AudioMixer4              oscmixer4;      //xy=924.6666831970215,430.6666851043701
AudioMixer4              oscmixer5;      //xy=924.6666831970215,525.6666851043701
AudioMixer4              oscmixer6;      //xy=933.6666831970215,616.6666851043701
AudioMixer4              oscmixer7;      //xy=936.6666831970215,710.6666851043701
AudioMixer4              oscmixer8;      //xy=938.6666831970215,827.6666851043701
AudioMixer4              oscmixer9;      //xy=961.6666831970215,965.6666851043701
AudioMixer4              oscmixer10;     //xy=973.6666831970215,1081.6666851043701
AudioMixer4              oscmixer11;     //xy=1021.6666831970215,1209.6666851043701
AudioMixer4              oscmixer13;     //xy=1037.6666831970215,1420.6666851043701
AudioMixer4              oscmixer12;     //xy=1038.6666831970215,1326.6666851043701
AudioEffectEnvelope      envelope5;      //xy=1065.6666831970215,528.6666851043701
AudioEffectEnvelope      envelope3;      //xy=1069.6666831970215,334.6666851043701
AudioEffectEnvelope      envelope4;      //xy=1073.6666831970215,427.6666851043701
AudioEffectEnvelope      envelope2;      //xy=1079.6666831970215,220.66668510437012
AudioEffectEnvelope      envelope8;      //xy=1085.6666831970215,835.6666851043701
AudioEffectEnvelope      envelope6;      //xy=1087.6666831970215,614.6666851043701
AudioEffectEnvelope      envelope7;      //xy=1092.6666831970215,718.6666851043701
AudioEffectEnvelope      envelope1;      //xy=1096.6666831970215,121.66668510437012
AudioEffectEnvelope      envelope9;      //xy=1134.6666831970215,963.6666851043701
AudioEffectEnvelope      envelope10;     //xy=1137.6666831970215,1059.6666851043701
AudioEffectEnvelope      envelope11;     //xy=1183.6666831970215,1189.6666851043701
AudioEffectEnvelope      envelope12;     //xy=1224.6666831970215,1313.6666851043701
AudioMixer4              mixer2;         //xy=1286.6666831970215,583.6666851043701
AudioMixer4              mixer1;         //xy=1297.6666831970215,314.6666851043701
AudioMixer4              mixer3;         //xy=1297.6666831970215,1011.6666851043701
AudioEffectEnvelope      envelope13;     //xy=1314.6666831970215,1402.6666851043701
AudioMixer4              mixer4;         //xy=1505.6666831970215,687.6666851043701
AudioSynthWaveformModulated waveformMod17;  //xy=1526.666648864746,820.6667194366455
AudioMixer4              mixer6;         //xy=1671.3333587646484,903.6666603088379
AudioFilterLadder        ladder1;        //xy=1680.9999389648438,727.0000152587891
AudioEffectEnsemble      ensemble;       //xy=1897.6665573120117,702.0000143051147
AudioMixer4              mixerensembleR;   //xy=2118.6666831970215,890.6666851043701
AudioMixer4              mixerensembleL;   //xy=2126.4166870117188,813.4167232513428
AudioEffectPlateReverb   reverb;         //xy=2331.6666831970215,688.6666851043701
AudioMixer4              mixerL;         //xy=2529.9167861938477,781.9166984558105
AudioMixer4              mixerR;         //xy=2533.6666831970215,883.6666851043701
AudioOutputUSB           usb1;           //xy=2670.50004196167,832.4167613983154
AudioAmplifier           amp2;           //xy=2775.00004196167,906.2500143051147
AudioAmplifier           amp1;           //xy=2780.00004196167,730.0000114440918
AudioOutputPT8211        pt8211_1;       //xy=2975.00004196167,809.3333864212036
AudioConnection          patchCord1(waveformModB1, 0, oscmixer1, 1);
AudioConnection          patchCord2(waveformModA1, 0, oscmixer1, 0);
AudioConnection          patchCord3(waveformModA2, 0, oscmixer2, 0);
AudioConnection          patchCord4(waveformModB2, 0, oscmixer2, 1);
AudioConnection          patchCord5(waveformModA3, 0, oscmixer3, 0);
AudioConnection          patchCord6(waveformModA6, 0, oscmixer6, 0);
AudioConnection          patchCord7(waveformModB3, 0, oscmixer3, 1);
AudioConnection          patchCord8(waveformModA5, 0, oscmixer5, 0);
AudioConnection          patchCord9(waveformModB6, 0, oscmixer6, 1);
AudioConnection          patchCord10(waveformModB5, 0, oscmixer5, 1);
AudioConnection          patchCord11(waveformModB4, 0, oscmixer4, 1);
AudioConnection          patchCord12(waveformModA8, 0, oscmixer8, 0);
AudioConnection          patchCord13(waveformModA4, 0, oscmixer4, 0);
AudioConnection          patchCord14(waveformModB9, 0, oscmixer9, 1);
AudioConnection          patchCord15(waveformModA9, 0, oscmixer9, 0);
AudioConnection          patchCord16(waveformModA7, 0, oscmixer7, 0);
AudioConnection          patchCord17(waveformModB8, 0, oscmixer8, 1);
AudioConnection          patchCord18(waveformModA10, 0, oscmixer10, 0);
AudioConnection          patchCord19(waveformModB7, 0, oscmixer7, 1);
AudioConnection          patchCord20(waveformModB10, 0, oscmixer10, 1);
AudioConnection          patchCord21(waveformModA11, 0, oscmixer11, 0);
AudioConnection          patchCord22(waveformModB11, 0, oscmixer11, 1);
AudioConnection          patchCord23(waveformModB12, 0, oscmixer12, 1);
AudioConnection          patchCord24(waveformModA12, 0, oscmixer12, 0);
AudioConnection          patchCord25(waveformModA13, 0, oscmixer13, 0);
AudioConnection          patchCord26(waveformModB13, 0, oscmixer13, 1);
AudioConnection          patchCord27(oscmixer3, envelope3);
AudioConnection          patchCord28(oscmixer2, envelope2);
AudioConnection          patchCord29(oscmixer1, envelope1);
AudioConnection          patchCord30(oscmixer4, envelope4);
AudioConnection          patchCord31(oscmixer5, envelope5);
AudioConnection          patchCord32(oscmixer6, envelope6);
AudioConnection          patchCord33(oscmixer7, envelope7);
AudioConnection          patchCord34(oscmixer8, envelope8);
AudioConnection          patchCord35(oscmixer9, envelope9);
AudioConnection          patchCord36(oscmixer10, envelope10);
AudioConnection          patchCord37(oscmixer11, envelope11);
AudioConnection          patchCord38(oscmixer13, envelope13);
AudioConnection          patchCord39(oscmixer12, envelope12);
AudioConnection          patchCord40(envelope5, 0, mixer2, 0);
AudioConnection          patchCord41(envelope3, 0, mixer1, 2);
AudioConnection          patchCord42(envelope4, 0, mixer1, 3);
AudioConnection          patchCord43(envelope2, 0, mixer1, 1);
AudioConnection          patchCord44(envelope8, 0, mixer2, 3);
AudioConnection          patchCord45(envelope6, 0, mixer2, 1);
AudioConnection          patchCord46(envelope7, 0, mixer2, 2);
AudioConnection          patchCord47(envelope1, 0, mixer1, 0);
AudioConnection          patchCord48(envelope9, 0, mixer3, 0);
AudioConnection          patchCord49(envelope10, 0, mixer3, 1);
AudioConnection          patchCord50(envelope11, 0, mixer3, 2);
AudioConnection          patchCord51(envelope12, 0, mixer3, 3);
AudioConnection          patchCord52(mixer2, 0, mixer4, 1);
AudioConnection          patchCord53(mixer1, 0, mixer4, 0);
AudioConnection          patchCord54(mixer3, 0, mixer4, 2);
AudioConnection          patchCord55(envelope13, 0, mixer4, 3);
AudioConnection          patchCord56(mixer4, 0, ladder1, 0);
AudioConnection          patchCord57(waveformMod17, 0, mixer6, 0);
AudioConnection          patchCord58(mixer6, 0, ladder1, 1);
AudioConnection          patchCord59(ladder1, 0, mixerensembleL, 1);
AudioConnection          patchCord60(ladder1, ensemble);
AudioConnection          patchCord61(ladder1, 0, ensemble, 1);
AudioConnection          patchCord62(ladder1, 0, mixerensembleR, 1);
AudioConnection          patchCord63(ensemble, 0, mixerensembleR, 0);
AudioConnection          patchCord64(ensemble, 1, mixerensembleL, 0);
AudioConnection          patchCord65(mixerensembleR, 0, reverb, 1);
AudioConnection          patchCord66(mixerensembleR, 0, mixerR, 1);
AudioConnection          patchCord67(mixerensembleL, 0, reverb, 0);
AudioConnection          patchCord68(mixerensembleL, 0, mixerL, 1);
AudioConnection          patchCord69(reverb, 0, mixerL, 0);
AudioConnection          patchCord70(reverb, 1, mixerR, 0);
AudioConnection          patchCord71(mixerL, 0, usb1, 0);
AudioConnection          patchCord72(mixerL, amp1);
AudioConnection          patchCord73(mixerR, 0, usb1, 1);
AudioConnection          patchCord74(mixerR, amp2);
AudioConnection          patchCord75(amp2, 0, pt8211_1, 1);
AudioConnection          patchCord76(amp1, 0, pt8211_1, 0);
// GUItool: end automatically generated code

//don't import this into teensy design tool, but may need to be adjusted manually
AudioEffectEnvelope      *envelope[13] = {&envelope1, &envelope2, &envelope3, &envelope4, &envelope5, &envelope6, &envelope7, &envelope8, &envelope9, &envelope10, &envelope11, &envelope12, &envelope13}; //xy=1866,1088
AudioSynthWaveformModulated       *waveformModA[13] = {&waveformModA1, &waveformModA2, &waveformModA3, &waveformModA4, &waveformModA5, &waveformModA6, &waveformModA7, &waveformModA8, &waveformModA9, &waveformModA10, &waveformModA11, &waveformModA12, &waveformModA13}; //xy=1890.3333740234375,1048
AudioSynthWaveformModulated       *waveformModB[13] = {&waveformModB1, &waveformModB2, &waveformModB3, &waveformModB4, &waveformModB5, &waveformModB6, &waveformModB7, &waveformModB8, &waveformModB9, &waveformModB10, &waveformModB11, &waveformModB12, &waveformModB13}; //xy=1890.3333740234375,1048
AudioMixer4 *oscmixer[13] = {&oscmixer1, &oscmixer2, &oscmixer3, &oscmixer4, &oscmixer5, &oscmixer6, &oscmixer7, &oscmixer8, &oscmixer9, &oscmixer10, &oscmixer11, &oscmixer12, &oscmixer13};

void setup() {
  Serial.begin(9600);
  analogReadResolution(POT_BIT_RES ); // set the analog read resolution to 7 bits (a range of 0 - 127)
  analogReadAveraging(POT_NUM_READS); // average the analog value by averaging 16 readings
  AudioMemory(100);

  if (headphoneout) {
    amp1.gain(0.1);  // lower output for headphones
    amp2.gain(0.1);  // lower output for headphones

  } else {
    amp1.gain(1);  // lower output for headphones
    amp2.gain(1);  // line out
  }

  attackParam = 50;
  decayParam = 50;
  sustainParam = 0.5;
  releaseParam = 250;

  ladder1.octaveControl(10);

  mixer1.gain(0, 0.25); // all add up to .9, less than 1 which would clip
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.25);

  mixer2.gain(0, 0.25); // all add up to .9, less than 1 which would clip
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  mixer2.gain(3, 0.25);

  mixer3.gain(0, 0.25); // all add up to .9, less than 1 which would clip
  mixer3.gain(1, 0.25);
  mixer3.gain(2, 0.25);
  mixer3.gain(3, 0.25);

  mixer4.gain(0, 1); // all add up to .9, less than 1 which would clip
  mixer4.gain(1, 1);
  mixer4.gain(2, 1);
  mixer4.gain(3, 0.25); //highest note on keyboard

  mixer6.gain(0, 1);
  mixer6.gain(1, 1);
}

void loop() {

  //ADSR
  attackParam  = map(analogRead(POT01), 0, 1023, 0, attackMax);
  decayParam   = map(analogRead(POT02), 0, 1023, 0, decayMax);
  sustainParam = mapf(analogRead(POT03), 0, 1023, 0, 1.0);
  releaseParam = map(analogRead(POT04), 0, 1023, 0, releaseMax);

  //oscillator mix
  float oscmixamt  = mapf(analogRead(POT05), 0, 1023, 0, 1.0);

  //LFO
  LFOfreq = powf((float)analogRead(POT09) / 1023.0, 3) * 10.0; //log scale 0 to 10
  LFOamt  = mapf(analogRead(POT10), 0, 1023, 0, 0.5);
  waveformMod17.frequency(LFOfreq);
  waveformMod17.amplitude(LFOamt);

  //filter
  cutoffParam = powf((float)analogRead(POT06) / 1023.0, 3) * 50.0; //log scale 0 to 50
  cutoffParam = mapf(cutoffParam, 0, 50, 1, 50); //limit it a bit, was getting weird
  resonanceParam = mapf(analogRead(POT07), 0, 1023, 0, 0.8);

  //chorus
  float chorus_wetdry  = mapf(analogRead(POT08), 0, 1023, 0, 1.0);
  mixerensembleL.gain(0, chorus_wetdry); //chorus
  mixerensembleL.gain(1, 1); //dry signal
  mixerensembleR.gain(0, chorus_wetdry); //chorus
  mixerensembleR.gain(1, 1); //dry signal

  //reverb
  float reverb_wetdry  = mapf(analogRead(POT11), 0, 1023, 0, 1.0);
  mixerL.gain(0, reverb_wetdry); //reverb
  mixerL.gain(1, 1); //dry signal
  mixerR.gain(0, reverb_wetdry); //reverb
  mixerR.gain(1, 1); //dry signal

  float reverb_size  = mapf(analogRead(POT12), 0, 1023, 0, 2.0);
  reverb.size(reverb_size);
  reverb.diffusion(1.0);
  reverb.lowpass(0.3);  // sets the reverb master lowpass filter
  reverb.lodamp(0.1);   // amount of low end loss in the reverb tail
  reverb.hidamp(0.2);   // amount of treble loss in the reverb tail

  for (uint8_t i = 0; i < 12; i++) {
    uint16_t pot_val = analogRead(pot[i]);
    if ((pot_val < prev_pot_val[i] - nbrhd) ||
        (pot_val > prev_pot_val[i] + nbrhd)) {
      usbMIDI.sendControlChange(cc[i], pot_val >> (POT_BIT_RES - 7), MIDI_CHANNEL);
      prev_pot_val[i] = pot_val;
    }
  }

  //MIDI Controllers should discard incoming MIDI messages.
  while (usbMIDI.read()) {
  }

  //populate touches array with current touched values
  //tried changing to i and didnt work
  for (int x = 0; x <= 18; x++) {

    // display fasttouch value
    //   Serial.print(touchkeys[x]);
    //   Serial.print(" =");
    //   Serial.println(fastTouchRead(touchkeys[x]));

    if (x == 12) {
      //special case for key 12 since it has a different sensitivity for some reason.
      if (fastTouchRead(touchkeys[x]) > 62) {
        touches[x] = 1;
        //Serial.println("yas");
      } else if (fastTouchRead(touchkeys[x]) < 59) {
        touches[x] = 0;
        //Serial.println("naw");
      }
    } else {
      if (fastTouchRead(touchkeys[x]) > 40) {
        touches[x] = 1;
      } else if (fastTouchRead(touchkeys[x]) < 30) {
        touches[x] = 0;
      }
    }
  }


  if ((touches[13]) && !(prevtouches[13])) {
    transpose = -24;
  }

  if ((touches[14]) && !(prevtouches[14])) {
    transpose = -12;
  }

  if ((touches[15]) && !(prevtouches[15])) {
    transpose = 0;
  }

  if ((touches[16]) && !(prevtouches[16])) {
    transpose = 12;
  }

  if ((touches[17]) && !(prevtouches[17])) {
    transpose = 24;
  }

  if ((touches[18]) && !(prevtouches[18])) {
    transpose = 36;
  }

  for (int i = 0; i <= 12; i++) { //18 for all buttons to be keys

    ladder1.resonance(resonanceParam);
    ladder1.frequency(cutoffParam * midi2freq(notes[i]));

    if ((touches[i]) && !(prevtouches[i])) {

      oscmixer[i]->gain(0, oscmixamt);
      oscmixer[i]->gain(1, 1 - oscmixamt);

      waveformModA[i]->begin(0.75f, 100.f, WAVEFORM_SAWTOOTH);
      waveformModA[i]->frequency(midi2freq(notes[i] + transpose + 12)); //adding octave!!
      waveformModA[i]->frequencyModulation(4); // 1.0 input increases 3 octaves

      waveformModB[i]->begin(0.75f, 100.f, WAVEFORM_SQUARE);
      waveformModB[i]->frequency(midi2freq(notes[i] + transpose));
      waveformModB[i]->frequencyModulation(4); // 1.0 input increases 3 octaves

      envelope[i]->attack(attackParam);
      envelope[i]->decay(decayParam);
      envelope[i]->sustain(sustainParam);
      envelope[i]->release(releaseParam);
      envelope[i]->noteOn();
      usbMIDI.sendNoteOn(notes[i] + transpose, 100, MIDI_CHANNEL); //should this be velocity of 100, like in ableton? or 128, full?
    }

    if (!(touches[i]) && (prevtouches[i])) {
      envelope[i]->noteOff();
      usbMIDI.sendNoteOff(notes[i] + transpose, 0, MIDI_CHANNEL);
    }
    prevtouches[i] = touches[i];
  }

} //end of loop


//convert midi to frequency
float midi2freq(int midinote) {
  float freq = 440 * pow(2, (midinote - 69.0) / 12.0);
  return freq;
}


// floating point map function taken from https://forum.arduino.cc/index.php?topic=361046.0
double mapf(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
