/************************************************************
  Windy1 USB Host Synth for Wind controllers
        MIT license
        Copyright (c) 2021 Tim King

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
****************************************************************/

#include "patches.h"


  // SysEx commands from Akai
#define MIDI_PRESET_DUMP  (0x00)      // PUT SELECTED button
#define MIDI_PRESET_DUMP_REQ  (0x40)
#define MIDI_QUICKPC_DUMP     (0x01)
#define MIDI_QUICKPC_DUMP_REQ (0x41)
#define MIDI_EDIT_LOAD        (0x10)
#define MIDI_EDIT_STORE       (0x11)
#define MIDI_EDIT_DUMP        (0x20)      // click on preset
#define MIDI_EDIT_DUMP_REQ    (0x60)
#define MIDI_SYSEX_HEADER     (0xf0)
#define MIDI_SYSEX_TRAILER    (0xf7)
#define MIDI_SYSEX_GEN_INFO   (0x06)
#define MIDI_SYSEX_ID_REQ     (0x01)
#define MIDI_SYSEX_ID         (0x02)
#define MIDI_SYSEX_AKAI_ID    (0x47)// 71.
#define MIDI_SYSEX_AKAI_EWI4K (0x64)// 100.
#define MIDI_SYSEX_CHANNEL    (0x00)
#define MIDI_SYSEX_NONREALTIME (0x7e)
#define MIDI_SYSEX_ALLCHANNELS (0x7f)
#define MIDI_CC_DATA_ENTRY    (0x06)
#define MIDI_CC_NRPN_LSB      (0x62)   // 98
#define MIDI_CC_NRPN_MSB      (0x63)// 99
#define MAX_SYSEX_LENGTH       (262144)
#define EWI_SYSEX_PRESET_DUMP_LEN (206) 
#define MIDI_PRESET_DUMP_REQ_LEN (0x07) 
#define EWI_SYSEX_QUICKPC_DUMP_LEN  (91)
#define EWI_SYSEX_ID_RESPONSE_LEN  (15)
#define EWI_NUM_QUICKPCS       (84)


void processNrpnMessage()
{
     switch(usbMidiNrpnMsbOld)
    {
        case NRPN_MSB_OSC1:
            current_patch.nrpn_msb_osc1[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_OSC2:
            current_patch.nrpn_msb_osc2[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_OSC_FILT1:
            current_patch.nrpn_msb_osc_filt1[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_OSC_FILT2:
            current_patch.nrpn_msb_osc_filt2[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_NOISE_FILT3:
            current_patch.nrpn_msb_noise_filt3[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_NOISE_FILT4:
            current_patch.nrpn_msb_noise_filt4[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_SPARE:
            current_patch.nrpn_msb_spare[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_NOISE:
            current_patch.nrpn_msb_noise[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_COMMON1:
            current_patch.nrpn_msb_common1[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_COMMON2:
            current_patch.nrpn_msb_common2[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_CHORUS:
            current_patch.nrpn_msb_chorus[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_DELAY:
            current_patch.nrpn_msb_delay[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        case NRPN_MSB_REVERB:
            current_patch.nrpn_msb_reverb[usbMidiNrpnLsbOld] = usbMidiNrpnData; 
            break;
        default:
            break;
    }
    patchToSynthVariables(&current_patch);
}


void processUsbMIDI(void) 
{
  byte type, channel, data1, data2; //, cable;
    unsigned int sysexSize = 0;
    byte *pSysexData = NULL;
  // fetch the MIDI message, defined by these 5 numbers (except SysEX)
  //
  type = usbMIDI.getType();       // which MIDI message, 128-255
  channel = usbMIDI.getChannel(); // which MIDI channel, 1-16
  data1 = usbMIDI.getData1();     // first data byte of message, 0-127
  data2 = usbMIDI.getData2();     // second data byte of message, 0-127
  //cable = usbMIDI.getCable();     // which virtual cable with MIDIx8, 0-7

  // uncomment if using multiple virtual cables
  //Serial8.print("cable ");
  //Serial8.print(cable, DEC);
  //Serial8.print(": ");

  // print info about the message
  //
  switch (type) {
    case usbMIDI.NoteOff: // 0x80
      Serial8.print("Note Off, ch=");
      Serial8.print(channel, DEC);
      Serial8.print(", note=");
      Serial8.print(data1, DEC);
      Serial8.print(", velocity=");
      Serial8.println(data2, DEC);
      break;

    case usbMIDI.NoteOn: // 0x90
      Serial8.print("Note On, ch=");
      Serial8.print(channel, DEC);
      Serial8.print(", note=");
      Serial8.print(data1, DEC);
      Serial8.print(", velocity=");
      Serial8.println(data2, DEC);
      break;

    case usbMIDI.AfterTouchPoly: // 0xA0
      Serial8.print("AfterTouch Change, ch=");
      Serial8.print(channel, DEC);
      Serial8.print(", note=");
      Serial8.print(data1, DEC);
      Serial8.print(", velocity=");
      Serial8.println(data2, DEC);
      break;

    case usbMIDI.ControlChange: // 0xB0
      switch (data1){ 
        case CC_NRPN_DATA_ENTRY:
            usbMidiNrpnData = data2;
            break;
        case CC_NRPN_LSB:
            usbMidiNrpnLsbOld = usbMidiNrpnLsbNew;
            usbMidiNrpnLsbNew = data2;
            break;
        case CC_NRPN_MSB:
            usbMidiNrpnMsbOld = usbMidiNrpnMsbNew;
            usbMidiNrpnMsbNew = data2;
            if (usbMidiNrpnMsbNew == 127 && usbMidiNrpnLsbNew == 127)
            {
                processNrpnMessage();
                sprintf(str_buf1, "NRPN(M) %d, NRPN(L) %d, Data: %d", usbMidiNrpnMsbOld, usbMidiNrpnLsbOld, usbMidiNrpnData );
                Serial8.println(str_buf1);
            }
            break;
        default:
            Serial8.print("Control Change, ch=");
            Serial8.print(channel, DEC);
            Serial8.print(", control=");
            Serial8.print(data1, DEC);
            Serial8.print(", value=");
            Serial8.println(data2, DEC);
            break;
      }
      break;

    case usbMIDI.ProgramChange: // 0xC0
      Serial8.print("Program Change, ch=");
      Serial8.print(channel, DEC);
      Serial8.print(", program=");
      Serial8.println(data1, DEC);
      break;

    case usbMIDI.AfterTouchChannel: // 0xD0
      Serial8.print("After Touch, ch=");
      Serial8.print(channel, DEC);
      Serial8.print(", pressure=");
      Serial8.println(data1, DEC);
      break;

    case usbMIDI.PitchBend: // 0xE0
      Serial8.print("Pitch Change, ch=");
      Serial8.print(channel, DEC);
      Serial8.print(", pitch=");
      Serial8.println(data1 + data2 * 128, DEC);
      break;

    case usbMIDI.SystemExclusive: // 0xF0
      // Messages larger than usbMIDI's internal buffer are truncated.
      // To receive large messages, you *must* use the 3-input function
      // handler.  See InputFunctionsComplete for details.
      sysexSize = (unsigned int)data1 + (unsigned int)data2*256; 
      pSysexData = usbMIDI.getSysExArray();
      Serial8.print("SysEx Size: ");
      Serial8.print(sysexSize, DEC);
      Serial8.print(" SysEx Message...\n");
      printBytes(pSysexData, sysexSize);
      Serial8.println();
      processSysex(pSysexData, sysexSize);
      break;

    case usbMIDI.TimeCodeQuarterFrame: // 0xF1
      Serial8.print("TimeCode, index=");
      Serial8.print(data1 >> 4, DEC);
      Serial8.print(", digit=");
      Serial8.println(data1 & 15, DEC);
      break;

    case usbMIDI.SongPosition: // 0xF2
      Serial8.print("Song Position, beat=");
      Serial8.println(data1 + data2 * 128);
      break;

    case usbMIDI.SongSelect: // 0xF3
      Serial8.print("Sond Select, song=");
      Serial8.println(data1, DEC);
      break;

    case usbMIDI.TuneRequest: // 0xF6
      Serial8.println("Tune Request");
      break;

    case usbMIDI.Clock: // 0xF8
      Serial8.println("Clock");
      break;

    case usbMIDI.Start: // 0xFA
      Serial8.println("Start");
      break;

    case usbMIDI.Continue: // 0xFB
      Serial8.println("Continue");
      break;

    case usbMIDI.Stop: // 0xFC
      Serial8.println("Stop");
      break;

    case usbMIDI.ActiveSensing: // 0xFE
      Serial8.println("Actvice Sensing");
      break;

    case usbMIDI.SystemReset: // 0xFF
      Serial8.println("System Reset");
      break;

    default:
      Serial8.println("Opps, an unknown MIDI message type!");
      break;
  }
}


void printBytes(const byte *data, unsigned int size) 
{
  while (size > 0) {
    byte b = *data++;
    if (b < 16) Serial8.print('0');
    Serial8.print(b, HEX);
    if (size > 1) Serial8.print(' ');
    size = size - 1;
  }
}


void processSysex(const uint8_t *pSysexData, unsigned int size)
{
    uint8_t i = 0;
    uint8_t patchNumber = 0x7F;  // valid numbers are from 0-0x63 (00-99dec)
    bool savePresetToSDFlag = false;
    bool updateCurrentPatchFlag = false;
    //bool fetchPresetFlag = false;
    if (pSysexData[0]==0xF0 && size == EWI_SYSEX_PRESET_DUMP_LEN) // EWI4k patch dump commands are always length of 206
    {
        switch (pSysexData[4]){ 
            case MIDI_PRESET_DUMP:  // pushed "PUT SELECTED" or "PUT" button
                // set flag to save preset to location pSysexData[5]
                //updateCurrentPatchFlag = true;
                patchNumber = pSysexData[5];
                if(patchNumber < NUMBER_OF_PATCHES)
                    savePresetToSDFlag = true;
                break;
            case MIDI_EDIT_DUMP:
                // set flag to update patch variables from pSysexData
                updateCurrentPatchFlag = true;
                break;
            default:
                Serial8.print("Sysex Command Type: ");
                Serial8.print(pSysexData[4], HEX);
                Serial8.println(" not supported.");
                return;
        }
        if(updateCurrentPatchFlag)
        { 
            for(i = 0; i<31; i++)
                current_patch.patch_string[i] = pSysexData[i+12];
            for( i = 0; i<NRPN_MSB_OSC1_LENGTH; i++)
               current_patch.nrpn_msb_osc1[i] = pSysexData[NRPN_MSB_OSC1_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_OSC2_LENGTH; i++)
               current_patch.nrpn_msb_osc2[i] = pSysexData[NRPN_MSB_OSC2_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_OSC_FILT1_LENGTH; i++)
               current_patch.nrpn_msb_osc_filt1[i] = pSysexData[NRPN_MSB_OSC_FILT1_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_OSC_FILT2_LENGTH; i++)
               current_patch.nrpn_msb_osc_filt2[i] = pSysexData[NRPN_MSB_OSC_FILT2_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_NOISE_FILT3_LENGTH; i++)
               current_patch.nrpn_msb_noise_filt3[i] = pSysexData[NRPN_MSB_NOISE_FILT3_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_NOISE_FILT4_LENGTH; i++)
               current_patch.nrpn_msb_noise_filt4[i] = pSysexData[NRPN_MSB_NOISE_FILT4_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_SPARE_LENGTH; i++)
               current_patch.nrpn_msb_spare[i] = pSysexData[NRPN_MSB_SPARE_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_NOISE_LENGTH; i++)
               current_patch.nrpn_msb_noise[i] = pSysexData[NRPN_MSB_NOISE_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_COMMON1_LENGTH; i++)
               current_patch.nrpn_msb_common1[i] = pSysexData[NRPN_MSB_COMMON1_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_COMMON2_LENGTH; i++)
               current_patch.nrpn_msb_common2[i] = pSysexData[NRPN_MSB_COMMON2_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_CHORUS_LENGTH; i++)
               current_patch.nrpn_msb_chorus[i] = pSysexData[NRPN_MSB_CHORUS_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_DELAY_LENGTH; i++)
               current_patch.nrpn_msb_delay[i] = pSysexData[NRPN_MSB_DELAY_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_REVERB_LENGTH; i++)
               current_patch.nrpn_msb_reverb[i] = pSysexData[NRPN_MSB_REVERB_SYSPOS+3+i];
            patchToSynthVariables(&current_patch);
            //PRINT_VALUES_FLAG = true;
        }
        if(savePresetToSDFlag)
        {
            //loadedPatches[patchNumber];
            for(i = 0; i<PATCH_STRING_LENGTH; i++)
                loadedPatches[patchNumber].patch_string[i] = pSysexData[i+12];
            for( i = 0; i<NRPN_MSB_OSC1_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_osc1[i] = pSysexData[NRPN_MSB_OSC1_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_OSC2_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_osc2[i] = pSysexData[NRPN_MSB_OSC2_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_OSC_FILT1_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_osc_filt1[i] = pSysexData[NRPN_MSB_OSC_FILT1_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_OSC_FILT2_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_osc_filt2[i] = pSysexData[NRPN_MSB_OSC_FILT2_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_NOISE_FILT3_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_noise_filt3[i] = pSysexData[NRPN_MSB_NOISE_FILT3_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_NOISE_FILT4_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_noise_filt4[i] = pSysexData[NRPN_MSB_NOISE_FILT4_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_SPARE_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_spare[i] = pSysexData[NRPN_MSB_SPARE_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_NOISE_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_noise[i] = pSysexData[NRPN_MSB_NOISE_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_COMMON1_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_common1[i] = pSysexData[NRPN_MSB_COMMON1_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_COMMON2_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_common2[i] = pSysexData[NRPN_MSB_COMMON2_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_CHORUS_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_chorus[i] = pSysexData[NRPN_MSB_CHORUS_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_DELAY_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_delay[i] = pSysexData[NRPN_MSB_DELAY_SYSPOS+3+i];
            for( i = 0; i<NRPN_MSB_REVERB_LENGTH; i++)
               loadedPatches[patchNumber].nrpn_msb_reverb[i] = pSysexData[NRPN_MSB_REVERB_SYSPOS+3+i];
            patchLoaded[patchNumber] = true;
            savePatchSD(patchNumber);
            savePresetToSDFlag = false;
        }
    }
    else if (pSysexData[0]==0xF0 && size == MIDI_PRESET_DUMP_REQ_LEN)
    {
        switch (pSysexData[4]){ 
            case MIDI_PRESET_DUMP_REQ:  // pushed "GET SELECTED" or "GET" button
                // set flag to save preset to location pSysexData[5]
                //fetchPresetFlag = true;
                patchNumber = pSysexData[5];
                sysexPresetSendBuffer[5] = patchNumber;
                for(i=0;i<PATCH_STRING_LENGTH;i++)
                    sysexPresetSendBuffer[PATCH_STRING_SYSPOS+i] =  loadedPatches[patchNumber].patch_string[i];
                //sysexPresetSendBuffer[NRPN_MSB_OSC1_SYSPOS] = NRPN_MSB_OSC1;   // these aren't necessary because the intialization of sysexPresetSendBuffer
                //sysexPresetSendBuffer[NRPN_MSB_OSC1_SYSPOS+1] = NRPN_MSB_OSC1_LENGTH;
                //sysexPresetSendBuffer[NRPN_MSB_OSC1_SYSPOS+2] = 0;
                for(i=0;i<NRPN_MSB_OSC1_LENGTH;i++)
                   sysexPresetSendBuffer[NRPN_MSB_OSC1_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_osc1[i];
                for( i = 0; i<NRPN_MSB_OSC2_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_OSC2_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_osc2[i];
                for( i = 0; i<NRPN_MSB_OSC_FILT1_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_OSC_FILT1_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_osc_filt1[i];
                for( i = 0; i<NRPN_MSB_OSC_FILT2_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_OSC_FILT2_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_osc_filt2[i];
                for( i = 0; i<NRPN_MSB_NOISE_FILT3_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_NOISE_FILT3_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_noise_filt3[i];
                for( i = 0; i<NRPN_MSB_NOISE_FILT4_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_NOISE_FILT4_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_noise_filt4[i];
                for( i = 0; i<NRPN_MSB_SPARE_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_SPARE_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_spare[i];
                for( i = 0; i<NRPN_MSB_NOISE_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_NOISE_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_noise[i];
                for( i = 0; i<NRPN_MSB_COMMON1_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_COMMON1_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_common1[i];
                for( i = 0; i<NRPN_MSB_COMMON2_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_COMMON2_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_common2[i];
                for( i = 0; i<NRPN_MSB_CHORUS_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_CHORUS_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_chorus[i];
                for( i = 0; i<NRPN_MSB_DELAY_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_DELAY_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_delay[i];
                for( i = 0; i<NRPN_MSB_REVERB_LENGTH; i++)
                   sysexPresetSendBuffer[NRPN_MSB_REVERB_SYSPOS+3+i] = loadedPatches[patchNumber].nrpn_msb_reverb[i];
                printBytes(sysexPresetSendBuffer, EWI_SYSEX_PRESET_DUMP_LEN); 
                Serial8.print("\n");
                usbMIDI.sendSysEx(EWI_SYSEX_PRESET_DUMP_LEN, sysexPresetSendBuffer, true); // true because buffer includes f0 and f7 
               break;
            default:
                Serial8.print("Sysex Command Type: ");
                Serial8.print(pSysexData[4], HEX);
                Serial8.println(" not supported.");
                return;
        }
    }
    else
    {
        sprintf(str_buf1, "ERROR: Sysex length is %d, but should be 206, or 7" , size);      
        Serial8.println(str_buf1);
        return;
    }
}
