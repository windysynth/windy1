/************************************************************
  Windy1 USB Host Synth for Wind controllers
        MIT license
        Copyright (c) 2021 Tim King

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
****************************************************************/

//
// Windy1 is a teensy 4.1 project and is
// a MIDI host synthesizer designed
// to work with usb class complaint Wind controllers (e.g., EWI5000, EWIUSB,NuEVI, NuRAD, WARBL2, etc.)
// The patches are editable with the Vyzex EWI4000S editor software
// and uses the Teensy Audio Library
//
// There are a couple of tweeks to the Teensy Audio Library
// First, to use WAVEFORM_BANDLIMIT_SAWTOOTH with the AudioSynthWaveformModulated
// you need at least Teensyduino 1.60 Beta #1
// on Arduino IDE 2.3.2 or higher
//    (remember to: Tools > Board: Teensy 4.1 and Tools > USB Type: Serial + MIDI)
// Next, I had to modify C:\Arduino\hardware\teensy\avr\libraries\Audio\filter_variable.cpp
// I've put my hacked version of the file in the repo here and renamed it "filter_variable_dot_cpp.txt"
// and "filter_variable_dot_h.txt"
// so just rename these to "filter_variable.cpp" and "filter_variable.h"
//  and put them wherever your ...\Audio folder is, replacing the originals
// These changes are in https://github.com/PaulStoffregen/Audio/pulls pull request #484
//          Teensyduino Audio git repo.
// This modified version does the following to AudioFilterStateVariable::update_variable(...) function
// 1. uncommented line 42 to #define IMPROVE_HIGH_FREQUENCY_ACCURACY
// 2. Reduced the fmult limit from 5378279 to 2965372 which is equivalent to f = 0.707
// 3. Changed the oversampling of filter from 2x to 4x
// 4. also you can change line 55 of filter_variable.h to
//          if (q < 0.5f) q = 0.5f; // min q 0.5 instead of 0.7
// 5. Also the octaveControl limit is now able to go to 7.9998f (instead of just 6.9999f)
//  This allows the filter to be stable with a resonance ('q') down to 0.5, while
//  still allowing the Cuttoff frequency (Fc) to go up to at least 20kHz.
//  Without this change, the filter can produce a really harsh white noise when
//  the fmult gets too high because of the combnation of:
//  resonance(), to low e.g 0.707
//  frequency(),  e.g above 113
//  octaveControl(),  e.g 7.0
//  Frequency Control input ("signal"), e.g close to 1.0
//  basically if F = Fc*2^(signal*octaves) > 14,744Hz you will get a full volume, random noise
//  out of the filter if you don't make this hack.
//  https://forum.pjrc.com/threads/67358-AudioFilterStateVariable-unwanted-noise

// Also added AudioSynthWaveformModulated::pulsewidth_offset(int16_t pulsewidth_offset) to
// allows the Shape signal to cause the duty cycle to wrap around--to mimic 4k behavior

//------------some define statements --------------------

// #define FREQQ (440)
#define PSRAM_INSTALLED

#define PATCH_NUMBER_EEPROM_ADDR ((int)0)
#define VOL_EEPROM_ADDR ((int)4)
#define MIX_LINEIN_EEPROM_ADDR ((int)8)
#define FINE_TUNE_CENTS_EEPROM_ADDR ((int)12)
#define TRANSPOSE_EEPROM_ADDR ((int)16)
#define OCTAVE_EEPROM_ADDR ((int)20)
#define BREATH_CC_EEPROM_ADDR ((int)24)
#define FXSOURCE_EEPROM_ADDR ((int)28)
#define COMP_PRAMS_ADDR ((int)32)
#define NNBMODCALPROM_ADDR ((int)56)

// #include <Arduino.h>
#include <U8g2lib.h>
#include <Encoder.h>
#include <Bounce2.h>
#include "globals.h"
#include "MenuSystem.h"
#include "menuSetup.h"
#include <USBHost_t36.h>
#include <MIDI.h>
#include <usb_midi.h> // This might be implicitly included by the core but can be added for clarity
#include <EEPROM.h>
#include "process_midi.h"

#include <OpenAudio_ArduinoLibrary.h> //for AudioConvert_I16toF32, AudioConvert_F32toI16, and AudioEffectGain_F32
#include <hexefx_audiolib_F32.h>      // for AudioEffectPlateReverb_F32 and AudioEffectDelayStereo_F32
#include "effect_compressor.h"

#include "patches.h"

const int ledPin = 13;

//-------- paste below the Auto generated code from Audio System Design Tool  --------
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformDc     dc_sweepDepthFilter3; //xy=99.5,1366.333267211914
AudioSynthWaveformDc     dc_breathLfoFilter3; //xy=105.5,1499.333267211914
AudioSynthWaveformSine   sine_lfoFilter3; //xy=132.5,1464.333267211914
AudioSynthWaveformDc     dc_sweepDepthOsc1; //xy=193.5,203.33326721191406
AudioSynthWaveformDc     dc_sweepDepthOsc2; //xy=218.5,523.3332672119141
AudioSynthWaveformDc     dc_sweepDepthFilterSign3; //xy=248.5,1402.333267211914
AudioSynthNoiseWhite     pink_Noise;     //xy=249.5,1165.333267211914
AudioEffectMultiply      sq_swpflt3;     //xy=297.5,1363.333267211914
AudioSynthWaveformDc     dc_modOffsetNoiseFilter3; //xy=312.5,1534.333267211914
AudioEffectMultiply      mult_lfoDepthFilter3; //xy=317.5,1495.333267211914
AudioSynthWaveformDc     dc_breathNoiseFilter3; //xy=319.5,1459.333267211914
AudioSynthWaveformDc     dc_portatimef;  //xy=336.5,416.33326721191406
AudioSynthWaveformDc     dc_timeNoise;   //xy=336.5,1309.333267211914
AudioSynthWaveformDc     dc_portatime;   //xy=341.5,383.33326721191406
AudioSynthWaveformDc     dc_pitchbend;   //xy=345.5,350.33326721191406
AudioSynthWaveformDc     dc_breathLfoFilter1; //xy=374.5,959.3332672119141
AudioSynthWaveformSine   sine_lfoFilter1; //xy=385.5,933.3332672119141
AudioEffectMultiply      sq_swpOsc1;     //xy=402.5,182.33326721191406
AudioEffectMultiply      sq_swpOsc2;     //xy=415.5,524.3332672119141
AudioFilterStateVariable filterPreNoise; //xy=433.5,1194.333267211914
AudioSynthWaveformDc     dc_sweepDepthOscSign1; //xy=441.5,299.33326721191406
AudioSynthWaveformDc     dc_sweepDepthOscSign2; //xy=452.5,618.3332672119141
AudioSynthWaveformDc     dc_sweepDepthFilter1; //xy=453.5,833.3332672119141
AudioAnalyzeRMS          rms_pink_Noise; //xy=476.5,1128.333267211914
AudioEffectMultiply      mlt_DepthFilterSign3; //xy=495.5,1368.333267211914
AudioSynthWaveformDc     dc_breathNoise; //xy=499.5,1272.333267211914
AudioEffectMultiply      sq_timeNoise;   //xy=499.5,1307.333267211914
AudioEffectMultiply      cube_swpOsc1;   //xy=505.5,249.33326721191406
AudioSynthWaveformSine   sine_lfoOsc1;   //xy=510.5,487.33326721191406
AudioMixer4              mix_dcStub;     //xy=511.5,393.33326721191406
AudioSynthWaveformDc     dc_pwOsc1;      //xy=512.5,450.33326721191406
AudioEffectMultiply      cube_swpOsc2;   //xy=531.5,571.3332672119141
AudioMixer4              mix_fcModFilter3; //xy=532.5,1484.333267211914
AudioSynthWaveformSine   sine_lfoOsc2;   //xy=567.5,737.3332672119141
AudioSynthWaveformDc     dc_modOffsetOscFilter1; //xy=570.5,988.3332672119141
AudioEffectMultiply      mult_lfoDepthFilter1; //xy=578.5,955.3332672119141
AudioSynthWaveformDc     dc_sweepDepthFilterSign1; //xy=579.5,867.3332672119141
AudioSynthWaveformDc     dc_breathOscFilter1; //xy=581.5,923.3332672119141
AudioSynthWaveformDc     dc_pwOsc2;      //xy=582.5,699.3332672119141
AudioEffectMultiply      sq_swpflt1;     //xy=640.5,831.3332672119141
AudioFilterOnepole       onepole_PreNoise;       //xy=643.8333282470703,1209.1666717529297
AudioMixer4              mix_pwOsc1;     //xy=676.5,450.33326721191406
AudioSynthWaveformDc     dc_breathLfoFilter4; //xy=677.5,1631.333267211914
AudioSynthWaveformDc     dc_breathSweepOsc1; //xy=685.5,357.33326721191406
AudioSynthWaveformSine   sine_lfoFilter4; //xy=684.5,1594.333267211914
AudioEffectMultiply      mlt_DepthOscSign1; //xy=693.5,299.33326721191406
AudioMixer4              mix_breathTimeNoise; //xy=689.5,1302.333267211914
AudioSynthWaveformDc     dc_beatOsc1;    //xy=713.5,389.33326721191406
AudioSynthWaveformDc     dc_breathSweepOsc2; //xy=728.5,638.3332672119141
AudioEffectMultiply      mlt_DepthOscSign2; //xy=729.5,595.3332672119141
AudioMixer4              mix_pwOsc2;     //xy=739.5,723.3332672119141
AudioSynthWaveformDc     dc_beatOsc2;    //xy=758.5,670.3332672119141
AudioSynthWaveformDc     dc_sweepDepthFilter4; //xy=762.5,1497.333267211914
AudioMixer4              mix_fcModFilter3_sweep; //xy=790.5,1431.333267211914
AudioSynthWaveformDc     dc_sweepDepthFilter2; //xy=805.5,1046.333267211914
AudioMixer4              mix_fcModFilter1; //xy=821.5,966.3332672119141
AudioEffectMultiply      mlt_DepthFilterSign1; //xy=831.5,867.3332672119141
AudioSynthWaveformDc     dc_breathLfoFilter2; //xy=840.5,1176.333267211914
AudioSynthWaveformSine   sine_lfoFilter2; //xy=843.5,1141.333267211914
AudioAnalyzeRMS          rms_filterPreNoise; //xy=878.5,1236.333267211914
AudioEffectMultiply      multiply2;      //xy=881.5,1299.333267211914
AudioEffectMultiply      mult_lfoDepthFilter4; //xy=880.5,1612.333267211914
AudioSynthWaveformDc     dc_breathNoiseFilter4; //xy=885.5,1578.333267211914
AudioSynthWaveformDc     dc_modOffsetNoiseFilter4; //xy=888.5,1679.333267211914
AudioSynthWaveformDc     dc_sweepDepthFilterSign4; //xy=896.5,1535.333267211914
AudioMixer4              mix_pitchModOsc1; //xy=907.5,370.33326721191406
AudioAnalyzeRMS          rms_mix_breathTimeNoise; //xy=906.5,1368.333267211914
AudioEffectMultiply      sq_swpflt4;     //xy=945.5,1499.333267211914
AudioMixer4              mix_pitchModOsc2; //xy=949.5,638.3332672119141
AudioSynthWaveformDc     dc_sweepDepthFilterSign2; //xy=972.5,1083.333267211914
AudioEffectMultiply      sq_swpflt2;     //xy=1021.5,1050.333267211914
AudioEffectMultiply      mult_lfoDepthFilter2; //xy=1035.5,1154.333267211914
AudioSynthWaveformDc     dc_modOffsetOscFilter2; //xy=1035.5,1202.333267211914
AudioSynthWaveformDc     dc_breathOscFilter2; //xy=1039.5,1119.333267211914
AudioMixer4              mix_levelNoise; //xy=1039.5,1318.333267211914
AudioAnalyzeRMS          rms_multiply2;  //xy=1042.5,1264.333267211914
AudioMixer4              mix_fcModFilter1_sweep; //xy=1069.5,962.3332672119141
AudioConvert_I16toF32    Int2FloatR; //xy=1077.3333740234375,1975.833251953125
AudioConvert_I16toF32    Int2FloatL; //xy=1078.3333740234375,1911.833251953125
AudioMixer4              mix_fcModFilter4; //xy=1126.5,1617.333251953125
AudioSynthWaveformModulated wfmod_triOsc1;  //xy=1137.5,459.33326721191406
AudioSynthWaveformModulated wfmod_sawOsc1;  //xy=1143.5,427.33326721191406
AudioSynthWaveformModulated wfmod_pulseOsc1; //xy=1143.5,490.33326721191406
AudioEffectMultiply      mlt_DepthFilterSign4; //xy=1139.5,1505.333267211914
AudioSynthWaveformModulated wfmod_triOsc2;  //xy=1152.5,643.3332672119141
AudioSynthWaveformModulated wfmod_sawOsc2;  //xy=1157.5,612.3332672119141
AudioSynthWaveformModulated wfmod_pulseOsc2; //xy=1165.5,675.3332672119141
AudioFilterStateVariable filter3;        //xy=1192.5,1403.333267211914
AudioEffectMultiply      mlt_DepthFilterSign2; //xy=1234.5,1065.333267211914
AudioEffectDelayStereo_F32 delay_F32 = AudioEffectDelayStereo_F32((uint32_t)1270, true);    //xy=1235.3333129882812,1948.833251953125
//AudioEffectDelayStereo_F32 delay_F32;
AudioMixer4              mix_fcModFilter2; //xy=1247.5,1178.333267211914
AudioSynthWaveformDc     dc_breathThreshOsc1; //xy=1275.4999389648438,354.333251953125
AudioSynthWaveformDc     dc_breathThreshOsc2; //xy=1282.5,570.3332672119141
AudioAnalyzeRMS          rms_mix_levelNoise; //xy=1301.5,1291.333267211914
AudioMixer4              mix_osc1;       //xy=1315.5,455.33326721191406
AudioMixer4              mix_osc2;       //xy=1356.5,642.3332672119141
AudioMixer4              mix_ntcFilter3; //xy=1357.5,1402.333267211914
AudioEffectPlateReverb_F32 verb_F32;     //xy=1385.3333129882812,1948.833251953125
AudioMixer4              mix_fcModFilter4_sweep; //xy=1430.5,1517.333267211914
AudioMixer4              mix_xfade;      //xy=1462.4999389648438,399.333251953125
AudioMixer4              mix_fcModFilter2_sweep; //xy=1494.5,1190.333267211914
AudioEffectGain_F32      amp_extraGainR_F32; //xy=1548.3331909179688,1991.166748046875
AudioEffectGain_F32      amp_extraGainL_F32;          //xy=1550.3331909179688,1899.1668090820312
AudioMixer4              mix_oscPreComp; //xy=1563.4999389648438,532.333251953125
AudioEffectMultiply      mult_thCurveOsc2; //xy=1590.5,614.3332672119141
AudioAnalyzeRMS          rms_mix_ntcFilter3; //xy=1588.5,1346.333267211914
AudioEffectMultiply      mult_thCurveOsc1; //xy=1613.4999389648438,455.3332214355469
AudioEffectFlange        flange1;        //xy=1643.5,1635.333251953125
AudioEffectFlange        flange2;        //xy=1643.5,1674.333251953125
AudioEffectFlange        flange3;        //xy=1643.5,1712.333251953125
AudioMixer4_F32          mix_pongR_F32; //xy=1764.3331909179688,1963.166748046875
AudioMixer4_F32          mix_pongL_F32;       //xy=1766.3331909179688,1898.1668090820312
AudioFilterStateVariable filter4;        //xy=1796.5,1434.333267211914
AudioEffectCompressor    compress_oscLevels1;    //xy=1811.3331909179688,502.8333740234375
AudioEffectCompressor    compress_oscLevels2; //xy=1812.333251953125,561.8333129882812
AudioMixer4              mix_chorus_fb;  //xy=1833.5001220703125,1598.333251953125
AudioMixer4              mix_chorus_wet; //xy=1834.4999389648438,1689.333251953125
AudioFilterStateVariable filter2;        //xy=1902.5,1230.333267211914
AudioConvert_F32toI16    Float2IntL;   //xy=1917.999984741211,1898.1666717529297
AudioConvert_F32toI16    Float2IntR;   //xy=1922.999984741211,1963.1666717529297
AudioFilterStateVariable filter_osc2;    //xy=1942.5,657.333251953125
AudioMixer4              mix_ntcFilter4; //xy=1958.5,1422.333267211914
AudioFilterStateVariable filter1;        //xy=1973.5,999.3332672119141
AudioFilterStateVariable filter_osc1;    //xy=1982.5,435.3332214355469
AudioMixer4              mix_ntcFilter2; //xy=2056.5,1220.333267211914
AudioAnalyzeRMS          rms_mix_ntcFilter4; //xy=2083.5,1331.333267211914
AudioFilterStateVariable filter_osc2b;   //xy=2096.5,635.333251953125
AudioMixer4              mix_chorus_dryL; //xy=2098.5000610351562,1659.333267211914
AudioMixer4              mix_chorus_dryR; //xy=2099.3333740234375,1749.833251953125
AudioFilterFIR           fir_formant;    //xy=2108.5,1284.333267211914
AudioEffectEnvelope      env_squelchL;   //xy=2109.5,1858.333267211914
AudioEffectEnvelope      env_squelchR;   //xy=2121.5,1959.333267211914
AudioFilterStateVariable filter_osc1b;   //xy=2128.5,514.333251953125
AudioMixer4              mix_ntcFilter1; //xy=2129.5,993.3332672119141
AudioMixer4              mix_Amp;        //xy=2188.5,1430.333267211914
AudioMixer4              mix_oscLevels;  //xy=2262.5,592.333251953125
AudioFilterStateVariable filter5;        //xy=2271.5,1299.333267211914
AudioFilterStateVariable filterPreMixHPR; //xy=2281.5,2026.333267211914
AudioFilterStateVariable filterPreMixHPL; //xy=2284.5,1890.333267211914
AudioInputI2S            i2s2;           //xy=2287.5,1954.333267211914
AudioEffectEnvelope      env_squelch;    //xy=2349.5,1438.333267211914
AudioMixer4              mix_limiter;    //xy=2428.5001220703125,690.333251953125
AudioEffectMultiply      mlt_sqLimter;   //xy=2479.500244140625,591.333251953125
AudioMixer4              mix_lineInL;    //xy=2477.5,1931.333267211914
AudioMixer4              mix_lineInR;    //xy=2482.5,2028.333267211914
AudioEffectMultiply      mlt_cubicLimter; //xy=2647.500244140625,590.333251953125
AudioOutputI2S           i2s1;           //xy=2646.5,1972.333267211914

AudioConnection          patchCord1(dc_sweepDepthFilter3, 0, sq_swpflt3, 0);
AudioConnection          patchCord2(dc_sweepDepthFilter3, 0, sq_swpflt3, 1);
AudioConnection          patchCord3(dc_breathLfoFilter3, 0, mult_lfoDepthFilter3, 1);
AudioConnection          patchCord4(sine_lfoFilter3, 0, mult_lfoDepthFilter3, 0);
AudioConnection          patchCord5(dc_sweepDepthOsc1, 0, sq_swpOsc1, 0);
AudioConnection          patchCord6(dc_sweepDepthOsc1, 0, sq_swpOsc1, 1);
AudioConnection          patchCord7(dc_sweepDepthOsc1, 0, cube_swpOsc1, 1);
AudioConnection          patchCord8(dc_sweepDepthOsc2, 0, sq_swpOsc2, 0);
AudioConnection          patchCord9(dc_sweepDepthOsc2, 0, sq_swpOsc2, 1);
AudioConnection          patchCord10(dc_sweepDepthOsc2, 0, cube_swpOsc2, 1);
AudioConnection          patchCord11(dc_sweepDepthFilterSign3, 0, mlt_DepthFilterSign3, 1);
AudioConnection          patchCord12(pink_Noise, rms_pink_Noise);
AudioConnection          patchCord13(pink_Noise, 0, filterPreNoise, 0);
AudioConnection          patchCord14(sq_swpflt3, 0, mlt_DepthFilterSign3, 0);
AudioConnection          patchCord15(dc_modOffsetNoiseFilter3, 0, mix_fcModFilter3, 3);
AudioConnection          patchCord16(mult_lfoDepthFilter3, 0, mix_fcModFilter3, 1);
AudioConnection          patchCord17(dc_breathNoiseFilter3, 0, mix_fcModFilter3, 0);
AudioConnection          patchCord18(dc_portatimef, 0, mix_dcStub, 3);
AudioConnection          patchCord19(dc_timeNoise, 0, sq_timeNoise, 0);
AudioConnection          patchCord20(dc_timeNoise, 0, sq_timeNoise, 1);
AudioConnection          patchCord21(dc_portatime, 0, mix_dcStub, 2);
AudioConnection          patchCord22(dc_pitchbend, 0, mix_dcStub, 1);
AudioConnection          patchCord23(dc_breathLfoFilter1, 0, mult_lfoDepthFilter1, 1);
AudioConnection          patchCord24(sine_lfoFilter1, 0, mult_lfoDepthFilter1, 0);
AudioConnection          patchCord25(sq_swpOsc1, 0, cube_swpOsc1, 0);
AudioConnection          patchCord26(sq_swpOsc2, 0, cube_swpOsc2, 0);
AudioConnection          patchCord27(filterPreNoise, 2, onepole_PreNoise, 0);
AudioConnection          patchCord28(dc_sweepDepthOscSign1, 0, mlt_DepthOscSign1, 1);
AudioConnection          patchCord29(dc_sweepDepthOscSign2, 0, mlt_DepthOscSign2, 1);
AudioConnection          patchCord30(dc_sweepDepthFilter1, 0, sq_swpflt1, 0);
AudioConnection          patchCord31(dc_sweepDepthFilter1, 0, sq_swpflt1, 1);
AudioConnection          patchCord32(mlt_DepthFilterSign3, 0, mix_fcModFilter3_sweep, 0);
AudioConnection          patchCord33(dc_breathNoise, 0, mix_breathTimeNoise, 0);
AudioConnection          patchCord34(sq_timeNoise, 0, mix_breathTimeNoise, 1);
AudioConnection          patchCord35(cube_swpOsc1, 0, mlt_DepthOscSign1, 0);
AudioConnection          patchCord36(sine_lfoOsc1, 0, mix_pwOsc1, 1);
AudioConnection          patchCord37(dc_pwOsc1, 0, mix_pwOsc1, 0);
AudioConnection          patchCord38(cube_swpOsc2, 0, mlt_DepthOscSign2, 0);
AudioConnection          patchCord39(mix_fcModFilter3, 0, mix_fcModFilter3_sweep, 1);
AudioConnection          patchCord40(sine_lfoOsc2, 0, mix_pwOsc2, 1);
AudioConnection          patchCord41(dc_modOffsetOscFilter1, 0, mix_fcModFilter1, 3);
AudioConnection          patchCord42(mult_lfoDepthFilter1, 0, mix_fcModFilter1, 1);
AudioConnection          patchCord43(dc_sweepDepthFilterSign1, 0, mlt_DepthFilterSign1, 1);
AudioConnection          patchCord44(dc_breathOscFilter1, 0, mix_fcModFilter1, 0);
AudioConnection          patchCord45(dc_pwOsc2, 0, mix_pwOsc2, 0);
AudioConnection          patchCord46(sq_swpflt1, 0, mlt_DepthFilterSign1, 0);
AudioConnection          patchCord47(onepole_PreNoise, rms_filterPreNoise);
AudioConnection          patchCord48(mix_pwOsc1, 0, wfmod_pulseOsc1, 1);
AudioConnection          patchCord49(dc_breathLfoFilter4, 0, mult_lfoDepthFilter4, 1);
AudioConnection          patchCord50(dc_breathSweepOsc1, 0, mix_pitchModOsc1, 1);
AudioConnection          patchCord51(sine_lfoFilter4, 0, mult_lfoDepthFilter4, 0);
AudioConnection          patchCord52(mlt_DepthOscSign1, 0, mix_pitchModOsc1, 0);
AudioConnection          patchCord53(mix_breathTimeNoise, 0, multiply2, 1);
AudioConnection          patchCord54(mix_breathTimeNoise, rms_mix_breathTimeNoise);
AudioConnection          patchCord55(dc_beatOsc1, 0, mix_pitchModOsc1, 2);
AudioConnection          patchCord56(dc_breathSweepOsc2, 0, mix_pitchModOsc2, 1);
AudioConnection          patchCord57(mlt_DepthOscSign2, 0, mix_pitchModOsc2, 0);
AudioConnection          patchCord58(mix_pwOsc2, 0, wfmod_pulseOsc2, 1);
AudioConnection          patchCord59(dc_beatOsc2, 0, mix_pitchModOsc2, 2);
AudioConnection          patchCord60(dc_sweepDepthFilter4, 0, sq_swpflt4, 0);
AudioConnection          patchCord61(dc_sweepDepthFilter4, 0, sq_swpflt4, 1);
AudioConnection          patchCord62(mix_fcModFilter3_sweep, 0, filter3, 1);
AudioConnection          patchCord63(dc_sweepDepthFilter2, 0, sq_swpflt2, 0);
AudioConnection          patchCord64(dc_sweepDepthFilter2, 0, sq_swpflt2, 1);
AudioConnection          patchCord65(mix_fcModFilter1, 0, mix_fcModFilter1_sweep, 1);
AudioConnection          patchCord66(mlt_DepthFilterSign1, 0, mix_fcModFilter1_sweep, 0);
AudioConnection          patchCord67(dc_breathLfoFilter2, 0, mult_lfoDepthFilter2, 1);
AudioConnection          patchCord68(sine_lfoFilter2, 0, mult_lfoDepthFilter2, 0);
AudioConnection          patchCord69(multiply2, 0, mix_levelNoise, 0);
AudioConnection          patchCord70(multiply2, rms_multiply2);
AudioConnection          patchCord71(mult_lfoDepthFilter4, 0, mix_fcModFilter4, 1);
AudioConnection          patchCord72(dc_breathNoiseFilter4, 0, mix_fcModFilter4, 0);
AudioConnection          patchCord73(dc_modOffsetNoiseFilter4, 0, mix_fcModFilter4, 3);
AudioConnection          patchCord74(dc_sweepDepthFilterSign4, 0, mlt_DepthFilterSign4, 1);
AudioConnection          patchCord75(mix_pitchModOsc1, 0, wfmod_sawOsc1, 0);
AudioConnection          patchCord76(mix_pitchModOsc1, 0, wfmod_triOsc1, 0);
AudioConnection          patchCord77(mix_pitchModOsc1, 0, wfmod_pulseOsc1, 0);
AudioConnection          patchCord78(sq_swpflt4, 0, mlt_DepthFilterSign4, 0);
AudioConnection          patchCord79(mix_pitchModOsc2, 0, wfmod_sawOsc2, 0);
AudioConnection          patchCord80(mix_pitchModOsc2, 0, wfmod_triOsc2, 0);
AudioConnection          patchCord81(mix_pitchModOsc2, 0, wfmod_pulseOsc2, 0);
AudioConnection          patchCord82(dc_sweepDepthFilterSign2, 0, mlt_DepthFilterSign2, 1);
AudioConnection          patchCord83(sq_swpflt2, 0, mlt_DepthFilterSign2, 0);
AudioConnection          patchCord84(mult_lfoDepthFilter2, 0, mix_fcModFilter2, 1);
AudioConnection          patchCord85(dc_modOffsetOscFilter2, 0, mix_fcModFilter2, 3);
AudioConnection          patchCord86(dc_breathOscFilter2, 0, mix_fcModFilter2, 0);
AudioConnection          patchCord87(mix_levelNoise, 0, filter3, 0);
AudioConnection          patchCord88(mix_levelNoise, 0, mix_ntcFilter3, 0);
AudioConnection          patchCord89(mix_levelNoise, rms_mix_levelNoise);
AudioConnection          patchCord90(mix_fcModFilter1_sweep, 0, filter1, 1);
AudioConnection_F32      patchCord91(Int2FloatR, 0, delay_F32, 1);
AudioConnection_F32      patchCord92(Int2FloatL, 0, delay_F32, 0);
AudioConnection          patchCord93(mix_fcModFilter4, 0, mix_fcModFilter4_sweep, 1);
AudioConnection          patchCord94(wfmod_triOsc1, 0, mix_osc1, 1);
AudioConnection          patchCord95(wfmod_sawOsc1, 0, mix_osc1, 0);
AudioConnection          patchCord96(wfmod_pulseOsc1, 0, mix_osc1, 2);
AudioConnection          patchCord97(mlt_DepthFilterSign4, 0, mix_fcModFilter4_sweep, 0);
AudioConnection          patchCord98(wfmod_triOsc2, 0, mix_osc2, 1);
AudioConnection          patchCord99(wfmod_sawOsc2, 0, mix_osc2, 0);
AudioConnection          patchCord100(wfmod_pulseOsc2, 0, mix_osc2, 2);
AudioConnection          patchCord101(filter3, 0, mix_ntcFilter3, 1);
AudioConnection          patchCord102(filter3, 1, mix_ntcFilter3, 2);
AudioConnection          patchCord103(filter3, 2, mix_ntcFilter3, 3);
AudioConnection          patchCord104(mlt_DepthFilterSign2, 0, mix_fcModFilter2_sweep, 0);
AudioConnection_F32      patchCord105(delay_F32, 0, verb_F32, 0);
AudioConnection_F32      patchCord106(delay_F32, 1, verb_F32, 1);
AudioConnection          patchCord107(mix_fcModFilter2, 0, mix_fcModFilter2_sweep, 1);
AudioConnection          patchCord108(dc_breathThreshOsc1, 0, mix_xfade, 0);
AudioConnection          patchCord109(dc_breathThreshOsc2, 0, mult_thCurveOsc2, 0);
AudioConnection          patchCord110(dc_breathThreshOsc2, 0, mix_xfade, 1);
AudioConnection          patchCord111(mix_osc1, 0, mult_thCurveOsc1, 1);
AudioConnection          patchCord112(mix_osc2, 0, mult_thCurveOsc2, 1);
AudioConnection          patchCord113(mix_ntcFilter3, 0, mix_ntcFilter4, 0);
AudioConnection          patchCord114(mix_ntcFilter3, 0, filter4, 0);
AudioConnection          patchCord115(mix_ntcFilter3, rms_mix_ntcFilter3);
AudioConnection_F32      patchCord116(verb_F32, 0, amp_extraGainL_F32, 0);
AudioConnection_F32      patchCord117(verb_F32, 1, amp_extraGainR_F32, 0);
AudioConnection          patchCord118(mix_fcModFilter4_sweep, 0, filter4, 1);
AudioConnection          patchCord119(mix_xfade, 0, mult_thCurveOsc1, 0);
AudioConnection          patchCord120(mix_fcModFilter2_sweep, 0, filter2, 1);
AudioConnection_F32      patchCord121(amp_extraGainR_F32, 0, mix_pongL_F32, 1);
AudioConnection_F32      patchCord122(amp_extraGainR_F32, 0, mix_pongR_F32, 1);
AudioConnection_F32      patchCord123(amp_extraGainL_F32, 0, mix_pongL_F32, 0);
AudioConnection_F32      patchCord124(amp_extraGainL_F32, 0, mix_pongR_F32, 0);
AudioConnection          patchCord125(mix_oscPreComp, 0, compress_oscLevels1, 1);
AudioConnection          patchCord126(mix_oscPreComp, 0, compress_oscLevels1, 2);
AudioConnection          patchCord127(mix_oscPreComp, 0, compress_oscLevels2, 1);
AudioConnection          patchCord128(mix_oscPreComp, 0, compress_oscLevels2, 0);
AudioConnection          patchCord129(mult_thCurveOsc2, 0, mix_oscPreComp, 1);
AudioConnection          patchCord130(mult_thCurveOsc2, 0, compress_oscLevels2, 0);
AudioConnection          patchCord131(mult_thCurveOsc1, 0, mix_oscPreComp, 0);
AudioConnection          patchCord132(mult_thCurveOsc1, 0, compress_oscLevels1, 0);
AudioConnection          patchCord133(flange1, 0, mix_chorus_wet, 0);
AudioConnection          patchCord134(flange2, 0, mix_chorus_wet, 1);
AudioConnection          patchCord135(flange3, 0, mix_chorus_wet, 2);
AudioConnection_F32      patchCord136(mix_pongR_F32, Float2IntR);
AudioConnection_F32      patchCord137(mix_pongL_F32, Float2IntL);
AudioConnection          patchCord138(filter4, 0, mix_ntcFilter4, 1);
AudioConnection          patchCord139(filter4, 1, mix_ntcFilter4, 2);
AudioConnection          patchCord140(filter4, 2, mix_ntcFilter4, 3);
AudioConnection          patchCord141(compress_oscLevels1, 0, filter_osc1, 0);
AudioConnection          patchCord142(compress_oscLevels2, 0, filter_osc2, 0);
AudioConnection          patchCord143(mix_chorus_fb, flange1);
AudioConnection          patchCord144(mix_chorus_fb, flange2);
AudioConnection          patchCord145(mix_chorus_fb, flange3);
AudioConnection          patchCord146(mix_chorus_wet, 0, mix_chorus_dryL, 1);
AudioConnection          patchCord147(mix_chorus_wet, 0, mix_chorus_fb, 1);
AudioConnection          patchCord148(mix_chorus_wet, 0, mix_chorus_dryR, 1);
AudioConnection          patchCord149(filter2, 0, mix_ntcFilter2, 1);
AudioConnection          patchCord150(filter2, 1, mix_ntcFilter2, 2);
AudioConnection          patchCord151(filter2, 2, mix_ntcFilter2, 3);
AudioConnection          patchCord152(Float2IntL, env_squelchL);
AudioConnection          patchCord153(Float2IntR, env_squelchR);
AudioConnection          patchCord154(filter_osc2, 2, filter_osc2b, 0);
AudioConnection          patchCord155(mix_ntcFilter4, 0, mix_Amp, 1);
AudioConnection          patchCord156(mix_ntcFilter4, rms_mix_ntcFilter4);
AudioConnection          patchCord157(filter1, 0, mix_ntcFilter1, 1);
AudioConnection          patchCord158(filter1, 1, mix_ntcFilter1, 2);
AudioConnection          patchCord159(filter1, 2, mix_ntcFilter1, 3);
AudioConnection          patchCord160(filter_osc1, 2, filter_osc1b, 0);
AudioConnection          patchCord161(mix_ntcFilter2, fir_formant);
AudioConnection          patchCord162(filter_osc2b, 2, mix_oscLevels, 1);
AudioConnection          patchCord163(mix_chorus_dryL, Int2FloatL);
AudioConnection          patchCord164(mix_chorus_dryR, Int2FloatR);
AudioConnection          patchCord165(fir_formant, 0, filter5, 0);
AudioConnection          patchCord166(env_squelchL, 0, filterPreMixHPL, 0);
AudioConnection          patchCord167(env_squelchR, 0, filterPreMixHPR, 0);
AudioConnection          patchCord168(filter_osc1b, 2, mix_oscLevels, 0);
AudioConnection          patchCord169(mix_ntcFilter1, 0, filter2, 0);
AudioConnection          patchCord170(mix_ntcFilter1, 0, mix_ntcFilter2, 0);
AudioConnection          patchCord171(mix_Amp, env_squelch);
AudioConnection          patchCord172(mix_oscLevels, 0, mlt_cubicLimter, 0);
AudioConnection          patchCord173(mix_oscLevels, 0, mlt_sqLimter, 0);
AudioConnection          patchCord174(mix_oscLevels, 0, mlt_sqLimter, 1);
AudioConnection          patchCord175(mix_oscLevels, 0, mix_limiter, 0);
AudioConnection          patchCord176(mix_oscLevels, 0, mix_limiter, 1);
AudioConnection          patchCord177(filter5, 2, mix_Amp, 0);
AudioConnection          patchCord178(filterPreMixHPR, 2, mix_lineInR, 0);
AudioConnection          patchCord179(filterPreMixHPL, 2, mix_lineInL, 0);
AudioConnection          patchCord180(i2s2, 0, mix_lineInL, 1);
AudioConnection          patchCord181(i2s2, 1, mix_lineInR, 1);
AudioConnection          patchCord182(env_squelch, 0, mix_chorus_dryL, 0);
AudioConnection          patchCord183(env_squelch, 0, mix_chorus_fb, 0);
AudioConnection          patchCord184(env_squelch, 0, mix_chorus_dryR, 0);
AudioConnection          patchCord185(mix_limiter, 0, mix_ntcFilter1, 0);
AudioConnection          patchCord186(mix_limiter, 0, filter1, 0);
AudioConnection          patchCord187(mlt_sqLimter, 0, mlt_cubicLimter, 1);
AudioConnection          patchCord188(mix_lineInL, 0, i2s1, 0);
AudioConnection          patchCord189(mix_lineInR, 0, i2s1, 1);
AudioConnection          patchCord190(mlt_cubicLimter, 0, mix_limiter, 2);

AudioControlSGTL5000     sgtl5000_1;     //xy=193.49998474121094,121.33329010009766
// GUItool: end automatically generated code

//-------- paste above the Auto generated code from Audio System Design Tool  --------

// I2C pins for Teensy 4.1 default: Wire pins (18=SDA,19=SCL) when using hardware I2C
// U8g2 constructor for common SSD1306 I2C 128x64
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
U8G2_SSD1306_128X64_NONAME_F_2ND_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

//-------------- Encoders ---------------------------
// Adjust these pins to your wiring
const uint8_t BOT_A = 36, BOT_B = 37, BOT_BTN = 32;
const uint8_t TOP_A = 31, TOP_B = 30, TOP_BTN = 29;
Encoder botEnc(BOT_A, BOT_B);
Encoder topEnc(TOP_A, TOP_B);
const uint32_t debounceDelay = 10; //  mS
Bounce knobBotButton(BOT_BTN, debounceDelay);
Bounce knobTopButton(TOP_BTN, debounceDelay);

//-------------- Msc Pins----------------------------
const int pwrDownSensePin = 25;              // teensy4.1 pin
Bounce pwrDownSense(pwrDownSensePin, 1);     // 1ms
const int LO_RIGHT_PLUG = 33;                // line out plug sense, high is plugged in
const int LO_LEFT_PLUG = 5;                  // line out plug sense, high is plugged in
Bounce loRightPluggedIn(LO_RIGHT_PLUG, 100); // 10ms
Bounce loLeftPluggedIn(LO_LEFT_PLUG, 100);   // 10ms

//-------------- MIDI DIN connection ----------------------
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIs1);

//-------------- USB HOST MIDI Class Compliant ------------
USBHost myUSBHost;
MIDIDevice midi_ho(myUSBHost);
//MIDIDevice_BigBuffer midi_ho(myUSBHost);

//-------------- usbMIDI for USB MIDI devices --------------
// usbMIDI is included in the Teensy Core Library, so usbMIDI.begin() not needed

//-------------- Squelch variables -------------------------
// squelched after squelchRelease and before squelchRelease + squelchDelay
// synth variables update after updateSynthDelay which should occur while squelched
// squechAttack starts at updateSynthDelay + squelchDelay
const uint32_t readKnobInterval = 33;                 // ms
const float ledMaxInterval = (float)readKnobInterval; // ms
const uint32_t updateSynthDelay = 70;                 // ms > squelchRelease
const float squelchRelease = 30.0f;                   // ms
const float squelchDelay = 60.0f;                     // ms
const float squelchAttack = 30.0f;                    // ms
const uint32_t readRMSInterval = 1000;                // ms

void setup()
{
  pinMode(ledPin, OUTPUT);
  // init str buffers
  // sprintf(str_buf, version_str.c_str());
  sprintf(str_buf1, version_str.c_str());
  sprintf(str_oledbuf, splashScreen_str.c_str());

  Serial8.begin(1000000);
  // Serial.begin(1000000); // for debugging over usb serial
  pinMode(BOT_BTN, INPUT_PULLUP);
  pinMode(TOP_BTN, INPUT_PULLUP);
  pinMode_INPUT_PULLUP_100K(pwrDownSensePin);

  uint32_t boatLoaderCatchTime = millis();
  do
  {
    delay(200);
    knobTopButton.update();
    knobBotButton.update();
  } while (!knobTopButton.read() && !knobBotButton.read()); // false is pressed because of PULLUP

  if (millis() - boatLoaderCatchTime >= 1500)
  {
    asm("bkpt #251"); // run bootloader
  }

  //--------Configure SD Card----------
  configureSD();

  // initial read of buttons to clear any bounces
  knobBotButton.update();
  knobTopButton.update();
  knobBotButton.rose();
  knobTopButton.rose();

#ifdef SGTL5000_CHECK_PWRON_DEFAULT // defined in control_sgtl5000.h
// ws added read_b4_enable() to avoid loud click due to calling enable() when already enabled
// when coming out of bootloader
#define SGTL5000_0030_PWRON_DEFAULT 0x7060
  unsigned int sgtl_val = sgtl5000_1.read_b4_enable((unsigned int)0x0030);
  if (SGTL5000_0030_PWRON_DEFAULT == sgtl_val)
  {
    sgtl5000_1.enable();
  }
  else
  {
    String test_str = verNum_str + "\n sgtl:\n 0x%04X";
    sprintf(str_oledbuf, test_str.c_str(), sgtl_val);
  }
#else
  sgtl5000_1.enable();
#endif
  sgtl5000_1.muteHeadphone();
  sgtl5000_1.muteLineout();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.dacVolumeRamp();
  sgtl5000_1.lineOutLevel(13); // 3.16V p-p
  sgtl5000_1.lineInLevel(0);   // 3.12V p-p
  sgtl5000_1.unmuteLineout();
  sgtl5000_1.unmuteHeadphone();
  sgtl5000_1.volume(0.80);

  //------------ configure AudioMemory (for 16bit int) ----------------
  AudioMemory(100); // TODO: how much AudioMemory do I need? (delay 2.9ms per block for 1270/2.9 = 438
  //------------ configure AudioMemory_F32 (for 32 bit float) ----------------1
  AudioMemory_F32(200);

  //-------------- confiugre starting synth params ---------------
  AudioNoInterrupts();

  dc_pwOsc1.amplitude(0.0); // PW now set wfmod_pulseOsc1.pulsewidth_offset()
  sine_lfoOsc1.amplitude(PwmDepthOsc1);
  sine_lfoOsc1.frequency(PwmFreqOsc1);
  dc_pwOsc2.amplitude(0.0); //  PW now set wfmod_pulseOsc2.pulsewidth_offset()
  sine_lfoOsc2.amplitude(PwmDepthOsc2);
  sine_lfoOsc2.frequency(PwmFreqOsc2);
  mix_pwOsc1.gain(0, 0.0);
  mix_pwOsc1.gain(1, maxPwmDepthOsc);
  mix_pwOsc2.gain(0, 0.0);
  mix_pwOsc2.gain(1, maxPwmDepthOsc);

  mix_pitchModOsc1.gain(0, 1.0);                  // use dc_sweepDepthOsc1 amplitude
  mix_pitchModOsc1.gain(1, limitBreathSweepOsc1); // use dc_breathSweepOsc1 amplitude
  mix_pitchModOsc1.gain(2, 1.0);                  // use dc_beatOsc1 amplitude
  mix_pitchModOsc2.gain(0, 1.0);                  // use dc_sweepDepthOsc1 amplitude
  mix_pitchModOsc2.gain(1, limitBreathSweepOsc2); // use dc_breathSweepOsc1 amplitude
  mix_pitchModOsc2.gain(2, 1.0);                  // use dc_beatOsc1 amplitude
  dc_beatOsc1.amplitude(0.0);                     // BeatOsc1 added directly to noteFreqOsc1 instead
  dc_beatOsc2.amplitude(0.0);                     // BeatOsc2 added directly to noteFreqOsc2 instead

  wfmod_sawOsc1.begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
  wfmod_sawOsc1.amplitude(SawOsc1);
  wfmod_sawOsc1.frequency(noteFreqOsc1);
  wfmod_sawOsc1.frequencyModulation(octaveControlOsc1); // max freq mod +/- 4.0 octaves
  wfmod_triOsc1.begin(WAVEFORM_TRIANGLE_VARIABLE);
  wfmod_triOsc1.amplitude(TriOsc1);
  wfmod_triOsc1.frequency(noteFreqOsc1);
  wfmod_triOsc1.frequencyModulation(octaveControlOsc1); // max freq mod +/- 4.0 octaves
  wfmod_pulseOsc1.begin(WAVEFORM_BANDLIMIT_PULSE);
  // wfmod_pulseOsc1.begin(WAVEFORM_PULSE);
  wfmod_pulseOsc1.amplitude(PulseOsc1);
  wfmod_pulseOsc1.frequency(noteFreqOsc1);
  wfmod_pulseOsc1.frequencyModulation(octaveControlOsc1); // max freq mod +/- 4.0 octaves
  wfmod_pulseOsc1.pulsewidth_offset(PwOsc1);              // ws added this to synth_waveform.cpp .h
  wfmod_sawOsc2.begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
  // wfmod_sawOsc2.begin(WAVEFORM_SAWTOOTH);
  wfmod_sawOsc2.amplitude(SawOsc2);
  wfmod_sawOsc2.frequency(noteFreqOsc2);
  wfmod_sawOsc2.frequencyModulation(octaveControlOsc2); // max freq mod +/- 4.0 octaves
  wfmod_triOsc2.begin(WAVEFORM_TRIANGLE_VARIABLE);
  wfmod_triOsc2.amplitude(TriOsc2);
  wfmod_triOsc2.frequency(noteFreqOsc2);
  wfmod_triOsc2.frequencyModulation(octaveControlOsc2); // max freq mod +/- 4.0 octaves
  wfmod_pulseOsc2.begin(WAVEFORM_BANDLIMIT_PULSE);
  // wfmod_pulseOsc2.begin(WAVEFORM_PULSE);
  wfmod_pulseOsc2.amplitude(PulseOsc2);
  wfmod_pulseOsc2.frequency(noteFreqOsc2);
  wfmod_pulseOsc2.frequencyModulation(octaveControlOsc2); // max freq mod +/- 4.0 octaves
  wfmod_pulseOsc2.pulsewidth_offset(PwOsc2);              // ws added this to synth_waveform.cpp .h
  pink_Noise.amplitude(NoiseLevel);                       //

  filter1.frequency(FreqOscFilter1);           // Freq slider
  filter1.resonance(QFactorOscFilter1);        // Q factor
  filter1.octaveControl(octaveControlFilter1); // sets range of control from mix_fcModFilter1
  filter2.frequency(FreqOscFilter2);           // Freq slider
  filter2.resonance(QFactorOscFilter2);        // Q factor
  filter2.octaveControl(octaveControlFilter2); // sets range of control from mix_fcModFilter2
  filter3.frequency(FreqNoiseFilter3);         // Freq slider
  filter3.resonance(QFactorNoiseFilter3);      // Q factor
  filter3.octaveControl(octaveControlFilter3); // sets range of control from mix_fcModFilter3
  filter4.frequency(FreqNoiseFilter4);         // Freq slider
  filter4.resonance(QFactorNoiseFilter4);      // Q factor
  filter4.octaveControl(octaveControlFilter4); // sets range of control from mix_fcModFilter4
  filter5.frequency(noteFreqFilter5);          // Freq slider
  // filter5.frequency(1.0f); // Freq slider
  filter5.resonance(QFactorFilter5);                   // Q factor
  filter5.octaveControl(octaveControlFilter5);         // sets range of control from mix_fcModFilter4
  filter_osc1.frequency(noteFreqFilterOsc1);           // Freq of osc1
  filter_osc1.resonance(0.7f);                         // Q factor
  filter_osc1.octaveControl(1.0f);                     // not using this control
  filter_osc1b.frequency(noteFreqFilterOsc1);          // Freq of osc1
  filter_osc1b.resonance(1.0f);                        // Q factor
  filter_osc1b.octaveControl(1.0f);                    // not using this control
  filter_osc2.frequency(noteFreqFilterOsc2);           // Freq of osc1
  filter_osc2.resonance(0.7f);                         // Q factor
  filter_osc2.octaveControl(1.0f);                     // not using this control
  filter_osc2b.frequency(noteFreqFilterOsc2);          // Freq of osc1
  filter_osc2b.resonance(1.0f);                        // Q factor
  filter_osc2b.octaveControl(1.0f);                    // not using this control
  filterPreNoise.frequency(clippedFreqFilterPreNoise); // highpass pre-filter for noise signal
  filterPreNoise.resonance(0.707);
  filterPreNoise.octaveControl(octaveControlPreNoiseFilter); // sets range of control from mix_fcModFilter4
  onepole_PreNoise.highpassOn(true);                         // highpass pre-filter for noise signal
  onepole_PreNoise.frequency(FreqPreNoiseFilter);            // highpass pre-filter for noise signal
  filterPreMixHPL.frequency(200);
  filterPreMixHPL.resonance(0.7);
  filterPreMixHPL.octaveControl(1.0);
  filterPreMixHPR.frequency(200);
  filterPreMixHPR.resonance(0.7);
  filterPreMixHPR.octaveControl(1.0);

  dc_sweepDepthOsc1.amplitude(0.0);
  dc_sweepDepthOsc2.amplitude(0.0);
  dc_breathLfoFilter1.amplitude(0.0);
  dc_breathLfoFilter2.amplitude(0.0);
  dc_breathLfoFilter3.amplitude(0.0);
  dc_breathLfoFilter4.amplitude(0.0);

  sine_lfoFilter1.amplitude(maxLfoDepthOscFilter1); // maxLfoDepthOscFilter1
  sine_lfoFilter1.frequency(LfoFreqOscFilter1);
  sine_lfoFilter2.amplitude(maxLfoDepthOscFilter2);
  sine_lfoFilter2.frequency(LfoFreqOscFilter2);
  sine_lfoFilter3.amplitude(maxLfoDepthNoiseFilter3);
  sine_lfoFilter3.frequency(LfoFreqNoiseFilter3);
  sine_lfoFilter4.amplitude(maxLfoDepthNoiseFilter4);
  sine_lfoFilter4.frequency(LfoFreqNoiseFilter4);

  if (XFade)
  {
    mix_xfade.gain(0, 1.0);
    mix_xfade.gain(1, -2.0);
  }
  else
  {
    mix_xfade.gain(0, 1.0); // all Osc1
    mix_xfade.gain(1, 0);   // no Osc2
  }

  // just needt to mix these, levels are set by wfmod_<name>.amplitude
  mix_osc1.gain(0, 1.0);
  mix_osc1.gain(1, 1.0); // triOsc1
  mix_osc1.gain(2, 1.0);
  mix_osc1.gain(3, 0.0);
  mix_osc2.gain(0, 1.0);
  mix_osc2.gain(1, 1.0); // triOsc2
  mix_osc2.gain(2, 1.0);
  mix_osc2.gain(3, 0.0);
  // cacluateHeadRoom();
  mix_oscLevels.gain(0, LevelOsc1 * LevelOscN_HeadRoom);
  mix_oscLevels.gain(1, LevelOsc2 * LevelOscN_HeadRoom);
  mix_oscPreComp.gain(0, LevelOsc1 * LevelOscN_HeadRoom);
  mix_oscPreComp.gain(1, LevelOsc2 * LevelOscN_HeadRoom);

  mix_limiter.gain(0, 1.0);
  mix_limiter.gain(1, LimiterAmount);
  mix_limiter.gain(2, -LimiterAmount);
  mix_limiter.gain(3, 0.0);

  mix_levelNoise.gain(0, 1.0); // pass thru for now
  // mix_levelNoise.gain(1, 1.0); // Debug path.
  mix_breathTimeNoise.gain(0, 1.0);
  mix_breathTimeNoise.gain(1, 1.0);

  mix_fcModFilter1.gain(0, 1.0);
  mix_fcModFilter1.gain(1, 1.0);
  mix_fcModFilter1.gain(2, 0.0);
  mix_fcModFilter1.gain(3, modOffsetFilter1);
  mix_fcModFilter1_sweep.gain(0, maxSweepDepthOscFilter);
  mix_fcModFilter1_sweep.gain(1, 1.0);
  mix_fcModFilter1_sweep.gain(2, 0.0);
  mix_fcModFilter1_sweep.gain(3, 0.0);

  mix_fcModFilter2.gain(0, 1.0);
  mix_fcModFilter2.gain(1, 1.0);
  mix_fcModFilter2.gain(2, 0.0);
  mix_fcModFilter2.gain(3, modOffsetFilter2);
  mix_fcModFilter2_sweep.gain(0, maxSweepDepthOscFilter);
  mix_fcModFilter2_sweep.gain(1, 1.0);
  mix_fcModFilter2_sweep.gain(2, 0.0);
  mix_fcModFilter2_sweep.gain(3, 0.0);

  mix_fcModFilter3.gain(0, 1.0);
  mix_fcModFilter3.gain(1, 1.0);
  mix_fcModFilter3.gain(2, 0.0);
  mix_fcModFilter3.gain(3, modOffsetFilter3);
  mix_fcModFilter3_sweep.gain(0, maxSweepDepthOscFilter);
  mix_fcModFilter3_sweep.gain(1, 1.0);
  mix_fcModFilter3_sweep.gain(2, 0.0);
  mix_fcModFilter3_sweep.gain(3, 0.0);

  mix_fcModFilter4.gain(0, 1.0);
  mix_fcModFilter4.gain(1, 1.0);
  mix_fcModFilter4.gain(2, 0.0);
  mix_fcModFilter4.gain(3, modOffsetFilter4);
  mix_fcModFilter4_sweep.gain(0, maxSweepDepthOscFilter);
  mix_fcModFilter4_sweep.gain(1, 1.0);
  mix_fcModFilter4_sweep.gain(2, 0.0);
  mix_fcModFilter4_sweep.gain(3, 0.0);

  mix_ntcFilter1.gain(0, 0.0);
  mix_ntcFilter1.gain(1, maxMixNtcFilter); // Default LP
  mix_ntcFilter1.gain(2, 0.0);
  mix_ntcFilter1.gain(3, 0.0);
  mix_ntcFilter2.gain(0, 0.0);
  mix_ntcFilter2.gain(1, maxMixNtcFilter); // Default LP
  mix_ntcFilter2.gain(2, 0.0);
  mix_ntcFilter2.gain(3, 0.0);
  mix_ntcFilter3.gain(0, 0.0);
  mix_ntcFilter3.gain(1, maxMixNtcFilter); // Default LP
  mix_ntcFilter3.gain(2, 0.0);
  mix_ntcFilter3.gain(3, 0.0);
  mix_ntcFilter4.gain(0, 0.0);
  mix_ntcFilter4.gain(1, maxMixNtcFilter); // Default LP
  mix_ntcFilter4.gain(2, 0.0);
  mix_ntcFilter4.gain(3, 0.0);

  fir_formant.begin(FIR_PASSTHRU, 0); // TODO: create formant filter

  mix_Amp.gain(0, AmpLevel * Amp_HeadRoom);
  mix_Amp.gain(1, 1.0f); // 4000s AmpLevel doesn't control Noise Level

  flange1.begin(delayline_flange1, FLANGE_DELAY_LENGTH,
                (int)EffectsChorusDelay1 % (int)(93.0f * 88.1f), (int)EffectsChorusMod1, EffectsChorusLfoFreq);
  flange2.begin(delayline_flange2, FLANGE_DELAY_LENGTH,
                (int)EffectsChorusDelay2 % (int)(93.0f * 88.1f), (int)EffectsChorusMod2, EffectsChorusLfoFreq);
  flange3.begin(delayline_flange3, FLANGE_DELAY_LENGTH,
                FLANGE_DELAY_PASSTHRU, 0, 0.0);
  mix_chorus_fb.gain(0, 1.0 * EffectsChorusFBHeadroom);
  mix_chorus_fb.gain(1, EffectsChorusFeedback * EffectsChorusFBHeadroom);
  mix_chorus_wet.gain(0, EffectsChorusWet1);
  mix_chorus_wet.gain(1, EffectsChorusWet2);
  mix_chorus_wet.gain(2, -0.5 * (EffectsChorusWet1 + EffectsChorusWet2)); // to cancel feed through of flange1
  if (ChorusOn == 1)
  {
    mix_chorus_dryL.gain(0, EffectsChorusDryLevel * EffectsChorusDryHeadroom);
    mix_chorus_dryL.gain(1, 1.0 * EffectsChorusDryHeadroom);
    mix_chorus_dryR.gain(0, EffectsChorusDryLevel * EffectsChorusDryHeadroom);
    mix_chorus_dryR.gain(1, 1.0 * EffectsChorusDryHeadroom);
  }
  else if(ChorusOn == 2)
  {
    mix_chorus_dryL.gain(0, EffectsChorusDryLevel * (2.0f*EffectsChorusDryHeadroom));
    mix_chorus_dryL.gain(1, 0.0);
    mix_chorus_dryR.gain(0, 0.0);
    mix_chorus_dryR.gain(1, 2.0 * EffectsChorusDryHeadroom);
  }
  else
  {
    mix_chorus_dryL.gain(0, 1.0 * EffectsChorusDryHeadroom);
    mix_chorus_dryL.gain(1, 0.0 * EffectsChorusDryHeadroom);
    mix_chorus_dryR.gain(0, 1.0 * EffectsChorusDryHeadroom);
    mix_chorus_dryR.gain(1, 0.0 * EffectsChorusDryHeadroom);
  }

  // setup delay_F32 here
  delay_F32.bypass_set(false);              // delay off at start
  delay_F32.time(EffectsDelayTimeL);        // 1.27sec and use psram
  delay_F32.feedback(EffectsDelayFeedback); // 0 - 1.0
  delay_F32.mix(EffectsDelayLevel);
  // delay_F32.delay(number_of_samples); //
  // delay_F32.inertia(0.5f); // 0 for fastest update - 1.0 for slowest update of delay time
  delay_F32.treble(EffectsDelayDamp);     // 0- 1.0
  delay_F32.treble_cut(EffectsDelayDamp); // 0- 1.0
  // delay_F32.bass(0.0f); // 0- 1.0
  // delay_F32.bass_cut(0.0f); // 0- 1.0
  // delay_F32.mod_rateHz(0.0f); // 0- 16.0hz
  // delay_F32.mod_rate(0.0f); // 0- 1.0
  // delay_F32.mix(maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));

  // setup verb_f32 here
  verb_F32.bypass_set(false);
  verb_F32.mix(EffectsReverbLevel);            // 1.0 = full wet
  verb_F32.size(EffectsReverbTime);            // sets reverb time (0.0 to 1.0f)
  verb_F32.diffusion(EffectsReverbDenseEarly); // lower values for more single repeats (echos) (0.0 to 1.0f)
  verb_F32.hidamp(EffectsReverbDamp);          // treble loss in reverb tails (0.0 to 1.0f)
  verb_F32.lodamp(0.0);                        // bass loss in reverb tails (0.0 to 1.0f)
  verb_F32.lowpass(1.0f);                      // output lp filter (0.0 to 1.0f)
  verb_F32.hipass(0.0f);                       // output hp filter (0.0 to 1.0f)
  // verb_F32.freezeBleedIn(tmp);
  // value = map(value, 0, 127, -12, 24);
  verb_F32.pitchSemitones(0);
  verb_F32.pitchMix(0.0f);
  verb_F32.shimmer(0.0f);
  // float value = map(0.0f, 0, 127, -12, 24);
  verb_F32.shimmerPitchSemitones(0.0f);

  Serial8.println(str_buf1);

  delay(200); // orignal has this, not sure why

  //-------------- setup MIDI --------------------
  MIDIs1.begin(MIDI_CHANNEL_OMNI); // midi din conncetion
  myUSBHost.begin();               // host port for connecting usb wind controller device
                                   // usbMIDI is included in the Teensy Core Library, so usbMIDI.begin() not needed

  //------------ load patches and Fx ----------
  loadAllPatches();
  loadGlobalFxSD();

  //------------ restore EEPROM values ---------------------
  EEPROM.get(PATCH_NUMBER_EEPROM_ADDR, eeprom_patchNumber);
  sprintf(str_buf1, "read eeprom_patchNumber (%03d)", eeprom_patchNumber);
  Serial8.println(str_buf1);
  if (eeprom_patchNumber < 0 || eeprom_patchNumber >= NUMBER_OF_PATCHES)
  {
    // loadPatchNumberEEPROM not within 0 to NUMBER_OF_PATCHES-1. overwrite with 000.
    sprintf(str_buf1, "loadPatchNumberEEPROM (%03d) out of range. set to 000.", eeprom_patchNumber);
    Serial8.println(str_buf1);
    eeprom_patchNumber = 0;
    EEPROM.put(PATCH_NUMBER_EEPROM_ADDR, eeprom_patchNumber); // address 0
  }

  EEPROM.get(FINE_TUNE_CENTS_EEPROM_ADDR, eeprom_FineTuneCents);
  sprintf(str_buf1, "read eeprom_FineTuneCents (%03d)", eeprom_FineTuneCents);
  Serial8.println(str_buf1);
  if (eeprom_FineTuneCents < 14 || eeprom_FineTuneCents > 114)
  {
    sprintf(str_buf1, "eeprom_FineTuneCents (%03d) out of range.", eeprom_FineTuneCents);
    Serial8.println(str_buf1);
    eeprom_FineTuneCents = 64; // default 0 cents
    EEPROM.put(FINE_TUNE_CENTS_EEPROM_ADDR, eeprom_FineTuneCents);
  }
  FineTuneCents = eeprom_FineTuneCents; // store as 14 to 114 for -50 to +50;
  FineTuneCentsf = ((float)FineTuneCents - 64.0f) / 100.0f;

  EEPROM.get(TRANSPOSE_EEPROM_ADDR, eeprom_Transpose);
  sprintf(str_buf1, "read eeprom_Transpose (%03d)", eeprom_Transpose);
  Serial8.println(str_buf1);
  if (eeprom_Transpose < 52 || eeprom_Transpose > 76)
  {
    sprintf(str_buf1, "eeprom_Transpose (%03d) out of range.", eeprom_Transpose);
    Serial8.println(str_buf1);
    eeprom_Transpose = 64; // default 0 transpose
    EEPROM.put(TRANSPOSE_EEPROM_ADDR, eeprom_Transpose);
  }
  Transpose = eeprom_Transpose; // store as 52 to 76 for -12 to +12
  Transposef = ((float)Transpose - 64.0f) * 12.0f;

  EEPROM.get(OCTAVE_EEPROM_ADDR, eeprom_Octave);
  sprintf(str_buf1, "read eeprom_Octave (%03d)", eeprom_Octave);
  Serial8.println(str_buf1);
  if (eeprom_Octave < 62 || eeprom_Octave > 66)
  {
    sprintf(str_buf1, "eeprom_Octave (%03d) out of range.", eeprom_Octave);
    Serial8.println(str_buf1);
    eeprom_Octave = 64; // default 0 octave
    EEPROM.put(OCTAVE_EEPROM_ADDR, eeprom_Octave);
  }
  Octave = eeprom_Octave; // store as 62 to 66 for -2 to +2
  Octavef = ((float)Octave - 64.0f) * 12.0f;

  EEPROM.get(BREATH_CC_EEPROM_ADDR, eeprom_breath_cc);
  sprintf(str_buf1, "read eeprom_breath_cc (%03d)", eeprom_breath_cc);
  Serial8.println(str_buf1);
  if (eeprom_breath_cc < 1 || eeprom_breath_cc > 11)
  {
    sprintf(str_buf1, "eeprom_breath_cc (%03d) out of range.", eeprom_breath_cc);
    Serial8.println(str_buf1);
    eeprom_breath_cc = 2; // default cc02 (breath)
    EEPROM.put(BREATH_CC_EEPROM_ADDR, eeprom_breath_cc);
  }

  breath_cc = eeprom_breath_cc;

  EEPROM.get(MIX_LINEIN_EEPROM_ADDR, eeprom_mix_linein);
  sprintf(str_buf1, "read eeprom_mix_linein (%03d)", eeprom_mix_linein);
  Serial8.println(str_buf1);
  if (eeprom_mix_linein < 0 || eeprom_mix_linein > 100)
  {
    eeprom_mix_linein = 0;
    EEPROM.put(MIX_LINEIN_EEPROM_ADDR, eeprom_mix_linein);
  }
  mix_linein = eeprom_mix_linein;
  mix_lineinf = ((float)mix_linein) / 100.0f;

  EEPROM.get(VOL_EEPROM_ADDR, eeprom_vol);
  sprintf(str_buf1, "read eeprom_vol (%03d)", eeprom_vol);
  Serial8.println(str_buf1);
  if (eeprom_vol < 0 || eeprom_vol > 100)
  {
    eeprom_vol = 0;
    EEPROM.put(VOL_EEPROM_ADDR, eeprom_vol);
  }
  vol = eeprom_vol;
  // volf = ((float)vol)/50.0f;
  volf = ((float)vol) / 100.0f;
  volf = (volf * volf) * 2.0f;

  EEPROM.get(FXSOURCE_EEPROM_ADDR, eeprom_fxSourcePatch);
  sprintf(str_buf1, "read eeprom_fxSourcePatch (%03d)", eeprom_fxSourcePatch);
  Serial8.println(str_buf1);
  if (eeprom_fxSourcePatch < 0 || eeprom_fxSourcePatch > 1)
  {
    eeprom_fxSourcePatch = 1;
    EEPROM.put(FXSOURCE_EEPROM_ADDR, eeprom_fxSourcePatch);
  }
  fxSourcePatch = eeprom_fxSourcePatch;

  int size = sizeof(eeprom_comp_params) / sizeof(eeprom_comp_params[0]);
  for (int i = 0; i < size; i++)
  {

    EEPROM.get(COMP_PRAMS_ADDR + i * sizeof(eeprom_comp_params[0]), eeprom_comp_params[i]);
    switch (i)
    {
    case CCCOMPATTACK:
      if (eeprom_comp_params[i] < 1 || eeprom_comp_params[i] > 127)
      {
        eeprom_comp_params[i] = 1; // 0.1-12.7ms (1-127) default 1 = 0.1ms
        EEPROM.put(COMP_PRAMS_ADDR + i * sizeof(eeprom_comp_params[0]), eeprom_comp_params[i]);
      }
      comp_paramsf[i] = (float)eeprom_comp_params[i] / 10.0f;
      break;
    case CCCOMPRELEASE:
      if (eeprom_comp_params[i] < 1 || eeprom_comp_params[i] > 127)
      {
        eeprom_comp_params[i] = 20; // 1-127ms default 20 = 20ms
        EEPROM.put(COMP_PRAMS_ADDR + i * sizeof(eeprom_comp_params[0]), eeprom_comp_params[i]);
      }
      comp_paramsf[i] = (float)eeprom_comp_params[i];
      break;
    case CCCOMPRATIO:
      if (eeprom_comp_params[i] < 1 || eeprom_comp_params[i] > 127)
      {
        eeprom_comp_params[i] = 23; // 0.1-12.7 (1-127), default 23 = 2.3
        EEPROM.put(COMP_PRAMS_ADDR + i * sizeof(eeprom_comp_params[0]), eeprom_comp_params[i]);
      }
      comp_paramsf[i] = (float)eeprom_comp_params[i] / 10.0f;
      break;
    case CCCOMPTHRESHOLD:
      if (eeprom_comp_params[i] < 24 || eeprom_comp_params[i] > 64)
      {
        eeprom_comp_params[i] = 48; // -40 to 0 = 24 to 64, default = 48 = -16
        EEPROM.put(COMP_PRAMS_ADDR + i * sizeof(eeprom_comp_params[0]), eeprom_comp_params[i]);
      }
      comp_paramsf[i] = (float)eeprom_comp_params[i] - 64.0f;
      break;
    case CCCOMPKNEE:
      if (eeprom_comp_params[i] < 0 || eeprom_comp_params[i] > 40)
      {
        eeprom_comp_params[i] = 16; // 0 to 40dB, default = 16db
        EEPROM.put(COMP_PRAMS_ADDR + i * sizeof(eeprom_comp_params[0]), eeprom_comp_params[i]);
      }
      comp_paramsf[i] = (float)eeprom_comp_params[i];
      break;
    case CCCOMPMAKEUPGAIN:
      if (eeprom_comp_params[i] < 0 || eeprom_comp_params[i] > 40)
      {
        eeprom_comp_params[i] = 9; // 0 to 40dB, default = 9dB
        EEPROM.put(COMP_PRAMS_ADDR + i * sizeof(eeprom_comp_params[0]), eeprom_comp_params[i]);
      }
      comp_paramsf[i] = (float)eeprom_comp_params[i];
      break;
    default:
      break;
    }
    comp_params[i] = (uint8_t)eeprom_comp_params[i];
  }

  EEPROM.get(NNBMODCALPROM_ADDR, eeprom_NNBModCal);
  sprintf(str_buf1, "read eeprom_NNBModCal (%03d)", eeprom_NNBModCal);
  Serial8.println(str_buf1);
  if (eeprom_NNBModCal < 60 || eeprom_NNBModCal > 100)
  {
    eeprom_NNBModCal = 87; // default 87
    EEPROM.put(NNBMODCALPROM_ADDR, eeprom_NNBModCal);
  }
  NNBModCal = eeprom_NNBModCal;

  /*
      It offers the following controls:
      Attack rates 0.1 - 2000 mS, float
      Release rates 0.1 - 2000mS, float
      Ratio between 1 to 32767, float
      Knee width between 0 to 40dB (soft to hard knee control), float
      Threshold between 0 to -40dB, float
      Makeup Gain between 0 to 40dB, float
      Multiple side chain inputs that can be selected at run time, uint8_t
  */
  compress_oscLevels1.enable();
  compress_oscLevels1.setAttack(comp_paramsf[CCCOMPATTACK]);
  compress_oscLevels1.setRelease(comp_paramsf[CCCOMPRELEASE]);
  compress_oscLevels1.setRatio(comp_paramsf[CCCOMPRATIO]);
  compress_oscLevels1.setThreshold(comp_paramsf[CCCOMPTHRESHOLD]);
  compress_oscLevels1.setKnee(comp_paramsf[CCCOMPKNEE]);
  compress_oscLevels1.setMakeupGain(comp_paramsf[CCCOMPMAKEUPGAIN]);
  compress_oscLevels1.setSideChain(comp_sideChain);
  // compress_oscLevels1.disable();
  compress_oscLevels2.enable();
  compress_oscLevels2.setAttack(comp_paramsf[CCCOMPATTACK]);
  compress_oscLevels2.setRelease(comp_paramsf[CCCOMPRELEASE]);
  compress_oscLevels2.setRatio(comp_paramsf[CCCOMPRATIO]);
  compress_oscLevels2.setThreshold(comp_paramsf[CCCOMPTHRESHOLD]);
  compress_oscLevels2.setKnee(comp_paramsf[CCCOMPKNEE]);
  compress_oscLevels2.setMakeupGain(comp_paramsf[CCCOMPMAKEUPGAIN]);
  compress_oscLevels2.setSideChain(comp_sideChain);
  // compress_oscLevels2.disable();

  amp_extraGainR_F32.setGain(extraAmpFactor);
  amp_extraGainL_F32.setGain(extraAmpFactor);
  mix_pongL_F32.gain(0, 0.5f * EffectsDelayPong + 0.5f);
  mix_pongL_F32.gain(1, -0.5f * EffectsDelayPong + 0.5f);
  mix_pongR_F32.gain(0, -0.5f * EffectsDelayPong + 0.5f);
  mix_pongR_F32.gain(1, 0.5f * EffectsDelayPong + 0.5f);

  // mix_lineInL.gain(0, volf(1.0-mix_lineinf));
  mix_lineInL.gain(0, volf);
  mix_lineInL.gain(1, mix_lineinf * extraLineInAmpFactor);
  mix_lineInL.gain(2, 0.0);
  mix_lineInL.gain(3, 0.0);
  // mix_lineInR.gain(0, volf(1.0-mix_lineinf));
  mix_lineInR.gain(0, volf);
  mix_lineInR.gain(1, mix_lineinf * extraLineInAmpFactor);
  mix_lineInR.gain(2, 0.0);
  mix_lineInR.gain(3, 0.0);
  sprintf(str_buf1, "loadPatchNumberEEPROM (%03d) ", eeprom_patchNumber);
  Serial8.println(str_buf1);
  // sprintf(str_buf1, "current_patchNumber (%03d) ", current_patchNumber );
  // Serial8.println(str_buf1);

  // if (patchLoaded[eeprom_patchNumber]){
  current_patchNumber = eeprom_patchNumber;
  // loadPatchSD(current_patchNumber);
  setCurrentPatch(current_patchNumber);
  patchToSynthVariables();
  sprintf(str_buf1, "current_patchNumber (%03d) ", current_patchNumber);
  Serial8.println(str_buf1);
  printPatchValues();
  //}

  updateSynthVariablesFlag = true;
  // printPatchValues();
  // PRINT_VALUES_FLAG = true;

  AudioInterrupts();

  //----------- reset debug tools ----------
  //  AudioProcessorUsageMaxReset();
  //  AudioMemoryUsageMaxReset();

  //------------ i2c setup----------
  Wire1.begin();

  //------------ u8g2 oled screen setup ------------
  if (!u8g2.begin())
  { // begin() will reset, configure, clear and disable power save
    // should never get here
    Serial8.println("u8g2 allocation failed");
  }
  // u8g2.setFont(u8g2_font_5x7_mf); // lines start at {7,14,21,28,35,42,49,56,63};
  //  set font for splash screen
  u8g2.setFont(u8g2_font_10x20_mf); // lines start at {8,16,24,32,40,48,56,64};
  // u8g2.drawStr(0,u8g2.getMaxCharHeight(),str_oledbuf);
  // myMenu.drawStrNl(0,u8g2.getMaxCharHeight(),str_oledbuf); //"Nl to handle new line
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  genSplashScreen(); // uses str_oledbuf
  u8g2.sendBuffer();
  // set font for menu screens
  u8g2.setFont(u8g2_font_6x10_mf); // lines start at {8,16,24,32,40,48,56,64};

  // setup menus
  setupMenus(u8g2);
  ms->begin();

  // myMenu.setCurrentMenu(&listTopMenu);
  //  Serial8.println(str_oledbuf);
  env_squelch.delay(squelchDelay);
  env_squelch.attack(squelchAttack);
  env_squelch.sustain(1.0f);
  env_squelch.release(squelchRelease);
  env_squelch.releaseNoteOn(squelchRelease);
  env_squelch.noteOn();
  env_squelchL.delay(squelchDelay);
  env_squelchL.attack(squelchAttack);
  env_squelchL.sustain(1.0f);
  env_squelchL.release(squelchRelease);
  env_squelchL.releaseNoteOn(squelchRelease);
  env_squelchL.noteOn();
  env_squelchR.delay(squelchDelay);
  env_squelchR.attack(squelchAttack);
  env_squelchR.sustain(1.0f);
  env_squelchR.release(squelchRelease);
  env_squelchR.releaseNoteOn(squelchRelease);
  env_squelchR.noteOn();

  // set to top menu after delay

  //------wait till powered up-------------
  do
  {
    pwrDownSense.update();
  } while (!pwrDownSense.read()); // false means powered down

  delay(3000); // show splash screen for 3s
  // initial draw
  ms->draw();
}

void loop()
{
  static unsigned long prevMilMenu = millis();
  static unsigned long prevMilUpdateSynth = millis();
  //  static unsigned long prevMilRMS = millis();

  //------------ check for SGTL5000 pwr down-------------
  pwrDownSense.update();
  if (!pwrDownSense.read())
  {
    sgtl_power_down(); // shut down dac "vag" to avoid audio "pop".
    // u8g2.setFont(u8g2_font_10x20_mf  ); // lines start at {8,16,24,32,40,48,56,64};
    // u8g2.drawStr(20,20,"BYE!");
    // u8g2.sendBuffer();
    while (true)
    {
    }
  }
  //-------------check for SGTL5000 pwr down-------------

  currentMillis = millis();
  //----------check knobs, buttons and updateUI----------
  currentUITimeoutTime = currentMillis;
  if (currentMillis - prevMilMenu >= readKnobInterval)
  { // every 33ms
    if (lastBreathf > 0.0f)
    {
      digitalWrite(ledPin, HIGH); // set the LED on
    }
    else
    {
      digitalWrite(ledPin, LOW); // set the LED off
    }

    prevMilMenu = currentMillis;
    updateUI();
    if (programChangeFlag)
    {
      patchSelect();
      programChangeFlag = false;
      ms->draw();
    }
  }
  //----------LED low based on lastBreathf
  if (currentMillis - prevMilMenu >= (uint32_t)(lastBreathf * ledMaxInterval))
  {
    digitalWrite(ledPin, LOW); // set the LED off
    // don't need this often for plug sense but it is convienient
    loRightPluggedIn.update();
    loLeftPluggedIn.update();
    monoModeFlag = loRightPluggedIn.read() && !loLeftPluggedIn.read();
  }
  //----------squelch audio when changing patches------
  if (preUpdateSynthVariablesFlag)
  {                         // set in patchToSynthVariables()
    env_squelch.noteOff();  // start squelch audio (release)
    env_squelchL.noteOff(); // start squelch audio (release)
    env_squelchR.noteOff(); // start squelch audio (release)
    if (currentMillis - prevMilUpdateSynth >= updateSynthDelay)
    {
      preUpdateSynthVariablesFlag = false;
      updateSynthVariablesFlag = true;
      env_squelch.noteOn();  // start un squelch audio (attack)
      env_squelchL.noteOn(); // start un squelch audio (attack)
      env_squelchR.noteOn(); // start un squelch audio (attack)
    }
  }
  else
  {
    prevMilUpdateSynth = currentMillis;
  }

  if (updateCalibrationFlag)
  {

    // cacluateHeadRoom();
    compress_oscLevels1.setAttack(comp_paramsf[CCCOMPATTACK]);
    compress_oscLevels1.setRelease(comp_paramsf[CCCOMPRELEASE]);
    compress_oscLevels1.setRatio(comp_paramsf[CCCOMPRATIO]);
    compress_oscLevels1.setThreshold(comp_paramsf[CCCOMPTHRESHOLD]);
    compress_oscLevels1.setKnee(comp_paramsf[CCCOMPKNEE]);
    compress_oscLevels1.setMakeupGain(comp_paramsf[CCCOMPMAKEUPGAIN]);
    compress_oscLevels2.setAttack(comp_paramsf[CCCOMPATTACK]);
    compress_oscLevels2.setRelease(comp_paramsf[CCCOMPRELEASE]);
    compress_oscLevels2.setRatio(comp_paramsf[CCCOMPRATIO]);
    compress_oscLevels2.setThreshold(comp_paramsf[CCCOMPTHRESHOLD]);
    compress_oscLevels2.setKnee(comp_paramsf[CCCOMPKNEE]);
    compress_oscLevels2.setMakeupGain(comp_paramsf[CCCOMPMAKEUPGAIN]);
    updateCalibrationFlag = false;
  }

  //-------------
  // Update Synth Variables if they changed
  //-------------
  if (updateSynthVariablesFlag)
  {
    changeFilterMode();
    if (XFade)
    {
      mix_xfade.gain(0, 1.0f);
      mix_xfade.gain(1, -2.0f);
    }
    else
    {
      mix_xfade.gain(0, 1.0f); // all Osc1
      mix_xfade.gain(1, 0.0f); // no Osc2
    }

    mix_oscLevels.gain(0, LevelOsc1 * LevelOscN_HeadRoom);
    mix_oscLevels.gain(1, LevelOsc2 * LevelOscN_HeadRoom);
    mix_oscPreComp.gain(0, LevelOsc1 * LevelOscN_HeadRoom);
    mix_oscPreComp.gain(1, LevelOsc2 * LevelOscN_HeadRoom);
    sine_lfoOsc1.amplitude(PwmDepthOsc1);
    sine_lfoOsc1.frequency(PwmFreqOsc1);
    sine_lfoOsc2.amplitude(PwmDepthOsc2);
    sine_lfoOsc2.frequency(PwmFreqOsc2);
    sine_lfoFilter1.frequency(LfoFreqOscFilter1);
    if (LinkOscFilters)
    {
      sine_lfoFilter2.frequency(LfoFreqOscFilter1);
    }
    else
    {
      sine_lfoFilter2.frequency(LfoFreqOscFilter2);
    }
    if (LinkNoiseFilters)
    {
      sine_lfoFilter4.frequency(LfoFreqNoiseFilter3);
    }
    else
    {
      sine_lfoFilter4.frequency(LfoFreqNoiseFilter4);
    }

    sine_lfoFilter3.frequency(LfoFreqNoiseFilter3);
    wfmod_sawOsc1.amplitude(SawOsc1);
    wfmod_triOsc1.amplitude(TriOsc1);
    wfmod_pulseOsc1.amplitude(PulseOsc1);
    wfmod_pulseOsc1.pulsewidth_offset(PwOsc1); // ws added this to synth_waveform.cpp .h
    wfmod_sawOsc2.amplitude(SawOsc2);
    wfmod_triOsc2.amplitude(TriOsc2);
    wfmod_pulseOsc2.amplitude(PulseOsc2);
    wfmod_pulseOsc2.pulsewidth_offset(PwOsc2); // ws added this to synth_waveform.cpp .h
    pink_Noise.amplitude(NoiseLevel);          //

    // wrap EffectsChorusDelay1 & 2 at 93ms (44.1*93 = 4101.3) (ewi 4k wraps like this)
    flange1.voices((int)EffectsChorusDelay1 % (int)(93.0f * 88.1f), (int)EffectsChorusMod1, EffectsChorusLfoFreq);
    flange2.voices((int)EffectsChorusDelay2 % (int)(93.0f * 88.1f), (int)EffectsChorusMod2, EffectsChorusLfoFreq);
    mix_chorus_fb.gain(0, 1.0 * EffectsChorusFBHeadroom);
    mix_chorus_fb.gain(1, EffectsChorusFeedback * EffectsChorusFBHeadroom);
    mix_chorus_wet.gain(0, EffectsChorusWet1);
    mix_chorus_wet.gain(1, EffectsChorusWet2);
    mix_chorus_wet.gain(2, -0.5 * (EffectsChorusWet1 + EffectsChorusWet2)); // to cancel feed through of flange1
    if (ChorusOn == 1)
    {
        mix_chorus_dryL.gain(0, EffectsChorusDryLevel * EffectsChorusDryHeadroom);
        mix_chorus_dryL.gain(1, 1.0 * EffectsChorusDryHeadroom);
        mix_chorus_dryR.gain(0, EffectsChorusDryLevel * EffectsChorusDryHeadroom);
        mix_chorus_dryR.gain(1, 1.0 * EffectsChorusDryHeadroom);
    }
    else if(ChorusOn == 2)
    {
        mix_chorus_dryL.gain(0, EffectsChorusDryLevel * (2.0f*EffectsChorusDryHeadroom));
        mix_chorus_dryL.gain(1, 0.0);
        mix_chorus_dryR.gain(0, 0.0);
        mix_chorus_dryR.gain(1, 2.0 * EffectsChorusDryHeadroom);
    }
    else
    {
        mix_chorus_dryL.gain(0, 1.0 * EffectsChorusDryHeadroom);
        mix_chorus_dryL.gain(1, 0.0 * EffectsChorusDryHeadroom);
        mix_chorus_dryR.gain(0, 1.0 * EffectsChorusDryHeadroom);
        mix_chorus_dryR.gain(1, 0.0 * EffectsChorusDryHeadroom);
    }

    verb_F32.mix(EffectsReverbLevel);            // 1.0 = full wet
    verb_F32.size(EffectsReverbTime);            // sets reverb time (0.0 to 1.0f)
    verb_F32.diffusion(EffectsReverbDenseEarly); // lower values for more single repeats (echos) (0.0 to 1.0f)
    verb_F32.hidamp(EffectsReverbDamp);          // treble loss in reverb tails (0.0 to 1.0f)

    delay_F32.time(EffectsDelayTimeL); // 1.27sec and use psram
    delay_F32.mix(EffectsDelayLevel);
    delay_F32.feedback(EffectsDelayFeedback);  // 0 - 1.0
    delay_F32.treble(1.0f - EffectsDelayDamp); // 0- 1.0
    delay_F32.treble_cut(EffectsDelayDamp);    // 0- 1.0
    delay_F32.set_ping_pong_on(EffectsDelayPong > 0.0f);
    delay_F32.treble_cut(EffectsDelayDamp); // 0- 1.0
    updateSynthVariablesFlag = false;
    // PRINT_VALUES_FLAG = true;
    PRINT_VALUES_FLAG = false;
  } // if (updateSynthVariablesFlag)

  // no reason to wait for these
  mix_lineInLR_gain_0 = volf;
  mix_lineInLR_gain_1 = mix_lineinf * extraLineInAmpFactor;

  //-------------------------------------------------------
  //  Update Realtime Audio System
  //      Time to update variables that need to be changed
  //      all at the same time.
  //-------------------------------------------------------
  AudioNoInterrupts();
  mix_Amp.gain(0, mix_Amp_gain_0);
  mix_Amp.gain(1, mix_Amp_gain_1); // 4000s AmpLevel doesn't control Noise Level
  if (monoModeFlag)
  {
    mix_lineInL.gain(0, 0.7 * mix_lineInLR_gain_0); // synth
    mix_lineInL.gain(1, 0.7 * mix_lineInLR_gain_1); // line in
    mix_lineInL.gain(2, 0.7 * mix_lineInLR_gain_0);
    mix_lineInL.gain(3, 0.7 * mix_lineInLR_gain_1);
    mix_lineInR.gain(0, 0.7 * mix_lineInLR_gain_0);
    mix_lineInR.gain(1, 0.7 * mix_lineInLR_gain_1);
    mix_lineInR.gain(2, 0.7 * mix_lineInLR_gain_0);
    mix_lineInR.gain(3, 0.7 * mix_lineInLR_gain_1);
  }
  else
  {
    mix_lineInL.gain(0, mix_lineInLR_gain_0);
    mix_lineInL.gain(1, mix_lineInLR_gain_1);
    mix_lineInL.gain(2, 0.0);
    mix_lineInL.gain(3, 0.0);
    mix_lineInR.gain(0, mix_lineInLR_gain_0);
    mix_lineInR.gain(1, mix_lineInLR_gain_1);
    mix_lineInR.gain(2, 0.0);
    mix_lineInR.gain(3, 0.0);
  }

  mix_pongL_F32.gain(0, 0.5f * EffectsDelayPong + 0.5f);
  mix_pongL_F32.gain(1, -0.5f * EffectsDelayPong + 0.5f);
  mix_pongR_F32.gain(0, -0.5f * EffectsDelayPong + 0.5f);
  mix_pongR_F32.gain(1, 0.5f * EffectsDelayPong + 0.5f);

  noteNumberOsc1 = porta_step ? round(dc_portatime.read() * 128.0) : dc_portatime.read() * 128;
  noteNumberOsc1 = BendStep ? round(noteNumberOsc1 + BendRange * dc_pitchbend.read())
                            : noteNumberOsc1 + BendRange * dc_pitchbend.read();
  noteNumberOsc1 = noteNumberOsc1 + OctOsc1 * 12.0 + SemiOsc1 + FineOsc1 + FineTuneCentsf + Transposef + Octavef;
  // noteNumberFilter1 = dc_portatimef.read()*128 + OctOsc1*12.0 + SemiOsc1+FineOsc1;
  // noteNumberFilter1 = dc_portatimef.read()*128 + SemiOsc1+FineOsc1;
  noteNumberFilter1 = dc_portatimef.read() * 128.0f + FineTuneCentsf + Transposef + Octavef;
  noteNumberOsc2 = porta_step ? round(dc_portatime.read() * 128.0) : dc_portatime.read() * 128;
  noteNumberOsc2 = BendStep ? round(noteNumberOsc2 + BendRange * dc_pitchbend.read())
                            : noteNumberOsc2 + BendRange * dc_pitchbend.read();
  noteNumberOsc2 = noteNumberOsc2 + OctOsc2 * 12.0 + SemiOsc2 + FineOsc2 + FineTuneCentsf + Transposef + Octavef;
  // noteNumberFilter2 = dc_portatimef.read()*128 + OctOsc2*12.0 + SemiOsc2+FineOsc2;
  // noteNumberFilter2 = dc_portatimef.read()*128 + SemiOsc2+FineOsc2;
  noteFreqOsc1 = 440.0 * pow(2, (noteNumberOsc1 - 69.0) / 12);                                               // 69 is note number for A4=440Hz
  noteFreqOsc1 = noteFreqOsc1 + BeatOsc1;                                                                    // BeatOsc1 is additive
  noteFreqOsc2 = 440.0 * pow(2, (noteNumberOsc2 - 69.0) / 12);                                               // 69 is note number for A4=440Hz
  noteFreqOsc2 = noteFreqOsc2 + BeatOsc2;                                                                    // BeatOsc2 is additive
  noteFreqFilterOsc1 = 440.0 * pow(2, (noteNumberOsc1 - 69.0f + 9.0f) / 12.0f);                              // 69 is note number for A4=440Hz
  noteFreqFilterOsc2 = 440.0 * pow(2, (noteNumberOsc2 - 69.0f + 9.0f) / 12.0f);                              // 69 is note number for A4=440Hz
  noteFreqFilter5 = 440.0 * pow(2, (min(noteNumberOsc1, noteNumberOsc2) - 69.0) / 12);                       // always at noteNumberOsc1 or 2 whichever is lower;  TODO: match 4000s
  keyfollowFilter1 = pow(2, (noteNumberFilter1 - offsetNoteKeyfollow) * KeyFollowOscFilter1 / 144.0);        // 72 is C5
  keyfollowFilter2 = pow(2, (noteNumberFilter1 - offsetNoteKeyfollow) * KeyFollowOscFilter2 / 144.0);        // 72 is C5
  keyfollowFilter3 = pow(2, (noteNumberFilter1 - offsetNoteKeyfollowNoise) * KeyFollowNoiseFilter3 / 144.0); // 72 is C5
  keyfollowFilter4 = pow(2, (noteNumberFilter1 - offsetNoteKeyfollowNoise) * KeyFollowNoiseFilter4 / 144.0); // 72 is C5
  // keyfollowFilter5 = pow(2, (noteNumberFilter1-offsetNoteKeyfollowFilter5)*KeyFollowFilter5/144.0);
  keyfollowFilterPreNoise = pow(2, ((noteNumberFilter1 < minPreNoiseNoteNumbr ? minPreNoiseNoteNumbr : noteNumberFilter1) - offsetNoteKeyfollowPreNoise) * KeyFollowPreNoiseFilter / 144.0); // 72 is C5
  wfmod_sawOsc1.frequency(noteFreqOsc1);
  wfmod_triOsc1.frequency(noteFreqOsc1);
  wfmod_pulseOsc1.frequency(noteFreqOsc1);
  wfmod_sawOsc2.frequency(noteFreqOsc2);
  wfmod_triOsc2.frequency(noteFreqOsc2);
  wfmod_pulseOsc2.frequency(noteFreqOsc2);
  // octaveControlFilter<N> can now go +/- 7.9998 Octaves A0 is 27.5Hz*2^8 = 7040, and 20000/2^8 = 78.125 ~= D#2, close enough
  // minFilterFreq of 65.4 = C2, 65.4*2^8 = 16742Hz

  clippedFreqFilter1 = limit(keyfollowFilter1 * FreqOscFilter1, maxFilterFreq, minFilterFreq);
  clippedFreqFilter2 = limit(keyfollowFilter2 * FreqOscFilter2, maxFilterFreq, minFilterFreq);
  clippedFreqFilter3 = limit(keyfollowFilter3 * FreqNoiseFilter3, maxFilterFreq, minFilterFreq);
  clippedFreqFilter4 = limit(keyfollowFilter4 * FreqNoiseFilter4, maxFilterFreq, minFilterFreq);
  clippedFreqFilterPreNoise = limit(keyfollowFilterPreNoise * FreqPreNoiseVariableFreq, maxFilterFreq, minFilterPreNoiseFreq);
  filter1.frequency(clippedFreqFilter1);
  filter1.resonance(QFactorOscFilter1); // Q factor
  mix_fcModFilter1.gain(0, BreathModOscFilter1 * FreqOscFilter1BModFactor);
  mix_fcModFilter1.gain(3, modOffsetFilter1 * FreqOscFilter1BModFactor); // modOffsetFilter1 = 1 in globals
  dc_modOffsetOscFilter1.amplitude(-BreathModOscFilter1);                // subtract off BM,
  if (LinkOscFilters)
  {
    filter2.frequency(clippedFreqFilter1);
    filter2.resonance(QFactorOscFilter1); // Q factor
    mix_fcModFilter2.gain(0, BreathModOscFilter1 * FreqOscFilter1BModFactor);
    mix_fcModFilter2.gain(3, modOffsetFilter1 * FreqOscFilter1BModFactor);
    dc_modOffsetOscFilter2.amplitude(-BreathModOscFilter1);
  }
  else
  {
    filter2.frequency(clippedFreqFilter2);
    filter2.resonance(QFactorOscFilter2); // Q factor
    mix_fcModFilter2.gain(0, BreathModOscFilter2 * FreqOscFilter2BModFactor);
    mix_fcModFilter2.gain(3, modOffsetFilter2 * FreqOscFilter2BModFactor);
    dc_modOffsetOscFilter2.amplitude(-BreathModOscFilter2);
  }
  filter3.frequency(clippedFreqFilter3);
  filter3.resonance(QFactorNoiseFilter3); // Q factor
  mix_fcModFilter3.gain(0, BreathModNoiseFilter3 * FreqNoiseFilter3BModFactor);
  mix_fcModFilter3.gain(3, modOffsetFilter3 * FreqNoiseFilter3BModFactor);
  dc_modOffsetNoiseFilter3.amplitude(-BreathModNoiseFilter3);
  if (LinkNoiseFilters)
  {
    filter4.frequency(clippedFreqFilter3);
    filter4.resonance(QFactorNoiseFilter3); // Q factor
    mix_fcModFilter4.gain(0, BreathModNoiseFilter3 * FreqNoiseFilter3BModFactor);
    mix_fcModFilter4.gain(3, modOffsetFilter3 * FreqNoiseFilter3BModFactor);
    dc_modOffsetNoiseFilter4.amplitude(-BreathModNoiseFilter3);
  }
  else
  {
    filter4.frequency(clippedFreqFilter4);
    filter4.resonance(QFactorNoiseFilter4); // Q factor
    mix_fcModFilter4.gain(0, BreathModNoiseFilter4 * FreqNoiseFilter4BModFactor);
    mix_fcModFilter4.gain(3, modOffsetFilter4 * FreqNoiseFilter4BModFactor);
    dc_modOffsetNoiseFilter4.amplitude(-BreathModNoiseFilter4);
  }
  filterPreNoise.frequency(clippedFreqFilterPreNoise);
  onepole_PreNoise.frequency(FreqPreNoiseFilter);
  filter_osc1.frequency(noteFreqFilterOsc1); // Freq of osc1
  filter_osc1b.frequency(noteFreqOsc1);      // Freq of osc1
  filter_osc2.frequency(noteFreqFilterOsc2); // Freq of osc1
  filter_osc2b.frequency(noteFreqOsc2);      // Freq of osc1
  // filter5.frequency(noteFreqFilter5); // HP filter post mix_Amp
  filter5.frequency(noteNumberFilter1 + 24.0f); // HP filter post mix_Amp
  // filterPreMixHPL.frequency(noteFreqFilter5);
  // filterPreMixHPR.frequency(noteFreqFilter5);
  filterPreMixHPL.frequency(0.1f); //(62.5f);
  filterPreMixHPR.frequency(0.1f); //(62.5f);
  //-----------------------------------------------------------
  //  end of Update Realtime Audio System
  //-----------------------------------------------------------
  AudioInterrupts();

  //-------------
  //  Some debug stuff
  //-------------
  if (0) // debug print
  {
    if (millis() - previousUsageTime >= 5000)
    {
      Serial8.print("Proc = ");
      Serial8.print(AudioProcessorUsage());
      Serial8.print(" (");
      Serial8.print(AudioProcessorUsageMax());
      Serial8.print("),  Mem = ");
      Serial8.print(AudioMemoryUsage());
      Serial8.print(" (");
      Serial8.print(AudioMemoryUsageMax());
      Serial8.println(")");
      previousUsageTime = millis();
      AudioMemoryUsageMaxReset();
      AudioProcessorUsageMaxReset();
    }
  }
  if (millis() - previousDebugPrintTime >= 500)
  {
    if (PRINT_VALUES_FLAG)
    {
      // printPatchValues(); // needs #define USEALL_PATCHFUNCS in patches.h
      //  printCurveMidiData();
      PRINT_VALUES_FLAG = false;
    }
    previousDebugPrintTime = millis();
  }

  //-------------------------------------------------------
  //  Read and process incomming Midi
  //-------------------------------------------------------
  while (MIDIs1.read()) // MIDI DIN
  {
    processMIDI(false); // process midi coming from the MIDIin 5pin DIN
  }
  while (midi_ho.read()) // MIDI HOST
  {
    // process midi coming from the ewi on Host port and mirror to MIDIout 5pin Din
    processMIDI(true);
  }
  // usbMIDI.read() needs to be called rapidly from loop().  When
  // each MIDI messages arrives, it return true.  The message must
  // be fully processed before usbMIDI.read() is called again.
  if (usbMIDI.read()) // MIDI DEVICE
  {
    processUsbMIDI(); // process midi coming from computer
  }

  //------------------------------------------------------
  // update eeprom if any values stored there have changed
  //------------------------------------------------------
  if (!updateEpromFlag && ((current_patchNumber != eeprom_patchNumber) || (eeprom_mix_linein != mix_linein) || (eeprom_FineTuneCents != FineTuneCents) || (eeprom_vol != vol) || (eeprom_Transpose != Transpose) || (eeprom_Octave != Octave) || (eeprom_breath_cc != breath_cc) || (eeprom_fxSourcePatch != fxSourcePatch) || (comp_params[CCCOMPATTACK] != eeprom_comp_params[CCCOMPATTACK]) || (comp_params[CCCOMPRELEASE] != eeprom_comp_params[CCCOMPRELEASE]) || (comp_params[CCCOMPRATIO] != eeprom_comp_params[CCCOMPRATIO]) || (comp_params[CCCOMPTHRESHOLD] != eeprom_comp_params[CCCOMPTHRESHOLD]) || (comp_params[CCCOMPKNEE] != eeprom_comp_params[CCCOMPKNEE]) || (comp_params[CCCOMPMAKEUPGAIN] != eeprom_comp_params[CCCOMPMAKEUPGAIN]) || (NNBModCal != eeprom_NNBModCal)))
  {
    updateEpromFlag = true;
    eepromCurrentMillis = millis();
    eepromPreviousMillis = eepromCurrentMillis;
  }
  if (updateEpromFlag)
  {
    eepromCurrentMillis = millis();
    if (eepromCurrentMillis - eepromPreviousMillis >= eepromUpdateInterval)
    {
      eepromPreviousMillis = eepromCurrentMillis;
      eeprom_patchNumber = current_patchNumber;
      sprintf(str_buf1, "Writing currentPatchNumber (%03d) to EEPROM", eeprom_patchNumber);
      Serial8.println(str_buf1);
      EEPROM_update(PATCH_NUMBER_EEPROM_ADDR, eeprom_patchNumber);
      eeprom_vol = vol;
      sprintf(str_buf1, "Writing vol (%03d) to EEPROM", eeprom_vol);
      Serial8.println(str_buf1);
      EEPROM_update(VOL_EEPROM_ADDR, eeprom_vol);
      eeprom_mix_linein = mix_linein;
      sprintf(str_buf1, "Writing mix_linein (%03d) to EEPROM", eeprom_mix_linein);
      Serial8.println(str_buf1);
      EEPROM_update(MIX_LINEIN_EEPROM_ADDR, eeprom_mix_linein);
      eeprom_fxSourcePatch = fxSourcePatch; // one if Patch, zero if global
      sprintf(str_buf1, "Writing fxSourcePatch (%03d) to EEPROM", eeprom_fxSourcePatch);
      Serial8.println(str_buf1);
      EEPROM_update(FXSOURCE_EEPROM_ADDR, eeprom_fxSourcePatch);
      eeprom_FineTuneCents = FineTuneCents;
      sprintf(str_buf1, "Writing FineTuneCents (%03d) to EEPROM", eeprom_FineTuneCents);
      Serial8.println(str_buf1);
      EEPROM_update(FINE_TUNE_CENTS_EEPROM_ADDR, eeprom_FineTuneCents);
      eeprom_Transpose = Transpose;
      sprintf(str_buf1, "Writing Transpose (%03d) to EEPROM", eeprom_Transpose);
      Serial8.println(str_buf1);
      EEPROM_update(TRANSPOSE_EEPROM_ADDR, eeprom_Transpose);
      eeprom_Octave = Octave;
      sprintf(str_buf1, "Writing Octave (%03d) to EEPROM", eeprom_Octave);
      Serial8.println(str_buf1);
      EEPROM_update(OCTAVE_EEPROM_ADDR, eeprom_Octave);
      sprintf(str_buf1, "Writing breath_cc (%03d) to EEPROM", eeprom_breath_cc);
      eeprom_breath_cc = breath_cc;
      Serial8.println(str_buf1);
      EEPROM_update(BREATH_CC_EEPROM_ADDR, eeprom_breath_cc);
      int size = sizeof(eeprom_comp_params) / sizeof(eeprom_comp_params[0]);
      for (int ii = 0; ii < size; ii++)
      {
        sprintf(str_buf1, "Writing comp_params[%d] (%03d) to EEPROM", ii, eeprom_comp_params[ii]);
        Serial8.println(str_buf1);
        eeprom_comp_params[ii] = (int)comp_params[ii];
        EEPROM_update(COMP_PRAMS_ADDR + ii * sizeof(eeprom_comp_params[0]), eeprom_comp_params[ii]);
      }
      sprintf(str_buf1, "Writing NNBModCal (%03d) to EEPROM", eeprom_NNBModCal);
      Serial8.println(str_buf1);
      eeprom_NNBModCal = NNBModCal;
      EEPROM_update(NNBMODCALPROM_ADDR, eeprom_NNBModCal);
      updateEpromFlag = false;
    }
  } // if (updateEpromFlag )

} // loop()

// to get pullup value other than default 22k
// https://forum.pjrc.com/index.php?threads/teensy-4-option-for-100-kohm-or-47-kohm-pull-up-setting-instead-of-22-kohm-default.70515/
// use IOMUXC_PAD_PUS(2) for 100k pull-up or IOMUXC_PAD_PUS(1) for 47k pull-up, like so:
void pinMode_INPUT_PULLUP_100K(uint8_t pin)
{
  const struct digital_pin_bitband_and_config_table_struct *p;

  if (pin >= CORE_NUM_DIGITAL)
    return;
  p = digital_pin_to_info_PGM + pin;
  *(p->reg + 1) &= ~(p->mask); // TODO: atomic
  *(p->pad) = IOMUXC_PAD_DSE(7) | IOMUXC_PAD_PKE | IOMUXC_PAD_PUE | IOMUXC_PAD_PUS(2) | IOMUXC_PAD_HYS;
  *(p->mux) = 5 | 0x10;
}

void pinMode_INPUT_PULLUP_47K(uint8_t pin)
{
  const struct digital_pin_bitband_and_config_table_struct *p;

  if (pin >= CORE_NUM_DIGITAL)
    return;
  p = digital_pin_to_info_PGM + pin;
  *(p->reg + 1) &= ~(p->mask); // TODO: atomic
  *(p->pad) = IOMUXC_PAD_DSE(7) | IOMUXC_PAD_PKE | IOMUXC_PAD_PUE | IOMUXC_PAD_PUS(1) | IOMUXC_PAD_HYS;
  *(p->mux) = 5 | 0x10;
}

void EEPROM_update(int addr, int val)
{
  int temp_var;
  EEPROM.get(addr, temp_var);
  sprintf(str_buf1, "addr: %d, temp_var: %d, val: %d", addr, temp_var, val);
  Serial8.println(str_buf1);
  if (temp_var != val)
  {
    EEPROM.put(addr, val);
  }
}

void UITimeout(void)
{
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  // TODO: resetUISM();
  ALREADY_TIMED_OUT = true;
}

void resetUITimeout(void)
{
  previousUITimeoutTime = currentUITimeoutTime;
  ALREADY_TIMED_OUT = false;
}

void sgtl_power_down()
{
  // 1. Mute the headphone outputs
  sgtl5000_1.muteHeadphone();
  sgtl5000_1.muteLineout();

  // ws, added to control_sgtl5000.h
  sgtl5000_1.vagrampdown(); // ws, added to control_sgtl5000.h

  u8g2.setFont(u8g2_font_10x20_mf); // lines start at {8,16,24,32,40,48,56,64};
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  u8g2.drawStr(20, 20, "BYE!");
  u8g2.sendBuffer();
  delay(390); // VAG_POWERUP to 0, 200-400ms before HEADPNONE_POWERUP and LINEOUT_POWERUP to 0

  // ws, added to control_sgtl5000.h
  sgtl5000_1.powerdownDACHp(); // DAC_POWERUP, HEADPNONE_POWERUP and LINEOUT_POWERUP to 0

  // ws, added to control_sgtl5000.h
  //  sgtl5000_1.powerdownDAP();
}

void updateUI()
{
  long botPos = botEnc.read() / 4; // reduce resolution
  long topPos = topEnc.read() / 4;
  int16_t botDelta = 0, topDelta = 0;
  if (botPos != 0)
  {
    botDelta = (int16_t)botPos;
    botEnc.write(0); // reset to zero
  }
  if (topPos != 0)
  {
    topDelta = (int16_t)topPos;
    topEnc.write(0); // reset to zero
  }

  knobBotButton.update();
  knobTopButton.update();
  bool botBtn = knobBotButton.rose();
  bool topBtn = knobTopButton.rose();

  if (botDelta || topDelta || botBtn || topBtn)
  {
    ms->pollEncoders(botDelta, topDelta, botBtn, topBtn);
    ms->draw();
  }
}

void goUpOne()
{
  // call the menu system to exit to parent if available
  if (ms)
  {
    ms->exitToParent();
  }
}

void pastePatchToDestination()
{
  // Serial8.println(F("writing patch to destination"));
  // trim_ws(myMenu.str_namingbuf);
  copyCurrentPatchToCopyBuffer();
  memcpy((byte *)copy_buffer_patch.patch_string, (byte *)namebuf,
         sizeof(namebuf) < sizeof(copy_buffer_patch.patch_string) ? sizeof(namebuf) : sizeof(copy_buffer_patch.patch_string));
  saveCoppiedPatchSD(paste_patchNumber);
  current_patchNumber = paste_patchNumber;
  loadPatchSD(paste_patchNumber);
  goUpOne();
}

void swapPatchWithDestination()
{
  // Serial8.println(F("swapping current patch with destination patch"));
  // trim_ws(myMenu.str_namingbuf);
  copyCurrentPatchToCopyBuffer();
  copyLoadedPatchToSwapBuffer(swap_patchNumber);
  saveCoppiedPatchSD(swap_patchNumber);
  saveSwappedPatchSD(current_patchNumber);
  loadPatchSD(swap_patchNumber);
  loadPatchSD(current_patchNumber);
  goUpOne();
}

void saveGlobalFxSD_wrap()
{
  saveGlobalFxSD();
  goUpOne();
}

void namingFinished(const char *s)
{
  // copy trimmed name back to buffer or handle elsewhere
  strncpy(namebuf, s, 20); // TODO: max Name width based on font
  namebuf[20] = 0;
  namingFinishedFlag = true;
}

void namingAborted()
{
  // do nothing special
}

void genSplashScreen()
{
  drawStrNl(0, u8g2.getMaxCharHeight(), str_oledbuf); //"Nl to handle new line
}
void drawStrNl(int x, int y, const char *str)
{
  int bufIdx = 0;
  int fontV = u8g2.getMaxCharHeight();
  // int fontH = u8g2.getMaxCharWidth();
  int dispH = u8g2.getDisplayWidth();
  // int fontD = u8g2.getDescent();
  int fontA = u8g2.getAscent();

  // str_buf using global so we allocate it statically
  for (int i = 0; str[i] != '\0'; i++)
  {
    if (str[i] == '\n')
    {
      str_buf[bufIdx] = '\0';
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, y - fontA, dispH, fontV);
      u8g2.setDrawColor(1);
      u8g2.drawStr(x, y, str_buf);
      y += fontV; // Move to next line
      bufIdx = 0; // reset buffer index
    }
    else
    {
      str_buf[bufIdx++] = str[i];
    }
  }
  // Print any remaining text after the last \n or if no \n was present
  str_buf[bufIdx] = '\0'; // Null-terminatet t
  u8g2.setDrawColor(0);
  u8g2.drawBox(0, y - fontA, dispH, fontV);
  u8g2.setDrawColor(1);
  u8g2.drawStr(x, y, str_buf);
}

// TODO : replace this with all float version
// This function converts linear to expo, using gamma as the exponential amount
// example:  lin_to_log(data2, 127, breath_gamma)/127.0
float lin_to_log(int input, int input_max, float gamma)
{
  float val = (float)(pow((float)input / (float)input_max, (float)gamma));
  val = limit(val, 1.0, 0.0) * (float)input_max;
  if (input > 0.0f)
  {
    val = val + 0.5;
  }
  return val;
}

// input from 0.0 to 127/128
// gamma = (1-input)^2.75*12.8  generates gamma to use for gamma() for BreathCurve (4000s isn't linear)
float gen_osc_gamma(float input)
{
  float x = 1.0 - input;
  float gamma = pow(x, 2.75) * 12.8;
  return gamma;
}

float gen_filter_gamma(float input)
{
  float x = 1.0 - input;
  // float gamma = pow( x, 1.0)*2.0;
  // float gamma = pow( x, 1.0f-x*0.5f)*2.0f;
  float gamma;
  //       if(input >= 64.0f/127.0f){
  //         gamma = 4.0f*pow(x,2.0f)-8.0f*input+4.0f;
  //       } else {
  gamma = pow(x, 1.0) * 2.0;
  //       }
  return gamma;
}

CurveLines gen_filter_curve_lines(float input)
{
  // generate piece-wise linear curve from:
  // loSlope, midIntercept (slope = 1), hiSlope
  CurveLines fcl = {0.0f, 1.0f, 1.0f}; // {midIntercept, loSlope, hiSlope}
  if (input >= 0.5f)
  {
    fcl.midIntercept = 2.0f * input - 1.0f;
    fcl.loSlope = 0.01f * exp(9.21f * input);
    fcl.hiSlope = 63.5f * exp(-8.3f * input);
  }
  else
  {
    fcl.midIntercept = input - 0.5f;
    fcl.loSlope = 0.2f * exp(3.219f * input);
    fcl.hiSlope = 10.0f * exp(-4.6f * input);
  }
  return fcl;
}

CurveLines gen_osc_curve_lines(float input)
{
  // generate piece-wise linear curve from:
  // loSlope, midIntercept (slope = 1), hiSlope
  // CurveLines fcl = {0.0f, 1.0f, 1.0f}; // {midIntercept, loSlope, hiSlope}
  float skewed_input = input >= 0.6f ? 5.0f / 4.0f * (input - 1.0f) + 1.0f : 5.0f / 6.0f * input;
  return gen_filter_curve_lines(skewed_input);
}

// CurveLines gen_noise_curve_lines(float input) {
//     float skewed_input = input >= 0.6 ? 5.0f/4.0f*(input-1.0f)+1.0f : 5.0f/6.0f*input;
//     return gen_filter_curve_lines(skewed_input);
// }

float piecewise_curve_func(float x, CurveLines fcl)
{
  // m*x+b for lo mid and high
  // if ( fcl.midIntercept == 0.0f ) { return x;}
  float loLine = x * fcl.loSlope;
  float midLine = x + fcl.midIntercept;
  float hiLine = fcl.hiSlope * (x - 1.0f) + 1.0f;
  if (fcl.midIntercept > 0.0f)
  {
    return min(min(loLine, midLine), hiLine);
  }
  else
  {
    return max(max(loLine, midLine), hiLine);
  }
}

float log_pot_curve(float x, float Ymid)
{
  // y = a*b^x - a; where b = (1/Ymid-1)^2, a = 1/(b-1);
  if (Ymid > 0.499f && Ymid < 0.501f)
    return x;
  float b = pow(1.0f / Ymid - 1.0f, 2.0f);
  return (pow(b, x) - 1.0f) / (b - 1.0f);
}

float gen_noise_gamma(float input)
{
  float x = 1.0 - input;
  float gamma = pow(x, 1.5) * 5.0;
  return gamma;
}

float gamma_func(float x, float gamma)
{
  if (x == 0.0f)
    return 0.0f;
  float y = pow(x, gamma);
  return y;
}

float amp_curve(float x)
{
  if (x >= 0.5)
    return x;
  float y = gamma_func(2.0f * x, 1.0f / 3.0f) / 2.0f;
  return y;
}

float limit(float input, float top, float bottom)
{
  return input > top ? top : (input < bottom ? bottom : input);
}

// function to threshold the control signal (x 0.0-1.0, 0.0 <= th <= 1.0)
// and keep it between 0.0 and 1.0 inclusive
float thresh(float x, float th)
{
  return (x > th) ? limit((x - th) / (1.0 - th), 1.0, 0.0) : 0.0;
}

// function to theshold the control signal for lfo for filters
//    dc_breathLfoFilter1_amp = lfoThresh(lastBreathf,LfoThreshOscFilter1,LfoDepthOscFilter1,LfoBreathOscFilter1);
float lfoThresh(float x, float th, float depth, float breath)
{
  if (breath < 0.0f)
  {
    return (x < th) ? limit(breath * (x / th - 1.0 * depth), 1.0, 0.0) : 0.0;
  }
  else
  {
    return (x > th) ? limit(depth + breath * (x - th) / (1.0 - th), 1.0, 0.0) : 0.0;
  }
}

void changeFilterMode(void)
{
  switch (ModeOscFilter1)
  {
  case THRU:
    mix_ntcFilter1.gain(0, maxMixNtcFilter); // pass thru
    mix_ntcFilter1.gain(1, 0.0);             // LP
    mix_ntcFilter1.gain(2, 0.0);             // BP
    mix_ntcFilter1.gain(3, 0.0);             // HP
    break;
  case LP:
    mix_ntcFilter1.gain(0, 0.0);             // pass thru
    mix_ntcFilter1.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter1.gain(2, 0.0);             // BP
    mix_ntcFilter1.gain(3, 0.0);             // HP
    break;
  case BP:
    mix_ntcFilter1.gain(0, 0.0);             // pass thru
    mix_ntcFilter1.gain(1, 0.0);             // LP
    mix_ntcFilter1.gain(2, maxMixNtcFilter); // BP
    mix_ntcFilter1.gain(3, 0.0);             // HP
    break;
  case HP:
    mix_ntcFilter1.gain(0, 0.0);             // pass thru
    mix_ntcFilter1.gain(1, 0.0);             // LP
    mix_ntcFilter1.gain(2, 0.0);             // BP
    mix_ntcFilter1.gain(3, maxMixNtcFilter); // HP
    break;
  case NTC:
    mix_ntcFilter1.gain(0, 0.0);             // pass thru
    mix_ntcFilter1.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter1.gain(2, 0.0);             // BP
    mix_ntcFilter1.gain(3, maxMixNtcFilter); // HP
    break;
  default:
    mix_ntcFilter1.gain(0, 0.0);             // pass thru
    mix_ntcFilter1.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter1.gain(2, 0.0);             // BP
    mix_ntcFilter1.gain(3, 0.0);             // HP
    break;
  }
  filterMode_t link_test = LinkOscFilters ? ModeOscFilter1 : ModeOscFilter2;
  switch (link_test)
  {
  case THRU:
    mix_ntcFilter2.gain(0, maxMixNtcFilter); // pass thru
    mix_ntcFilter2.gain(1, 0.0);             // LP
    mix_ntcFilter2.gain(2, 0.0);             // BP
    mix_ntcFilter2.gain(3, 0.0);             // HP
    break;
  case LP:
    mix_ntcFilter2.gain(0, 0.0);             // pass thru
    mix_ntcFilter2.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter2.gain(2, 0.0);             // BP
    mix_ntcFilter2.gain(3, 0.0);             // HP
    break;
  case BP:
    mix_ntcFilter2.gain(0, 0.0);             // pass thru
    mix_ntcFilter2.gain(1, 0.0);             // LP
    mix_ntcFilter2.gain(2, maxMixNtcFilter); // BP
    mix_ntcFilter2.gain(3, 0.0);             // HP
    break;
  case HP:
    mix_ntcFilter2.gain(0, 0.0);             // pass thru
    mix_ntcFilter2.gain(1, 0.0);             // LP
    mix_ntcFilter2.gain(2, 0.0);             // BP
    mix_ntcFilter2.gain(3, maxMixNtcFilter); // HP
    break;
  case NTC:
    mix_ntcFilter2.gain(0, 0.0);             // pass thru
    mix_ntcFilter2.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter2.gain(2, 0.0);             // BP
    mix_ntcFilter2.gain(3, maxMixNtcFilter); // HP
    break;
  default:
    mix_ntcFilter2.gain(0, 0.0);             // pass thru
    mix_ntcFilter2.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter2.gain(2, 0.0);             // BP
    mix_ntcFilter2.gain(3, 0.0);             // HP
    break;
  }
  switch (ModeNoiseFilter3)
  {
  case THRU:
    mix_ntcFilter3.gain(0, maxMixNtcFilter); // pass thru
    mix_ntcFilter3.gain(1, 0.0);             // LP
    mix_ntcFilter3.gain(2, 0.0);             // BP
    mix_ntcFilter3.gain(3, 0.0);             // HP
    break;
  case LP:
    mix_ntcFilter3.gain(0, 0.0);             // pass thru
    mix_ntcFilter3.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter3.gain(2, 0.0);             // BP
    mix_ntcFilter3.gain(3, 0.0);             // HP
    break;
  case BP:
    mix_ntcFilter3.gain(0, 0.0);             // pass thru
    mix_ntcFilter3.gain(1, 0.0);             // LP
    mix_ntcFilter3.gain(2, maxMixNtcFilter); // BP
    mix_ntcFilter3.gain(3, 0.0);             // HP
    break;
  case HP:
    mix_ntcFilter3.gain(0, 0.0);             // pass thru
    mix_ntcFilter3.gain(1, 0.0);             // LP
    mix_ntcFilter3.gain(2, 0.0);             // BP
    mix_ntcFilter3.gain(3, maxMixNtcFilter); // HP
    break;
  case NTC:
    mix_ntcFilter3.gain(0, 0.0);             // pass thru
    mix_ntcFilter3.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter3.gain(2, 0.0);             // BP
    mix_ntcFilter3.gain(3, maxMixNtcFilter); // HP
    break;
  default:
    mix_ntcFilter3.gain(0, 0.0);             // pass thru
    mix_ntcFilter3.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter3.gain(2, 0.0);             // BP
    mix_ntcFilter3.gain(3, 0.0);             // HP
    break;
  }
  link_test = LinkNoiseFilters ? ModeNoiseFilter3 : ModeNoiseFilter4;
  switch (link_test)
  {
  case THRU:
    mix_ntcFilter4.gain(0, maxMixNtcFilter); // pass thru
    mix_ntcFilter4.gain(1, 0.0);             // LP
    mix_ntcFilter4.gain(2, 0.0);             // BP
    mix_ntcFilter4.gain(3, 0.0);             // HP
    break;
  case LP:
    mix_ntcFilter4.gain(0, 0.0);             // pass thru
    mix_ntcFilter4.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter4.gain(2, 0.0);             // BP
    mix_ntcFilter4.gain(3, 0.0);             // HP
    break;
  case BP:
    mix_ntcFilter4.gain(0, 0.0);             // pass thru
    mix_ntcFilter4.gain(1, 0.0);             // LP
    mix_ntcFilter4.gain(2, maxMixNtcFilter); // BP
    mix_ntcFilter4.gain(3, 0.0);             // HP
    break;
  case HP:
    mix_ntcFilter4.gain(0, 0.0);             // pass thru
    mix_ntcFilter4.gain(1, 0.0);             // LP
    mix_ntcFilter4.gain(2, 0.0);             // BP
    mix_ntcFilter4.gain(3, maxMixNtcFilter); // HP
    break;
  case NTC:
    mix_ntcFilter4.gain(0, 0.0);             // pass thru
    mix_ntcFilter4.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter4.gain(2, 0.0);             // BP
    mix_ntcFilter4.gain(3, maxMixNtcFilter); // HP
    break;
  default:
    mix_ntcFilter4.gain(0, 0.0);             // pass thru
    mix_ntcFilter4.gain(1, maxMixNtcFilter); // LP
    mix_ntcFilter4.gain(2, 0.0);             // BP
    mix_ntcFilter4.gain(3, 0.0);             // HP
    break;
  }
} // changeFilterMode()

//------------------------------------------------------
//  see https://www.pjrc.com/teensy/td_libs_MIDI.html for
//            MIDI using
//------------------------------------------------------
void processMIDI(bool midi_from_host_flag)
{
  // Note: callback method was too slow
  //      see https://www.pjrc.com/teensy/td_midi.html
  if (midi_from_host_flag)
  {
    type = midi_ho.getType();
    data1 = midi_ho.getData1();
    data2 = midi_ho.getData2();
    channel = midi_ho.getChannel();
  }
  else
  {
    type = MIDIs1.getType();
    data1 = MIDIs1.getData1();
    data2 = MIDIs1.getData2();
    channel = MIDIs1.getChannel();
  }
  // const uint8_t *sys = midi_ho.getSysExArray();
  // sprintf(str_buf1, "type: %d, data1: %d, data2: %d, channel: %d", type,data1, data2, channel);
  // Serial8.println(str_buf1);

  switch (type)
  {
  case midi_ho.ProgramChange: // 0xC0
    programChangeData = data1;
    programChangeFlag = true; // used in UISM
    sprintf(str_buf1, "type: %d, data1: %d, channel: %d", type, data1, channel);
    Serial8.println(str_buf1);
    if (midi_from_host_flag)
      MIDIs1.sendProgramChange(data1, channel);
    else 
    {
      midi_ho.sendProgramChange(data1, channel);
    }
    break;
  case midi_ho.AfterTouchPoly: // 0xA0
    if (midi_from_host_flag)
      MIDIs1.sendAfterTouch(data1, data2, channel);
    else 
    {
      midi_ho.sendAfterTouch(data1, data2, channel);
    }
    // MIDIs1.sendPolyPressure(data1,data2,channel); // deprecated, so use the overloaded one
    break;
  case midi_ho.AfterTouchChannel: // 0xD0
    if (midi_from_host_flag)
      MIDIs1.sendAfterTouch(data1, channel); // TODO: add AT as method of breath control
    else
    {
      midi_ho.sendAfterTouch(data1, channel); // TODO: add AT as method of breath control
    }
    break;
  case midi_ho.ControlChange:                // 0xB0
    if (midi_from_host_flag)
      MIDIs1.sendControlChange(data1, data2, channel);
    else
    {
      midi_ho.sendControlChange(data1, data2, channel);
    }
    switch (data1)
    {
    case CC_MODULATION_WHEEL:
    case CC_BREATH:
    case 3:
    case CC_FOOT_PEDAL:
    case CC_VOLUME:
    case CC_BALANCE:
    case 9:
    case CC_PAN:
    case CC_EXPRESSION:
      if (data1 != (uint8_t)breath_cc)
      {
        break;
      }
      else
      {
        data2f = ((float)data2) * DIV127;
        previousBreathf = lastBreathf;
        lastBreathf = data2f;
        dc_breathLfoFilter1_amp = lfoThresh(data2f, LfoThreshOscFilter1, LfoDepthOscFilter1, LfoBreathOscFilter1);
        dc_breathLfoFilter2_amp = lfoThresh(data2f, LfoThreshOscFilter2, LfoDepthOscFilter2, LfoBreathOscFilter2);
        dc_breathLfoFilter3_amp = lfoThresh(data2f, LfoThreshNoiseFilter3, LfoDepthNoiseFilter3, LfoBreathNoiseFilter3);
        dc_breathLfoFilter4_amp = lfoThresh(data2f, LfoThreshNoiseFilter4, LfoDepthNoiseFilter4, LfoBreathNoiseFilter4);
        dc_breathLfoFilter1.amplitude(dc_breathLfoFilter1_amp, dc_breathLfoFilter_rampTime);
        dc_breathLfoFilter2.amplitude(dc_breathLfoFilter2_amp, dc_breathLfoFilter_rampTime);
        dc_breathLfoFilter3.amplitude(dc_breathLfoFilter3_amp, dc_breathLfoFilter_rampTime);
        dc_breathLfoFilter4.amplitude(dc_breathLfoFilter4_amp, dc_breathLfoFilter_rampTime);
        dc_breathOscFilter1_amp = piecewise_curve_func(data2f, BreathOscFiltCurveLines1);
        dc_breathOscFilter2_amp = piecewise_curve_func(data2f, BreathOscFiltCurveLines2);
        dc_breathNoiseFilter3_amp = piecewise_curve_func(data2f, BreathNoiseFiltCurveLines3);
        dc_breathNoiseFilter4_amp = piecewise_curve_func(data2f, BreathNoiseFiltCurveLines4);
        // dc_breathNoise_amp = gamma_func(data2f,NoiseBreathCurve);
        dc_breathNoise_amp = piecewise_curve_func(data2f, NoiseBreathCurveLines);
        if (BreathAttainOsc1 > 0.0f)
          dc_breathSweepOsc1.amplitude(BreathDepthOsc1 * (1.0 - limit(data2f / BreathAttainOsc1, 1.0, -1.0)),
                                       dc_breathSweepOscN_rampTime); // smoothing
        else
          dc_breathSweepOsc1.amplitude(0.0f);
        if (BreathAttainOsc2 > 0.0f)
          dc_breathSweepOsc2.amplitude(BreathDepthOsc2 * (1.0 - limit(data2f / BreathAttainOsc2, 1.0, -1.0)),
                                       dc_breathSweepOscN_rampTime); //  smoothing
        else
          dc_breathSweepOsc2.amplitude(0.0);

        if (note_is_on)
        {
          // dc_breathThreshOsc1_amp = gamma_func(thresh( data2f,BreathThreshOsc1), BreathCurveOsc1);
          // dc_breathThreshOsc2_amp = gamma_func(thresh( data2f,BreathThreshOsc2), BreathCurveOsc2);
          dc_breathThreshOsc1_amp = piecewise_curve_func(thresh(data2f, BreathThreshOsc1), BreathOscCurveLines1);
          // float dynoMax = 35.0f;
          // float dynoIntercept = 75.0f;
          // float rampTimeDynOscOn1 = limit((dc_breathThreshOsc1_amp-lastBreathf)
          //                             *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime);
          dc_breathThreshOsc2_amp = piecewise_curve_func(thresh(data2f, BreathThreshOsc2), BreathOscCurveLines2);
          // float rampTimeDynOscOn2 = limit((dc_breathThreshOsc2_amp-lastBreathf)
          //                             *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime);
          dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp, dc_breathThreshOscN_rampTime);
          dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp, dc_breathThreshOscN_rampTime);
          // dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,rampTimeDynOscOn1);
          // dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,rampTimeDynOscOn2);
          dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp, dc_breathFilterN_rampTime);
          dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp, dc_breathFilterN_rampTime);
          dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp, dc_breathFilterN_rampTime);
          dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp, dc_breathFilterN_rampTime);
          if (NoiseLevel > 0.0f)
          {
            // float dynoMax = 40.0f;
            // float dynoIntercept = 90.0f;
            // float rampTimeDynNoiseOn = limit((dc_breathNoise_amp-lastBreathf)
            //                             *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime);
            dc_breathNoise.amplitude(dc_breathNoise_amp, dc_breathNoise_rampTime);
            // dc_breathNoise.amplitude(dc_breathNoise_amp,rampTimeDynNoiseOn);
          }
        }
        else // note is off
        {
          dc_breathThreshOsc1_amp = 0;
          dc_breathThreshOsc2_amp = 0;
          dc_breathNoise_amp = 0;
          // TODO: make rampOffTime slower for BC higher values
          // float dynoMax = 35.0f;
          // float dynoIntercept = 75.0f;
          // float rampTimeDynOscOn1 = limit((previousBreathf-dc_breathThreshOsc1_amp)
          //                        *dynoIntercept,dynoMax,dc_breathOff_rampTime);
          // float rampTimeDynOscOn2 = limit((previousBreathf-dc_breathThreshOsc1_amp)
          //                        *dynoIntercept,dynoMax,dc_breathOff_rampTime);
          // float rampTimeDynNoiseOn = limit((previousBreathf-dc_breathNoise_amp)
          //                            *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime);
          // dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,rampTimeDynOscOn1);
          // dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,rampTimeDynOscOn2);
          dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp, dc_breathOff_rampTime);
          dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp, dc_breathOff_rampTime);
          dc_breathOscFilter1.amplitude(0, dc_breathOff_rampTime);
          dc_breathOscFilter2.amplitude(0, dc_breathOff_rampTime);
          dc_breathNoiseFilter3.amplitude(0, dc_breathOff_rampTime);
          dc_breathNoiseFilter4.amplitude(0, dc_breathOff_rampTime);
          // dc_breathNoise.amplitude(0,rampTimeDynNoiseOn);
          dc_breathNoise.amplitude(0, dc_breathOff_rampTime);
        }
        break;
      }
    case CC_PORTA_TIME:
      // PortamentoTime MSB
      dc_portatime_val = gamma_func(((float)data2) * DIV127, dc_portatime_gamma);
      if (note_is_on)
      { // update portamento any time it changes and note is on
        portatime_temp = dc_portatime_val * dc_portatime_range * fMidiNoteNorm_diff;
        if (portatime_temp < portatime_min)
          portatime_temp = portatime_min;
        // dc_portatime.amplitude(fMidiNoteNorm, max(6.0f, portatime_temp) );
        dc_portatime.amplitude(fMidiNoteNorm, portatime_temp);
      }
      break;
    }
    break;

  case midi_ho.NoteOn: //(type==0x90)
    if (midi_from_host_flag)
      MIDIs1.sendNoteOn(data1, data2, channel);
    else
    {
      midi_ho.sendNoteOn(data1, data2, channel);
    }
    //  TODO: create amplitude transition between legato notes
    if (data2 == 0)
    {
      noteOffFun();
      break;
    }
    data2f = ((float)data2) * DIV127;
    if (lastBreathf <= 0.0f)
    {
      previousBreathf = 0.0f;
      lastBreathf = data2f;
    }
    //  Only treat Note on Velocity as a Breath value if lastBreathf was zero
    dc_breathThreshOsc1_amp = piecewise_curve_func(thresh(lastBreathf, BreathThreshOsc1), BreathOscCurveLines1);
    dc_breathThreshOsc2_amp = piecewise_curve_func(thresh(lastBreathf, BreathThreshOsc2), BreathOscCurveLines2);
    dc_breathLfoFilter1_amp = lfoThresh(lastBreathf, LfoThreshOscFilter1, LfoDepthOscFilter1, LfoBreathOscFilter1);
    dc_breathLfoFilter2_amp = lfoThresh(lastBreathf, LfoThreshOscFilter2, LfoDepthOscFilter2, LfoBreathOscFilter2);
    dc_breathLfoFilter3_amp = lfoThresh(lastBreathf, LfoThreshNoiseFilter3, LfoDepthNoiseFilter3, LfoBreathNoiseFilter3);
    dc_breathLfoFilter4_amp = lfoThresh(lastBreathf, LfoThreshNoiseFilter4, LfoDepthNoiseFilter4, LfoBreathNoiseFilter4);
    dc_breathLfoFilter1.amplitude(dc_breathLfoFilter1_amp, dc_breathLfoFilter_rampTime);
    dc_breathLfoFilter2.amplitude(dc_breathLfoFilter2_amp, dc_breathLfoFilter_rampTime);
    dc_breathLfoFilter3.amplitude(dc_breathLfoFilter3_amp, dc_breathLfoFilter_rampTime);
    dc_breathLfoFilter4.amplitude(dc_breathLfoFilter4_amp, dc_breathLfoFilter_rampTime);
    dc_breathOscFilter1_amp = piecewise_curve_func(lastBreathf, BreathOscFiltCurveLines1);
    dc_breathOscFilter2_amp = piecewise_curve_func(lastBreathf, BreathOscFiltCurveLines2);
    dc_breathNoiseFilter3_amp = piecewise_curve_func(lastBreathf, BreathNoiseFiltCurveLines3);
    dc_breathNoiseFilter4_amp = piecewise_curve_func(lastBreathf, BreathNoiseFiltCurveLines4);
    dc_breathNoise_amp = piecewise_curve_func(lastBreathf, NoiseBreathCurveLines);
    fMidiNoteNorm = ((float)data1) / 128.0;
    fMidiNoteNorm_diff = abs((float)(data1 - currentMidiNote));
    if (note_is_on) // legato
    {
      // 2nd dc_portatime for filter movement instead of using the one for osc freq for both.
      portatime_temp = dc_portatime_val * dc_portatime_range * fMidiNoteNorm_diff;
      if (portatime_temp < portatime_min)
        portatime_temp = portatime_min;
      dc_portatime.amplitude(fMidiNoteNorm, portatime_temp);
      dc_portatimef.amplitude(fMidiNoteNorm, portatimef_t);
    }
    else // note_is_on==false, so non-legato
    {
      dc_portatime.amplitude(fMidiNoteNorm);
      dc_portatimef.amplitude(fMidiNoteNorm, portatimef_t);
    }
    // note_freq = NOTEFREQS[data1];
    if (!note_is_on || !KeyTriggerSingle)
    {
      // non-legato section uses shorter rampTimnes
      float rampTimeShortening = 0.82;
      float filterRampTimeShortening = 0.25;
      // float dynoMax = 35.0f;
      // float dynoIntercept = 75.0f;
      //  lower values of lastBreath get extended rampTime for breathThreshOsc<N>.amplitiude
      //  (1.0-lastBreathf)*rampTimeExtensionFactor;
      // float rampTimeDynOscN = limit((dc_breathThreshOsc1_amp-lastBreathf)*dynoIntercept,dc_breathThreshOscN_rampTime,dynoMax);
      // float rampTimeDynOsc1 = limit((dc_breathThreshOsc1_amp-lastBreathf)*dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime);
      // float rampTimeDynOsc2 = limit((dc_breathThreshOsc2_amp-lastBreathf)*dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime);
      dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp, dc_breathThreshOscN_rampTime * rampTimeShortening);
      dc_breathThreshOsc2.amplitude(dc_breathThreshOsc1_amp, dc_breathThreshOscN_rampTime * rampTimeShortening);
      // dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,rampTimeDynOsc1);
      // dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,rampTimeDynOsc2);
      dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp, dc_breathFilterN_rampTime * filterRampTimeShortening);
      dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp, dc_breathFilterN_rampTime * filterRampTimeShortening);
      dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp, dc_breathFilterN_rampTime * filterRampTimeShortening);
      dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp, dc_breathFilterN_rampTime * filterRampTimeShortening);
      if (NoiseLevel > 0.0f)
      {
        // float dynoMax = 40.0f;
        // float dynoIntercept = 90.0f;
        // float rampTimeDynNoiseOn = limit((dc_breathNoise_amp-lastBreathf)
        //                           *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime);
        dc_breathNoise.amplitude(dc_breathNoise_amp, dc_breathNoise_rampTime * rampTimeShortening);
        // dc_breathNoise.amplitude(dc_breathNoise_amp,rampTimeDynNoiseOn);
      }
      if (BreathAttainOsc1 > 0.0f)
      {
        dc_breathSweepOsc1.amplitude(BreathDepthOsc1 * (1.0 - limit(lastBreathf / BreathAttainOsc1, 1.0, -1.0)),
                                     dc_breathSweepOscN_rampTime * rampTimeShortening);
      }
      else
      {
        dc_breathSweepOsc1.amplitude(0.0f);
      }
      if (BreathAttainOsc2 > 0.0f)
      {
        dc_breathSweepOsc2.amplitude(BreathDepthOsc2 * (1.0 - limit(lastBreathf / BreathAttainOsc2, 1.0, -1.0)),
                                     dc_breathSweepOscN_rampTime * rampTimeShortening);
      }
      else
      {
        dc_breathSweepOsc2.amplitude(0.0f);
      }

      if (SweepDepthOsc1 != 0.0f)
      {
        // the sign signal now is aplitude as well, so the sq_swpOscN always runs between 1.0 and 0
        dc_sweepDepthOscSign1.amplitude(SweepDepthOsc1);
        dc_sweepDepthOsc1.amplitude(1.0f);
        dc_sweepDepthOsc1.amplitude(0, SweepTimeOsc1);
      }
      if (SweepDepthOsc2 != 0.0f)
      {
        // the sign signal now is aplitude as well, so the sq_swpOscN always runs between 1.0 and 0
        dc_sweepDepthOscSign2.amplitude(SweepDepthOsc2);
        dc_sweepDepthOsc2.amplitude(1.0f);
        dc_sweepDepthOsc2.amplitude(0, SweepTimeOsc2);
      }
      // dc_beatOsc1.amplitude(BeatOsc1); // BeatOsc1 added directly to noteFreqOsc1 instead
      // dc_beatOsc2.amplitude(BeatOsc2); // BeatOsc2 added directly to noteFreqOsc2 instead
      if (NoiseLevel > 0.0f)
      {
        dc_timeNoise.amplitude(TimeNoiseAmp);    // effectively set by pink_Noise.amplitude(NoiseLevel);
        dc_timeNoise.amplitude(0.0f, NoiseTime); // ramp down the noise level
      }
      else
      {
        dc_timeNoise.amplitude(0.0f);
      }
      // the sign signal now is aplitude as well, so the sq_swpfltN always runs between 1.0 and 0
      dc_sweepDepthFilterSign1.amplitude(SweepDepthOscFilter1);
      dc_sweepDepthFilter1.amplitude(1.0f);
      dc_sweepDepthFilter1.amplitude(0.0f, SweepTimeOscFilter1);
      if (LinkOscFilters)
      {
        dc_sweepDepthFilterSign2.amplitude(SweepDepthOscFilter1);
        dc_sweepDepthFilter2.amplitude(1.0f);
        dc_sweepDepthFilter2.amplitude(0.0f, SweepTimeOscFilter1);
      }
      else
      {
        dc_sweepDepthFilterSign2.amplitude(SweepDepthOscFilter2);
        dc_sweepDepthFilter2.amplitude(1.0f);
        dc_sweepDepthFilter2.amplitude(0.0f, SweepTimeOscFilter2);
      }

      dc_sweepDepthFilterSign3.amplitude(SweepDepthNoiseFilter3);
      dc_sweepDepthFilter3.amplitude(1.0f);
      dc_sweepDepthFilter3.amplitude(0.0f, SweepTimeNoiseFilter3);
      if (LinkNoiseFilters)
      {
        dc_sweepDepthFilterSign4.amplitude(SweepDepthNoiseFilter3);
        dc_sweepDepthFilter4.amplitude(1.0f);
        dc_sweepDepthFilter4.amplitude(0.0f, SweepTimeNoiseFilter3);
      }
      else
      {
        dc_sweepDepthFilterSign4.amplitude(SweepDepthNoiseFilter4);
        dc_sweepDepthFilter4.amplitude(1.0f);
        dc_sweepDepthFilter4.amplitude(0.0f, SweepTimeNoiseFilter4);
      }
    }
    else
    {
      // legato section uses regular rampTimes
      dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp, dc_breathThreshOscN_rampTime);
      dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp, dc_breathThreshOscN_rampTime);
      dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp, dc_breathFilterN_rampTime);
      dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp, dc_breathFilterN_rampTime);
      dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp, dc_breathFilterN_rampTime);
      dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp, dc_breathFilterN_rampTime);
      if (NoiseLevel > 0.0f)
      {
        dc_breathNoise.amplitude(dc_breathNoise_amp, dc_breathNoise_rampTime);
      }
      if (BreathAttainOsc1 > 0.0f)
      {
        dc_breathSweepOsc1.amplitude(BreathDepthOsc1 * (1.0 - limit(lastBreathf / BreathAttainOsc1, 1.0, -1.0)),
                                     dc_breathSweepOscN_rampTime);
      }
      else
      {
        dc_breathSweepOsc1.amplitude(0.0f);
      }
      if (BreathAttainOsc2 > 0.0f)
      {
        dc_breathSweepOsc2.amplitude(BreathDepthOsc2 * (1.0 - limit(lastBreathf / BreathAttainOsc2, 1.0, -1.0)),
                                     dc_breathSweepOscN_rampTime);
      }
      else
      {
        dc_breathSweepOsc2.amplitude(0.0f);
      }
    }
    currentMidiNote = data1;
    note_is_on = true;
    break;

  case midi_ho.NoteOff: //(type==0x80)
    if (midi_from_host_flag)
      MIDIs1.sendNoteOff(data1, data2, channel);
    else
    {
      midi_ho.sendNoteOff(data1, data2, channel);
    }
    noteOffFun();
    break;

  case midi_ho.PitchBend:
    if (midi_from_host_flag)
      MIDIs1.sendPitchBend(data1 + data2 * 128 - 8192, channel); // 0 = no bend, -2^13 ot +2^13-1
    else
    {
      midi_ho.sendPitchBend(data1 + data2 * 128 - 8192, channel); // 0 = no bend, -2^13 ot +2^13-1
    }
    dc_pitchbend.amplitude((data1 + data2 * 128.0 - 8192.0) * DIV8192, dc_pitchbend_ramp_rate);
    break;
  default:
    sprintf(str_buf1, "Default! type: %d, data1: %d, channel: %d", type, data1, channel);
    Serial8.println(str_buf1);
    break;

  } // switch (type)
} // processMIDI()

void noteOffFun(void)
{
  if (data1 == currentMidiNote)
  {
    note_is_on = false;
    fMidiNoteNorm = ((float)data1) / 128.0;
    dc_portatime.amplitude(fMidiNoteNorm);                //
    dc_portatimef.amplitude(fMidiNoteNorm, portatimef_t); //

    // in case no breath signal comes after NoteOff
    previousBreathf = lastBreathf;
    lastBreathf = 0.0f;
    dc_breathThreshOsc1_amp = 0;
    dc_breathThreshOsc2_amp = 0;
    dc_breathNoise_amp = 0;

    // float dynoMax = 34.0f;
    // float dynoIntercept = 74.0f;
    // float rampTimeDynOscOff = limit((previousBreathf-dc_breathThreshOsc0_amp) //TODO: (previous breathThresh value - 0)
    //                         *dynoIntercept,dynoMax,dc_breathOff_rampTime);
    // float rampTimeDynNoiseOff = limit((previousBreathf-dc_breathNoise_amp)
    //                         *dynoIntercept,dynoMax,dc_breathOff_rampTime);
    // dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,rampTimeDynOscOff);
    // dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,rampTimeDynOscOff);
    dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp, dc_breathOff_rampTime);
    // dc_breathThreshOscN_rampTime
    dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp, dc_breathOff_rampTime);
    dc_breathOscFilter1.amplitude(0, dc_breathOff_rampTime);
    dc_breathOscFilter2.amplitude(0, dc_breathOff_rampTime);
    dc_breathNoiseFilter3.amplitude(0, dc_breathOff_rampTime);
    dc_breathNoiseFilter4.amplitude(0, dc_breathOff_rampTime);
    // dc_breathNoise.amplitude(0,rampTimeDynNoiseOff);
    dc_breathNoise.amplitude(0, dc_breathOff_rampTime);
  }
} // noteOffFun(void)
