# Windy1 Synthesizer
Windy1 is a teensy 4.1 based MIDI host synthesizer designed to work with usb class complaint Wind controllers (e.g., EWI5000, EWIUSB,NuEVI, NuRAD, WARBL2, etc.)
The patches are editable with the Vyzex EWI4000S editor software or internally through the UI.

## Building the Windy1
Windy1 is easiest to build using the Arduino IDE. You will also need to download and install Teensyduino to build for and upload to the Teensy.

### libraries
You must replace the audio folder...
C:\Users\<user_name>\AppData\Local\Arduino15\packages\teensy\hardware\avr\0.60.1\libraries\Audio
...with my forked version of this library (or just replace the files that are different with mine).
Currently, you want the "puleswidth_offset" branch here:  https://github.com/windysynth/Audio/tree/pulsewidth_offset

Also, use my forked version of hexefx_audiolib_F32, here...
https://github.com/windysynth/hexefx_audiolib_F32
clone the "ws_mods" branch to your libraries directory.
I have my "Preferenences>Sketchbook location" here...
c:\work\repositories
So my libraries are here...
C:\work\repositories\libraries

### hardware
I've created my own teensy 4.1 based board, but it mimics the 4.1 + audio shield.
The power supply section is a bit different, so I added pwrDownSense() (pin 25) to shut down the sgtl5000 chip before the power is removed fromt the audio circuits to minimize a loud pop as it is turned off. 

The Delay effect requires that the extra serial RAM chip is installed on the back of the teensy 4.1, which is why PSRAM_INSTALLED is defined
