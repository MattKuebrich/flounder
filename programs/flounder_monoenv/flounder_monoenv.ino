/* 
 
flounder 

program: monoenv

description: a single mono oscillator and ADSR envelope, to demonstrate how the touch keyboard works. 
this can be a basis for a more complicated synth. also works as a midi controller.

notes: use Teensyduino and make sure USB type is set to "Serial + MIDI + Audio"

knob layout:

ATTACK   DECAY   SUSTAIN   RELEASE

*/

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

uint8_t cc[12] = {CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12};
uint8_t pot[12] = {POT01, POT02, POT03, POT04, POT05, POT06, POT07, POT08, POT09, POT10, POT11, POT12};

// Use this MIDI channel.
#define MIDI_CHANNEL 1

#define POT_BIT_RES         10 // 10 works, 7-16 is valid
#define POT_NUM_READS       32 // 32 works

// Prevent jitter when reading the potentiometers.
// Higher value is less chance of jitter but also less precision.
const uint8_t nbrhd = 5;

// GUItool: begin automatically generated code
AudioSynthWaveformModulated waveformMod1;   //xy=243,203
AudioEffectEnvelope      envelope1;      //xy=448,203
AudioOutputUSB           usb1;           //xy=654,245
AudioOutputPT8211        pt8211_1;       //xy=660,163
AudioConnection          patchCord1(waveformMod1, envelope1);
AudioConnection          patchCord2(envelope1, 0, usb1, 0);
AudioConnection          patchCord3(envelope1, 0, usb1, 1);
AudioConnection          patchCord4(envelope1, 0, pt8211_1, 0);
AudioConnection          patchCord5(envelope1, 0, pt8211_1, 1);
// GUItool: end automatically generated code

void setup() {
  Serial.begin(9600);
  analogReadResolution(POT_BIT_RES ); // set the analog read resolution to 7 bits (a range of 0 - 127)
  analogReadAveraging(POT_NUM_READS); // average the analog value by averaging 16 readings
  AudioMemory(20);

  attackParam = 50;
  decayParam = 50;
  sustainParam = 0.5;
  releaseParam = 250;
  waveformMod1.begin(0.75f, 100.f, WAVEFORM_SAWTOOTH);
}

void loop() {

  //ADSR
  attackParam  = map(analogRead(POT01), 0, 1023, 0, attackMax);
  decayParam   = map(analogRead(POT02), 0, 1023, 0, decayMax);
  sustainParam = mapf(analogRead(POT03), 0, 1023, 0, 1.0);
  releaseParam = map(analogRead(POT04), 0, 1023, 0, releaseMax);

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
  for (int x = 0; x <= 18; x++) {

    // display fasttouch value.
    //   Serial.print(touchkeys[x]);
    //   Serial.print(" =");
    //   Serial.println(fastTouchRead(touchkeys[x]));

    if (x == 12) {
      //special case for key 12 since it has a different sensitivity
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

  //bottom buttons act as transpose for notes

  if ((touches[13]) && !(prevtouches[13])) {
    transpose = -24;
    usbMIDI.sendControlChange(123, 0, MIDI_CHANNEL); //sends all notes off, to avoid notes getting stuck.
    //if a note is held down and transpose pressed, it gets stuck.
  }

  if ((touches[14]) && !(prevtouches[14])) {
    transpose = -12;
    usbMIDI.sendControlChange(123, 0, MIDI_CHANNEL);
  }

  if ((touches[15]) && !(prevtouches[15])) {
    transpose = 0;
    usbMIDI.sendControlChange(123, 0, MIDI_CHANNEL);
  }

  if ((touches[16]) && !(prevtouches[16])) {
    transpose = 12;
    usbMIDI.sendControlChange(123, 0, MIDI_CHANNEL);
  }

  if ((touches[17]) && !(prevtouches[17])) {
    transpose = 24;
    usbMIDI.sendControlChange(123, 0, MIDI_CHANNEL);
  }

  if ((touches[18]) && !(prevtouches[18])) {
    transpose = 36;
    usbMIDI.sendControlChange(123, 0, MIDI_CHANNEL);
  }

  for (int i = 0; i <= 12; i++) { //18 for all buttons to be keys
    if ((touches[i]) && !(prevtouches[i])) {
      waveformMod1.frequency(midi2freq(notes[i] + transpose));
      envelope1.attack(attackParam);
      envelope1.decay(decayParam);
      envelope1.sustain(sustainParam);
      envelope1.release(releaseParam);
      envelope1.noteOn();
      usbMIDI.sendNoteOn(notes[i] + transpose, 100, MIDI_CHANNEL); //should this be velocity of 100, like in ableton? or 128, full?
    }
    if (!(touches[i]) && (prevtouches[i])) {
      envelope1.noteOff();
      usbMIDI.sendNoteOff(notes[i] + transpose, 0, MIDI_CHANNEL);
    }
    prevtouches[i] = touches[i];
  }

} //end of loop


// convert midi to frequency
float midi2freq(int midinote) {
  float freq = 440 * pow(2, (midinote - 69.0) / 12.0);
  return freq;
}


// floating point map function taken from https://forum.arduino.cc/index.php?topic=361046.0
double mapf(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
