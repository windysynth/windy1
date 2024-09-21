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
        RVB Patch: Short Press to enter RVB Patch
            RVB Patch: Turn knob to adjust amount of reverb for the currently selected patch
                        Short press to go back up to Menu Mode
        DLY Patch: Short Press to enter DLY Patch
            DLY Patch: Turn knob to adjust amount of Delay for the currently selected patch
                        Short press to go back up to Menu Mode
        CHS Patch: Short Press to enter CHS Patch
            CHS Patch: Turn knob to toggle Chorus on/off for the currently selected patch
                        Short press to go back up to Menu Mode
        WR PCH FX: Short Press to enter WR PCH FX
             WR PCH FX: Turn knob to toggle between Skip and Write
                        Short press if Skip selected: goes back up to Menu Mode and does NOT write Patch Effects
                        Short press if Write selected: goes back up to Menu Mode and DOES write Patch Effects

    After 20 seconds of no activity the EEPROM is updated with any changes to the following settings:
        Volume, Patch, MIX, TUNING, TRANS, OCT
    The Patch Effects settings (RVB Patch, DLY Patch and CHS Patch) are only written to non-volatile memory with the WR PCH FX command. 
        

# Version History
ver 0.0.44:  Changed to stereo reverb (freeverbs) and increased maxDenseEarly to 0.8 from 0.5

ver 0.0.43:  Check if SGTL5000 already enable before enable(), to avoid loud click when exiting bootloader

ver 0.0.41:  Fixed UISM first state after splash screen to Vol instead of Menu Exit

ver 0.0.39:  Updated to Arduino IDE 2.3.2 and Teensyduino 1.60 Beta #1

# Build tools
Arduino IDE 2.3.2

Teensyduino 1.6 Beta #1 (plus custom changes in Pull Request #484 to https://github.com/PaulStoffregen/Audio/ )
 
# TODO
* Add Formant Filters
* Clean up code
