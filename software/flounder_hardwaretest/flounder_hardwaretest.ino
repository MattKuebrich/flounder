// roundabout flounder 1.0
// midi keyboard + controls + stereo audio

// hardware test

// after assembling the pcb, run this sketch and follow these steps to verify everything works

// 1. open the serial monitor, then move each of the 12 pots and you should see a readout from around 0 to 1024. in the ballpark is fine.
// 2. touch all the keys and you should see a readout of around 64 when fully touched
// 3. listen to the output and you should hear a sine tone from both channels
// 4. setting the "detailedtesttouches" variable below to 1 will give a continuous FastTouch readout for all the touch keys. useful for debugging.
// 5. all good!

byte detailedtesttouches = 0; 

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
int upperthreshold = 40;
int lowerthreshold = 30; 

//threashold for pin 13
int upperthreshold2 = 63;
int lowerthreshold2 = 58;

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
uint16_t pot_val[12];
uint16_t prev_pot_val[12];

#define POT_BIT_RES         10 // 10 works, 7-16 is valid
#define POT_NUM_READS       32 // 32 works

// Prevent jitter when reading the potentiometers.
// Higher value is less chance of jitter but also less precision.
const uint8_t nbrhd = 15;

// using arrays with teensy design tool
// https://forum.pjrc.com/threads/61270-Array-handling-in-the-audio-tool
// https://forum.pjrc.com/threads/58158-Accessing-Audio-Objects-from-an-Array

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=186.25000381469727,170.000018119812
AudioOutputUSB           usb1;           //xy=371.75000762939453,215.75001907348633
AudioOutputPT8211        pt8211_1;       //xy=377.5000114440918,135.00000762939453
AudioConnection          patchCord1(sine1, 0, usb1, 0);
AudioConnection          patchCord2(sine1, 0, usb1, 1);
AudioConnection          patchCord3(sine1, 0, pt8211_1, 0);
AudioConnection          patchCord4(sine1, 0, pt8211_1, 1);
// GUItool: end automatically generated code


void setup() {

  pinMode(13, INPUT_PULLUP);

  Serial.begin(9600);

  analogReadResolution(POT_BIT_RES ); // set the analog read resolution to 7 bits (a range of 0 - 127)
  analogReadAveraging(POT_NUM_READS); // average the analog value by averaging 16 readings

  AudioMemory(10);

  sine1.frequency(300);
  sine1.amplitude(0.9);

}

void loop() {
if (detailedtesttouches) { 
  testtouchesdetailed();
} else {
  testpots();
  testtouches();
}
} //end of loop


void testtouches(){
  for (int x = 0; x <= 18; x++) {
    // display fasttouch values when touched
    if (x == 12) {
      //special case for the pin 13 (LED), which has a different sensitivity. can be solved by unsoldering amber teensy LED. 
      if (fastTouchRead(touchkeys[x]) > 62) {
        touches[x] = 1;
        Serial.print("TOUCH ");
        Serial.print(touchkeys[x]);
        Serial.print(" = ");
        Serial.println(fastTouchRead(touchkeys[x]));
      } else if (fastTouchRead(touchkeys[x]) < 59) {
        touches[x] = 0;
      }
    } else {
      if (fastTouchRead(touchkeys[x]) > 40) {
        touches[x] = 1;
        Serial.print("TOUCH ");
        Serial.print(touchkeys[x]);
        Serial.print(" = ");
        Serial.println(fastTouchRead(touchkeys[x]));
      } else if (fastTouchRead(touchkeys[x]) < 30) {
        touches[x] = 0;
      }
    }
  }
}


void testtouchesdetailed(){
  for (int x = 0; x <= 18; x++) {
  Serial.print("T");
  Serial.print(touchkeys[x]);
  Serial.print("= ");
  Serial.print(fastTouchRead(touchkeys[x])); Serial.print(" | ");        
  }
  Serial.println();
  delay(200);
}



void testpots() {
  for (int i = 0; i < 12; i++) {
    pot_val[i] = analogRead(pot[i]);

    //different ways to do same thing
    if (pot_val[i] > prev_pot_val[i] + nbrhd || pot_val[i] < prev_pot_val[i] - nbrhd) {
      //if ((pot_val[i] < prev_pot_val[i] - nbrhd) || (pot_val[i] > prev_pot_val[i] + nbrhd)) {
      //if (abs(pot_val[i] - prev_pot_val[i]) > nbrhd) {

      Serial.print("POT ");
      Serial.print(i + 1);
      Serial.print(" = ");
      Serial.println(pot_val[i]);
      prev_pot_val[i] = pot_val[i];
    }
  }
}
