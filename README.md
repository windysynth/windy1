# Windy1
Midi Host synthesizer using Teensy 4.1 and Audio Adapter board circuitry, compatible with Vyzex EWI4000s Editor.

The Windy 1 mimics the synth inside an EWI 4000 (monophonic only). 

# User Interface
    Normal Mode (upon power up): short press to toggle between Volume and Patch select.
        Long Press (> 0.5 sec) enters Menu Mode.
    Menu Mode
        EXIT: short press to exit back to Normal Mode
            Rotate knob to select between EXIT, MIX, TUNING, TRANS, OCT
        MIX: Short press to enter MixLineIn
            MixLineIn: turn knob to select % LineIn signal (100 is only line in, 0 is only Synth)
                        Short press to go back to MENU Mode
        TUNING: Short Press to enter Cents
            Cents: Turn knob to select tuning value (range: +/- 100 cents)
                        Short press to go back to MENU Mode
        TRANS: Short Press to enter Semi
            Semi: Turn knob to transpose by a number of semi-tones (range: +/- 12 semi-tones)
                        Short press to go back to MENU Mode
        OCT: Short Press to enter Oct
            Oct: Turn knob to set octave (range: +/- 2 octaves)
                        Short press to go back to MENU Mode
    After 20 seconds of no activity the EEPROM is updated with any changes to the above settings
        

# Version
ver 0.0.23
 
# TODO
* Add Formant Filters
* Clean up code
