# Windy1 Synthesizer
Windy1 is a teensy 4.1 based MIDI host synthesizer designed to work with usb class complaint Wind controllers (e.g., EWI5000, EWIUSB,NuEVI, NuRAD, WARBL2, etc.)
The patches are editable with the Vyzex EWI4000S editor software or internally through the UI.

## Building the Windy1
Windy1 is easiest to build using the Arduino IDE. You will also need to download and install Teensyduino to build for and upload to the Teensy.

### libraries
You must replace the audio folder...
C:\Users\<user_name>\AppData\Local\Arduino15\packages\teensy\hardware\avr\0.60.1\libraries\Audio
with my forked version of this library (or just replace the files that are different with mine).
Currently, you want the "puleswidth_offset" branch here:  https://github.com/windysynth/Audio/tree/pulsewidth_offset

