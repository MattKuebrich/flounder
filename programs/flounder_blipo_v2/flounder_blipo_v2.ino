/* 

flounder

program: blipo_v2

description: an attempt to digitally emulate Rob Hordijk's Blippoo Box.

notes: this is based on the V2 version of the Blippoo Box (as is the Supercollider/Norns version).

knob layout:

RATE A       RATE B      PEAK 1           PEAK 2
FM B TO A    FM A TO B   S&H TO RES       RUNGLER A TO RES
RUNGLER A    RUNGER B    S&H TO RATE A    RUNGLER B TO RES

FIRST FUNCTION BUTTON = TOGGLES S&H MODE 

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
AudioMixer4              mixer4;         //xy=263.33331298828125,816.6666667461395
AudioMixer4              mixer3;         //xy=272.33331298828125,554.6666667461395
AudioSynthWaveformModulated waveformMod4;   //xy=474.33331298828125,770.6666667461395
AudioSynthWaveformModulated waveformMod3;   //xy=476.33331298828125,736.6666667461395
AudioSynthWaveformModulated waveformMod1;   //xy=480.33331298828125,520.6666667461395
AudioSynthWaveformModulated waveformMod2;   //xy=481.33331298828125,557.6666667461395
AudioMixer4              mixer6;         //xy=730.3333129882812,346.6666667461395
AudioEffectRungler       rungler1;       //xy=735.3333129882812,759.6666667461395
AudioMixer4              mixer7;         //xy=763.3333129882812,439.6666667461395
AudioEffectRungler       rungler2;       //xy=780.3333129882812,862.6666667461395
AudioEffectComparator    compare1;       //xy=892.3333129882812,564.6666667461395
AudioEffectSampleAndHold samplehold1;    //xy=908.3333129882812,418.6666667461395
AudioAnalyzePrint        print1;         //xy=1007.3333129882812,908.6666667461395
AudioMixer4              mixer2;         //xy=1054.3333129882812,735.6666667461395
AudioMixer4              mixer8;         //xy=1114.3333129882812,385.6666667461395
AudioFilterLadder        ladder1;        //xy=1238.3333129882812,495.6666667461395
AudioMixer4              mixer5;         //xy=1248.3333129882812,802.6666667461395
AudioFilterLadder        ladder2;        //xy=1272.3333129882812,614.6666667461395
AudioAmplifier           amp4;           //xy=1478.3333129882812,644.6666667461395
AudioMixer4              mixer1;         //xy=1666.6666069030762,557.666690826416
AudioMixer4              mixer9;         //xy=1671.6667728424072,649.3333206176758
AudioOutputPT8211        pt8211_1;       //xy=1821.999870300293,451.33337688446045
AudioOutputUSB           usb1;           //xy=1838.3332023620605,681.3333530426025
AudioConnection          patchCord1(mixer4, 0, waveformMod3, 0);
AudioConnection          patchCord2(mixer4, 0, waveformMod4, 0);
AudioConnection          patchCord3(mixer3, 0, waveformMod1, 0);
AudioConnection          patchCord4(mixer3, 0, waveformMod2, 0);
AudioConnection          patchCord5(waveformMod4, 0, rungler1, 0);
AudioConnection          patchCord6(waveformMod4, 0, rungler2, 1);
AudioConnection          patchCord7(waveformMod3, 0, compare1, 1);
AudioConnection          patchCord8(waveformMod3, 0, mixer3, 2);
AudioConnection          patchCord9(waveformMod3, 0, mixer6, 0);
AudioConnection          patchCord10(waveformMod3, print1);
AudioConnection          patchCord11(waveformMod1, 0, compare1, 0);
AudioConnection          patchCord12(waveformMod1, 0, mixer4, 0);
AudioConnection          patchCord13(waveformMod2, 0, rungler1, 1);
AudioConnection          patchCord14(waveformMod2, 0, rungler2, 0);
AudioConnection          patchCord15(waveformMod2, 0, mixer7, 0);
AudioConnection          patchCord16(mixer6, 0, samplehold1, 0);
AudioConnection          patchCord17(rungler1, 0, mixer2, 1);
AudioConnection          patchCord18(rungler1, 0, mixer4, 1);
AudioConnection          patchCord19(rungler1, 0, mixer6, 1);
AudioConnection          patchCord20(mixer7, 0, samplehold1, 1);
AudioConnection          patchCord21(rungler2, 0, mixer3, 0);
AudioConnection          patchCord22(rungler2, 0, mixer2, 2);
AudioConnection          patchCord23(rungler2, 0, mixer6, 2);
AudioConnection          patchCord24(compare1, 0, ladder2, 0);
AudioConnection          patchCord25(compare1, 0, mixer7, 1);
AudioConnection          patchCord26(compare1, 0, ladder1, 0);
AudioConnection          patchCord27(samplehold1, 0, mixer3, 1);
AudioConnection          patchCord28(samplehold1, 0, mixer2, 0);
AudioConnection          patchCord29(mixer2, 0, mixer5, 0);
AudioConnection          patchCord30(mixer2, 0, mixer8, 0);
AudioConnection          patchCord31(mixer8, 0, ladder1, 1);
AudioConnection          patchCord32(ladder1, 0, mixer1, 0);
AudioConnection          patchCord33(ladder1, 0, mixer8, 1);
AudioConnection          patchCord34(ladder1, 0, mixer9, 0);
AudioConnection          patchCord35(mixer5, 0, ladder2, 1);
AudioConnection          patchCord36(ladder2, amp4);
AudioConnection          patchCord37(ladder2, 0, mixer5, 1);
AudioConnection          patchCord38(amp4, 0, mixer1, 1);
AudioConnection          patchCord39(amp4, 0, mixer9, 1);
AudioConnection          patchCord40(mixer1, 0, pt8211_1, 0);
AudioConnection          patchCord41(mixer1, 0, pt8211_1, 1);
AudioConnection          patchCord42(mixer9, 0, usb1, 0);
AudioConnection          patchCord43(mixer9, 0, usb1, 1);
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
int touches = 0;
int ledState = LOW;     // the current state of LED
int lastButtonState;    // the previous state of button
int currentButtonState; // the current state of button
int upperthreshold = 62; 
int lowerthreshold = 50; //55 

float oscAfreq;
float oscBfreq;
float FMBtoA;
float FMAtoB;
float peak1;
float peak2;
float SHtoRes;
float SHtoOscA;
float RunglerAtoRes;
float RunglerBtoRes;
float RunglerAtoOscB;
float RunglerBtoOscA;
float FilterFeedback;

int SHmode = 1; 

void setup() {
  AudioMemory(20);
  Serial.begin(9600);


//print1.length(1000);



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
  mixer9.gain(0, 1);
  mixer9.gain(1, 1);

  //output mixer for output jack
  if (headphoneout) {
    mixer1.gain(0, 0.1);  // lower output for headphones
    mixer1.gain(1, 0.1);
  } else {
    mixer1.gain(0, 1);  // line out
    mixer1.gain(1, 1);
  }

//FilterFeedback = 0.75/2;  //0.75
FilterFeedback = 0; //kind of like this without the filter feedback

  mixer5.gain(0, 1);
  mixer5.gain(1, FilterFeedback); //for twin peak distortion.
  mixer8.gain(0, 1); 
  mixer8.gain(1, FilterFeedback); //for twin peak distortion.

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


  //inverted gain for twin peaks filter
  amp4.gain(-1); //invert

  ladder1.frequency(5000);
  ladder2.frequency(5000);
  ladder1.resonance(0.94); //1 gives too much resonance
  ladder2.resonance(0.94);
  ladder1.octaveControl(12); //2.6?
  ladder2.octaveControl(12);
}

void loop() {
//print1.trigger();

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
}

//toggle between s&h modes
if (ledState == HIGH){
 SHmode = 2;
} else {
 SHmode = 1;
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



//analogWrite(LED_BUILTIN, brightness); //control brightness of built-in LED, could be useful for "modes"

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

FMBtoA = mapf(analogRead(POT05), 0, 1023, 0, 1.0);
FMAtoB = mapf(analogRead(POT06), 0, 1023, 0, 1.0);
SHtoRes    = mapf(analogRead(POT07), 0, 1023, 0, 1.0);
RunglerAtoRes = mapf(analogRead(POT08), 0, 1023, 0, 1.0);
RunglerAtoOscB = mapf(analogRead(POT09), 0, 1023, 0, 1.0);
RunglerBtoOscA = mapf(analogRead(POT10), 0, 1023, 0, 1.0);
SHtoOscA   = mapf(analogRead(POT11), 0, 1023, 0, 1.0);
RunglerBtoRes = mapf(analogRead(POT12), 0, 1023, 0, 1.0);

waveformMod1.frequency(oscAfreq); //tri
waveformMod2.frequency(oscAfreq); //square

waveformMod3.frequency(oscBfreq); //tri
waveformMod4.frequency(oscBfreq); //square

// stuff going into filter cutoff
mixer2.gain(0, SHtoRes / 3);
mixer2.gain(1, RunglerAtoRes / 3);
mixer2.gain(2, RunglerBtoRes / 3);

//stuff going into OscAfreq
mixer3.gain(0, RunglerBtoOscA / 3);
mixer3.gain(1, SHtoOscA / 3);
mixer3.gain(2, FMBtoA / 3);

//stuff going into OscBfreq
mixer4.gain(0, FMAtoB / 2);
mixer4.gain(1, RunglerAtoOscB / 2);

ladder1.frequency(peak1);
ladder2.frequency(peak2);

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
