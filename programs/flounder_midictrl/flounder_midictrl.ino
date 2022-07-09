// roundabout flounder 

// program: midictrl v1.0
// description: midi controller. 12 MIDI-mappable knobs + a full-octave touch MIDI keyboard + octave control (using the 6 "function" buttons)
// notes: use Teensyduino and make sure USB type is set to "Serial + MIDI"

// thanks to:
// https://github.com/andr-ew/bleached
// https://www.criusdigitalstudio.com/projects-english/cius-touch-midi-keyboard
// http://little-scale.blogspot.com/2021/05/xiao-mx-midi-device.html
// https://www.benjaminpoilve.com/projects/synthetica.html
// https://www.pjrc.com/teensy/td_midi.html
// https://llllllll.co/t/four-n-128-sized-knob-controller/24
// https://forum.arduino.cc/t/debouncing-capacitive-sensor/421300/3

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

uint8_t cc[12] = {CC01, CC02, CC03, CC04, CC05, CC06, CC07, CC08, CC09, CC10, CC11, CC12};
uint8_t pot[12] = {POT01, POT02, POT03, POT04, POT05, POT06, POT07, POT08, POT09, POT10, POT11, POT12};

// Use this MIDI channel.
#define MIDI_CHANNEL 1

#define POT_BIT_RES         10 // 10 works, 7-16 is valid
#define POT_NUM_READS       32 // 32 works

// Prevent jitter when reading the potentiometers.
// Higher value is less chance of jitter but also less precision.
const uint8_t nbrhd = 5;

void setup() {
  Serial.begin(9600);
  analogReadResolution(POT_BIT_RES ); // set the analog read resolution to 7 bits (a range of 0 - 127)
  analogReadAveraging(POT_NUM_READS); // average the analog value by averaging 16 readings
}

void loop() {

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
    if (x == 12) {
      //special case for key 12 (pin 13) since it has a different sensitivity
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
    usbMIDI.sendControlChange(123,0,MIDI_CHANNEL); //sends all notes off, to avoid notes getting stuck. 
    //if a note is held down and transpose pressed, it gets stuck.
  }

  if ((touches[14]) && !(prevtouches[14])) {
    transpose = -12;
    usbMIDI.sendControlChange(123,0,MIDI_CHANNEL);
  }

  if ((touches[15]) && !(prevtouches[15])) {
    transpose = 0;
    usbMIDI.sendControlChange(123,0,MIDI_CHANNEL);
  }

  if ((touches[16]) && !(prevtouches[16])) {
    transpose = 12;
    usbMIDI.sendControlChange(123,0,MIDI_CHANNEL);
  }

  if ((touches[17]) && !(prevtouches[17])) {
    transpose = 24;
    usbMIDI.sendControlChange(123,0,MIDI_CHANNEL);
  }

  if ((touches[18]) && !(prevtouches[18])) {
    transpose = 36;
    usbMIDI.sendControlChange(123,0,MIDI_CHANNEL);
  }

  
  for (int i = 0; i <= 12; i++) { //18 for all buttons to be keys
    if ((touches[i]) && !(prevtouches[i])) {
      usbMIDI.sendNoteOn(notes[i] + transpose, 100, MIDI_CHANNEL); //100 or 127?
    }
    if (!(touches[i]) && (prevtouches[i])) {
      usbMIDI.sendNoteOff(notes[i] + transpose, 0, MIDI_CHANNEL);
    }
    prevtouches[i] = touches[i];
  }
} //end of loop
