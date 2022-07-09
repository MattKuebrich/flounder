//issues:
// something the right-most 2 keys don't work with the sinesynth, but DO with MIDI. sometimes they work though after flashing???
// i think increasing audiomemory from 20 to 30 fixed it

// roundabout flounder 1.0
// midi keyboard + controls + stereo audio
// sinesynth + midi right now

// use Teensyduino and make sure USB type is set to "Serial + MIDI"

// thanks to:
// https://github.com/andr-ew/bleached
// https://www.criusdigitalstudio.com/projects-english/cius-touch-midi-keyboard
// http://little-scale.blogspot.com/2021/05/xiao-mx-midi-device.html
// https://www.benjaminpoilve.com/projects/synthetica.html
// https://www.pjrc.com/teensy/td_midi.html
// https://llllllll.co/t/four-n-128-sized-knob-controller/24
// https://github.com/GadgetReboot/ADSR_Envelope_Demo/blob/master/ADSR_Envelope_Demo.ino
// https://forum.arduino.cc/t/debouncing-capacitive-sensor/421300/3

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <FastTouch.h>

int notes[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66}; //13 midi notes (octave)
int transpose = 0;
//0-13 are the keys, 28-33 are the pads
int touchkeys[] = {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 28, 29, 30, 31, 32, 33};
int touches[19];
int prevtouches[19];

//threshold for FastTouch sensing
int upperthreshold = 40; // 63    lower works better for all but 13, like 39.
int lowerthreshold = 30; // 50 works well for all but 13.

int upperthreshold2 = 63;
int lowerthreshold2 = 58;

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

// limit maximum timing for parameters set by pots
#define attackMax  2000
#define decayMax   2000
#define releaseMax 2000

float pressuregain;

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
// copy below chunk into audio tool, but NOT the *envelope and *waveform pointer arrays

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=303.5714454650879,234.42855644226074
AudioSynthWaveform       waveform1;      //xy=304.5714454650879,191.42855644226074
AudioSynthWaveform       waveform8;      //xy=306.5714454650879,493.42855644226074
AudioSynthWaveform       waveform3;      //xy=308.5714454650879,276.42855644226074
AudioSynthWaveform       waveform6;      //xy=308.5714454650879,406.42855644226074
AudioSynthWaveform       waveform10;     //xy=309.5714454650879,573.4285564422607
AudioSynthWaveform       waveform4;      //xy=311.5714454650879,316.42855644226074
AudioSynthWaveform       waveform7;      //xy=311.5714454650879,449.42855644226074
AudioSynthWaveform       waveform9;      //xy=313.5714454650879,536.4285564422607
AudioSynthWaveform       waveform5;      //xy=314.5714454650879,358.42855644226074
AudioSynthWaveform       waveform12;     //xy=313.5714454650879,654.4285564422607
AudioSynthWaveform       waveform13;     //xy=316.5714454650879,698.4285564422607
AudioSynthWaveform       waveform11;     //xy=318.5714454650879,614.4285564422607
AudioEffectEnvelope      envelope13;     //xy=530.5714454650879,696.4285564422607
AudioEffectEnvelope      envelope9;      //xy=532.5714454650879,516.4285564422607
AudioEffectEnvelope      envelope12;     //xy=535.5714454650879,651.4285564422607
AudioEffectEnvelope      envelope10;     //xy=537.5714454650879,559.4285564422607
AudioEffectEnvelope      envelope11;     //xy=537.5714454650879,603.4285564422607
AudioEffectEnvelope      envelope4;      //xy=542.5714454650879,303.42855644226074
AudioEffectEnvelope      envelope8;      //xy=543.5714454650879,468.42855644226074
AudioEffectEnvelope      envelope1;      //xy=545.5714454650879,191.42855644226074
AudioEffectEnvelope      envelope6;      //xy=545.5714454650879,384.42855644226074
AudioEffectEnvelope      envelope7;      //xy=545.5714454650879,428.42855644226074
AudioEffectEnvelope      envelope2;      //xy=547.5714454650879,229.42855644226074
AudioEffectEnvelope      envelope3;      //xy=548.5714454650879,266.42855644226074
AudioEffectEnvelope      envelope5;      //xy=550.5714454650879,344.42855644226074
AudioMixer4              mixer2;         //xy=757.571460723877,383.00000381469727
AudioMixer4              mixer1;         //xy=760.4285659790039,293.00003242492676
AudioMixer4              mixer3;         //xy=760.7143669128418,475.1428680419922
AudioMixer4              mixer4;         //xy=999.8570976257324,454.00000190734863
AudioOutputUSB           usb1;           //xy=1186.8571662902832,498.7143154144287
AudioOutputPT8211        pt8211_1;       //xy=1192.8570442199707,434.85712909698486
AudioConnection          patchCord1(waveform2, envelope2);
AudioConnection          patchCord2(waveform1, envelope1);
AudioConnection          patchCord3(waveform8, envelope8);
AudioConnection          patchCord4(waveform3, envelope3);
AudioConnection          patchCord5(waveform6, envelope6);
AudioConnection          patchCord6(waveform10, envelope10);
AudioConnection          patchCord7(waveform4, envelope4);
AudioConnection          patchCord8(waveform7, envelope7);
AudioConnection          patchCord9(waveform9, envelope9);
AudioConnection          patchCord10(waveform5, envelope5);
AudioConnection          patchCord11(waveform12, envelope12);
AudioConnection          patchCord12(waveform13, envelope13);
AudioConnection          patchCord13(waveform11, envelope11);
AudioConnection          patchCord14(envelope13, 0, mixer4, 3);
AudioConnection          patchCord15(envelope9, 0, mixer3, 0);
AudioConnection          patchCord16(envelope12, 0, mixer3, 3);
AudioConnection          patchCord17(envelope10, 0, mixer3, 1);
AudioConnection          patchCord18(envelope11, 0, mixer3, 2);
AudioConnection          patchCord19(envelope4, 0, mixer1, 3);
AudioConnection          patchCord20(envelope8, 0, mixer2, 3);
AudioConnection          patchCord21(envelope1, 0, mixer1, 0);
AudioConnection          patchCord22(envelope6, 0, mixer2, 1);
AudioConnection          patchCord23(envelope7, 0, mixer2, 2);
AudioConnection          patchCord24(envelope2, 0, mixer1, 1);
AudioConnection          patchCord25(envelope3, 0, mixer1, 2);
AudioConnection          patchCord26(envelope5, 0, mixer2, 0);
AudioConnection          patchCord27(mixer2, 0, mixer4, 1);
AudioConnection          patchCord28(mixer1, 0, mixer4, 0);
AudioConnection          patchCord29(mixer3, 0, mixer4, 2);
AudioConnection          patchCord30(mixer4, 0, pt8211_1, 1);
AudioConnection          patchCord31(mixer4, 0, pt8211_1, 0);
AudioConnection          patchCord32(mixer4, 0, usb1, 1);
AudioConnection          patchCord33(mixer4, 0, usb1, 0);
// GUItool: end automatically generated code 

//don't import this into teensy design tool, but may need to be adjusted manually
AudioEffectEnvelope      *envelope[13] = {&envelope1, &envelope2, &envelope3, &envelope4, &envelope5, &envelope6, &envelope7, &envelope8, &envelope9, &envelope10, &envelope11, &envelope12, &envelope13}; //xy=1866,1088
AudioSynthWaveform       *waveform[13] = {&waveform1, &waveform2, &waveform3, &waveform4, &waveform5, &waveform6, &waveform7, &waveform8, &waveform9, &waveform10, &waveform11, &waveform12, &waveform13}; //xy=1890.3333740234375,1048


void setup() {
  Serial.begin(9600);
  analogReadResolution(POT_BIT_RES ); // set the analog read resolution to 7 bits (a range of 0 - 127)
  analogReadAveraging(POT_NUM_READS); // average the analog value by averaging 16 readings

  AudioMemory(30);
  waveform1.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform2.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform3.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform4.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform5.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform6.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform7.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform8.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform9.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform10.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform11.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform12.begin(0.75f, 100.f, WAVEFORM_SINE);
  waveform13.begin(0.75f, 100.f, WAVEFORM_SINE);



  // https://github.com/GadgetReboot/ADSR_Envelope_Demo/blob/master/ADSR_Envelope_Demo.ino

  attackParam = 50;
  decayParam = 50;
  sustainParam = 0.5;
  releaseParam = 250;

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




  //AudioInterrupts();

}

void loop() {

  attackParam  = map(analogRead(POT01), 0, 1023, 0, attackMax);
  decayParam   = map(analogRead(POT02), 0, 1023, 0, decayMax);
  sustainParam = mapf(analogRead(POT03), 0, 1023, 0, 1.0);
  releaseParam = map(analogRead(POT04), 0, 1023, 0, releaseMax);

  for (uint8_t i = 0; i < 12; i++) {
    uint16_t pot_val = analogRead(pot[i]);
    if ((pot_val < prev_pot_val[i] - nbrhd) ||
        (pot_val > prev_pot_val[i] + nbrhd)) {
      usbMIDI.sendControlChange(cc[i], pot_val >> (POT_BIT_RES - 7), MIDI_CHANNEL);
      //Serial.print(pot_val); Serial.println(" pot1");
      prev_pot_val[i] = pot_val;
    }
  }


   //MIDI Controllers should discard incoming MIDI messages.
    while (usbMIDI.read()) {
    }



  //populate touches array with current touched values
  //tried changing to i and didnt work
  for (int x = 0; x <= 18; x++) {

    // display fasttouch value. most are around 19 (13-26) when not touched but pin 13 is around 53. all are 64 when touched.
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




  //bottom buttons act as transpose for notes. i could simplify this code with a loop, but want to be able to assign these to different things.
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
    if ((touches[i]) && !(prevtouches[i])) {
      waveform[i]->begin(0.75f, 100.f, WAVEFORM_TRIANGLE);
      waveform[i]->frequency(midi2freq(notes[i] + transpose));
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
      //Serial.print(i); Serial.println(" released");
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
