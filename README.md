![flounder](https://github.com/MattKuebrich/flounder/blob/main/images/flounder_v1_assembled.jpg)

# flounder
usb midi keyboard + controls + stereo audio


**Tech details:**
* 12 MIDI mappable potentiometers
* full octave touch keyboard
* 6 function buttons
* stereo output
* programmable via Teensy 4.0 

**Programs:**

* **midictrl** is a basic midi controller with 12 MIDI-mappable knobs, a full-octave touch MIDI keyboard and octave control (using the 6 "function" buttons). 

*  **blipo_v2 / blipo_2018** are attempts to digitally emulate Rob Hordijk's Blippoo Box.

*  **hardwaretest** helps verify that everything is working properly by sending knob/key values to the serial monitor.

**BOM:**

https://docs.google.com/spreadsheets/d/1cLHydvrlG6ZNgeznXWJLk1cLfvSbNpC10iksKwk7y4E

**General parts / build notes:**

This PCB requires minimal parts, but there are a few tricky bits. A SMD header is used to access the 10 SMD pads on the bottom of the Teensy 4.0, which isn't easy to solder. [This helpful video from Befaco](https://www.youtube.com/watch?v=itzAFOCuFH4) shows how to solder the SMD header (and the other pin headers).

Flounder uses the PT8211 stereo DAC chip, which isn't widely available, but can be found either as part of [this low cost kit from PJRC](https://www.pjrc.com/store/pt8211_kit.html), on [AliExpress](https://www.aliexpress.com/wholesale?catId=0&initiative_id=SB_20220626100338&SearchText=PT8211&spm=a2g0o.home.1000002.0) or [this alternative](https://www.lcsc.com/product-detail/Digital-To-Analog-Converters-DACs_TM8211_C92003.html) from LCSC, which I haven't tried but should work. 

**Software installation:**

In order to program the Teensy 4.0, you'll need to install [TeensyDuino](https://www.pjrc.com/teensy/teensyduino.html). You'll also need to install the [FastTouch library](https://github.com/adrianfreed/FastTouch), which is needed for the touch keyboard. Download FastTouch from Github and then in TeensyDuino, go to Sketch -> Include Library -> Add .ZIP Library and select the .ZIP file. 

Plug the [(USB Micro-B to standard USB) cable](https://www.pjrc.com/store/cable_usb_micro_b.html) from the Teensy to your computer. In TeensyDuino, under Tools->Port, you should see select the Teensy there. Also, Tools->USB Type should be set to "Serial + MIDI + Audio". 

The audio output uses both the on-board jack and USB audio. In code, the output can be adjusted to be suitable for headphones or line level.

**Power:**

flounder is usually powered via the USB connection to your computer or another MIDI host (like monome norns). When using it as an audio source, it can also be powered by a cellphone charger or portable power bank.
