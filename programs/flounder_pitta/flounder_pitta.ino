/*

  flounder

  program: pitta

  description: a polysynth

  notes: use Teensyduino and make sure USB type is set to "Serial + MIDI + Audio"

  knob layout:

  ATTACK         DECAY          SUSTAIN                 RELEASE
  TRI/SAW MIX    CUTOFF         RES                     DELAY
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

*/

//set to 1 for use headphone output, 0 for line out
int headphoneout = 1;

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "effect_platervbstereo.h"

#include <FastTouch.h>

int notes[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66}; //13 midi notes (octave)
int transpose = 0;
//0-13 are the keys, 28-33 are the pads
int touchkeys[] = {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 28, 29, 30, 31, 32, 33};
int touches[19];
int prevtouches[19];

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

// GUItool: begin automatically generated code
AudioSynthWaveformModulated waveformModB1;  //xy=956.6666259765625,588.3333129882812
AudioSynthWaveformModulated waveformModA1;  //xy=960.6666259765625,552.3333129882812
AudioSynthWaveformModulated waveformModA2;  //xy=963.6666259765625,658.3333129882812
AudioSynthWaveformModulated waveformModB2;  //xy=963.6666259765625,698.3333129882812
AudioSynthWaveformModulated waveformModA3;  //xy=966.6666259765625,753.3333129882812
AudioSynthWaveformModulated waveformModA6;  //xy=965.6666259765625,1050.3333129882812
AudioSynthWaveformModulated waveformModB3;  //xy=967.6666259765625,786.3333129882812
AudioSynthWaveformModulated waveformModA5;  //xy=968.6666259765625,943.3333129882812
AudioSynthWaveformModulated waveformModB6;  //xy=968.6666259765625,1080.3333129882812
AudioSynthWaveformModulated waveformModB5;  //xy=969.6666259765625,979.3333129882812
AudioSynthWaveformModulated waveformModB4;  //xy=970.6666259765625,873.3333129882812
AudioSynthWaveformModulated waveformModA8;  //xy=969.6666259765625,1243.3333129882812
AudioSynthWaveformModulated waveformModA4;  //xy=971.6666259765625,839.3333129882812
AudioSynthWaveformModulated waveformModB9;  //xy=969.6666259765625,1396.3333129882812
AudioSynthWaveformModulated waveformModA9;  //xy=972.6666259765625,1360.3333129882812
AudioSynthWaveformModulated waveformModA7;  //xy=973.6666259765625,1140.3333129882812
AudioSynthWaveformModulated waveformModB8;  //xy=973.6666259765625,1280.3333129882812
AudioSynthWaveformModulated waveformModA10; //xy=973.6666259765625,1495.3333129882812
AudioSynthWaveformModulated waveformModB7;  //xy=982.6666259765625,1174.3333129882812
AudioSynthWaveformModulated waveformModB10; //xy=993.6666259765625,1534.3333129882812
AudioSynthWaveformModulated waveformModA11; //xy=1001.6666259765625,1595.3333129882812
AudioSynthWaveformModulated waveformModB11; //xy=1002.6666259765625,1630.3333129882812
AudioSynthWaveformModulated waveformModB12; //xy=1007.6666259765625,1734.3333129882812
AudioSynthWaveformModulated waveformModA12; //xy=1008.6666259765625,1702.3333129882812
AudioSynthWaveformModulated waveformModA13; //xy=1018.6666259765625,1798.3333129882812
AudioSynthWaveformModulated waveformModB13; //xy=1021.6666259765625,1838.3333129882812
AudioMixer4              oscmixer3;      //xy=1127.6666259765625,770.3333129882812
AudioMixer4              oscmixer2;      //xy=1129.6666259765625,670.3333129882812
AudioMixer4              oscmixer1;      //xy=1130.6666259765625,566.3333129882812
AudioMixer4              oscmixer4;      //xy=1130.6666259765625,871.3333129882812
AudioMixer4              oscmixer5;      //xy=1130.6666259765625,966.3333129882812
AudioMixer4              oscmixer6;      //xy=1139.6666259765625,1057.3333129882812
AudioMixer4              oscmixer7;      //xy=1142.6666259765625,1151.3333129882812
AudioMixer4              oscmixer8;      //xy=1144.6666259765625,1268.3333129882812
AudioMixer4              oscmixer9;      //xy=1167.6666259765625,1406.3333129882812
AudioMixer4              oscmixer10;     //xy=1179.6666259765625,1522.3333129882812
AudioMixer4              oscmixer11;     //xy=1227.6666259765625,1650.3333129882812
AudioMixer4              oscmixer13;     //xy=1243.6666259765625,1861.3333129882812
AudioMixer4              oscmixer12;     //xy=1244.6666259765625,1767.3333129882812
AudioEffectEnvelope      envelope5;      //xy=1271.6666259765625,969.3333129882812
AudioEffectEnvelope      envelope3;      //xy=1275.6666259765625,775.3333129882812
AudioEffectEnvelope      envelope4;      //xy=1279.6666259765625,868.3333129882812
AudioEffectEnvelope      envelope2;      //xy=1285.6666259765625,661.3333129882812
AudioEffectEnvelope      envelope8;      //xy=1291.6666259765625,1276.3333129882812
AudioEffectEnvelope      envelope6;      //xy=1293.6666259765625,1055.3333129882812
AudioEffectEnvelope      envelope7;      //xy=1298.6666259765625,1159.3333129882812
AudioEffectEnvelope      envelope1;      //xy=1302.6666259765625,562.3333129882812
AudioEffectEnvelope      envelope9;      //xy=1340.6666259765625,1404.3333129882812
AudioEffectEnvelope      envelope10;     //xy=1343.6666259765625,1500.3333129882812
AudioEffectEnvelope      envelope11;     //xy=1389.6666259765625,1630.3333129882812
AudioEffectEnvelope      envelope12;     //xy=1430.6666259765625,1754.3333129882812
AudioMixer4              mixer2;         //xy=1492.6666259765625,1024.3333129882812
AudioMixer4              mixer1;         //xy=1503.6666259765625,755.3333129882812
AudioMixer4              mixer3;         //xy=1503.6666259765625,1452.3333129882812
AudioEffectEnvelope      envelope13;     //xy=1520.6666259765625,1843.3333129882812
AudioMixer4              mixer4;         //xy=1711.6666259765625,1128.3333129882812
AudioSynthWaveformModulated waveformMod17;  //xy=1732.6666259765625,1261.3333129882812
AudioMixer4              mixer6;         //xy=1877.6666259765625,1344.3333129882812
AudioFilterLadder        ladder1;        //xy=1886.6666259765625,1168.3333129882812
AudioMixer4              mixerdelay; //xy=2140.416534423828,1222.3332710266113
AudioEffectDelay         delay1;         //xy=2155.750030517578,1413.7500190734863
AudioEffectPlateReverb   reverb;         //xy=2537.6666259765625,1129.3333129882812
AudioMixer4              mixerL;         //xy=2735.6666259765625,1222.3333129882812
AudioMixer4              mixerR;         //xy=2739.6666259765625,1324.3333129882812
AudioOutputUSB           usb1;           //xy=2876.6666259765625,1273.3333129882812
AudioAmplifier           amp2;           //xy=2981.6666259765625,1347.3333129882812
AudioAmplifier           amp1;           //xy=2986.6666259765625,1171.3333129882812
AudioOutputPT8211        pt8211_1;       //xy=3181.6666259765625,1250.3333129882812
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
AudioConnection          patchCord59(ladder1, 0, mixerdelay, 1);
AudioConnection          patchCord60(mixerdelay, delay1);
AudioConnection          patchCord61(mixerdelay, 0, reverb, 0);
AudioConnection          patchCord62(mixerdelay, 0, reverb, 1);
AudioConnection          patchCord63(mixerdelay, 0, mixerL, 1);
AudioConnection          patchCord64(mixerdelay, 0, mixerR, 1);
AudioConnection          patchCord65(delay1, 0, mixerdelay, 0);
AudioConnection          patchCord66(reverb, 0, mixerL, 0);
AudioConnection          patchCord67(reverb, 1, mixerR, 0);
AudioConnection          patchCord68(mixerL, 0, usb1, 0);
AudioConnection          patchCord69(mixerL, amp1);
AudioConnection          patchCord70(mixerR, 0, usb1, 1);
AudioConnection          patchCord71(mixerR, amp2);
AudioConnection          patchCord72(amp2, 0, pt8211_1, 1);
AudioConnection          patchCord73(amp1, 0, pt8211_1, 0);
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
  AudioMemory(1000);

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

  envelope1.hold(0);

  ladder1.octaveControl(10);
  delay1.delay(0, 300);

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

  //print max audiomemory
  int mem = AudioMemoryUsageMax();
  Serial.println(mem);

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

  //delay
  float delayfeedback  = mapf(analogRead(POT08), 0, 1023, 0, 1.0);
  mixerdelay.gain(0, 0.5 * delayfeedback); // delay signal
  mixerdelay.gain(1, 1); //dry signal

  // reverb
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

  //populate touches array with current touched values
  for (int x = 0; x <= 18; x++) {

    //   display fasttouch value
    //   Serial.print(touchkeys[x]);
    //   Serial.print(" =");
    //   Serial.println(fastTouchRead(touchkeys[x]));

    if (x == 12) {
      //special case for key 12 since it has a different sensitivity for some reason.
      if (fastTouchRead(touchkeys[x]) > 62) {
        touches[x] = 1;
      } else if (fastTouchRead(touchkeys[x]) < 59) {
        touches[x] = 0;
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
    }

    if (!(touches[i]) && (prevtouches[i])) {
      envelope[i]->noteOff();
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
