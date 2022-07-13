/*  

flounder

program: blipo_2018

description: an attempt to digitally emulate Rob Hordijk's Blippoo Box.

notes: this is based on the 2018 version of the Blippoo Box, explained here:
https://www.youtube.com/watch?v=ftyw5_hX4H0&t=306s

knob layout:

RATE A         RATE B          PEAK 1                 PEAK 2
RUNGLER > A    RUNGLER > B     RUNGLER > PEAK 1       RUNGLER > PEAK 2
S&H > RATE A   S&H > RATE A    S&H SPREADS PEAKS      FILTER FEEDBACK

FIRST FUNCTION BUTTON = TOGGLES S&H MODE BETWEEN R MIX AND TRI B.

further notes:
the on-board LED will light up when toggling between the S&H mode.
"filter feedback" crudely attempts to add the filter distortion in the original hardware.
rather than a fixed value, i've made this adjustable since this knob was original an "audio in"
which flounder doesn't support.

*/

//set to 1 for use headphone output, 0 for line out
int headphoneout = 0;

#include "effect_samplehold.h"
#include "effect_rungler.h"
#include "effect_comparator.h"

#include <FastTouch.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioMixer4              mixer4;         //xy=142.5,948.5
AudioMixer4              mixer3;         //xy=199.5,640.5
AudioSynthWaveformModulated waveformMod4;   //xy=356.5,909.5
AudioSynthWaveformModulated waveformMod3;   //xy=358.5,875.5
AudioSynthWaveformModulated waveformMod1;   //xy=362.5,659.5
AudioSynthWaveformModulated waveformMod2;   //xy=363.5,696.5
AudioMixer4              mixer6;         //xy=612.5,485.5
AudioMixer4              mixer7;         //xy=615.5,573.5
AudioEffectRungler       rungler1;       //xy=665.5,940.5
AudioEffectRungler       rungler2;       //xy=667.5,993.5
AudioEffectComparator    compare1;       //xy=774.5,703.5
AudioEffectSampleAndHold samplehold1;    //xy=827.5,462.5
AudioMixer4              mixer5;         //xy=1016.5,863.5
AudioMixer4              mixer2;         //xy=1023.3888893127441,561.7222518920898
AudioFilterLadder        ladder2;        //xy=1154.5,753.5
AudioFilterLadder        ladder1;        //xy=1158.5,669.5
AudioAmplifier           amp4;           //xy=1323.5,744.5
AudioMixer4              mixer1;         //xy=1450.5,696.5
AudioMixer4              mixer8;         //xy=1451.5,836.5
AudioOutputUSB           usb1;           //xy=1639.5,685.5
AudioOutputPT8211        pt8211_1;       //xy=1684.5,821.5
AudioConnection          patchCord1(mixer4, 0, waveformMod3, 0);
AudioConnection          patchCord2(mixer4, 0, waveformMod4, 0);
AudioConnection          patchCord3(mixer3, 0, waveformMod1, 0);
AudioConnection          patchCord4(mixer3, 0, waveformMod2, 0);
AudioConnection          patchCord5(waveformMod4, 0, rungler1, 0);
AudioConnection          patchCord6(waveformMod4, 0, rungler2, 1);
AudioConnection          patchCord7(waveformMod3, 0, compare1, 1);
AudioConnection          patchCord8(waveformMod3, 0, mixer6, 0);
AudioConnection          patchCord9(waveformMod1, 0, compare1, 0);
AudioConnection          patchCord10(waveformMod2, 0, rungler1, 1);
AudioConnection          patchCord11(waveformMod2, 0, rungler2, 0);
AudioConnection          patchCord12(waveformMod2, 0, mixer7, 0);
AudioConnection          patchCord13(mixer6, 0, samplehold1, 0);
AudioConnection          patchCord14(mixer7, 0, samplehold1, 1);
AudioConnection          patchCord15(rungler1, 0, mixer4, 1);
AudioConnection          patchCord16(rungler1, 0, mixer6, 1);
AudioConnection          patchCord17(rungler1, 0, mixer2, 0);
AudioConnection          patchCord18(rungler2, 0, mixer3, 0);
AudioConnection          patchCord19(rungler2, 0, mixer6, 2);
AudioConnection          patchCord20(rungler2, 0, mixer5, 0);
AudioConnection          patchCord21(compare1, 0, ladder1, 0);
AudioConnection          patchCord22(compare1, 0, ladder2, 0);
AudioConnection          patchCord23(compare1, 0, mixer7, 1);
AudioConnection          patchCord24(samplehold1, 0, mixer3, 1);
AudioConnection          patchCord25(samplehold1, 0, mixer4, 0);
AudioConnection          patchCord26(samplehold1, 0, mixer2, 1);
AudioConnection          patchCord27(samplehold1, 0, mixer5, 1);
AudioConnection          patchCord28(mixer5, 0, ladder2, 1);
AudioConnection          patchCord29(mixer2, 0, ladder1, 1);
AudioConnection          patchCord30(ladder2, amp4);
AudioConnection          patchCord31(ladder2, 0, mixer5, 2);
AudioConnection          patchCord32(ladder1, 0, mixer1, 0);
AudioConnection          patchCord33(ladder1, 0, mixer8, 0);
AudioConnection          patchCord34(ladder1, 0, mixer2, 2);
AudioConnection          patchCord36(amp4, 0, mixer8, 1);
AudioConnection          patchCord35(amp4, 0, mixer1, 1);
AudioConnection          patchCord37(mixer1, 0, usb1, 0);
AudioConnection          patchCord38(mixer1, 0, usb1, 1);
AudioConnection          patchCord39(mixer8, 0, pt8211_1, 0);
AudioConnection          patchCord40(mixer8, 0, pt8211_1, 1);


//stereo
//AudioConnection          patchCord37(amp4, 0, usb1, 0);
//AudioConnection          patchCord38(ladder1, 0, usb1, 1);


// GUItool: end automatically generated code
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

//touch stuff
int touchkeys[] = {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 28, 29, 30, 31, 32, 33};
int upperthreshold = 62;
int lowerthreshold = 50; //55

int touches = 0;
int ledState = LOW;     // the current state of LED
int lastButtonState;    // the previous state of button
int currentButtonState; // the current state of button

int touches2 = 0;
int ledState2 = LOW;     // the current state of LED
int lastButtonState2;    // the previous state of button
int currentButtonState2; // the current state of button

float oscAfreq;
float oscBfreq;
float SHtoOscA;
float SHtoOscB;
float peak1;
float peak2;
float SHSpreadPeaks;

float RunglerAtoOscB;
float RunglerBtoOscA;

float RunglerAtoPeak1;
float RunglerBtoPeak2;
float filterdrive;
float filterpbgain;
float FilterFeedback;

int SHmode = 3; // runglers mixed

void setup() {
  AudioMemory(20);
  Serial.begin(9600);

  //oscA
  waveformMod1.begin(WAVEFORM_TRIANGLE);
  waveformMod1.amplitude(1);

  waveformMod2.begin(WAVEFORM_SQUARE);
  waveformMod2.amplitude(1);

  //oscB
  waveformMod3.begin(WAVEFORM_TRIANGLE);
  waveformMod3.amplitude(1);

  waveformMod4.begin(WAVEFORM_SQUARE);
  waveformMod4.amplitude(1);

  //output mixer for usb
  mixer1.gain(0, 1);  
  mixer1.gain(1, 1);

  //output mixer for output jack
  if (headphoneout) {
    mixer8.gain(0, 0.1);  // lower output for headphones
    mixer8.gain(1, 0.1);
  } else {
    mixer8.gain(0, 1);  // line out
    mixer8.gain(1, 1);
  }

  //s&h trigger selection
  if (SHmode == 1) {
    mixer7.gain(0, 1); // osc A square
    mixer6.gain(0, 1); // osc B triangle
    //off
    mixer6.gain(1, 0); // runglers combine
    mixer6.gain(2, 0); // runglers combine
    mixer7.gain(1, 0); // comparator (osc A and osc B compared)
  }

  if (SHmode == 2) {
    mixer7.gain(1, 1); // comparator (osc A and osc B compared)
    mixer6.gain(0, 1); // osc B triangle

    //off
    mixer6.gain(1, 0); // runglers combine
    mixer6.gain(2, 0); // runglers combine
    mixer7.gain(0, 0); // comparator (osc A and osc B compared)
  }

  if (SHmode == 3) {
    mixer7.gain(0, 1); // osc A square
    mixer6.gain(1, 0.5); // runglers combine 0.5?
    mixer6.gain(2, 0.5); // runglers combine

    //off
    mixer7.gain(1, 0); // comparator (osc A and osc B compared)
    mixer6.gain(0, 0); // osc B triangle
  }


  //inverted gain for twin peaks filter
  amp4.gain(-1); //invert

  ladder1.frequency(5000);
  ladder2.frequency(5000);
  ladder1.resonance(0.94);
  ladder2.resonance(0.94);
  ladder1.octaveControl(10); //18?
  ladder2.octaveControl(10);
  ladder1.inputDrive(0.5); // fixed value
  ladder2.inputDrive(0.5); // fixed value

}

void loop() {

  //looks for a press of the first function button and toggle built in led
  if (fastTouchRead(28) > upperthreshold) {
    touches = 1;
  } else if (fastTouchRead(28) < lowerthreshold) {
    touches = 0;
  }

  lastButtonState    = currentButtonState;      // save the last state
  currentButtonState = touches; // read new state

  if (lastButtonState == HIGH && currentButtonState == LOW) {

    // toggle state of LED
    ledState = !ledState;

    // control LED arccoding to the toggled state
    digitalWrite(LED_BUILTIN, ledState);
    //analogWrite(LED_BUILTIN, brightness); //control brightness of built-in LED, could be useful for "modes"
  }



  //looks for a press of the first function button and toggle built in led
  if (fastTouchRead(29) > upperthreshold) {
    touches2 = 1;
  } else if (fastTouchRead(29) < lowerthreshold) {
    touches2 = 0;
  }

  lastButtonState2    = currentButtonState2;      // save the last state
  currentButtonState2 = touches2; // read new state

  if (lastButtonState2 == HIGH && currentButtonState2 == LOW) {

    // toggle state of LED
    ledState2 = !ledState2;

    // control LED arccoding to the toggled state
    //digitalWrite(LED_BUILTIN, ledState);
  }


  //toggle between s&h modes
  if (ledState == HIGH) {
    SHmode = 1;

  } else {
    SHmode = 3;
  }

  //toggle rungler loop
  if (ledState2 == HIGH) {
    rungler1.loopEnable(1);
    rungler2.loopEnable(1);
    Serial.println("loop enabled");

  } else {
    rungler1.loopEnable(0);
    rungler2.loopEnable(0);
    Serial.println("loop disabled");
  }

  //Serial.println(SHmode);

  //s&h trigger selection
  if (SHmode == 1) {
    mixer7.gain(0, 1); // osc A square
    mixer6.gain(0, 1); // osc B triangle
    //off
    mixer6.gain(1, 0); // runglers combine
    mixer6.gain(2, 0); // runglers combine
    mixer7.gain(1, 0); // comparator (osc A and osc B compared)
  }

  if (SHmode == 2) {
    mixer7.gain(1, 1); // comparator (osc A and osc B compared)
    mixer6.gain(0, 1); // osc B triangle

    //off
    mixer6.gain(1, 0); // runglers combine
    mixer6.gain(2, 0); // runglers combine
    mixer7.gain(0, 0); // comparator (osc A and osc B compared)
  }

  if (SHmode == 3) {
    mixer7.gain(0, 1); // osc A square
    mixer6.gain(1, 0.5); // runglers combine
    mixer6.gain(2, 0.5); // runglers combine

    //off
    mixer7.gain(1, 0); // comparator (osc A and osc B compared)
    mixer6.gain(0, 0); // osc B triangle
  }





  //all pots

  // oscAfreq  = mapf(analogRead(POT01), 0, 1023, 0.1, 20000);
  oscAfreq = fscale( 0, 1023.0, 0.1, 12000.0, analogRead(POT01), -9.3); //pdf says oscs go to 12khz, sc goes to 20000
  //Serial.println(oscAfreq);

  // oscBfreq  = map(analogRead(POT02), 0, 1023, 0, 60);
  oscBfreq = fscale( 0, 1023.0, 0.1, 12000.0, analogRead(POT02), -9.3);

  //peak1 = mapf(analogRead(POT03), 0, 1023, 0, 12000);
  peak1 = fscale( 0, 1023.0, 20, 12000.0, analogRead(POT03), -9.3);

  //peak2 = mapf(analogRead(POT04), 0, 1023, 0, 12000);
  peak2 = fscale( 0, 1023.0, 20, 12000.0, analogRead(POT04), -9.3);

  RunglerAtoOscB = mapf(analogRead(POT05), 0, 1023, 0, 1.0);
  RunglerBtoOscA = mapf(analogRead(POT06), 0, 1023, 0, 1.0);

  RunglerAtoPeak1 = mapf(analogRead(POT07), 0, 1023, 0, 1.0);
  RunglerBtoPeak2 = mapf(analogRead(POT08), 0, 1023, 0, 1.0);

  SHtoOscA   = mapf(analogRead(POT09), 0, 1023, 0, 1.0);
  SHtoOscB   = mapf(analogRead(POT10), 0, 1023, 0, 1.0);

  SHSpreadPeaks    = mapf(analogRead(POT11), 0, 1023, 0, 1.0); //used as gain, so this SHOULD be 0 to 1, right? if using -1 to 1 the knob would need to be set at the middle to give NO modulation.

  //pot 12 on the 2018 blippoo is usually a volume control, but testing out filter drive
  //filterdrive    = mapf(analogRead(POT12), 0, 1023, 1.0, 4.0);
  FilterFeedback    = mapf(analogRead(POT12), 0, 1023, 0, 4);


  //Serial.println(FilterFeedback);


  waveformMod1.frequency(oscAfreq); //tri
  waveformMod2.frequency(oscAfreq); //square

  waveformMod3.frequency(oscBfreq); //tri
  waveformMod4.frequency(oscBfreq); //square

  // stuff going into filter cutoff
  mixer2.gain(0, RunglerAtoPeak1 / 2);
  mixer2.gain(1, SHSpreadPeaks / 2);
  mixer2.gain(2, FilterFeedback / 2);



  mixer5.gain(0, RunglerBtoPeak2 / 2);
  mixer5.gain(1, (SHSpreadPeaks / 2) * -1); //??? trying to invert this to spread
  mixer5.gain(2, FilterFeedback / 2);



  //stuff going into OscAfreq
  mixer3.gain(0, RunglerBtoOscA / 2);
  mixer3.gain(1, SHtoOscA / 2);

  //stuff going into OscBfreq
  mixer4.gain(0, SHtoOscB / 2);
  mixer4.gain(1, RunglerAtoOscB / 2);

  ladder1.frequency(peak1);
  ladder2.frequency(peak2);

  //ladder1.inputDrive(filterdrive);
  //ladder2.inputDrive(filterdrive);


} //end of loop


// floating point map function taken from https://forum.arduino.cc/index.php?topic=361046.0
double mapf(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//fscale for log response on frequency
//https://playground.arduino.cc/Main/Fscale/
float fscale( float originalMin, float originalMax, float newBegin, float
              newEnd, float inputValue, float curve) {

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;

  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin) {
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float


  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0) {
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}
