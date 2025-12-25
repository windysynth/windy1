// process_midi.h
/************************************************************
  Windy1 USB Host Synth for Wind controllers
        MIT license
        Copyright (c) 2021 Tim King

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

****************************************************************/
#ifndef process_midi_h_
#define process_midi_h_
#include <Arduino.h>
#include <MIDI.h>
#include "globals.h"

// SysEx commands from Akai
#define MIDI_PRESET_DUMP (0x00) // PUT SELECTED button
#define MIDI_PRESET_DUMP_REQ (0x40)
#define MIDI_QUICKPC_DUMP (0x01)
#define MIDI_QUICKPC_DUMP_REQ (0x41)
#define MIDI_EDIT_LOAD (0x10)
#define MIDI_EDIT_STORE (0x11)
#define MIDI_EDIT_DUMP (0x20) // click on preset
#define MIDI_EDIT_DUMP_REQ (0x60)
#define MIDI_SYSEX_HEADER (0xf0)
#define MIDI_SYSEX_TRAILER (0xf7)
#define MIDI_SYSEX_GEN_INFO (0x06)
#define MIDI_SYSEX_ID_REQ (0x01)
#define MIDI_SYSEX_ID (0x02)
#define MIDI_SYSEX_AKAI_ID (0x47)    // 71.
#define MIDI_SYSEX_AKAI_EWI4K (0x64) // 100.
#define MIDI_SYSEX_CHANNEL (0x00)
#define MIDI_SYSEX_NONREALTIME (0x7e)
#define MIDI_SYSEX_ALLCHANNELS (0x7f)
#define MIDI_CC_DATA_ENTRY (0x06)
#define MIDI_CC_NRPN_LSB (0x62) // 98
#define MIDI_CC_NRPN_MSB (0x63) // 99
#define MAX_SYSEX_LENGTH (262144)
#define EWI_SYSEX_PRESET_DUMP_LEN (206)
#define MIDI_PRESET_DUMP_REQ_LEN (0x07)
#define EWI_SYSEX_QUICKPC_DUMP_LEN (91)
#define EWI_SYSEX_ID_RESPONSE_LEN (15)
#define EWI_NUM_QUICKPCS (84)

// Forward-declare USB host MIDI device type (defined in the USB host library)
// class MIDIDevice_BigBuffer;

// Declare the USB-host MIDI device instance defined in `main.cpp`
// extern MIDIDevice_BigBuffer midi_ho;

// Declare the Serial MIDI instance created by `MIDI_CREATE_INSTANCE` in `main.cpp`.
// `MIDI.h` provides the `MIDI_NAMESPACE` and `MidiInterface` templates.
// extern MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>> MIDIs1;

// Function prototypes
void processNrpnMessage();
void processUsbMIDI(void);
void printBytes(const byte *data, unsigned int size);
void processSysex(const uint8_t *pSysexData, unsigned int size);
#endif // process_midi_h_
