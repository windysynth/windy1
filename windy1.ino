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

#define CC_MODULATION_WHEEL               1
#define CC_BREATH                         2
#define CC_VOLUME                         7
#define CC_EXPRESSION                     11
#define CC_PORTA_TIME                     5
#define CC_NRPN_DATA_ENTRY                6
#define CC_NRPN_LSB                       98
#define CC_NRPN_MSB                       99

//------------some define statements --------------------
// #define FREQQ (440)

#define PATCH_NUMBER_EEPROM_ADDR ( (int)0 )
#define VOL_EEPROM_ADDR ( (int)4 )
#define MIX_LINEIN_EEPROM_ADDR  ( (int)8 )
#define FINE_TUNE_CENTS_EEPROM_ADDR ( (int)12 )
#define TRANSPOSE_EEPROM_ADDR ( (int)16 )
#define OCTAVE_EEPROM_ADDR ( (int)20 )
#define BREATH_CC_EEPROM_ADDR ( (int)24 )

//------------ includes -------------------------------
#include <Bounce2.h>
#include <USBHost_t36.h>
#include <MIDI.h>
#include "patches.h"
#include <EEPROM.h>
//#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//-------- paste below here Auto generated code from Audio System Design Tooll --------
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformDc     dc_sweepDepthFilter3; //xy=102.5,1060
AudioSynthWaveformDc     dc_breathLfoFilter3; //xy=108.49993896484375,1193
AudioSynthWaveformSine   sine_lfoFilter3; //xy=135.49993896484375,1158
AudioSynthWaveformDc     dc_sweepDepthFilterSign3; //xy=251.5,1096
AudioEffectMultiply      sq_swpflt3;     //xy=300.5,1057
AudioSynthWaveformDc     dc_modOffsetNoiseFilter3; //xy=315.49993896484375,1228
AudioEffectMultiply      mult_lfoDepthFilter3; //xy=320.49993896484375,1189
AudioSynthWaveformDc     dc_breathNoiseFilter3; //xy=322.49993896484375,1153
AudioSynthWaveformDc     dc_timeNoise;   //xy=339.333251953125,1003
AudioSynthWaveformDc     dc_portatimef;  //xy=364.333251953125,175
AudioSynthWaveformDc     dc_portatime;   //xy=369.333251953125,142
AudioSynthWaveformDc     dc_pitchbend;   //xy=373.333251953125,109
AudioSynthWaveformDc     dc_breathLfoFilter1; //xy=377.33323669433594,653
AudioSynthWaveformSine   sine_lfoFilter1; //xy=388.33323669433594,627
AudioMixer4              mix_chorus_fb;  //xy=388.33323669433594,1573
AudioSynthWaveformDc     dc_sweepDepthFilter1; //xy=456.33323669433594,527
AudioEffectMultiply      mlt_DepthFilterSign3; //xy=498.5,1062
AudioSynthWaveformDc     dc_breathNoise; //xy=502.333251953125,966
AudioEffectMultiply      sq_timeNoise;   //xy=502.333251953125,1001
AudioSynthNoiseWhite     pink_Noise;     //xy=524.333251953125,916
AudioSynthWaveformSine   sine_lfoOsc1;   //xy=538.333251953125,246
AudioMixer4              mix_dcStub;     //xy=539.333251953125,152
AudioMixer4              mix_fcModFilter3; //xy=535.4999389648438,1178
AudioSynthWaveformDc     dc_pwOsc1;      //xy=540.333251953125,209
AudioSynthWaveformSine   sine_lfoOsc2;   //xy=570.333251953125,431
AudioSynthWaveformDc     dc_modOffsetOscFilter1; //xy=573.3332366943359,682
AudioEffectMultiply      mult_lfoDepthFilter1; //xy=581.3332366943359,649
AudioSynthWaveformDc     dc_sweepDepthFilterSign1; //xy=582.3332366943359,561
AudioSynthWaveformDc     dc_breathOscFilter1; //xy=584.3332366943359,617
AudioSynthWaveformDc     dc_pwOsc2;      //xy=585.333251953125,393
AudioMixer4              mix_chorus_wet; //xy=624.3332366943359,1744
AudioEffectFlange        flange2;        //xy=628.3332366943359,1588
AudioEffectFlange        flange3;        //xy=629.3332366943359,1619
AudioEffectFlange        flange1;        //xy=631.3332366943359,1554
AudioEffectMultiply      sq_swpflt1;     //xy=643.3332366943359,525
AudioSynthWaveformDc     dc_breathLfoFilter4; //xy=680.333251953125,1325.0000610351562
AudioSynthWaveformSine   sine_lfoFilter4; //xy=687.333251953125,1288.0000610351562
AudioMixer4              mix_breathTimeNoise; //xy=692.333251953125,996
AudioSynthWaveformDc     dc_wfoldOsc1;   //xy=703.333251953125,261
AudioMixer4              mix_pwOsc1;     //xy=704.333251953125,209
AudioFilterStateVariable filterPreNoise; //xy=706.333251953125,933
AudioSynthWaveformDc     dc_sweepDepthOsc1; //xy=713.333251953125,85
AudioSynthWaveformDc     dc_breathSweepOsc1; //xy=713.333251953125,116
AudioSynthWaveformDc     dc_breathSweepOsc2; //xy=731.333251953125,332
AudioSynthWaveformDc     dc_sweepDepthOsc2; //xy=734.333251953125,300
AudioSynthWaveformDc     dc_wfoldOsc2;   //xy=737.333251953125,473
AudioSynthWaveformDc     dc_beatOsc1;    //xy=741.333251953125,148
AudioMixer4              mix_pwOsc2;     //xy=742.333251953125,417
AudioSynthWaveformDc     dc_beatOsc2;    //xy=761.333251953125,364
AudioSynthWaveformDc     dc_sweepDepthFilter4; //xy=765.333251953125,1191.0000610351562
AudioMixer4              mix_fcModFilter3_sweep; //xy=782.3333129882812,1093
AudioSynthWaveformDc     dc_sweepDepthFilter2; //xy=808.333251953125,740
AudioMixer4              mix_fcModFilter1; //xy=824.3332366943359,660
AudioEffectMultiply      mlt_DepthFilterSign1; //xy=834.3332366943359,561
AudioSynthWaveformDc     dc_breathLfoFilter2; //xy=843.333251953125,870
AudioSynthWaveformSine   sine_lfoFilter2; //xy=846.333251953125,835
AudioMixer4              mix_chorus_dry; //xy=875.3332366943359,1633
AudioEffectMultiply      multiply2;      //xy=884.333251953125,993
AudioEffectMultiply      mult_lfoDepthFilter4; //xy=883.333251953125,1306.0000610351562
AudioSynthWaveformDc     dc_breathNoiseFilter4; //xy=888.333251953125,1272.0000610351562
AudioSynthWaveformDc     dc_modOffsetNoiseFilter4; //xy=891.333251953125,1373.0000610351562
AudioEffectWaveFolder    wavefolder_pwOsc1; //xy=903.333251953125,225
AudioSynthWaveformDc     dc_sweepDepthFilterSign4; //xy=899.333251953125,1229.0000610351562
AudioMixer4              mix_pitchModOsc1; //xy=935.333251953125,129
AudioEffectMultiply      sq_swpflt4;     //xy=948.333251953125,1193.0000610351562
AudioMixer4              mix_pitchModOsc2; //xy=952.333251953125,332
AudioEffectWaveFolder    wavefolder_pwOsc2; //xy=970.333251953125,403
AudioSynthWaveformDc     dc_sweepDepthFilterSign2; //xy=975.333251953125,777
AudioEffectWaveshaper    waveshape3; //xy=1009,1079.3333740234375
AudioEffectMultiply      sq_swpflt2;     //xy=1024.333251953125,744
AudioEffectMultiply      mult_lfoDepthFilter2; //xy=1038.333251953125,848
AudioSynthWaveformDc     dc_modOffsetOscFilter2; //xy=1038.333251953125,896
AudioSynthWaveformDc     dc_breathOscFilter2; //xy=1042.333251953125,813
AudioMixer4              mix_levelNoise; //xy=1042.333251953125,1012
AudioMixer4              mix_fcModFilter1_sweep; //xy=1072.333236694336,656
AudioSynthWaveformModulated wfmod_triOsc1;  //xy=1140.333251953125,153
AudioSynthWaveformModulated wfmod_sawOsc1;  //xy=1146.333251953125,121
AudioSynthWaveformModulated wfmod_pulseOsc1; //xy=1146.333251953125,184
AudioEffectMultiply      mlt_DepthFilterSign4; //xy=1142.333251953125,1199.0000610351562
AudioMixer4              mix_fcModFilter4; //xy=1143.333251953125,1328.0000610351562
AudioSynthWaveformModulated wfmod_triOsc2;  //xy=1155.333251953125,337
AudioSynthWaveformModulated wfmod_sawOsc2;  //xy=1160.333251953125,306
AudioSynthWaveformModulated wfmod_pulseOsc2; //xy=1168.333251953125,369
AudioFilterStateVariable filter3;        //xy=1195.333251953125,1097
AudioMixer4              mix_delayFeedback; //xy=1220.3331756591797,1630
AudioEffectMultiply      mlt_DepthFilterSign2; //xy=1237.333251953125,759
AudioMixer4              mix_fcModFilter2; //xy=1250.333251953125,872
AudioSynthWaveformDc     dc_breathThreshOsc2; //xy=1285.333251953125,264
AudioSynthWaveformDc     dc_breathThreshOsc1; //xy=1291.333251953125,70
AudioMixer4              mix_osc1;       //xy=1336.333251953125,158
AudioEffectWaveshaper    waveshape1;     //xy=1335.6668090820312,659.6667785644531
AudioMixer4              mix_osc2;       //xy=1359.333251953125,336
AudioMixer4              mix_ntcFilter3; //xy=1360.333251953125,1096
AudioEffectDelay         dly_delayEffects; //xy=1430.3332977294922,1629
AudioMixer4              mix_fcModFilter4_sweep; //xy=1433.333251953125,1211.0000610351562
AudioFilterStateVariable filterPostDelayL; //xy=1477.9999237060547,1503.3333740234375
AudioFilterStateVariable filterPostDelayR; //xy=1478.3332977294922,1755
AudioMixer4              mix_fcModFilter2_sweep; //xy=1497.333251953125,884
AudioMixer4              mix_xfade;      //xy=1505.333251953125,122
AudioFilterStateVariable filter1;        //xy=1553.333251953125,658
AudioEffectWaveshaper    waveshape4; //xy=1589,1136.3333740234375
AudioEffectMultiply      mult_thCurveOsc2; //xy=1640.333251953125,298
AudioMixer4              mix_delayLevelR; //xy=1655.3334197998047,1844
AudioMixer4              mix_delayLevelL; //xy=1661.3332977294922,1430.666748046875
AudioEffectMultiply      mult_thCurveOsc1; //xy=1674.333251953125,177
AudioMixer4              mix_ntcFilter1; //xy=1709.333251953125,652
AudioEffectWaveshaper    waveshape2;  //xy=1734.666748046875,925.3333740234375
AudioFilterStateVariable filter4;        //xy=1799.333251953125,1128
AudioMixer4              mix_preVerb; //xy=1839.3332977294922,1655.666748046875
AudioMixer4              mix_oscLevels;  //xy=1861.333251953125,234
AudioFilterStateVariable filter2;        //xy=1905.333251953125,924
AudioMixer4              mix_ntcFilter4; //xy=1961.333251953125,1116
AudioEffectFreeverbStereo freeverbs1;     //xy=2007.3332977294922,1668
AudioMixer4              mix_ntcFilter2; //xy=2059.333251953125,914
AudioFilterFIR           fir_formant;    //xy=2111.333251953125,978
AudioMixer4              mix_Amp;        //xy=2131.333251953125,1127
AudioMixer4              mix_reverbL;    //xy=2164.333297729492,1598
AudioMixer4              mix_reverbR;    //xy=2167.333297729492,1759
AudioFilterStateVariable filter5;        //xy=2289.333251953125,1137
AudioFilterStateVariable filterPreMixLPL; //xy=2338.333297729492,1599
AudioFilterStateVariable filterPreMixLPR; //xy=2341.333297729492,1760
AudioFilterStateVariable filterPreMixHPL; //xy=2538.333297729492,1606
AudioFilterStateVariable filterPreMixHPR; //xy=2539.333297729492,1761
AudioInputI2S            i2s2;           //xy=2541.333297729492,1670
AudioMixer4              mix_lineInL;    //xy=2731.333297729492,1647
AudioMixer4              mix_lineInR;    //xy=2736.333297729492,1744
AudioOutputI2S           i2s1;           //xy=2900.333297729492,1688

AudioConnection          patchCord1(dc_sweepDepthFilter3, 0, sq_swpflt3, 0);
AudioConnection          patchCord2(dc_sweepDepthFilter3, 0, sq_swpflt3, 1);
AudioConnection          patchCord3(dc_breathLfoFilter3, 0, mult_lfoDepthFilter3, 1);
AudioConnection          patchCord4(sine_lfoFilter3, 0, mult_lfoDepthFilter3, 0);
AudioConnection          patchCord5(dc_sweepDepthFilterSign3, 0, mlt_DepthFilterSign3, 1);
AudioConnection          patchCord6(sq_swpflt3, 0, mlt_DepthFilterSign3, 0);
AudioConnection          patchCord7(dc_modOffsetNoiseFilter3, 0, mix_fcModFilter3, 3);
AudioConnection          patchCord8(mult_lfoDepthFilter3, 0, mix_fcModFilter3, 1);
AudioConnection          patchCord9(dc_breathNoiseFilter3, 0, mix_fcModFilter3, 0);
AudioConnection          patchCord10(dc_timeNoise, 0, sq_timeNoise, 0);
AudioConnection          patchCord11(dc_timeNoise, 0, sq_timeNoise, 1);
AudioConnection          patchCord12(dc_portatimef, 0, mix_dcStub, 3);
AudioConnection          patchCord13(dc_portatime, 0, mix_dcStub, 2);
AudioConnection          patchCord14(dc_pitchbend, 0, mix_dcStub, 1);
AudioConnection          patchCord15(dc_breathLfoFilter1, 0, mult_lfoDepthFilter1, 1);
AudioConnection          patchCord16(sine_lfoFilter1, 0, mult_lfoDepthFilter1, 0);
AudioConnection          patchCord17(mix_chorus_fb, flange1);
AudioConnection          patchCord18(mix_chorus_fb, flange2);
AudioConnection          patchCord19(mix_chorus_fb, flange3);
AudioConnection          patchCord20(dc_sweepDepthFilter1, 0, sq_swpflt1, 0);
AudioConnection          patchCord21(dc_sweepDepthFilter1, 0, sq_swpflt1, 1);
AudioConnection          patchCord22(mlt_DepthFilterSign3, 0, mix_fcModFilter3_sweep, 0);
AudioConnection          patchCord23(dc_breathNoise, 0, mix_breathTimeNoise, 0);
AudioConnection          patchCord24(sq_timeNoise, 0, mix_breathTimeNoise, 1);
AudioConnection          patchCord25(pink_Noise, 0, filterPreNoise, 0);
AudioConnection          patchCord26(sine_lfoOsc1, 0, mix_pwOsc1, 1);
AudioConnection          patchCord27(mix_fcModFilter3, 0, mix_fcModFilter3_sweep, 1);
AudioConnection          patchCord28(dc_pwOsc1, 0, mix_pwOsc1, 0);
AudioConnection          patchCord29(sine_lfoOsc2, 0, mix_pwOsc2, 1);
AudioConnection          patchCord30(dc_modOffsetOscFilter1, 0, mix_fcModFilter1, 3);
AudioConnection          patchCord31(mult_lfoDepthFilter1, 0, mix_fcModFilter1, 1);
AudioConnection          patchCord32(dc_sweepDepthFilterSign1, 0, mlt_DepthFilterSign1, 1);
AudioConnection          patchCord33(dc_breathOscFilter1, 0, mix_fcModFilter1, 0);
AudioConnection          patchCord34(dc_pwOsc2, 0, mix_pwOsc2, 0);
AudioConnection          patchCord35(mix_chorus_wet, 0, mix_chorus_dry, 1);
AudioConnection          patchCord36(mix_chorus_wet, 0, mix_chorus_fb, 1);
AudioConnection          patchCord37(flange2, 0, mix_chorus_wet, 1);
AudioConnection          patchCord38(flange3, 0, mix_chorus_wet, 2);
AudioConnection          patchCord39(flange1, 0, mix_chorus_wet, 0);
AudioConnection          patchCord40(sq_swpflt1, 0, mlt_DepthFilterSign1, 0);
AudioConnection          patchCord41(dc_breathLfoFilter4, 0, mult_lfoDepthFilter4, 1);
AudioConnection          patchCord42(sine_lfoFilter4, 0, mult_lfoDepthFilter4, 0);
AudioConnection          patchCord43(mix_breathTimeNoise, 0, multiply2, 1);
AudioConnection          patchCord44(dc_wfoldOsc1, 0, wavefolder_pwOsc1, 1);
AudioConnection          patchCord45(mix_pwOsc1, 0, wavefolder_pwOsc1, 0);
AudioConnection          patchCord46(filterPreNoise, 2, multiply2, 0);
AudioConnection          patchCord47(dc_sweepDepthOsc1, 0, mix_pitchModOsc1, 0);
AudioConnection          patchCord48(dc_breathSweepOsc1, 0, mix_pitchModOsc1, 1);
AudioConnection          patchCord49(dc_breathSweepOsc2, 0, mix_pitchModOsc2, 1);
AudioConnection          patchCord50(dc_sweepDepthOsc2, 0, mix_pitchModOsc2, 0);
AudioConnection          patchCord51(dc_wfoldOsc2, 0, wavefolder_pwOsc2, 1);
AudioConnection          patchCord52(dc_beatOsc1, 0, mix_pitchModOsc1, 2);
AudioConnection          patchCord53(mix_pwOsc2, 0, wavefolder_pwOsc2, 0);
AudioConnection          patchCord54(dc_beatOsc2, 0, mix_pitchModOsc2, 2);
AudioConnection          patchCord55(dc_sweepDepthFilter4, 0, sq_swpflt4, 0);
AudioConnection          patchCord56(dc_sweepDepthFilter4, 0, sq_swpflt4, 1);
AudioConnection          patchCord57(mix_fcModFilter3_sweep, waveshape3);
AudioConnection          patchCord58(dc_sweepDepthFilter2, 0, sq_swpflt2, 0);
AudioConnection          patchCord59(dc_sweepDepthFilter2, 0, sq_swpflt2, 1);
AudioConnection          patchCord60(mix_fcModFilter1, 0, mix_fcModFilter1_sweep, 1);
AudioConnection          patchCord61(mlt_DepthFilterSign1, 0, mix_fcModFilter1_sweep, 0);
AudioConnection          patchCord62(dc_breathLfoFilter2, 0, mult_lfoDepthFilter2, 1);
AudioConnection          patchCord63(sine_lfoFilter2, 0, mult_lfoDepthFilter2, 0);
AudioConnection          patchCord64(mix_chorus_dry, 0, mix_delayFeedback, 0);
AudioConnection          patchCord65(mix_chorus_dry, 0, mix_delayLevelR, 0);
AudioConnection          patchCord66(mix_chorus_dry, 0, mix_delayLevelL, 0);
AudioConnection          patchCord67(multiply2, 0, mix_levelNoise, 0);
AudioConnection          patchCord68(mult_lfoDepthFilter4, 0, mix_fcModFilter4, 1);
AudioConnection          patchCord69(dc_breathNoiseFilter4, 0, mix_fcModFilter4, 0);
AudioConnection          patchCord70(dc_modOffsetNoiseFilter4, 0, mix_fcModFilter4, 3);
AudioConnection          patchCord71(wavefolder_pwOsc1, 0, wfmod_pulseOsc1, 1);
AudioConnection          patchCord72(dc_sweepDepthFilterSign4, 0, mlt_DepthFilterSign4, 1);
AudioConnection          patchCord73(mix_pitchModOsc1, 0, wfmod_sawOsc1, 0);
AudioConnection          patchCord74(mix_pitchModOsc1, 0, wfmod_triOsc1, 0);
AudioConnection          patchCord75(mix_pitchModOsc1, 0, wfmod_pulseOsc1, 0);
AudioConnection          patchCord76(sq_swpflt4, 0, mlt_DepthFilterSign4, 0);
AudioConnection          patchCord77(mix_pitchModOsc2, 0, wfmod_sawOsc2, 0);
AudioConnection          patchCord78(mix_pitchModOsc2, 0, wfmod_triOsc2, 0);
AudioConnection          patchCord79(mix_pitchModOsc2, 0, wfmod_pulseOsc2, 0);
AudioConnection          patchCord80(wavefolder_pwOsc2, 0, wfmod_pulseOsc2, 1);
AudioConnection          patchCord81(dc_sweepDepthFilterSign2, 0, mlt_DepthFilterSign2, 1);
AudioConnection          patchCord82(waveshape3, 0, filter3, 1);
AudioConnection          patchCord83(sq_swpflt2, 0, mlt_DepthFilterSign2, 0);
AudioConnection          patchCord84(mult_lfoDepthFilter2, 0, mix_fcModFilter2, 1);
AudioConnection          patchCord85(dc_modOffsetOscFilter2, 0, mix_fcModFilter2, 3);
AudioConnection          patchCord86(dc_breathOscFilter2, 0, mix_fcModFilter2, 0);
AudioConnection          patchCord87(mix_levelNoise, 0, filter3, 0);
AudioConnection          patchCord88(mix_levelNoise, 0, mix_ntcFilter3, 0);
AudioConnection          patchCord89(mix_fcModFilter1_sweep, waveshape1);
AudioConnection          patchCord90(wfmod_triOsc1, 0, mix_osc1, 1);
AudioConnection          patchCord91(wfmod_sawOsc1, 0, mix_osc1, 0);
AudioConnection          patchCord92(wfmod_pulseOsc1, 0, mix_osc1, 2);
AudioConnection          patchCord93(mlt_DepthFilterSign4, 0, mix_fcModFilter4_sweep, 0);
AudioConnection          patchCord94(mix_fcModFilter4, 0, mix_fcModFilter4_sweep, 1);
AudioConnection          patchCord95(wfmod_triOsc2, 0, mix_osc2, 1);
AudioConnection          patchCord96(wfmod_sawOsc2, 0, mix_osc2, 0);
AudioConnection          patchCord97(wfmod_pulseOsc2, 0, mix_osc2, 2);
AudioConnection          patchCord98(filter3, 0, mix_ntcFilter3, 1);
AudioConnection          patchCord99(filter3, 1, mix_ntcFilter3, 2);
AudioConnection          patchCord100(filter3, 2, mix_ntcFilter3, 3);
AudioConnection          patchCord101(mix_delayFeedback, dly_delayEffects);
AudioConnection          patchCord102(mlt_DepthFilterSign2, 0, mix_fcModFilter2_sweep, 0);
AudioConnection          patchCord103(mix_fcModFilter2, 0, mix_fcModFilter2_sweep, 1);
AudioConnection          patchCord104(dc_breathThreshOsc2, 0, mult_thCurveOsc2, 0);
AudioConnection          patchCord105(dc_breathThreshOsc2, 0, mix_xfade, 1);
AudioConnection          patchCord106(dc_breathThreshOsc1, 0, mix_xfade, 0);
AudioConnection          patchCord107(mix_osc1, 0, mult_thCurveOsc1, 1);
AudioConnection          patchCord108(waveshape1, 0, filter1, 1);
AudioConnection          patchCord109(mix_osc2, 0, mult_thCurveOsc2, 1);
AudioConnection          patchCord110(mix_ntcFilter3, 0, mix_ntcFilter4, 0);
AudioConnection          patchCord111(mix_ntcFilter3, waveshape4);
AudioConnection          patchCord112(dly_delayEffects, 0, filterPostDelayL, 0);
AudioConnection          patchCord113(dly_delayEffects, 1, filterPostDelayR, 0);
AudioConnection          patchCord114(mix_fcModFilter4_sweep, 0, filter4, 1);
AudioConnection          patchCord115(filterPostDelayL, 0, mix_delayLevelL, 1);
AudioConnection          patchCord116(filterPostDelayL, 0, mix_delayFeedback, 1);
AudioConnection          patchCord117(filterPostDelayR, 0, mix_delayLevelR, 1);
AudioConnection          patchCord118(filterPostDelayR, 0, mix_delayFeedback, 2);
AudioConnection          patchCord119(mix_fcModFilter2_sweep, waveshape2);
AudioConnection          patchCord120(mix_xfade, 0, mult_thCurveOsc1, 0);
AudioConnection          patchCord121(filter1, 0, mix_ntcFilter1, 1);
AudioConnection          patchCord122(filter1, 1, mix_ntcFilter1, 2);
AudioConnection          patchCord123(filter1, 2, mix_ntcFilter1, 3);
AudioConnection          patchCord124(waveshape4, 0, filter4, 0);
AudioConnection          patchCord125(mult_thCurveOsc2, 0, mix_oscLevels, 1);
AudioConnection          patchCord126(mix_delayLevelR, 0, mix_reverbR, 0);
AudioConnection          patchCord127(mix_delayLevelR, 0, mix_preVerb, 1);
AudioConnection          patchCord128(mix_delayLevelL, 0, mix_reverbL, 0);
AudioConnection          patchCord129(mix_delayLevelL, 0, mix_preVerb, 0);
AudioConnection          patchCord130(mult_thCurveOsc1, 0, mix_oscLevels, 0);
AudioConnection          patchCord131(mix_ntcFilter1, 0, filter2, 0);
AudioConnection          patchCord132(mix_ntcFilter1, 0, mix_ntcFilter2, 0);
AudioConnection          patchCord133(waveshape2, 0, filter2, 1);
AudioConnection          patchCord134(filter4, 0, mix_ntcFilter4, 1);
AudioConnection          patchCord135(filter4, 1, mix_ntcFilter4, 2);
AudioConnection          patchCord136(filter4, 2, mix_ntcFilter4, 3);
AudioConnection          patchCord137(mix_preVerb, freeverbs1);
AudioConnection          patchCord138(mix_oscLevels, 0, filter1, 0);
AudioConnection          patchCord139(mix_oscLevels, 0, mix_ntcFilter1, 0);
AudioConnection          patchCord140(filter2, 0, mix_ntcFilter2, 1);
AudioConnection          patchCord141(filter2, 1, mix_ntcFilter2, 2);
AudioConnection          patchCord142(filter2, 2, mix_ntcFilter2, 3);
AudioConnection          patchCord143(mix_ntcFilter4, 0, mix_Amp, 1);
AudioConnection          patchCord144(freeverbs1, 0, mix_reverbL, 1);
AudioConnection          patchCord145(freeverbs1, 1, mix_reverbR, 1);
AudioConnection          patchCord146(mix_ntcFilter2, fir_formant);
AudioConnection          patchCord147(fir_formant, 0, mix_Amp, 0);
AudioConnection          patchCord148(mix_Amp, 0, filter5, 0);
AudioConnection          patchCord149(mix_reverbL, 0, filterPreMixLPL, 0);
AudioConnection          patchCord150(mix_reverbR, 0, filterPreMixLPR, 0);
AudioConnection          patchCord151(filter5, 2, mix_chorus_fb, 0);
AudioConnection          patchCord152(filter5, 2, mix_chorus_dry, 0);
AudioConnection          patchCord153(filterPreMixLPL, 0, filterPreMixHPL, 0);
AudioConnection          patchCord154(filterPreMixLPR, 0, filterPreMixHPR, 0);
AudioConnection          patchCord155(filterPreMixHPL, 2, mix_lineInL, 0);
AudioConnection          patchCord156(filterPreMixHPR, 2, mix_lineInR, 0);
AudioConnection          patchCord157(i2s2, 0, mix_lineInL, 1);
AudioConnection          patchCord158(i2s2, 1, mix_lineInR, 1);
AudioConnection          patchCord159(mix_lineInL, 0, i2s1, 0);
AudioConnection          patchCord160(mix_lineInR, 0, i2s1, 1);

AudioControlSGTL5000     sgtl5000_1;     //xy=346.333251953125,219
// GUItool: end automatically generated code


//-------- paste above here Auto generated code from Audio System Design Tooll --------


//-------------- i2c busses for Teensy 4.1 --------------------------
    // i2c busses are Wire, Wire1 and Wire2 for
    // Wire: SCL pin 19, SDA pin 18,  (these are needed for SGTL5000 on Audio Shield)
    // Wire1: SCL1 pin 16, SDA1 pin 17
    // Wire2: SCL2 pin 24, SDA2 pin 25
    // begin.blahblah(IIC_ADDR,&Wire1)  // to use Wire1 pins
//----------------------------------------------------

//------------------- ssd1306_128x32_i2c OLED --------------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     33 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);


//------- Encoder knob, button and UI state machine stuff----------------

Encoder knob(30, 31);
int32_t newKnob = 0;
const int knobButtonPin = 32;
const int patchNextButton = 29;
const uint32_t debounceDelay = 50; // 50 ms
uint32_t patchNextButtonPressedTime = 0;
uint32_t knobButtonPressedTime = 0;
Bounce knobButton(knobButtonPin, debounceDelay);
int currentKnobButtonState = 0;
int oldKnobButtonState = 0;
bool longKnobButtonPressPending = false;
bool longKnobButtonPress = false;
uint32_t longKnobButtonPressTime = 500;  // milliiseconds
bool shortKnobButtonPress = false;
bool firstKnobButtonPress = true;
bool KnobButtonRising = false;
bool submenu_active = false;
const uint32_t UITimeoutInterval = 7000;  // milliseconds
typedef enum {SPLASH_SCREEN, VOL_ADJ, PATCH_SEL, MENU} uism_t;
uism_t UISM = SPLASH_SCREEN;
typedef enum {MENU_EXIT, MENU_MIX, MENU_TUNING, MENU_TRANS, MENU_OCT, MENU_BR,
              MENU_RVB_PATCH, MENU_DLY_PATCH, MENU_CHRS_PATCH, MENU_WR_PATCH_FX } menusm_t;
menusm_t MENUSM = MENU_EXIT;

bool patchFxWriteFlag = false;

//const uint32_t gatherSensorInterval = 50;  // milliseconds
const uint32_t readKnobInterval = 50;  // milliseconds

//-------------- MIDI DIN connection ----------------------
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIs1);



//-------------- USB HOST MIDI Class Compliant --------------------------
USBHost myUSBHost;
//MIDIDevice midi_ho(myUSBHost);
MIDIDevice_BigBuffer midi_ho(myUSBHost);

uint8_t type = 0;
uint8_t data1 = 0;
uint8_t data2 = 0;  
uint8_t channel = 0; 

uint8_t usbMidiNrpnLsbOld = 0;
uint8_t usbMidiNrpnMsbOld = 0;
uint8_t usbMidiNrpnLsbNew = 0;
uint8_t usbMidiNrpnMsbNew = 0;
uint8_t usbMidiNrpnData = 0;

// globals for debugging
String verNum_str = {"0.0.53"};
String verTxt_str = {"version: "}; 
String splashTxt = {"Windy 1\n  ver:\n   "}; 
String version_str = verTxt_str + verNum_str;
String splashScreen_str = {splashTxt + verNum_str};
char str_buf[64] = {0};
char str_buf1[64] = {0};
char str_oledbuf[64] = {0};
bool PRINT_VALUES_FLAG = false;

// globals for loop control
uint32_t previousMillis = 0;
uint32_t currentMillis = 0;  
uint32_t previousUsageTime = 0;
uint32_t previousUITimeoutTime = 0;
uint32_t currentUITimeoutTime = 0;
uint32_t previousDebugPrintTime = 0;
bool ALREADY_TIMED_OUT = false;

// globals for synth control parameters
uint8_t currentMidiNote = 21;  // A0
//int   dc_pitchbend_value;
bool note_is_on = false;
int   dc_pitchbend_ramp_rate        = 6;
float DIV8192 = 1.0/8192.0;
float DIV127 = 1.0/127.0;
float DIV128 = 1.0/128.0;
float DIV64 = 1.0/64.0;
bool updateSynthVariablesFlag = false;

// globals for synth
float dc_breathOscFilter1_amp = 0.0;
float dc_breathOscFilter2_amp = 0.0;
float dc_breathNoiseFilter3_amp = 0.0;
float dc_breathNoiseFilter4_amp = 0.0;
float dc_breathFilterN_rampTime = 4.0;
float dc_breathNoise_amp = 0.0;
float dc_breathNoise_rampTime = 4.0;
float dc_breathThreshOsc1_amp = 0.0;
float dc_breathThreshOsc2_amp = 0.0;
float dc_breathSweepOscN_rampTime = 4.0;
float dc_breathThreshNoise_amp = 0.0;
float dc_breathThreshOscN_rampTime = 4.0;
float dc_breathOff_rampTime = 4.0;
float dc_breathLfoFilter1_amp = 0.0;
float dc_breathLfoFilter2_amp = 0.0;
float dc_breathLfoFilter3_amp = 0.0;
float dc_breathLfoFilter4_amp = 0.0;
float dc_breathLfoFilter_rampTime = 4.0;
float dc_portatime_val = 0.0;
float dc_portatime_range = 2500.0/12.0; // number of ms when _val is 1.0
float dc_portatime_gamma = 4.0;  // TODO: match VL-70m
float noteNumberOsc1 = 69.0;  
float noteFreqOsc1 = 440.0;  
float noteNumberOsc2 = 69.0;  
float noteFreqOsc2 = 440.0;  
float noteFreqFilter5 = 220.0;  
float noteNumberFilter1 = 69.0;  
float noteNumberFilter2 = 69.0;  
float portatimef_t = 6;
float portatime_temp= 0;
float portatime_min= 0;
float fMidiNoteNorm =69.0/128.0;  //440.0;  
float fMidiNoteNorm_diff = 0.0;
float data2f = 0.0;
float data1f = 0.0;
float lastBreathf = 0.0;
int32_t vol = 75;
int eeprom_vol = 75;
float volf = 1.5;
int32_t mix_linein = 0;
int eeprom_mix_linein = 0;
float mix_lineinf = 0.0;
int32_t FineTuneCents = 0;  	// 100 cents per halfstep, FineTune Control +/- 100
int eeprom_FineTuneCents = 0;
float FineTuneCentsf = 0.0;  	
int32_t Transpose = 0;  	// semitones +/- 12 
int eeprom_Transpose = 0;
float Transposef = 0.0;  	
int32_t Octave = 0;  	// octaves  +/- 2 
int eeprom_Octave = 0;
float Octavef = 0.0;  
int32_t breath_cc = 2;  // can be 2 or 11 for cc02 or cc11 (default cc02)
int eeprom_breath_cc = 2;

// synth variable limits and references
float maxPwmLfoFreq = 10.0;         // 4000s is 10 Hz at 100%
float maxPwmDepthOsc = 0.50;        // 4000s is +/- 25% at 100% depth
float maxSweepTimeOsc = 500.0;       // TODO: set this to match 4000s
float sweepTimeOscGamma = 3.50;       // TODO: adjust this to match 4000s
float maxSweepTimeOscFilter = 1000.0; // 255.0;
float sweepTimeOscFilterGamma = 4.0;       // TODO: adjust this to match 4000s
float maxSweepTimeNoiseFilter = 750.0;
float sweepTimeNoiseFilterGamma = 4.0;       // TODO: adjust this to match 4000s
float maxSweepDepthFilter = 1.0;   // 7/8 = 0.875 (because new max filter modulation is 8 octaves) TODO: set this to match 4000s
float maxSweepDepthOscFilter = 0.5;  	//72,9,0,127,

float WAVESHAPER_ARRAY[] = {-16.0/16.0, -15.0/16.0, -14.0/16, -13.0/16.0, -12.0/16.0,-11.0/16.0, 
                            -10.0/16.0, -9.0/16.0, -8.0/16.0, -7.0/16.0, -6.0/16.0, -5.0/16.0, 
                            -4.0/16.0, -3.0/16.0, -2.0/16.0, -1.0/16.0, 0.0, 1.0/16.0, 2.0/16.0, 
                            3.0/16.0, 4.0/16.0, 5.0/16.0, 6.0/16.0, 7.0/16.0, 8.0/16.0, 9.0/16.0, 
                            10.0/16.0, 11.0/16.0, 12.0/16.0, 13.0/16.0, 13.5/16.0, 13.75/16.0, 14.0/16.0};

float maxLfoFreqFilter1 = 100.0;
float maxLfoFreqFilter2 = 100.0;
float maxLfoFreqFilter3 = 100.0;
float maxLfoFreqFilter4 = 100.0;
float maxLfoDepthOscFilter1 = 0.15;  // 4000s goes up and down about 2 octaves (so 0.28 should be right, but seems too much)
float maxLfoDepthOscFilter2 = 0.15;  // 4000s goes up and down about 2 octaves 
float maxLfoDepthNoiseFilter3 = 0.28;  // 4000s goes up and down about 2 octaves 
float maxLfoDepthNoiseFilter4 = 0.28;  // 4000s goes up and down about 2 octaves 
float modOffsetFilter1 = 1;    // -0.25, Hack to start filter at lower frequency
float modOffsetFilter2 = 1;
float modOffsetFilter3 = 1;
float modOffsetFilter4 = 1;
float clippedFreqFilter1 = 1046.5;    // C6
float clippedFreqFilter2 = 1046.5;    // C6
float clippedFreqFilter3 = 1046.5;    // C6
float clippedFreqFilter4 = 1046.5;    // C6
float clippedFreqFilterPreNoise = 600;    // C6
float filterPreNoiseFreqFactor = 4.0;    // C6
float keyfollowFilter1 = 1.0;
float keyfollowFilter2 = 1.0;
float keyfollowFilter3 = 1.0;
float keyfollowFilter4 = 1.0;
//float octaveControlFilter1 = 7.0;
//float octaveControlFilter2 = 7.0;
//float octaveControlFilter3 = 7.0;
//float octaveControlFilter4 = 7.0;
float octaveControlFilter1 = 8.0;
float octaveControlFilter2 = 8.0;
float octaveControlFilter3 = 8.0;
float octaveControlFilter4 = 8.0;

float octaveControlFilter5 = 0.0;
float offsetNoteKeyfollow = 86.0;  // 84 = C6, 72 = C5, 60 = C4
float offsetNoteKeyfollowNoise = 60.0;  // 84 = C6, 72 = C5, 60 = C4
float minPreNoiseNoteNumbr = 60.0;  // 84 = C6, 72 = C5, 60 = C4  4000s noise stops changing below about C4
const uint32_t updatedAudioSystemInterval = 1;  // milliseconds
float freqOscFilterOctaveOffset  = 0.0;    // use 3 to divide FreqOscFilterN by 2^3 to allow modulation to go from -1 to +3/7 
//float fOFOFactor = 1.0; 
float maxFilterFreq = 20000; //12000.0; 
float minFilterFreq = 65.4; // min note number 36
float minFilterPreNoiseFreq = 261.63; // middle C (C4)
float maxDelayDamp = 3000.0; //TODO: find out correct value
float maxDelayLevel = 0.15; //TODO: find out correct value
float gammaDelayLevel = 3.0; //TODO: find out correct value
float gammaDelayFeedback = 1.5; //TODO: find out correct value
float maxTimeNoise = 1000;  // 1000 ms
float TimeNoiseGamma = 4.0;  
float maxNoiseLevel = 0.5; //0.23;  
float minGamma = 0.1;
float maxGamma = 2.0;
float maxReverbLevel = 0.3;
float maxDenseEarly = 0.8;
bool  porta_step = false;    // round to nearest note for portamento = glissando  // Not really in EWI 4k, but I like it :)
float octaveControlOsc1 = 1.0;
float octaveControlOsc2 = 1.0;
float limitBreathSweepOsc1 = 1.55/(octaveControlOsc1*12);  // 4000s is about 1.55 semitiones 
float limitBreathSweepOsc2 = 1.55/(octaveControlOsc2*12); // 4000s is about 1.55 semitiones  
float maxMixNtcFilter = 1.0;  //0.6; 
float fPotValue = 0.0; 
float iPotValue = 0; 
float extraAmpFactor = 3.0;
float extraLineInAmpFactor = 1.0;

// Number of samples in each delay line
// Allocate the delay lines for left and right channels



// current patch nrpn numbers
patch_t current_patch = {
 "DummyPatchNameString32Character",
 {0x40, 0x40, 0x40, 0x40, 0x00, 0x7b, 0x34, 0x00, 0x44, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x7f, 0x00, 0x7f},
 {0x40, 0x40, 0x40, 0x48, 0x00, 0x7f, 0x44, 0x00, 0x64, 0x11, 0x1e, 0x40, 0x00, 0x40, 0x00, 0x7f, 0x00, 0x0a},
 {0x00, 0x7c, 0x05, 0x40, 0x7f, 0x0e, 0x00, 0x40, 0x00, 0x00, 0x10, 0x40},
 {0x00, 0x7c, 0x05, 0x40, 0x7f, 0x01, 0x00, 0x40, 0x00, 0x2c, 0x24, 0x4c},
 {0x00, 0x52, 0x0b, 0x40, 0x6c, 0x4d, 0x00, 0x40, 0x4c, 0x50, 0x28, 0x40},
 {0x00, 0x3f, 0x7f, 0x40, 0x00, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40},
 {0x01, 0x4c, 0x0c},
 {0x00, 0x7f, 0x32},
 {0x02, 0x00, 0x7f, 0x02, 0x02, 0x00, 0x00, 0x01, 0x7f, 0x01},
 {0x7f, 0x7f, 0x40},
 {0x20, 0x3b, 0x6c, 0x48, 0x46, 0x5c, 0x60, 0x0f, 0x7f},
 {0x24, 0x2c, 0x20, 0x44, 0x7f},
 {0x7f, 0x6c, 0x55, 0x16, 0x3a}
};  // 154 bytes long

const uint32_t eepromUpdateInterval = 20000;  // milliseconds
uint32_t eepromPreviousMillis = 0;
uint32_t eepromCurrentMillis = 0;
bool programChangeFlag = false; // new programChange is happening
int programChangeData = 0; // value from midi_ho
bool updateEpromFlag = false;
int current_patchNumber = 0;
int eeprom_patchNumber = 0;
patch_t loadedPatches[NUMBER_OF_PATCHES];
bool patchLoaded[NUMBER_OF_PATCHES] = {0};
File dataFile;
uint8_t sysexPresetSendBuffer[206] = {    // EWI_SYSEX_PRESET_DUMP_LEN
    0xF0, 0x47, 0x64, 0x00, 0x20, 0x00, 0x45, 0x34, 0x6B, 0x09, 0x26, 0x06, 0x53, 0x75, 0x6D, 0x6F,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x40, 0x12, 0x00, 0x40,
    0x40, 0x40, 0x40, 0x00, 0x4E, 0x00, 0x47, 0x27, 0x18, 0x0F, 0x2B, 0x08, 0x41, 0x00, 0x3F, 0x00,
    0x7F, 0x41, 0x12, 0x00, 0x41, 0x47, 0x40, 0x40, 0x00, 0x00, 0x5B, 0x00, 0x59, 0x27, 0x1E, 0x3F,
    0x18, 0x40, 0x00, 0x4D, 0x27, 0x7F, 0x48, 0x0C, 0x00, 0x00, 0x69, 0x16, 0x4A, 0x35, 0x68, 0x00,
    0x40, 0x7D, 0x3C, 0x57, 0x48, 0x49, 0x0C, 0x00, 0x00, 0x7C, 0x0B, 0x40, 0x00, 0x01, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x4A, 0x0C, 0x00, 0x00, 0x6D, 0x0B, 0x49, 0x6C, 0x4D, 0x00, 0x40, 0x4C,
    0x40, 0x0F, 0x40, 0x4B, 0x0C, 0x00, 0x04, 0x24, 0x0B, 0x40, 0x00, 0x01, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x4F, 0x03, 0x00, 0x01, 0x4C, 0x0C, 0x50, 0x03, 0x00, 0x4C, 0x46, 0x00, 0x51, 0x0A,
    0x00, 0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7F, 0x00, 0x58, 0x03, 0x00, 0x42, 0x17,
    0x17, 0x70, 0x09, 0x00, 0x03, 0x3B, 0x7C, 0x11, 0x3C, 0x7F, 0x46, 0x32, 0x7F, 0x71, 0x05, 0x00,
    0x2A, 0x2C, 0x12, 0x45, 0x7F, 0x72, 0x05, 0x00, 0x7F, 0x68, 0x55, 0x18, 0x3A, 0xF7
};

//typedef enum { THRU, LP, BP, HP, NTC } filterMode_t;
typedef enum { LP, HP, BP, NTC, THRU } filterMode_t;

// patch synth variables
float OctOsc1 = 0;      //64,0,62,66,
float SemiOsc1 = 0.0;     //64,1,52,76, -12 to +12
float FineOsc1 = 0.0;  	//64,2,14,114, -50cents/100 to +50cents/100
float BeatOsc1 = 0.0;  	//64,3,0,127, 0 to 1.0
float SawOsc1 = 0.0;  	//64,5,0,127, 0 to 1.0
float TriOsc1 = 1.0;  	//64,6,0,127, 0 to 1.0
float PulseOsc1 = 0.0;  	//64,7,0,127, 0 to 1.0
float PwOffsetOsc1 = 0.0;  	//64,8,0,127, 0 to 1.0
float PwOsc1 = PwOffsetOsc1;  	//64,8,0,127, 0 to 1.0
float PwmFreqOsc1 = 0.0*maxPwmLfoFreq;   	//64,9,0,127,   // 0.0 to 1.0 * maxPwmLfoFreq 
float PwmDepthOsc1 = 0.0;  	//64,10,0,127,
float SweepDepthOsc1 = 0.0;  	//64,11,0,127,
float SweepTimeOsc1 = 0.0;  	//64,12,0,127,
float BreathDepthOsc1 = 0.0;  	//64,13,0,127, <64 start flat, >64 start sharp, -1.0 to +1.0
float BreathAttainOsc1 = 0.0;  	//64,14,0,127,
float BreathCurveOsc1 = 0.7;  	//64,15,0,127,
float BreathThreshOsc1 = 0;  	//64,16,0,127,
float LevelOsc1 = 0;  		//64,17,0,127,
float LevelOscN_HeadRoom = 1.0f/8.0f;  		//64,17,0,127,
float Amp_HeadRoom = 1.0f;  		//64,17,0,127,
float OctOsc2 = 0;  	//65,0,62,66,
float SemiOsc2 = 0;  	//65,1,52,76,
float FineOsc2 = 0;  	//65,2,14,114,
float BeatOsc2 = 0;  	//65,3,0,127,
float SawOsc2 = 1.0;  	//65,5,0,127,
float TriOsc2 = 0;  	//65,6,0,127,
float PulseOsc2 = 0.0;  	//65,7,0,127,
float PwOffsetOsc2 = 0.0;  	//64,8,0,127, 0 to 1.0
float PwOsc2 = PwOffsetOsc2;  	//65,8,0,127,
float PwmFreqOsc2 = 0.0;  	//65,9,0,127,
float PwmDepthOsc2 = 0.0;  	//65,10,0,127,
float SweepDepthOsc2 = 0.0;  	//65,11,0,127,
float SweepTimeOsc2 = 0.0;  	//65,12,0,127,
float BreathDepthOsc2 = 0.0;  	//65,13,0,127,
float BreathAttainOsc2 = 0.0;  	//65,14,0,127,
float BreathCurveOsc2 = 1.2;  	//65,15,0,127, +63, -64, so 1+(x-64)*DIV64
float BreathThreshOsc2 = 0.0;  	//65,16,0,127,
float LevelOsc2 = 0;  	//65,17,0,127,
filterMode_t ModeOscFilter1 = LP;  	//72,0,0,4,//LP HP BP NTC OFF
float FreqOscFilter1  = 1046.5;    // C6  	//72,1,36,124,//Midi Note 36 to 124
float QFactorOscFilter1 = 0.707;  	//72,2,5,127,"// 5=0.5, 127=12.7"
float KeyFollowOscFilter1 = 12;  	//72,3,52,88,// -12 to +24 num semi tones
float BreathModOscFilter1 = 0;  	//72,4,0,127,
float LfoFreqOscFilter1 = 0;  	//72,5,0,127,
float LfoDepthOscFilter1 = 0;  	//72,6,0,127,
float LfoBreathOscFilter1 = 0;  	//72,7,0,127,
float LfoThreshOscFilter1 = 0;  	//72,8,0,127,
float SweepDepthOscFilter1 = 0;  	//72,9,0,127,
float SweepTimeOscFilter1 = 0;  	//72,10,0,127,
float BreathCurveOscFilter1 = 1.0;  	//72,11,0,127,  TODO: hook this up
filterMode_t ModeOscFilter2 = LP;  	//73,0,0,4,//LP HP BP NTC OFF
float FreqOscFilter2 = 1046.5;    // C6  	//73,1,36,124,//Midi Note 36 to 124
float QFactorOscFilter2 = 0.707;  	//73,2,5,127,"// 5=0.5, 127=12.7"
float KeyFollowOscFilter2 = 12;  	//73,3,52,88,// -12 to +24 num semi tones
float BreathModOscFilter2 = 0;  	//73,4,0,127,
float LfoFreqOscFilter2 = 0;  	//73,5,0,127,
float LfoDepthOscFilter2 = 0;  	//73,6,0,127,
float LfoBreathOscFilter2 = 0;  	//73,7,0,127,
float LfoThreshOscFilter2 = 0;  	//73,8,0,127,
float SweepDepthOscFilter2 = 0;  	//73,9,0,127,
float SweepTimeOscFilter2 = 0;  	//73,10,0,127,
float BreathCurveOscFilter2 = 1.0;  	//73,11,0,127, TODO: hook this up
filterMode_t ModeNoiseFilter3 = LP;  	//74,0,0,4,//LP HP BP NTC OFF
float FreqNoiseFilter3 = 1046.5;    // C6  	//74,1,36,124,//Midi Note 36 to 124
float QFactorNoiseFilter3 = 1.0;  	//74,2,5,127,"// 5=0.5, 127=12.7"
float KeyFollowNoiseFilter3 = 12;  	//74,3,52,88,// -12 to +24 num semi tones
float BreathModNoiseFilter3 = 0;  	//74,4,0,127,
float LfoFreqNoiseFilter3 = 0;  	//74,5,0,127,
float LfoDepthNoiseFilter3 = 0;  	//74,6,0,127,
float LfoBreathNoiseFilter3 = 0;  	//74,7,0,127,
float LfoThreshNoiseFilter3 = 0;  	//74,8,0,127,
float SweepDepthNoiseFilter3 = 0;  	//74,9,0,127,
float SweepTimeNoiseFilter3 = 0;  	//74,10,0,127,
float BreathCurveNoiseFilter3 = 1.0;  	//74,11,0,127, TODO: hook this up
filterMode_t ModeNoiseFilter4 = LP;  	//75,0,0,4,//LP HP BP NTC OFF
float FreqNoiseFilter4 = 1046.5;    // C6  	//75,1,36,124,//Midi Note 36 to 124
float QFactorNoiseFilter4 = 2.5;  	//75,2,5,127,"// 5=0.5, 127=12.7"
float QFactorFilter5 = .707;  	//75,2,5,127,"// 5=0.5, 127=12.7"
float KeyFollowNoiseFilter4 = 12;  	//75,3,52,88,// -12 to +24 num semi tones
float KeyFollowFilter5 = 12;  	//75,3,52,88,// -12 to +24 num semi tones
float BreathModNoiseFilter4 = 0;  	//75,4,0,127,
float LfoFreqNoiseFilter4 = 0;  	//75,5,0,127,
float LfoDepthNoiseFilter4 = 0;  	//75,6,0,127,
float LfoBreathNoiseFilter4 = 0;  	//75,7,0,127,
float LfoThreshNoiseFilter4 = 0;  	//75,8,0,127,
float SweepDepthNoiseFilter4 = 0;  	//75,9,0,127,
float SweepTimeNoiseFilter4 = 0;  	//75,10,0,127,
float BreathCurveNoiseFilter4 = 1.0;  	//75,11,0,127, TODO: hook this up
float KeyFollowPreNoiseFilter = 5;  // TODO: match 4000s
float keyfollowFilterPreNoise = 1.0; 
float FreqPreNoiseFilter = 3000.0; // TODO: match 4000s
float NoiseTime = 0.0;  	//80,0,0,127,
float TimeNoiseAmp = 1.0;  	//80,0,0,127,
float NoiseBreathCurve = 1.0;  	//80,1,0,127,
float NoiseLevel = 1.0;  	//80,2,0,127,
float BendRange = 2.0;  	//81,0,0,12,// num semitones
bool BendStep = false;  	//81,1,0,1,//0=off 1=on
float VibratoPitch = 0;  	//81,2,0,127,
bool LinkOscFilters = true;  	//81,3,1,2,//1=Link on 2= Link off  TODO: hook this up
bool LinkNoiseFilters = true;  	//81,4,1,2,                         TODO: hook this up
uint8_t Formant = 0;  	        //81,5,0,2,//0=off 1=W.Wind 2=String
bool XFade = true;  	        //81,6,0,1,//0=off 1=on
bool KeyTriggerSingle = 1;  	    //81,7,0,1,//0=Multi 1=Single
float s81_8 = 127;
bool ChorusOn = 1;
float VibratoAmp = 0;  	    //88,0,0,127,
float AmpLevel = 0.5;  	    //88,1,0,127,
float OctButtonLevel = 0;  	    //88,2,0,127,
float EffectsChorusDelay1 = 0;  //112,0,0,127,
float EffectsChorusMod1 = 0;  	//112,1,0,127,
float EffectsChorusWet1 = 0;  	//112,2,0,127,
float EffectsChorusDelay2 = 0;  //112,3,0,127,
float EffectsChorusMod2 = 0;  	//112,4,0,127,
float EffectsChorusWet2 = 0;  	//112,5,0,127,
float EffectsChorusFeedback = 0;//112,6,0,127,
float EffectsChorusLfoFreq = 0; //112,7,0,127,
float EffectsChorusDryLevel = 0;//112,8,0,127,
float EffectsDelayTimeL = 0;  	//113,0,0,127,
float EffectsDelayTimeR= 0;  	//113,0,0,127,
float EffectsDelayFeedback = 0; //113,1,0,127,
float EffectsDelayDamp = 10000;  	//113,2,0,127,
float EffectsDelayLevel = 0;  	//113,3,0,127,
float EffectsDelaySpare = 1.0;  	//113,3,0,127,
float EffectsReverbSpare = 1.0;  	//114,1,0,127,
float EffectsReverbLevel = 0;  	//114,1,0,127,
float EffectsReverbDenseEarly = 0;  	//114,2,0,127,
float EffectsReverbTime = 0;  	//114,3,10,50,//1.0 to 5.0 sec
float EffectsReverbDamp = 0;  	//114,4,54,74,//-10 to +10
float EffectsChorusFBHeadroom = 0.8; // to reduce levels of chorus fb mixer to keep from clipping
float EffectsChorusDryHeadroom = 0.8; // to reduce levels of chorus dry mixer to keep from clipping

// Number of samples in each delay line
// AUDIO_BLOCK_SAMPLES = 128
// 4k GUI says 127 mS max delay = 44 blocks at 44.1khz but flange cuts delay in half
// 44.1 samples/ms, so 88.2 samples/ms when using flange
#define FLANGE_DELAY_LENGTH (176*AUDIO_BLOCK_SAMPLES)
// Allocate the delay lines for left and right channels
short delayline_flange1[FLANGE_DELAY_LENGTH];
short delayline_flange2[FLANGE_DELAY_LENGTH];
short delayline_flange3[FLANGE_DELAY_LENGTH];

filterMode_t modeFilter1_old = THRU;
filterMode_t modeFilter2_old = THRU;
filterMode_t modeFilter3_old = THRU;
filterMode_t modeFilter4_old = THRU;

void setup() {
    // put your setup code here, to run once:
    
    //init str buffers
    sprintf(str_buf, version_str.c_str());
    sprintf(str_buf1, version_str.c_str());
    sprintf(str_oledbuf, splashScreen_str.c_str());

    pinMode(patchNextButton, INPUT_PULLUP);
    pinMode(knobButtonPin, INPUT_PULLUP);
    Serial8.begin(1000000);
    MIDIs1.begin(MIDI_CHANNEL_OMNI);
    configureSD();
    //delay(10000);
   
    //  
    #ifdef SGTL5000_CHECK_PWRON_DEFAULT // defined in control_sgtl5000.h
    // ws added read_b4_enable() to avoid loud click due to calling enable() when already enabled
    // when coming out of bootloader 
    #define SGTL5000_0030_PWRON_DEFAULT 0x7060 
    unsigned int sgtl_val = sgtl5000_1.read_b4_enable((unsigned int)0x0030);
    if ( SGTL5000_0030_PWRON_DEFAULT == sgtl_val ){  
        sgtl5000_1.enable();
    }
    else{
        String test_str= verNum_str +  "\n sgtl:\n 0x%04X";
        sprintf(str_oledbuf, test_str.c_str(), sgtl_val);
    }
    #else
        sgtl5000_1.enable();
    #endif
    sgtl5000_1.muteHeadphone();
    sgtl5000_1.muteLineout();
    sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
    sgtl5000_1.dacVolumeRamp();
    sgtl5000_1.lineOutLevel(13);  //3.16V p-p
    sgtl5000_1.lineInLevel(0);    // 3.12V p-p
    sgtl5000_1.unmuteLineout();
    sgtl5000_1.unmuteHeadphone();
    sgtl5000_1.volume(0.80);
    
    AudioMemory(540);  //TODO: how much AudioMemory do I need? (delay 2.9ms per block for 1270/2.9 = 438
    AudioNoInterrupts();
    dc_pwOsc1.amplitude(PwOsc1); // default 50% dutycycle
    sine_lfoOsc1.amplitude(PwmDepthOsc1);
    sine_lfoOsc1.frequency(PwmFreqOsc1);
    dc_pwOsc2.amplitude(PwOsc2); // default 50% dutycycle
    dc_wfoldOsc1.amplitude(0.0625*2.0); // this value is just to make wavefolder_pwOsc1 work properly
    dc_wfoldOsc2.amplitude(0.0625*2.0); // this value is just to make wavefolder_pwOsc1 work properly
    sine_lfoOsc2.amplitude(PwmDepthOsc2);
    sine_lfoOsc2.frequency(PwmFreqOsc2);
    //mix_pwOsc1.gain(0,1.0);  // use dc_pwOsc1.ammplitude to adjust
    //mix_pwOsc1.gain(1,maxPwmDepthOsc);  // 
    //mix_pwOsc2.gain(0,1.0);  // use dc_pwOsc1.ammplitude to adjust
    //mix_pwOsc2.gain(1,maxPwmDepthOsc);  //
    mix_pwOsc1.gain(0,1.0*0.5);  // use dc_pwOsc1.ammplitude to adjust.  *0.5 then *2.0 in wavefolder
    mix_pwOsc1.gain(1,maxPwmDepthOsc*0.5);  // 
    mix_pwOsc2.gain(0,1.0*0.5);  // use dc_pwOsc1.ammplitude to adjust
    mix_pwOsc2.gain(1,maxPwmDepthOsc*0.5);  //

    mix_pitchModOsc1.gain(0, 1.0);  // use dc_sweepDepthOsc1 amplitude 
    mix_pitchModOsc1.gain(1, limitBreathSweepOsc1);  // use dc_breathSweepOsc1 amplitude  
    mix_pitchModOsc1.gain(2, 1.0);  // use dc_beatOsc1 amplitude 
    mix_pitchModOsc2.gain(0, 1.0);  // use dc_sweepDepthOsc1 amplitude 
    mix_pitchModOsc2.gain(1, limitBreathSweepOsc2);  // use dc_breathSweepOsc1 amplitude
    mix_pitchModOsc2.gain(2, 1.0);  // use dc_beatOsc1 amplitude
    dc_beatOsc1.amplitude(0.0); // TODO calculate beat number
    dc_beatOsc2.amplitude(0.0); // TODO calculate beat number

    wfmod_sawOsc1.begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
    wfmod_sawOsc1.amplitude(SawOsc1);
    wfmod_sawOsc1.frequency(noteFreqOsc1);
    wfmod_sawOsc1.frequencyModulation(octaveControlOsc1);  // max freq mod +/- 4.0 octaves
    wfmod_triOsc1.begin(WAVEFORM_TRIANGLE_VARIABLE);
    wfmod_triOsc1.amplitude(TriOsc1);
    wfmod_triOsc1.frequency(noteFreqOsc1);
    wfmod_triOsc1.frequencyModulation(octaveControlOsc1);  // max freq mod +/- 4.0 octaves
    wfmod_pulseOsc1.begin(WAVEFORM_BANDLIMIT_PULSE);
    wfmod_pulseOsc1.amplitude(PulseOsc1);
    wfmod_pulseOsc1.frequency(noteFreqOsc1);
    wfmod_pulseOsc1.frequencyModulation(octaveControlOsc1);  // max freq mod +/- 4.0 octaves
    wfmod_sawOsc2.begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
    wfmod_sawOsc2.amplitude(SawOsc2);
    wfmod_sawOsc2.frequency(noteFreqOsc2);
    wfmod_sawOsc2.frequencyModulation(octaveControlOsc2);  // max freq mod +/- 4.0 octaves
    wfmod_triOsc2.begin(WAVEFORM_TRIANGLE_VARIABLE);
    wfmod_triOsc2.amplitude(TriOsc2);
    wfmod_triOsc2.frequency(noteFreqOsc2);
    wfmod_triOsc2.frequencyModulation(octaveControlOsc2);  // max freq mod +/- 4.0 octaves
    wfmod_pulseOsc2.begin(WAVEFORM_BANDLIMIT_PULSE);
    wfmod_pulseOsc2.amplitude(PulseOsc2);
    wfmod_pulseOsc2.frequency(noteFreqOsc2);
    wfmod_pulseOsc2.frequencyModulation(octaveControlOsc2);  // max freq mod +/- 4.0 octaves
    pink_Noise.amplitude(NoiseLevel);  //
    filter1.frequency(FreqOscFilter1); // Freq slider
    filter1.resonance(QFactorOscFilter1);   // Q factor
    filter1.octaveControl(octaveControlFilter1); // sets range of control from mix_fcModFilter1  
    filter2.frequency(FreqOscFilter2); // Freq slider
    filter2.resonance(QFactorOscFilter2);   // Q factor
    filter2.octaveControl(octaveControlFilter2); // sets range of control from mix_fcModFilter2 
    filter3.frequency(FreqNoiseFilter3); // Freq slider
    filter3.resonance(QFactorNoiseFilter3);   // Q factor
    filter3.octaveControl(octaveControlFilter3); // sets range of control from mix_fcModFilter3 
    filter4.frequency(FreqNoiseFilter4); // Freq slider
    filter4.resonance(QFactorNoiseFilter4);   // Q factor
    filter4.octaveControl(octaveControlFilter4); // sets range of control from mix_fcModFilter4 
    filter5.frequency(noteFreqFilter5); // Freq slider
    filter5.resonance(QFactorFilter5);   // Q factor
    filter5.octaveControl(octaveControlFilter5); // sets range of control from mix_fcModFilter4 
    filterPreNoise.frequency(clippedFreqFilterPreNoise);   // highpass pre-filter for noise signal 
    filterPreNoise.resonance(0.707);
    filterPostDelayL.frequency(EffectsDelayDamp);
    filterPostDelayL.resonance(0.707);
    filterPostDelayR.frequency(EffectsDelayDamp);
    filterPostDelayR.resonance(0.707);
    filterPreMixHPL.frequency(200);
    filterPreMixHPL.resonance(0.5);
    filterPreMixHPL.octaveControl(1.0);
    filterPreMixHPR.frequency(200);
    filterPreMixHPR.resonance(0.5);
    filterPreMixHPR.octaveControl(1.0);
    filterPreMixLPL.frequency(20500);  // LP filter
    filterPreMixLPL.resonance(0.707);
    filterPreMixLPL.octaveControl(1.0);
    filterPreMixLPR.frequency(20500);  // LP filter
    filterPreMixLPR.resonance(0.707);
    filterPreMixLPR.octaveControl(1.0);
    //ws_threshOsc1.shape(waveShape_threshOsc1, 3);
    //ws_threshOsc2.shape(waveShape_threshOsc2, 3);

    dc_sweepDepthOsc1.amplitude(0.0);
    dc_sweepDepthOsc2.amplitude(0.0);
    dc_breathLfoFilter1.amplitude(0.0);
    dc_breathLfoFilter2.amplitude(0.0);
    dc_breathLfoFilter3.amplitude(0.0);
    dc_breathLfoFilter4.amplitude(0.0);

    sine_lfoFilter1.amplitude(maxLfoDepthOscFilter1);    //maxLfoDepthOscFilter1
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
        mix_xfade.gain(0, 1.0);  // all Osc1
        mix_xfade.gain(1, 0);    // no Osc2 
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
    cacluateHeadRoom();
    mix_oscLevels.gain(0, LevelOsc1*LevelOscN_HeadRoom);
    mix_oscLevels.gain(1, LevelOsc2*LevelOscN_HeadRoom);
    mix_levelNoise.gain(0, 1.0); // pass thru for now
    //mix_levelNoise.gain(1, 1.0); // Debug path. 
    mix_breathTimeNoise.gain(0, 1.0); // TODO: set balance between breath and Time for noise  
    mix_breathTimeNoise.gain(1, 1.0); // TODO: set balance between breath and Time for nois

    waveshape1.shape(WAVESHAPER_ARRAY,33);
    mix_fcModFilter1.gain(0, 1.0);  
    mix_fcModFilter1.gain(1, 1.0);  
    //mix_fcModFilter1.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter1.gain(2, 0.0);  
    mix_fcModFilter1.gain(3, modOffsetFilter1);  
    mix_fcModFilter1_sweep.gain(0, maxSweepDepthOscFilter);  
    mix_fcModFilter1_sweep.gain(1, 1.0);  
    mix_fcModFilter1_sweep.gain(2, 0.0);  
    mix_fcModFilter1_sweep.gain(3, 0.0);  

    waveshape2.shape(WAVESHAPER_ARRAY,33);
    mix_fcModFilter2.gain(0, 1.0);  
    mix_fcModFilter2.gain(1, 1.0);  
    //mix_fcModFilter2.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter2.gain(2, 0.0);  
    mix_fcModFilter2.gain(3, modOffsetFilter2);  
    mix_fcModFilter2_sweep.gain(0, maxSweepDepthOscFilter);  
    mix_fcModFilter2_sweep.gain(1, 1.0);  
    mix_fcModFilter2_sweep.gain(2, 0.0);  
    mix_fcModFilter2_sweep.gain(3, 0.0);  

    waveshape3.shape(WAVESHAPER_ARRAY,33);
    mix_fcModFilter3.gain(0, 1.0);  
    mix_fcModFilter3.gain(1, 1.0);  
    //mix_fcModFilter3.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter3.gain(2, 0.0);  
    mix_fcModFilter3.gain(3, modOffsetFilter3);  
    mix_fcModFilter3_sweep.gain(0, maxSweepDepthOscFilter);  
    mix_fcModFilter3_sweep.gain(1, 1.0);  
    mix_fcModFilter3_sweep.gain(2, 0.0);  
    mix_fcModFilter3_sweep.gain(3, 0.0);  

    waveshape4.shape(WAVESHAPER_ARRAY,33);
    mix_fcModFilter4.gain(0, 1.0);  
    mix_fcModFilter4.gain(1, 1.0);  
    //mix_fcModFilter4.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter4.gain(2, 0.0);  
    mix_fcModFilter4.gain(3, modOffsetFilter4);  
    mix_fcModFilter4_sweep.gain(0, maxSweepDepthOscFilter);  
    mix_fcModFilter4_sweep.gain(1, 1.0);  
    mix_fcModFilter4_sweep.gain(2, 0.0);  
    mix_fcModFilter4_sweep.gain(3, 0.0);  

    mix_ntcFilter1.gain(0,0.0);  
    mix_ntcFilter1.gain(1,maxMixNtcFilter);  // Default LP
    mix_ntcFilter1.gain(2,0.0);
    mix_ntcFilter1.gain(3,0.0);
    mix_ntcFilter2.gain(0,0.0);  
    mix_ntcFilter2.gain(1,maxMixNtcFilter);  // Default LP
    mix_ntcFilter2.gain(2,0.0);
    mix_ntcFilter2.gain(3,0.0);
    mix_ntcFilter3.gain(0,0.0);  
    mix_ntcFilter3.gain(1,maxMixNtcFilter);  // Default LP
    mix_ntcFilter3.gain(2,0.0);
    mix_ntcFilter3.gain(3,0.0);
    mix_ntcFilter4.gain(0,0.0);  
    mix_ntcFilter4.gain(1,maxMixNtcFilter);  // Default LP
    mix_ntcFilter4.gain(2,0.0);
    mix_ntcFilter4.gain(3,0.0);

    fir_formant.begin(FIR_PASSTHRU, 0);  // TODO: create formant filter

    mix_Amp.gain(0, AmpLevel*Amp_HeadRoom); 
    mix_Amp.gain(1, 1.0);  // 4000s AmpLevel doesn't control Noise Level

    flange1.begin(delayline_flange1,FLANGE_DELAY_LENGTH,
                (int)EffectsChorusDelay1,(int)EffectsChorusMod1,EffectsChorusLfoFreq);
    flange2.begin(delayline_flange2,FLANGE_DELAY_LENGTH,
                (int)EffectsChorusDelay2,(int)EffectsChorusMod2,EffectsChorusLfoFreq);
    flange3.begin(delayline_flange3,FLANGE_DELAY_LENGTH,
                FLANGE_DELAY_PASSTHRU,0,0.0);
    mix_chorus_fb.gain(0,1.0*EffectsChorusFBHeadroom);
    mix_chorus_fb.gain(1,EffectsChorusFeedback*EffectsChorusFBHeadroom);
    mix_chorus_wet.gain(0,EffectsChorusWet1);
    mix_chorus_wet.gain(1,EffectsChorusWet2);
    mix_chorus_wet.gain(2,-0.5*(EffectsChorusWet1+EffectsChorusWet2)); // to cancel feed through of flange1
    if (ChorusOn)
    {
        mix_chorus_dry.gain(0,EffectsChorusDryLevel*EffectsChorusDryHeadroom);
        mix_chorus_dry.gain(1,1.0*EffectsChorusDryHeadroom);
    }
    else
    {
        mix_chorus_dry.gain(0,1.0*EffectsChorusDryHeadroom);
        mix_chorus_dry.gain(1,0.0*EffectsChorusDryHeadroom);
    }


    mix_delayFeedback.gain(0,1.0);
    mix_delayFeedback.gain(1,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
    mix_delayFeedback.gain(2,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
    dly_delayEffects.delay(0, EffectsDelayTimeL);
    EffectsDelayTimeR = EffectsDelayTimeL*0.85f;
    dly_delayEffects.delay(1, EffectsDelayTimeR);
    dly_delayEffects.disable(2);
    dly_delayEffects.disable(3);
    dly_delayEffects.disable(4);
    dly_delayEffects.disable(5);
    dly_delayEffects.disable(6);
    dly_delayEffects.disable(7);

    freeverbs1.roomsize(EffectsReverbDenseEarly);
    freeverbs1.damping(EffectsReverbDamp);
    mix_reverbL.gain(0, 1.0);
    mix_reverbL.gain(1, EffectsReverbLevel);
    mix_reverbR.gain(0, 1.0);
    mix_reverbR.gain(1, EffectsReverbLevel);
//    rvb_reverb.reverbTime(EffectsReverbTime);
    mix_delayLevelL.gain(0,maxDelayLevel);
    mix_delayLevelL.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));
    mix_delayLevelR.gain(0,maxDelayLevel);
    mix_delayLevelR.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));
    mix_preVerb.gain(0,.7);
    mix_preVerb.gain(1,.7); // TODO: set these levels for not clipping (based on similarity of l/r delays

    Serial8.println(str_buf1);
    //sprintf(str_buf1, "AUDIO_BLOCK_SAMPLES: (%03d)", AUDIO_BLOCK_SAMPLES );     
    //Serial8.println(str_buf1);
    delay(200);

    myUSBHost.begin();
 
    loadAllPatches();
    EEPROM.get(PATCH_NUMBER_EEPROM_ADDR, eeprom_patchNumber);
    sprintf(str_buf1, "read eeprom_patchNumber (%03d)", eeprom_patchNumber );     
    Serial8.println(str_buf1);
    if(eeprom_patchNumber < 0 || eeprom_patchNumber >= NUMBER_OF_PATCHES)
    {
        //loadPatchNumberEEPROM not within 0 to NUMBER_OF_PATCHES-1. overwrite with 000. 
        sprintf(str_buf1, "loadPatchNumberEEPROM (%03d) out of range. set to 000.", eeprom_patchNumber ); 
        Serial8.println(str_buf1);
        eeprom_patchNumber = 0;
        EEPROM.put(PATCH_NUMBER_EEPROM_ADDR, eeprom_patchNumber);  // address 0
    }
    
    EEPROM.get(FINE_TUNE_CENTS_EEPROM_ADDR, eeprom_FineTuneCents);
    sprintf(str_buf1, "read eeprom_FineTuneCents (%03d)", eeprom_FineTuneCents );     
    Serial8.println(str_buf1);
    if(eeprom_FineTuneCents < -100 || eeprom_FineTuneCents > 100)
    {
        sprintf(str_buf1, "eeprom_FineTuneCents (%03d) out of range.", eeprom_FineTuneCents );     
        Serial8.println(str_buf1);
        eeprom_FineTuneCents = 0;
        EEPROM.put(FINE_TUNE_CENTS_EEPROM_ADDR,eeprom_FineTuneCents);
    }
    FineTuneCents = eeprom_FineTuneCents; 
    FineTuneCentsf =  ((float)FineTuneCents)/100.0f;

    EEPROM.get(TRANSPOSE_EEPROM_ADDR, eeprom_Transpose);
    sprintf(str_buf1, "read eeprom_Transpose (%03d)", eeprom_Transpose );     
    Serial8.println(str_buf1);
    if(eeprom_Transpose < -12 || eeprom_Transpose > 12)
    {
        sprintf(str_buf1, "eeprom_Transpose (%03d) out of range.", eeprom_Transpose );     
        Serial8.println(str_buf1);
        eeprom_Transpose = 0;
        EEPROM.put(TRANSPOSE_EEPROM_ADDR,eeprom_Transpose);
    }
    Transpose = eeprom_Transpose; 
    Transposef =  ((float)Transpose);

    EEPROM.get(OCTAVE_EEPROM_ADDR, eeprom_Octave);
    sprintf(str_buf1, "read eeprom_Octave (%03d)", eeprom_Octave );     
    Serial8.println(str_buf1);
    if(eeprom_Octave < -2 || eeprom_Octave > 2)
    {
        sprintf(str_buf1, "eeprom_Octave (%03d) out of range.", eeprom_Octave );     
        Serial8.println(str_buf1);
        eeprom_Octave = 0;
        EEPROM.put(OCTAVE_EEPROM_ADDR,eeprom_Octave);
    }
    Octave = eeprom_Octave; 
    Octavef =  ((float)Octave*12.0);

    EEPROM.get(BREATH_CC_EEPROM_ADDR, eeprom_breath_cc);
    sprintf(str_buf1, "read eeprom_breath_cc (%03d)", eeprom_breath_cc );     
    Serial8.println(str_buf1);
    if(eeprom_breath_cc < 1 || eeprom_breath_cc > 11)
    {
        sprintf(str_buf1, "eeprom_breath_cc (%03d) out of range.", eeprom_breath_cc );     
        Serial8.println(str_buf1);
        eeprom_breath_cc = 2; // default cc02 (breath)
        EEPROM.put(BREATH_CC_EEPROM_ADDR,eeprom_breath_cc);
    }

    breath_cc = eeprom_breath_cc;

    EEPROM.get(MIX_LINEIN_EEPROM_ADDR, eeprom_mix_linein);
    sprintf(str_buf1, "read eeprom_mix_linein (%03d)", eeprom_mix_linein );     
    Serial8.println(str_buf1);
    if(eeprom_mix_linein <0 || eeprom_mix_linein > 100)
    {
        eeprom_mix_linein = 0;
        EEPROM.put(MIX_LINEIN_EEPROM_ADDR,eeprom_mix_linein);
    }
    mix_linein = eeprom_mix_linein; 
    mix_lineinf =  ((float)mix_linein)/100.0f;

    EEPROM.get(VOL_EEPROM_ADDR, eeprom_vol);
    sprintf(str_buf1, "read eeprom_vol (%03d)", eeprom_vol );     
    Serial8.println(str_buf1);
    if(eeprom_vol <0 || eeprom_vol > 100)
    {
        eeprom_vol = 0;
        EEPROM.put(VOL_EEPROM_ADDR, eeprom_vol);
    }
    vol = eeprom_vol;
    //volf = ((float)vol)/50.0f;
    volf = ((float)vol)/100.0f;
    volf = (volf*volf)*2.0f;

    //mix_lineInL.gain(0, volf*extraAmpFactor*(1.0-mix_lineinf));
    mix_lineInL.gain(0, volf*extraAmpFactor);
    mix_lineInL.gain(1, volf*extraLineInAmpFactor*mix_lineinf);
    mix_lineInL.gain(2, 0.0);
    mix_lineInL.gain(3, 0.0);
    //mix_lineInR.gain(0, volf*extraAmpFactor*(1.0-mix_lineinf));
    mix_lineInR.gain(0, volf*extraAmpFactor);
    mix_lineInR.gain(1, volf*extraLineInAmpFactor*mix_lineinf);
    mix_lineInR.gain(2, 0.0);
    mix_lineInR.gain(3, 0.0);



    sprintf(str_buf1, "loadPatchNumberEEPROM (%03d) ", eeprom_patchNumber );     
    Serial8.println(str_buf1);
   // sprintf(str_buf1, "current_patchNumber (%03d) ", current_patchNumber );     
   // Serial8.println(str_buf1);

    //if (patchLoaded[eeprom_patchNumber]){
        current_patchNumber = eeprom_patchNumber;
        loadPatchSD(current_patchNumber);
        sprintf(str_buf1, "current_patchNumber (%03d) ", current_patchNumber );     
        Serial8.println(str_buf1);
        //setCurrentPatch(current_patchNumber);
        //patchToSynthVariables(&current_patch);
    //}

    //patchToSynthVariables(&current_patch);
    updateSynthVariablesFlag = true;
    printPatchValues();   

    AudioInterrupts();

    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
    //readPot();
    knob.write(0); // reset knob position to zero

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial8.println("SSD1306 allocation failed");
        //for(;;); // Don't proceed, loop forever
    }
    else
    {
        display.setTextSize(2); // configuration of size of characters 
        display.setTextColor(WHITE);
        display.clearDisplay(); // erase display
        display.display(); // refresh display
        display.setCursor(1,5);
        display.println(str_oledbuf);
        display.display();
        resetUITimeout();
    }


} // setup()

void loop() 
{

    //-------------------------------------------------------
    //   Read Knob and Knob button
    //   TODO: Gather sensor data
    //      capacitive sensors, FSLP, 9-DOF, etc.
    //      only need to gather data every few milliseconds
    //      because we smooth the data like with Midi data
    //-------------------------------------------------------
    // control how often we measure sensors with readKnobInterval
    currentMillis = millis();
    currentUITimeoutTime = currentMillis;
    if (currentMillis - previousMillis >= readKnobInterval)
    {
        previousMillis = currentMillis;
        // print somee stuff for debugging
        // sprintf(str_buf, "%d,%d", fslpPressure, fslpPosition);
        // readPot();
        readKnobAndKnobButton();
    }
    if( programChangeFlag )
    {
        resetUITimeout();
        updateUISM();  // updated User Interface State Machine
    }

    if ( (!ALREADY_TIMED_OUT)&&(currentUITimeoutTime - previousUITimeoutTime >= UITimeoutInterval) )
    {
        UITimeout();
    }
  
    //-------------------------------------------------------
    //  Calculate some parameters
    //  that will be used to update the Audio System 
    //  during AudioNoInterrupts()
    //-------------------------------------------------------
    changeFilterMode(); 

 

    //-------------------------------------------------------
    //  Update SynthVariables if they changed
    //-------------------------------------------------------
    if (updateSynthVariablesFlag)
    {
        if (XFade)  
        {
            mix_xfade.gain(0, 1.0); 
            mix_xfade.gain(1, -2.0);
        }
        else
        {
            mix_xfade.gain(0, 1.0);  // all Osc1
            mix_xfade.gain(1, 0);    // no Osc2 
        }
        cacluateHeadRoom();
        mix_oscLevels.gain(0, LevelOsc1*LevelOscN_HeadRoom);
        mix_oscLevels.gain(1, LevelOsc2*LevelOscN_HeadRoom);
        dc_pwOsc1.amplitude(PwOsc1); // default 50% dutycycle
        sine_lfoOsc1.amplitude(PwmDepthOsc1);
        sine_lfoOsc1.frequency(PwmFreqOsc1);
        dc_pwOsc2.amplitude(PwOsc2); // default 50% dutycycle
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
        wfmod_sawOsc2.amplitude(SawOsc2);
        wfmod_triOsc2.amplitude(TriOsc2);
        wfmod_pulseOsc2.amplitude(PulseOsc2);
        pink_Noise.amplitude(NoiseLevel);  //
        
        // TODO: wrap EffectsChorusDelay1 & 2 at 93ms (44.1*93 = 4101.3) (ewi 4k wraps like this)
        flange1.voices( (int)EffectsChorusDelay1,(int)EffectsChorusMod1,EffectsChorusLfoFreq);
        flange2.voices((int)EffectsChorusDelay2,(int)EffectsChorusMod2,EffectsChorusLfoFreq);
        mix_chorus_fb.gain(0,1.0*EffectsChorusFBHeadroom);
        mix_chorus_fb.gain(1,EffectsChorusFeedback*EffectsChorusFBHeadroom);
        mix_chorus_wet.gain(0,EffectsChorusWet1);
        mix_chorus_wet.gain(1,EffectsChorusWet2);
        mix_chorus_wet.gain(2,-0.5*(EffectsChorusWet1+EffectsChorusWet2)); // to cancel feed through of flange1
        if (ChorusOn)
        {
            mix_chorus_dry.gain(0,EffectsChorusDryLevel*EffectsChorusDryHeadroom);
            mix_chorus_dry.gain(1,1.0*EffectsChorusDryHeadroom);
        }
        else
        {
            mix_chorus_dry.gain(0,1.0*EffectsChorusDryHeadroom);
            mix_chorus_dry.gain(1,0.0*EffectsChorusDryHeadroom);
        }

        freeverbs1.roomsize(EffectsReverbDenseEarly);
        freeverbs1.damping(EffectsReverbDamp);
        filterPostDelayL.frequency(EffectsDelayDamp);
        filterPostDelayR.frequency(EffectsDelayDamp);
        mix_reverbL.gain(0, 1.0);
        mix_reverbL.gain(1, EffectsReverbLevel);
        mix_reverbR.gain(0, 1.0);
        mix_reverbR.gain(1, EffectsReverbLevel);
        mix_Amp.gain(0, AmpLevel*Amp_HeadRoom); //  Amp_HeadRoom is from cacluateHeadRoom() called above
        mix_Amp.gain(1, 1.0);  // 4000s AmpLevel doesn't control Noise Level
        //mix_lineInL.gain(0, volf*extraAmpFactor*(1.0-mix_lineinf));
        mix_lineInL.gain(0, volf*extraAmpFactor);
        mix_lineInL.gain(1, volf*extraLineInAmpFactor*mix_lineinf);
        mix_lineInL.gain(2, 0.0);
        mix_lineInL.gain(3, 0.0);
        //mix_lineInR.gain(0, volf*extraAmpFactor*(1.0-mix_lineinf));
        mix_lineInR.gain(0, volf*extraAmpFactor);
        mix_lineInR.gain(1, volf*extraLineInAmpFactor*mix_lineinf);
        mix_lineInR.gain(2, 0.0);
        mix_lineInR.gain(3, 0.0);

        dly_delayEffects.delay(0, EffectsDelayTimeL);
        EffectsDelayTimeR = EffectsDelayTimeL*0.85f;  //TODO: create separate menu entries for left right disparity
        dly_delayEffects.delay(1, EffectsDelayTimeR);  //TODO: adjust feedback gain lower as L/R times get similar
        mix_delayFeedback.gain(1,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
        mix_delayFeedback.gain(2,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
        mix_delayLevelL.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));       //finer adjustment at low end
        mix_delayLevelR.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));       //finer adjustment at low end
        mix_preVerb.gain(0,.7); // TODO: set these levels for not clipping (based on similarity of l/r delays
        mix_preVerb.gain(1,.7); 
        updateSynthVariablesFlag = false;
        PRINT_VALUES_FLAG = true;
    }

    //-------------------------------------------------------
    //  Update Realtime Audio System
    //      Time to update variables that need to be changed 
    //      all at the same time.  
    //-------------------------------------------------------
    AudioNoInterrupts();
    noteNumberOsc1 = porta_step ? round(dc_portatime.read()*128.0) : dc_portatime.read()*128;
    noteNumberOsc1 = BendStep ? round( noteNumberOsc1 + BendRange * dc_pitchbend.read() )
                                     : noteNumberOsc1 + BendRange * dc_pitchbend.read();
    noteNumberOsc1 = noteNumberOsc1 + OctOsc1*12.0
                    + SemiOsc1+FineOsc1 + FineTuneCentsf + Transposef + Octavef;
    //noteNumberFilter1 = dc_portatimef.read()*128 + OctOsc1*12.0 + SemiOsc1+FineOsc1;
    noteNumberFilter1 = dc_portatimef.read()*128 + SemiOsc1+FineOsc1;
    noteNumberOsc2 = porta_step ? round(dc_portatime.read()*128.0) : dc_portatime.read()*128;
    noteNumberOsc2 = BendStep ? round( noteNumberOsc2 + BendRange * dc_pitchbend.read() )
                                     : noteNumberOsc2 + BendRange * dc_pitchbend.read();
    noteNumberOsc2 = noteNumberOsc2 + OctOsc2*12.0
                    + SemiOsc2+FineOsc2 + FineTuneCentsf + Transposef + Octavef;
    //noteNumberFilter2 = dc_portatimef.read()*128 + OctOsc2*12.0 + SemiOsc2+FineOsc2;
    noteNumberFilter2 = dc_portatimef.read()*128 + SemiOsc2+FineOsc2;
    noteFreqOsc1 = 440.0 * pow(2, (noteNumberOsc1-69.0)/12 );  // 69 is note number for A4=440Hz
    noteFreqOsc2 = 440.0 * pow(2, (noteNumberOsc2-69.0)/12 );  // 69 is note number for A4=440Hz
    noteFreqFilter5 = 440.0 * pow(2, (min(60,min(noteNumberOsc1,noteNumberOsc2))-69.0-12.0)/12 );  // always Oct below noteNumberOsc1 or 2 whichever is lower;  TODO: match 4000s
    //noteFreqFilter5 = 440.0 * pow(2, (noteNumberOsc1-69.0-12.0)/12 );  // always Oct below noteNumberOsc1 or 2 whichever is lower;  TODO: match 4000s
    keyfollowFilter1 = pow(2, (noteNumberFilter1-offsetNoteKeyfollow)*KeyFollowOscFilter1/144.0); //72 is C5   
    keyfollowFilter2 = pow(2, (noteNumberFilter2-offsetNoteKeyfollow)*KeyFollowOscFilter2/144.0); //72 is C5 
    keyfollowFilter3 = pow(2, (noteNumberFilter1-offsetNoteKeyfollow)*KeyFollowNoiseFilter3/144.0); //72 is C5
    keyfollowFilter4 = pow(2, (noteNumberFilter1-offsetNoteKeyfollow)*KeyFollowNoiseFilter4/144.0); //72 is C5 
    keyfollowFilterPreNoise = pow(2, ( (noteNumberFilter1 < minPreNoiseNoteNumbr ? minPreNoiseNoteNumbr : noteNumberFilter1)- offsetNoteKeyfollowNoise )
                                *KeyFollowPreNoiseFilter/144.0); //72 is C5 
    wfmod_sawOsc1.frequency(noteFreqOsc1);
    wfmod_triOsc1.frequency(noteFreqOsc1);
    wfmod_pulseOsc1.frequency(noteFreqOsc1);
    wfmod_sawOsc2.frequency(noteFreqOsc2);
    wfmod_triOsc2.frequency(noteFreqOsc2);
    wfmod_pulseOsc2.frequency(noteFreqOsc2);
    // octaveControlFilter<N> can now go +/- 7.9998 Octaves A0 is 27.5Hz*2^8 = 7040, and 20000/2^8 = 78.125 ~= D#2, close enough
    // minFilterFreq of 65.4 = C2, 65.4*2^8 = 16742Hz
    clippedFreqFilter1 = (keyfollowFilter1*FreqOscFilter1 < minFilterFreq)  
                        ? minFilterFreq : (keyfollowFilter1*FreqOscFilter1 < maxFilterFreq) 
                        ? keyfollowFilter1*FreqOscFilter1 : maxFilterFreq;  
    clippedFreqFilter2 = (keyfollowFilter2*FreqOscFilter2 < minFilterFreq)  
                        ? minFilterFreq : (keyfollowFilter2*FreqOscFilter2 < maxFilterFreq) 
                        ? keyfollowFilter2*FreqOscFilter2 : maxFilterFreq;  
    clippedFreqFilter3 = (keyfollowFilter3*FreqNoiseFilter3 < minFilterFreq)  
                        ? minFilterFreq : (keyfollowFilter3*FreqNoiseFilter3 < maxFilterFreq) 
                        ? keyfollowFilter3*FreqNoiseFilter3 : maxFilterFreq;  
    clippedFreqFilter4 = (keyfollowFilter4*FreqNoiseFilter4 < minFilterFreq)  
                        ? minFilterFreq : (keyfollowFilter4*FreqNoiseFilter4 < maxFilterFreq) 
                        ? keyfollowFilter4*FreqNoiseFilter4 : maxFilterFreq;  
    clippedFreqFilterPreNoise = keyfollowFilterPreNoise*FreqPreNoiseFilter < minFilterPreNoiseFreq // TODO: add offset or factor relative to current note freq
                        ? minFilterPreNoiseFreq : keyfollowFilterPreNoise*FreqPreNoiseFilter < maxFilterFreq
                        ? keyfollowFilterPreNoise*FreqPreNoiseFilter : maxFilterFreq;
    filter1.frequency(clippedFreqFilter1);  
    filter1.resonance(QFactorOscFilter1);   // Q factor
    mix_fcModFilter1.gain(0, BreathModOscFilter1*1.25f); 
    mix_fcModFilter1.gain(3, modOffsetFilter1*1.25f); 
    dc_modOffsetOscFilter1.amplitude(-BreathModOscFilter1);  // subtract off BM, 
    if (LinkOscFilters) 
    {
        filter2.frequency(clippedFreqFilter1);
        filter2.resonance(QFactorOscFilter1);   // Q factor
        mix_fcModFilter2.gain(0,  BreathModOscFilter1*1.25f); 
        mix_fcModFilter2.gain(3, modOffsetFilter1*1.25f); 
        ModeOscFilter2 = ModeOscFilter1;    
        dc_modOffsetOscFilter2.amplitude(-BreathModOscFilter1);
    }
    else
    {
        filter2.frequency(clippedFreqFilter2);
        filter2.resonance(QFactorOscFilter2);   // Q factor
        mix_fcModFilter2.gain(0,  BreathModOscFilter2*1.25f); 
        mix_fcModFilter2.gain(3, modOffsetFilter2*1.25f); 
        dc_modOffsetOscFilter2.amplitude(-BreathModOscFilter2);
    }
    filter3.frequency(clippedFreqFilter3);  
    filter3.resonance(QFactorNoiseFilter3);   // Q factor
    mix_fcModFilter3.gain(0,  BreathModNoiseFilter3*1.0f); 
    mix_fcModFilter3.gain(3, modOffsetFilter3*1.0f); 
    dc_modOffsetNoiseFilter3.amplitude(-BreathModNoiseFilter3);
    if (LinkNoiseFilters)
    {
        filter4.frequency(clippedFreqFilter3);
        filter4.resonance(QFactorNoiseFilter3);   // Q factor
        mix_fcModFilter4.gain(0,  BreathModNoiseFilter3*1.0f); 
        mix_fcModFilter4.gain(3, modOffsetFilter3*1.0f); 
        ModeNoiseFilter4 = ModeNoiseFilter3;    
        dc_modOffsetNoiseFilter4.amplitude(-BreathModNoiseFilter3);
    }
    else
    {
        filter4.frequency(clippedFreqFilter4);
        filter4.resonance(QFactorNoiseFilter4);   // Q factor
        mix_fcModFilter4.gain(0,  BreathModNoiseFilter4*1.0f); 
        mix_fcModFilter4.gain(3, modOffsetFilter4*1.0f); 
        dc_modOffsetNoiseFilter4.amplitude(-BreathModNoiseFilter4);
    }
    filterPreNoise.frequency(clippedFreqFilterPreNoise); 
    filter5.frequency(noteFreqFilter5); // HP filter post mix_Amp
    filterPreMixHPL.frequency(noteFreqFilter5);
    filterPreMixHPR.frequency(noteFreqFilter5);


    //-----------------------------------------------------------
    //  end of Update Realtime Audio System 
    //-----------------------------------------------------------
    AudioInterrupts();


    if(0)   // debug print
    {
        if(millis() - previousUsageTime >= 5000) {
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
    
    if(millis() - previousDebugPrintTime >=500)
    {
        if(PRINT_VALUES_FLAG)
        {
            printPatchValues();   
            //printCurveMidiData();
            PRINT_VALUES_FLAG = false;
        }
        previousDebugPrintTime = millis();
    }


    //-------------------------------------------------------
    //  Read and process incomming Midi 
    //-------------------------------------------------------
    while ( MIDIs1.read() )
    {
        processMIDI(false); // process midi coming from the MIDIin 5pin DIN
    }
   
    while ( midi_ho.read() ) 
    {
        // process midi coming from the ewi on Host port and mirror to MIDIout 5pin Din
        processMIDI(true); 
    }

    // usbMIDI.read() needs to be called rapidly from loop().  When
    // each MIDI messages arrives, it return true.  The message must
    // be fully processed before usbMIDI.read() is called again.
    if (usbMIDI.read()) {
        processUsbMIDI();  // process midi coming from computer
    }
    
    //------------------------------------------------------
    // update eeprom if any values stored there have changed
    //------------------------------------------------------
    if( !updateEpromFlag && ((current_patchNumber != eeprom_patchNumber)
                            ||(eeprom_mix_linein != mix_linein)||(eeprom_FineTuneCents != FineTuneCents)
                            ||(eeprom_vol != vol) || (eeprom_Transpose != Transpose) 
                            || (eeprom_Octave != Octave) || (eeprom_breath_cc != breath_cc)  ))
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
            eeprom_patchNumber = current_patchNumber ;
            sprintf(str_buf1, "Writing currentPatchNumber (%03d) to EEPROM", eeprom_patchNumber );     
            Serial8.println(str_buf1);
            EEPROM_update(PATCH_NUMBER_EEPROM_ADDR, eeprom_patchNumber);  
            eeprom_vol = vol;
            sprintf(str_buf1, "Writing vol (%03d) to EEPROM", eeprom_vol );     
            Serial8.println(str_buf1);
            EEPROM_update(VOL_EEPROM_ADDR, eeprom_vol);  
            eeprom_mix_linein = mix_linein;
            sprintf(str_buf1, "Writing mix_linein (%03d) to EEPROM", eeprom_mix_linein );     
            Serial8.println(str_buf1);
            EEPROM_update(MIX_LINEIN_EEPROM_ADDR, eeprom_mix_linein);  
            eeprom_FineTuneCents = FineTuneCents;
            sprintf(str_buf1, "Writing FineTuneCents (%03d) to EEPROM", eeprom_FineTuneCents );     
            Serial8.println(str_buf1);
            EEPROM_update(FINE_TUNE_CENTS_EEPROM_ADDR, eeprom_FineTuneCents);  
            eeprom_Transpose = Transpose;
            sprintf(str_buf1, "Writing Transpose (%03d) to EEPROM", eeprom_Transpose );     
            Serial8.println(str_buf1);
            EEPROM_update(TRANSPOSE_EEPROM_ADDR, eeprom_Transpose);  
            eeprom_Octave = Octave;
            sprintf(str_buf1, "Writing Octave (%03d) to EEPROM", eeprom_Octave );     
            Serial8.println(str_buf1);
            EEPROM_update(OCTAVE_EEPROM_ADDR, eeprom_Octave);  
            sprintf(str_buf1, "Writing breath_cc (%03d) to EEPROM", eeprom_breath_cc );     
            eeprom_breath_cc = breath_cc;
            Serial8.println(str_buf1);
            EEPROM_update(BREATH_CC_EEPROM_ADDR, eeprom_breath_cc);  
            updateEpromFlag = false;
        }
    }

 
}



//-------------
// HeadRoom Balance based on Q of filter 1 and 2 and filter mode on
//------------
void cacluateHeadRoom(){

/*
    float HR1 = 1.0f;
    float HR2 = 1.0f;

    //LinkOscFilters
    float BW1 = FreqOscFilter1/QFactorOscFilter1;
    float BW2 = FreqOscFilter2/QFactorOscFilter2;
    float HBWs = (BW2+BW1)*0.5f;
    float FreqOscProximity = (HBSs - abs(FreqOscFilter2 - FreqOscFilter2))/HBWs;
    float QCombiningFactor = 1.0f;
    float LevelEstimate = (SawOsc1+TriOsc1+PulseOsc1)*LevelOsc1 + (SawOsc2+TriOsc2+PulseOsc2)*LevelOsc2;
    switch (ModeOscFilter1)
    {
        case LP:
        case BP:
        case HP:
            HR1= QFactorOscFilter1 > 1.0f ? (1.0f - QFactorOscFilter1) : 0.0f;
            QCombiningFactor = FreqOscProximity > 0.0f ? FreqOscProximity : 0.0f;
            break;
        case THRU:
        case NTC:
        default:
            HR1= 0.0f;
            break;
    }

    switch (ModeOscFilter2)
    {
        case LP:
        case BP:
        case HP:
            HR2= QFactorOscFilter2 > 1.0f ? (1.0f - QFactorOscFilter2) : 0.0f;
            break;
        case THRU:
        case NTC:
        default:
            HR2= 0.0f;
            break;
    }
    HR2 = HR2*1.414; // RMS to peak
*/ 
        LevelOscN_HeadRoom = 1.0f/1.0f;
        //extraAmpFactor = 8.0f*(1.0f-LevelEstimate/6.0f*
        extraAmpFactor = 3.0f; 
        Amp_HeadRoom = 1.0f; 
//    }   
}

//TODO: replace this with all float version
//This function converts linear to expo, using gamma as the exponential amount
// example:  lin_to_log(data2, 127, breath_gamma)/127.0
float lin_to_log( int input, int input_max, float gamma) {
    float val = (float) ( pow ((float)input / (float)input_max, (float)gamma));
    val = limit(val,1.0,0.0) * (float)input_max ; 
    if(input>0.0)
    {
       val = val  + 0.5;
    }
  return val;
}

// input from 0.0 to 127/128
// gamma = (1-input)^2.75*12.8  generates gamma to use for gamma() for BreathCurve (4000s isn't linear)
float gen_osc_gamma(float input) {
       float x = 1.0-input; 
       float gamma = pow( x, 2.75)*12.8; 
    return gamma;
}

float gen_filter_gamma(float input) {
       float x = 1.0-input; 
       float gamma = pow( x, 1.0)*2.0; 
    return gamma;
}

float gen_noise_gamma(float input) {
       float x = 1.0-input; 
       float gamma = pow( x, 1.5)*5.0; 
    return gamma;
}

float gamma_func(float x, float gamma) {
    if (x == 0.0)
        return 0.0;
    float y = pow(x, gamma);
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
    return (x > th) ? limit((x-th)/(1.0-th),1.0,0.0) : 0.0;
}

// function to theshold the control signal for lfo for filters
//    dc_breathLfoFilter1_amp = lfoThresh(data2f,LfoThreshOscFilter1,LfoDepthOscFilter1,LfoBreathOscFilter1);
float lfoThresh(float x, float th, float depth, float breath)
{
    if(breath < 0.0)
    {
        return (x < th) ? limit( breath*(x/th - 1.0*depth),1.0,0.0) : 0.0;
    }
    else
    {
        return (x > th) ? limit( depth+breath*(x-th)/(1.0-th),1.0,0.0) : 0.0;
    }
}


void changeFilterMode(void)
{
    if (ModeOscFilter1 !=modeFilter1_old)
    {
        modeFilter1_old = ModeOscFilter1;
        switch (ModeOscFilter1)
        {
            case THRU:
                mix_ntcFilter1.gain(0,maxMixNtcFilter);  // pass thru
                mix_ntcFilter1.gain(1,0.0);  // LP 
                mix_ntcFilter1.gain(2,0.0);  // BP
                mix_ntcFilter1.gain(3,0.0);  // HP break; case LP:
                mix_ntcFilter1.gain(0,0.0);  // pass thru
                mix_ntcFilter1.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter1.gain(2,0.0);  // BP
                mix_ntcFilter1.gain(3,0.0);  // HP
                break;
            case BP:
                mix_ntcFilter1.gain(0,0.0);  // pass thru
                mix_ntcFilter1.gain(1,0.0);  // LP 
                mix_ntcFilter1.gain(2,maxMixNtcFilter);  // BP
                mix_ntcFilter1.gain(3,0.0);  // HP
                break;
            case HP:
                mix_ntcFilter1.gain(0,0.0);  // pass thru
                mix_ntcFilter1.gain(1,0.0);  // LP 
                mix_ntcFilter1.gain(2,0.0);  // BP
                mix_ntcFilter1.gain(3,maxMixNtcFilter);  // HP
                break;
            case NTC:
                mix_ntcFilter1.gain(0,0.0);  // pass thru
                mix_ntcFilter1.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter1.gain(2,0.0);  // BP
                mix_ntcFilter1.gain(3,maxMixNtcFilter);  // HP
                break;
            default:
                mix_ntcFilter1.gain(0,0.0);  // pass thru
                mix_ntcFilter1.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter1.gain(2,0.0);  // BP
                mix_ntcFilter1.gain(3,0.0);  // HP
                break;
        }
    }   
    if (ModeOscFilter2 !=modeFilter2_old)
    {
        modeFilter2_old = ModeOscFilter2;
        switch (ModeOscFilter2)
        {
            case THRU:
                mix_ntcFilter2.gain(0,maxMixNtcFilter);  // pass thru
                mix_ntcFilter2.gain(1,0.0);  // LP 
                mix_ntcFilter2.gain(2,0.0);  // BP
                mix_ntcFilter2.gain(3,0.0);  // HP
                break;
            case LP:
                mix_ntcFilter2.gain(0,0.0);  // pass thru
                mix_ntcFilter2.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter2.gain(2,0.0);  // BP
                mix_ntcFilter2.gain(3,0.0);  // HP
                break;
            case BP:
                mix_ntcFilter2.gain(0,0.0);  // pass thru
                mix_ntcFilter2.gain(1,0.0);  // LP 
                mix_ntcFilter2.gain(2,maxMixNtcFilter);  // BP
                mix_ntcFilter2.gain(3,0.0);  // HP
                break;
            case HP:
                mix_ntcFilter2.gain(0,0.0);  // pass thru
                mix_ntcFilter2.gain(1,0.0);  // LP 
                mix_ntcFilter2.gain(2,0.0);  // BP
                mix_ntcFilter2.gain(3,maxMixNtcFilter);  // HP
                break;
            case NTC:
                mix_ntcFilter2.gain(0,0.0);  // pass thru
                mix_ntcFilter2.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter2.gain(2,0.0);  // BP
                mix_ntcFilter2.gain(3,maxMixNtcFilter);  // HP
                break;
            default:
                mix_ntcFilter2.gain(0,0.0);  // pass thru
                mix_ntcFilter2.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter2.gain(2,0.0);  // BP
                mix_ntcFilter2.gain(3,0.0);  // HP
                break;
        }
    }   
    if (ModeNoiseFilter3 !=modeFilter3_old)
    {
        modeFilter3_old = ModeNoiseFilter3;
        switch (ModeNoiseFilter3)
        {
            case THRU:
                mix_ntcFilter3.gain(0,maxMixNtcFilter);  // pass thru
                mix_ntcFilter3.gain(1,0.0);  // LP 
                mix_ntcFilter3.gain(2,0.0);  // BP
                mix_ntcFilter3.gain(3,0.0);  // HP
                break;
            case LP:
                mix_ntcFilter3.gain(0,0.0);  // pass thru
                mix_ntcFilter3.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter3.gain(2,0.0);  // BP
                mix_ntcFilter3.gain(3,0.0);  // HP
                break;
            case BP:
                mix_ntcFilter3.gain(0,0.0);  // pass thru
                mix_ntcFilter3.gain(1,0.0);  // LP 
                mix_ntcFilter3.gain(2,maxMixNtcFilter);  // BP
                mix_ntcFilter3.gain(3,0.0);  // HP
                break;
            case HP:
                mix_ntcFilter3.gain(0,0.0);  // pass thru
                mix_ntcFilter3.gain(1,0.0);  // LP 
                mix_ntcFilter3.gain(2,0.0);  // BP
                mix_ntcFilter3.gain(3,maxMixNtcFilter);  // HP
                break;
            case NTC:
                mix_ntcFilter3.gain(0,0.0);  // pass thru
                mix_ntcFilter3.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter3.gain(2,0.0);  // BP
                mix_ntcFilter3.gain(3,maxMixNtcFilter);  // HP
                break;
            default:
                mix_ntcFilter3.gain(0,0.0);  // pass thru
                mix_ntcFilter3.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter3.gain(2,0.0);  // BP
                mix_ntcFilter3.gain(3,0.0);  // HP
                break;
        }
    }  
    if (ModeNoiseFilter4 !=modeFilter4_old)
    {
        modeFilter4_old = ModeNoiseFilter4;
        switch (ModeNoiseFilter4)
        {
            case THRU:
                mix_ntcFilter4.gain(0,maxMixNtcFilter);  // pass thru
                mix_ntcFilter4.gain(1,0.0);  // LP 
                mix_ntcFilter4.gain(2,0.0);  // BP
                mix_ntcFilter4.gain(3,0.0);  // HP
                break;
            case LP:
                mix_ntcFilter4.gain(0,0.0);  // pass thru
                mix_ntcFilter4.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter4.gain(2,0.0);  // BP
                mix_ntcFilter4.gain(3,0.0);  // HP
                break;
            case BP:
                mix_ntcFilter4.gain(0,0.0);  // pass thru
                mix_ntcFilter4.gain(1,0.0);  // LP 
                mix_ntcFilter4.gain(2,maxMixNtcFilter);  // BP
                mix_ntcFilter4.gain(3,0.0);  // HP
                break;
            case HP:
                mix_ntcFilter4.gain(0,0.0);  // pass thru
                mix_ntcFilter4.gain(1,0.0);  // LP 
                mix_ntcFilter4.gain(2,0.0);  // BP
                mix_ntcFilter4.gain(3,maxMixNtcFilter);  // HP
                break;
            case NTC:
                mix_ntcFilter4.gain(0,0.0);  // pass thru
                mix_ntcFilter4.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter4.gain(2,0.0);  // BP
                mix_ntcFilter4.gain(3,maxMixNtcFilter);  // HP
                break;
            default:
                mix_ntcFilter4.gain(0,0.0);  // pass thru
                mix_ntcFilter4.gain(1,maxMixNtcFilter);  // LP 
                mix_ntcFilter4.gain(2,0.0);  // BP
                mix_ntcFilter4.gain(3,0.0);  // HP
                break;
        }
    }    
}



//------------------------------------------------------
//  see https://www.pjrc.com/teensy/td_libs_MIDI.html for
//            MIDI using
//------------------------------------------------------
void processMIDI(bool midi_from_host_flag) 
{
    // Note: callback method was too slow
    //      see https://www.pjrc.com/teensy/td_midi.html
    if (midi_from_host_flag){
      type =       midi_ho.getType();
      data1 =      midi_ho.getData1();
      data2 =      midi_ho.getData2();
      channel =    midi_ho.getChannel();
    } else{
      type =       MIDIs1.getType();
      data1 =      MIDIs1.getData1();
      data2 =      MIDIs1.getData2();
      channel =    MIDIs1.getChannel();
    } 
     // const uint8_t *sys = midi_ho.getSysExArray();
     //sprintf(str_buf, "type: %d, data1: %d, data2: %d, channel: %d", type,data1, data2, channel);
     //Serial8.println(str_buf);

    switch (type) 
    {
        case midi_ho.ProgramChange: //0xC0
            programChangeData = data1;
            programChangeFlag = true; // used in UISM
            sprintf(str_buf, "type: %d, data1: %d, channel: %d", type,data1, channel);
            Serial8.println(str_buf);
            if (midi_from_host_flag)
                MIDIs1.sendProgramChange(data1,channel);
            break;
        case midi_ho.AfterTouchPoly: //0xA0
            if (midi_from_host_flag)
                MIDIs1.sendPolyPressure(data1,data2,channel);
        case midi_ho.AfterTouchChannel: //0xD0
            if (midi_from_host_flag)
                MIDIs1.sendAfterTouch(data1,channel); // TODO: add AT as method of breath control
        case midi_ho.ControlChange: //0xB0
            if (midi_from_host_flag)
                MIDIs1.sendControlChange(data1,data2,channel);
            switch (data1){
                case CC_MODULATION_WHEEL:
                case CC_BREATH: 
                case CC_VOLUME:
                case CC_EXPRESSION:
                    if (data1 != (uint8_t)breath_cc )
                    {
                        break;
                    }
                    else
                    {
                        data2f = ((float)data2) * DIV127;
                        lastBreathf = data2f;
                        dc_breathLfoFilter1_amp = lfoThresh(data2f,LfoThreshOscFilter1,LfoDepthOscFilter1,LfoBreathOscFilter1);
                        dc_breathLfoFilter2_amp = lfoThresh(data2f,LfoThreshOscFilter2,LfoDepthOscFilter2,LfoBreathOscFilter2);
                        dc_breathLfoFilter3_amp = lfoThresh(data2f,LfoThreshNoiseFilter3,LfoDepthNoiseFilter3,LfoBreathNoiseFilter3);
                        dc_breathLfoFilter4_amp = lfoThresh(data2f,LfoThreshNoiseFilter4,LfoDepthNoiseFilter4,LfoBreathNoiseFilter4);
                        dc_breathLfoFilter1.amplitude(dc_breathLfoFilter1_amp,dc_breathLfoFilter_rampTime);
                        dc_breathLfoFilter2.amplitude(dc_breathLfoFilter2_amp,dc_breathLfoFilter_rampTime);
                        dc_breathLfoFilter3.amplitude(dc_breathLfoFilter3_amp,dc_breathLfoFilter_rampTime);
                        dc_breathLfoFilter4.amplitude(dc_breathLfoFilter4_amp,dc_breathLfoFilter_rampTime);
                        dc_breathOscFilter1_amp = gamma_func(data2f, BreathCurveOscFilter1);
                        dc_breathOscFilter2_amp = gamma_func(data2f, BreathCurveOscFilter2);
                        dc_breathNoiseFilter3_amp = gamma_func(data2f, BreathCurveNoiseFilter3);
                        dc_breathNoiseFilter4_amp = gamma_func(data2f, BreathCurveNoiseFilter4);
                        dc_breathNoise_amp = gamma_func(data2f,NoiseBreathCurve);
                        if(BreathAttainOsc1 > 0.0)
                            dc_breathSweepOsc1.amplitude(BreathDepthOsc1*(1.0-limit(data2f/BreathAttainOsc1,1.0,-1.0)),
                                                        dc_breathSweepOscN_rampTime); // smoothing
                        else
                            dc_breathSweepOsc1.amplitude(0.0);
                        if(BreathAttainOsc2 > 0.0)
                            dc_breathSweepOsc2.amplitude(BreathDepthOsc2*(1.0-limit(data2f/BreathAttainOsc2,1.0,-1.0)),
                                                        dc_breathSweepOscN_rampTime); //  smoothing
                        else
                            dc_breathSweepOsc2.amplitude(0.0);

                        if(note_is_on)
                        {
                            dc_breathThreshOsc1_amp = gamma_func(thresh( data2f,BreathThreshOsc1), BreathCurveOsc1);
                            dc_breathThreshOsc2_amp = gamma_func(thresh( data2f,BreathThreshOsc2), BreathCurveOsc2);
                            dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathThreshOscN_rampTime);
                            dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,dc_breathThreshOscN_rampTime);      
                            dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp,dc_breathFilterN_rampTime);
                            dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp,dc_breathFilterN_rampTime);
                            dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp,dc_breathFilterN_rampTime);
                            dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp,dc_breathFilterN_rampTime);
                            if( NoiseLevel > 0 )
                            {
                                dc_breathNoise.amplitude(dc_breathNoise_amp,dc_breathNoise_rampTime);
                            }
                        }
                        else
                        {
                            dc_breathThreshOsc1_amp = 0;
                            dc_breathThreshOsc2_amp = 0;
                            dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathThreshOscN_rampTime);
                            dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,dc_breathThreshOscN_rampTime);      
                            dc_breathOscFilter1.amplitude(0,dc_breathOff_rampTime);
                            dc_breathOscFilter2.amplitude(0,dc_breathOff_rampTime);
                            dc_breathNoiseFilter3.amplitude(0,dc_breathOff_rampTime);
                            dc_breathNoiseFilter4.amplitude(0,dc_breathOff_rampTime);
                            dc_breathNoise.amplitude(0,dc_breathOff_rampTime);
                        }
                        break;
                    }
                case CC_PORTA_TIME:
                    // PortamentoTime MSB
                    dc_portatime_val = gamma_func( ((float)data2)*DIV127, dc_portatime_gamma ); 
                    if(note_is_on) 
                    {   // update portamento any time it changes and note is on
                        portatime_temp = dc_portatime_val*dc_portatime_range*fMidiNoteNorm_diff;
                        if ( portatime_temp < portatime_min )
                            portatime_temp = portatime_min;
                        //dc_portatime.amplitude(fMidiNoteNorm, max(6.0f, portatime_temp) );
                        dc_portatime.amplitude(fMidiNoteNorm,  portatime_temp );
                    }
                  break;
            }
            break;     

        case midi_ho.NoteOn: //(type==0x90)
            if (midi_from_host_flag)
                MIDIs1.sendNoteOn(data1,data2,channel);
          //  TODO: create amplitude transition between legato notes
            data2f = ((float)data2)* DIV127;
            if (lastBreathf <= 0.0f){ lastBreathf = data2f; } 
         // /* don't treat Note on Velocity as a Breath value
            dc_breathThreshOsc1_amp = gamma_func(thresh( lastBreathf,BreathThreshOsc1), BreathCurveOsc1);
            dc_breathThreshOsc2_amp = gamma_func(thresh( lastBreathf,BreathThreshOsc2), BreathCurveOsc2);
            dc_breathLfoFilter1_amp = lfoThresh(lastBreathf,LfoThreshOscFilter1,LfoDepthOscFilter1,LfoBreathOscFilter1);
            dc_breathLfoFilter2_amp = lfoThresh(lastBreathf,LfoThreshOscFilter2,LfoDepthOscFilter2,LfoBreathOscFilter2);
            dc_breathLfoFilter3_amp = lfoThresh(lastBreathf,LfoThreshNoiseFilter3,LfoDepthNoiseFilter3,LfoBreathNoiseFilter3);
            dc_breathLfoFilter4_amp = lfoThresh(lastBreathf,LfoThreshNoiseFilter4,LfoDepthNoiseFilter4,LfoBreathNoiseFilter4);
            dc_breathLfoFilter1.amplitude(dc_breathLfoFilter1_amp,dc_breathLfoFilter_rampTime);
            dc_breathLfoFilter2.amplitude(dc_breathLfoFilter2_amp,dc_breathLfoFilter_rampTime);
            dc_breathLfoFilter3.amplitude(dc_breathLfoFilter3_amp,dc_breathLfoFilter_rampTime);
            dc_breathLfoFilter4.amplitude(dc_breathLfoFilter4_amp,dc_breathLfoFilter_rampTime);
            dc_breathOscFilter1_amp = gamma_func(lastBreathf, BreathCurveOscFilter1);
            dc_breathOscFilter2_amp = gamma_func(lastBreathf, BreathCurveOscFilter2);
            dc_breathNoiseFilter3_amp = gamma_func(lastBreathf, BreathCurveNoiseFilter3);
            dc_breathNoiseFilter4_amp = gamma_func(lastBreathf, BreathCurveNoiseFilter4);
            dc_breathNoise_amp = gamma_func(lastBreathf,NoiseBreathCurve);
          //  */
            fMidiNoteNorm = ((float)data1)/128.0;
            fMidiNoteNorm_diff = abs( (float)(data1 - currentMidiNote));
            if(note_is_on) // legato 
            {
                // 2nd dc_portatime for filter movement instead of using the one for osc freq for both.
                portatime_temp = dc_portatime_val*dc_portatime_range*fMidiNoteNorm_diff;
                if ( portatime_temp < portatime_min )
                    portatime_temp = portatime_min;
                dc_portatime.amplitude(fMidiNoteNorm, portatime_temp);
                dc_portatimef.amplitude(fMidiNoteNorm, portatimef_t);
            }
            else  // note_is_on==false, so non-legato
            {
                dc_portatime.amplitude(fMidiNoteNorm);
                dc_portatimef.amplitude(fMidiNoteNorm,portatimef_t);
            }         
            // note_freq = NOTEFREQS[data1];
            if(!note_is_on || !KeyTriggerSingle)
            {
                // non-legato section uses shorter rampTimnes
                float rampTimeShortening = 0.5;
                dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathThreshOscN_rampTime*rampTimeShortening);
                dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,dc_breathThreshOscN_rampTime*rampTimeShortening);      
                dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp,dc_breathFilterN_rampTime*rampTimeShortening);
                dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp,dc_breathFilterN_rampTime*rampTimeShortening);
                dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp,dc_breathFilterN_rampTime*rampTimeShortening);
                dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp,dc_breathFilterN_rampTime*rampTimeShortening);
                if( NoiseLevel > 0) { dc_breathNoise.amplitude(dc_breathNoise_amp,dc_breathNoise_rampTime*rampTimeShortening); }
                if(BreathAttainOsc1 > 0.0) {
                    dc_breathSweepOsc1.amplitude(BreathDepthOsc1*(1.0-limit(lastBreathf/BreathAttainOsc1,1.0,-1.0)),
                                                dc_breathSweepOscN_rampTime*rampTimeShortening); 
                }
                else {
                    dc_breathSweepOsc1.amplitude(0.0);
                }
                if(BreathAttainOsc2 > 0.0) {
                    dc_breathSweepOsc2.amplitude(BreathDepthOsc2*(1.0-limit(lastBreathf/BreathAttainOsc2,1.0,-1.0)),
                                                dc_breathSweepOscN_rampTime*rampTimeShortening); 
                }
                else {
                    dc_breathSweepOsc2.amplitude(0.0);
                }

                if(SweepDepthOsc1 != 0.0)
                {
                    dc_sweepDepthOsc1.amplitude(SweepDepthOsc1);
                    dc_sweepDepthOsc1.amplitude(0,SweepTimeOsc1);
                }
                if(SweepDepthOsc2 != 0.0)
                {
                    dc_sweepDepthOsc2.amplitude(SweepDepthOsc2);
                    dc_sweepDepthOsc2.amplitude(0,SweepTimeOsc2);
                }
                // dc_beatOsc1.amplitude(beatOsc1); // TODO calculate beat number
                // dc_beatOsc2.amplitude(beatOsc2); // TODO calculate beat number
                if(NoiseLevel >0)
                {
                    dc_timeNoise.amplitude(TimeNoiseAmp); // effectively set by pink_Noise.amplitude(NoiseLevel);
                    dc_timeNoise.amplitude(0.0,NoiseTime); // ramp down the noise level
                }
                else
                {
                    dc_timeNoise.amplitude(0.0);
                }
                if(SweepDepthOscFilter1 > 0.0)
                {   
                    // the sign signal now is aplitude as well, so the sq_swpfltN always runs between 1.0 and 0
                    dc_sweepDepthFilterSign1.amplitude(SweepDepthOscFilter1);
                    dc_sweepDepthFilter1.amplitude(1.0);
                    dc_sweepDepthFilter1.amplitude(0,SweepTimeOscFilter1);
                    if(LinkOscFilters)
                    {
                        dc_sweepDepthFilterSign2.amplitude(SweepDepthOscFilter1);
                        dc_sweepDepthFilter2.amplitude(1.0);
                        dc_sweepDepthFilter2.amplitude(0,SweepTimeOscFilter1);
                    }
                }
                else if(SweepDepthOscFilter1 < 0.0)
                {
                    dc_sweepDepthFilterSign1.amplitude(-SweepDepthOscFilter1);
                    dc_sweepDepthFilter1.amplitude(1.0);
                    dc_sweepDepthFilter1.amplitude(0,SweepTimeOscFilter1);
                    if(LinkOscFilters)
                    {
                        dc_sweepDepthFilterSign2.amplitude(-SweepDepthOscFilter1);
                        dc_sweepDepthFilter2.amplitude(1.0);
                        dc_sweepDepthFilter2.amplitude(0,SweepTimeOscFilter1);
                    }
                }
                if(!LinkOscFilters)
                {
                    if(SweepDepthOscFilter2 >0.0)
                    {
                        dc_sweepDepthFilterSign2.amplitude(SweepDepthOscFilter2);
                        dc_sweepDepthFilter2.amplitude(1.0);
                        dc_sweepDepthFilter2.amplitude(0,SweepTimeOscFilter2);
                    }
                    else if(SweepDepthOscFilter2 < 0.0)
                    {
                        dc_sweepDepthFilterSign2.amplitude(-SweepDepthOscFilter2);
                        dc_sweepDepthFilter2.amplitude(1.0);
                        dc_sweepDepthFilter2.amplitude(0,SweepTimeOscFilter2);
                    }
                }
                if(SweepDepthNoiseFilter3 >0.0)
                {
                    dc_sweepDepthFilterSign3.amplitude(SweepDepthNoiseFilter3);
                    dc_sweepDepthFilter3.amplitude(1.0);
                    dc_sweepDepthFilter3.amplitude(0,SweepTimeNoiseFilter3);
                    if(LinkNoiseFilters)
                    {
                        dc_sweepDepthFilterSign4.amplitude(SweepDepthNoiseFilter3);
                        dc_sweepDepthFilter4.amplitude(1.0);
                        dc_sweepDepthFilter4.amplitude(0,SweepTimeNoiseFilter3);
                    }
                }
                else if(SweepDepthNoiseFilter3 < 0.0)
                {
                    dc_sweepDepthFilterSign3.amplitude(-SweepDepthNoiseFilter3);
                    dc_sweepDepthFilter3.amplitude(1.0);
                    dc_sweepDepthFilter3.amplitude(0,SweepTimeNoiseFilter3);
                    if(LinkNoiseFilters)
                    {
                        dc_sweepDepthFilterSign4.amplitude(-SweepDepthNoiseFilter3);
                        dc_sweepDepthFilter4.amplitude(1.0);
                        dc_sweepDepthFilter4.amplitude(0,SweepTimeNoiseFilter3);
                    }
                }
                if(!LinkNoiseFilters)
                {
                    if(SweepDepthNoiseFilter4 > 0.0)
                    {
                        dc_sweepDepthFilterSign4.amplitude(SweepDepthNoiseFilter4);
                        dc_sweepDepthFilter4.amplitude(1.0);
                        dc_sweepDepthFilter4.amplitude(0,SweepTimeNoiseFilter4);
                    }
                    else if(SweepDepthNoiseFilter4 < 0.0)
                    {
                        dc_sweepDepthFilterSign4.amplitude(-SweepDepthNoiseFilter4);
                        dc_sweepDepthFilter4.amplitude(1.0);
                        dc_sweepDepthFilter4.amplitude(0,SweepTimeNoiseFilter4);
                    }
                }
            }
            else
            {   
                // legato section uses regular rampTimes
                dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathThreshOscN_rampTime);
                dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,dc_breathThreshOscN_rampTime);      
                dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp,dc_breathFilterN_rampTime);
                dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp,dc_breathFilterN_rampTime);
                dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp,dc_breathFilterN_rampTime);
                dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp,dc_breathFilterN_rampTime);
                if( NoiseLevel > 0) { dc_breathNoise.amplitude(dc_breathNoise_amp,dc_breathNoise_rampTime); }
                if(BreathAttainOsc1 > 0.0) {
                    dc_breathSweepOsc1.amplitude(BreathDepthOsc1*(1.0-limit(lastBreathf/BreathAttainOsc1,1.0,-1.0)),
                                                dc_breathSweepOscN_rampTime); 
                }
                else {
                    dc_breathSweepOsc1.amplitude(0.0);
                }
                if(BreathAttainOsc2 > 0.0) {
                    dc_breathSweepOsc2.amplitude(BreathDepthOsc2*(1.0-limit(lastBreathf/BreathAttainOsc2,1.0,-1.0)),
                                                dc_breathSweepOscN_rampTime); 
                }
                else {
                    dc_breathSweepOsc2.amplitude(0.0);
                }
            }
            currentMidiNote = data1;
            note_is_on = true;
            break;

        case midi_ho.NoteOff:  //(type==0x80)
            if (midi_from_host_flag)
                MIDIs1.sendNoteOff(data1,data2,channel);
            if(data1==currentMidiNote)
            {
                note_is_on = false;
                fMidiNoteNorm = ((float)data1)/128.0;
                dc_portatime.amplitude(fMidiNoteNorm); // 
                dc_portatimef.amplitude(fMidiNoteNorm,portatimef_t); // 

                // in case no breath signal comes after NoteOff
                dc_breathThreshOsc1_amp = 0;
                dc_breathThreshOsc2_amp = 0;
                dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathThreshOscN_rampTime);
                dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,dc_breathThreshOscN_rampTime);      
                dc_breathOscFilter1.amplitude(0,dc_breathOff_rampTime);
                dc_breathOscFilter2.amplitude(0,dc_breathOff_rampTime);
                dc_breathNoiseFilter3.amplitude(0,dc_breathOff_rampTime);
                dc_breathNoiseFilter4.amplitude(0,dc_breathOff_rampTime);
                dc_breathNoise.amplitude(0,dc_breathOff_rampTime);
            }
            break;

        case midi_ho.PitchBend:
            if (midi_from_host_flag)
                MIDIs1.sendPitchBend(data1+data2*128-8192,channel); // 0 = no bend, -2^13 ot +2^13-1
            dc_pitchbend.amplitude((data1+data2*128.0-8192.0)*DIV8192, dc_pitchbend_ramp_rate);
            break;
        default: 
            sprintf(str_buf, "Default! type: %d, data1: %d, channel: %d", type,data1, channel);
            Serial8.println(str_buf);
            break;

    } // switch (type)
}

#if 0
void readPot(void)
{
    int a = analogRead(A1);
    if (a > iPotValue + 50 || a < iPotValue - 50) {
        iPotValue = a;
        fPotValue = (float)iPotValue/1023;
        sprintf(str_buf1, "Knob (pin A1) = %04d, fPotValue: %5.3f", a, fPotValue );
        Serial8.println(str_buf1);
        updateSynthVariablesFlag = true;
    }
}
#endif

// TODO: add overall reverb level, patch rvb level, patch delay level, patch chorus on, patch volume
// EffectsReverbLevel,EffectsDelayLevel, ChorusOn
void updateUISM(void)
{
    switch (UISM)
    {
        case SPLASH_SCREEN: 
            if(shortKnobButtonPress || newKnob)
            {
                shortKnobButtonPress = false;
                UISM = VOL_ADJ;
                sprintf(str_oledbuf, "Vol: %03ld", vol);
                break; 
            }
            
        case VOL_ADJ: 
            if(shortKnobButtonPress)
            {
                shortKnobButtonPress = false;
                UISM = PATCH_SEL;
                String ps( current_patch.patch_string );
                ps.setCharAt( ps.indexOf(' '), '\n');
                sprintf(str_oledbuf, "Patch: %03d\n%s", current_patchNumber+1, ps.c_str() );
                break; 
            }
            if(longKnobButtonPress)
            {
                longKnobButtonPress = false;
                UISM = MENU;
                MENUSM = MENU_EXIT;
                sprintf(str_oledbuf, "MENU:\n EXIT");
                resetUITimeout();
                break; 
            }
            if (programChangeFlag)
            {
                UISM = PATCH_SEL;
               // String ps( current_patch.patch_string );
               // ps.setCharAt( ps.indexOf(' '), '\n');
               // sprintf(str_oledbuf, "Patch: %03d\n%s", current_patchNumber+1, ps.c_str() );
                break;
            }
            if (newKnob)
            {
                vol = vol + newKnob*4;
                vol = vol < 0 ? 0 : vol >= 100 ? 100 : vol;
                //volf = ((float)vol)/50.0f;
                volf = ((float)vol)/100.0f;
                volf = (volf*volf)*2.0f;
                updateSynthVariablesFlag = true;
            }
            sprintf(str_oledbuf, "Vol: %03ld", vol);
            sprintf(str_buf1, "Vol: %03ld", vol);
            Serial8.println(str_buf1);
            break;
        case PATCH_SEL:
            if (programChangeFlag)
            {
                current_patchNumber = programChangeData % NUMBER_OF_PATCHES;
                programChangeFlag = false;
                // load the patch 
                if (patchLoaded[current_patchNumber])
                {
                    setCurrentPatch(current_patchNumber);
                    patchToSynthVariables(&current_patch);
                } 
                else 
                {
                    loadPatchSD(current_patchNumber);
                }
                String ps( current_patch.patch_string );
                ps.setCharAt( ps.indexOf(' '), '\n');
                sprintf(str_oledbuf, "Patch: %03d\n%s", current_patchNumber+1, ps.c_str() );
                Serial8.println(current_patch.patch_string);
                if (updateEpromFlag)
                {
                    eepromCurrentMillis = millis();
                    eepromPreviousMillis = eepromCurrentMillis; // reset timer every knob turn 
                }
                break;
            }
            if(shortKnobButtonPress)
            {
                shortKnobButtonPress = false;
                UISM = VOL_ADJ;
                sprintf(str_oledbuf, "Vol: %03ld", vol);
                break; 
            }
            if(longKnobButtonPress)
            {
                longKnobButtonPress = false;
                UISM = MENU;
                MENUSM = MENU_EXIT;
                sprintf(str_oledbuf, "MENU:\n EXIT");
                resetUITimeout();
                break; 
            }
            if (newKnob)
            {
                if(current_patchNumber >= NUMBER_OF_PATCHES-1 && newKnob > 0)
                    current_patchNumber = 0; 
                else if(current_patchNumber <= 0 && newKnob < 0)
                    current_patchNumber = NUMBER_OF_PATCHES-1; 
                else
                    current_patchNumber = current_patchNumber + newKnob;
                // load the patch 
                if (patchLoaded[current_patchNumber])
                {
                    setCurrentPatch(current_patchNumber);
                    patchToSynthVariables(&current_patch);
                } 
                else 
                {
                    loadPatchSD(current_patchNumber);
                }
                String ps( current_patch.patch_string );
                ps.setCharAt( ps.indexOf(' '), '\n');
                sprintf(str_oledbuf, "Patch: %03d\n%s", current_patchNumber+1, ps.c_str() );
                Serial8.println(current_patch.patch_string);
                if (updateEpromFlag)
                {
                    eepromCurrentMillis = millis();
                    eepromPreviousMillis = eepromCurrentMillis; // reset timer every knob turn 
                }
            }
            break;
        case  MENU: 
            // MENU_EXIT, MENU_MIX, MENU_TUNING, MENU_TRANS, MENU_OCT, MENU_BR, 
            // MENU_RVB_PATCH, MENU_DLY_PATCH, MENU_CHRS_PATCH, MENU_WR_PATCH_FX
            if(longKnobButtonPress)
            {
                longKnobButtonPress = false;
                UISM = MENU;
                MENUSM = MENU_EXIT;
                submenu_active = false;
                sprintf(str_oledbuf, "MENU:\n EXIT");
                resetUITimeout();
                break; 
            }
            switch (MENUSM)   
            { 
                case MENU_EXIT:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_MIX;
                            sprintf(str_oledbuf, "MENU:\n MIX");
                            break; 
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_WR_PATCH_FX;
                            sprintf(str_oledbuf, "MENU:\n WR PCH FX");
                            break; 
                        }
                        else
                        {
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU:\n EXIT");
                        }
                    }
                    if(shortKnobButtonPress)
                    {
                        submenu_active = false;
                        shortKnobButtonPress = false;
                        UISM = VOL_ADJ;
                        sprintf(str_oledbuf, "Vol: %03ld", vol);
                    }
                    break;
                case MENU_MIX:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_TUNING;
                            sprintf(str_oledbuf, "MENU:\n TUNING");
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU:\n EXIT");
                        }
                        else
                        {
                            if(shortKnobButtonPress)
                            {
                                submenu_active = true;
                                shortKnobButtonPress = false;
                                sprintf(str_oledbuf, "MixLineIn: %03ld", mix_linein);
                                break; 
                            }
                            MENUSM = MENU_MIX;
                            sprintf(str_oledbuf, "MENU:\n MIX");
                        }
                    }
                    else
                    {
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU:\n EXIT");
                            break; 
                        }
                        if (newKnob)
                        {
                            mix_linein = mix_linein + newKnob;
                            mix_linein = mix_linein < 0 ? 0 : mix_linein >= 100 ? 100 : mix_linein;
                            mix_lineinf = ((float)mix_linein)/100.0f;
                            updateSynthVariablesFlag = true;
                        }
                        sprintf(str_oledbuf, "MixLineIn: %03ld", mix_linein);
                        sprintf(str_buf1, "MixLineIn: %03ld", mix_linein);
                        Serial8.println(str_buf1);
                    }

                    break;
                case MENU_TUNING:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_TRANS;
                            sprintf(str_oledbuf, "MENU:\n TRANS");
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_MIX;
                            sprintf(str_oledbuf, "MENU:\n MIX");
                        }
                        else
                        {
                            if(shortKnobButtonPress)
                            {
                                submenu_active = true;
                                shortKnobButtonPress = false;
                                sprintf(str_oledbuf, "Cents: %03ld", FineTuneCents);
                                break; 
                            }
                            MENUSM = MENU_TUNING;
                            sprintf(str_oledbuf, "MENU:\n TUNING");
                        }
                    }
                    else 
                    {
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU:\n EXIT");
                            break; 
                        }
                        if (newKnob)
                        {
                            FineTuneCents = FineTuneCents + newKnob;
                            FineTuneCents = FineTuneCents < -100 ? -100 
                                            : FineTuneCents >= 100 ? 100 : FineTuneCents;
                            FineTuneCentsf = ((float)FineTuneCents)/100.0f;
                            updateSynthVariablesFlag = true;
                        }
                        sprintf(str_oledbuf, "Cents: %03ld", FineTuneCents);
                        sprintf(str_buf1, "FineTuneCents: %03ld", FineTuneCents);
                        Serial8.println(str_buf1);
                    }
                    break;
                case MENU_TRANS:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_OCT;
                            sprintf(str_oledbuf, "MENU:\n OCT");
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_TUNING;
                            sprintf(str_oledbuf, "MENU:\n TUNING");
                        }
                        else
                        {
                            if(shortKnobButtonPress)
                            {
                                submenu_active = true;
                                shortKnobButtonPress = false;
                                sprintf(str_oledbuf, "Semi: %03ld", Transpose);
                                break; 
                            }
                            MENUSM = MENU_TRANS;
                            sprintf(str_oledbuf, "MENU:\n TRANS");
                        }
                    }
                    else 
                    {
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU:\n EXIT");
                            break; 
                        }
                        if (newKnob)
                        {
                            Transpose = Transpose + newKnob;
                            Transpose = Transpose < -12 ? -12 
                                            : Transpose >= 12 ? 12 : Transpose;
                            Transposef = ((float)Transpose);
                            updateSynthVariablesFlag = true;
                        }
                        sprintf(str_oledbuf, "Semi: %03ld", Transpose);
                        sprintf(str_buf1, "Transpose: %03ld", Transpose);
                        Serial8.println(str_buf1);
                    }
                    break;

                case MENU_OCT:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            //MENUSM = MENU_RVB_PATCH;
                            MENUSM = MENU_BR;
                            sprintf(str_oledbuf, "MENU:\n BrCC ");
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_TRANS;
                            sprintf(str_oledbuf, "MENU:\n TRANS");
                        }
                        else
                        {
                            if(shortKnobButtonPress)
                            {
                                submenu_active = true;
                                shortKnobButtonPress = false;
                                sprintf(str_oledbuf, "Oct: %03ld", Octave);
                                break; 
                            }
                            MENUSM = MENU_OCT;
                            sprintf(str_oledbuf, "MENU:\n OCT");
                        }
                    }
                    else 
                    {
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU:\n EXIT");
                            break; 
                        }
                        if (newKnob)
                        {
                            Octave = Octave + newKnob;
                            Octave = Octave < -2 ? -2 
                                            : Octave >= 2 ? 2 : Octave;
                            Octavef = ((float)Octave*12);
                            updateSynthVariablesFlag = true;
                        }
                        sprintf(str_oledbuf, "Oct: %03ld", Octave);
                        sprintf(str_buf1, "Octave: %03ld", Octave);
                        Serial8.println(str_buf1);
                    }
                    break;

                case MENU_BR:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_RVB_PATCH;
                            sprintf(str_oledbuf, "MENU:\n RVB Patch");
                        }
                        else if(newKnob < 0) 
                        {
                            //MENUSM = MENU_TRANS;
                            MENUSM = MENU_OCT;
                            sprintf(str_oledbuf, "MENU:\n OCT");
                        }
                        else
                        {
                            if(shortKnobButtonPress)
                            {
                                submenu_active = true;
                                shortKnobButtonPress = false;
                                sprintf(str_oledbuf, "BrCC: %03ld", breath_cc );
                                break; 
                            }
                            MENUSM = MENU_BR;
                            sprintf(str_oledbuf, "MENU:\n BrCC");
                        }
                    }
                    else 
                    {
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU:\n EXIT");
                            break; 
                        }
                        if (newKnob)
                        {
                           switch (breath_cc)
                           {
                                case CC_MODULATION_WHEEL:
                                    breath_cc = newKnob > 0 ? CC_BREATH : CC_EXPRESSION;
                                    break;
                                case CC_BREATH: 
                                    breath_cc = newKnob > 0 ? CC_VOLUME : CC_MODULATION_WHEEL;
                                    break;
                                case CC_VOLUME:
                                    breath_cc = newKnob > 0 ? CC_EXPRESSION : CC_BREATH;
                                    break;
                                case CC_EXPRESSION:
                                    breath_cc = newKnob > 0 ? CC_MODULATION_WHEEL : CC_VOLUME;
                                    break;
                                default:
                                    breath_cc = CC_BREATH;
                                    break;
                            }
                        }
                        sprintf(str_oledbuf, "BrCC: %03ld", breath_cc);
                        sprintf(str_buf1, "BrCC: %03ld", breath_cc);
                        Serial8.println(str_buf1);
                    }
                    break;

                case MENU_RVB_PATCH:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_DLY_PATCH;
                            sprintf(str_oledbuf, "MENU:\n DLY Patch");
                        }
                        else if(newKnob < 0) 
                        {
                            //MENUSM = MENU_OCT;
                            MENUSM = MENU_BR;
                            sprintf(str_oledbuf, "MENU:\n BrCC");
                        }
                        else
                        {
                            if(shortKnobButtonPress)
                            {
                                submenu_active = true;
                                shortKnobButtonPress = false;
                                uint8_t temp_rvb = (uint8_t)current_patch.nrpn_msb_reverb[CCEFFECTSREVERBLEVEL];
                                sprintf(str_oledbuf, "RVB Patch: %03d", temp_rvb);
                                break; 
                            }
                            MENUSM = MENU_RVB_PATCH;
                            sprintf(str_oledbuf, "MENU:\n RVB Patch");
                        }
                    }
                    else 
                    {
                        int16_t temp_rvb = (int16_t)current_patch.nrpn_msb_reverb[CCEFFECTSREVERBLEVEL];
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            //MENUSM = MENU_EXIT;
                            //sprintf(str_oledbuf, "MENU:\n EXIT");
                            MENUSM = MENU_RVB_PATCH;
                            sprintf(str_oledbuf, "MENU:\n RVB Patch");
                            break; 
                        }
                        if (newKnob)
                        { 
                            temp_rvb = temp_rvb + newKnob;
                            temp_rvb = temp_rvb < 0 ? 0 
                                            : temp_rvb >= 127 ? 127 : temp_rvb;
                            current_patch.nrpn_msb_reverb[CCEFFECTSREVERBLEVEL] = (uint8_t)temp_rvb;
                            EffectsReverbLevel = ((float)temp_rvb*DIV127*maxReverbLevel);
                            updateSynthVariablesFlag = true;
                        }
                        sprintf(str_oledbuf, "RVB Patch: %03d", temp_rvb);
                        sprintf(str_buf1, "RVB Patch: %03d",temp_rvb);
                        Serial8.println(str_buf1);
                    }
                    break;

                case MENU_DLY_PATCH:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_CHRS_PATCH;
                            sprintf(str_oledbuf, "MENU:\n CHS Patch");
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_RVB_PATCH;
                            sprintf(str_oledbuf, "MENU:\n RVB Patch");
                        }
                        else
                        {
                            if(shortKnobButtonPress)
                            {
                                submenu_active = true;
                                shortKnobButtonPress = false;
                                uint8_t temp_dly = (uint8_t)current_patch.nrpn_msb_delay[CCEFFECTSDELAYLEVEL];
                                sprintf(str_oledbuf, "DLY Patch: %03d", temp_dly);
                                break; 
                            }
                            MENUSM = MENU_DLY_PATCH;
                            sprintf(str_oledbuf, "MENU:\n DLY Patch");
                        }
                    }
                    else 
                    {
                        int16_t temp_dly = (int16_t)current_patch.nrpn_msb_delay[CCEFFECTSDELAYLEVEL];
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            //MENUSM = MENU_EXIT;
                            //sprintf(str_oledbuf, "MENU:\n EXIT");
                            MENUSM = MENU_DLY_PATCH;
                            sprintf(str_oledbuf, "MENU:\n DLY Patch");
                            break; 
                        }
                        if (newKnob)
                        { 
                            temp_dly = temp_dly + newKnob;
                            temp_dly = temp_dly < 0 ? 0 
                                            : temp_dly >= 127 ? 127 : temp_dly;
                            current_patch.nrpn_msb_delay[CCEFFECTSDELAYLEVEL] = (uint8_t)temp_dly;
                            EffectsDelayLevel = ((float)temp_dly*DIV127);
                            updateSynthVariablesFlag = true;
                        }
                        sprintf(str_oledbuf, "DLY Patch: %03d", temp_dly);
                        sprintf(str_buf1, "DLY Patch: %03d",temp_dly);
                        Serial8.println(str_buf1);
                    }
                    break;

                case MENU_CHRS_PATCH:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_WR_PATCH_FX;
                            patchFxWriteFlag = false;
                            sprintf(str_oledbuf, "MENU:\n WR PCH FX");
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_DLY_PATCH;
                            sprintf(str_oledbuf, "MENU:\n DLY Patch");
                        }
                        else
                        {
                            if(shortKnobButtonPress)
                            {
                                submenu_active = true;
                                shortKnobButtonPress = false;
                                ChorusOn = (bool)current_patch.nrpn_msb_common1[CCCHORUSON];  	    
                                sprintf(str_oledbuf, "Chorus:\n %s", ChorusOn ? "On" : "Off"); //
                                break; 
                            }
                            MENUSM = MENU_CHRS_PATCH;
                            sprintf(str_oledbuf, "MENU:\n CHS Patch");
                        }
                    }
                    else 
                    {
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            //MENUSM = MENU_EXIT;
                            //sprintf(str_oledbuf, "MENU:\n EXIT");
                            MENUSM = MENU_CHRS_PATCH;
                            sprintf(str_oledbuf, "MENU:\n CHS Patch");
                            break; 
                        }
                        if (newKnob)
                        {
                            ChorusOn = (bool)current_patch.nrpn_msb_common1[CCCHORUSON];  	    
                            ChorusOn = ChorusOn ? false : true; // toggle On/Off
                            updateSynthVariablesFlag = true;
                            current_patch.nrpn_msb_common1[CCCHORUSON] = ChorusOn;
                        }
                        sprintf(str_oledbuf, "Chorus:\n %s", ChorusOn ? "On" : "Off");
                        sprintf(str_buf1, "Chorus:\n %s", ChorusOn ? "On" : "Off");
                        Serial8.println(str_buf1);
                    }
                    break;


                case MENU_WR_PATCH_FX:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU:\n EXIT");
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_CHRS_PATCH;
                            sprintf(str_oledbuf, "MENU:\n CHS Patch");
                        }
                        else
                        {
                            if(shortKnobButtonPress)
                            {
                                submenu_active = true;
                                patchFxWriteFlag = false;
                                shortKnobButtonPress = false;
                                sprintf(str_oledbuf, "WR PCH FX:\n Skip"); //
                                break; 
                            }
                            MENUSM = MENU_WR_PATCH_FX;
                            sprintf(str_oledbuf, "MENU:\n WR PCH FX");
                        }
                    }
                    else 
                    {
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            MENUSM = MENU_EXIT;
                           if(patchFxWriteFlag)
                            {
                                patchFxWriteFlag = false;
                                sprintf(str_oledbuf, "\nWriting...");
                                Serial8.println(str_buf1);
                                saveCurrentPatchSD(current_patchNumber);
                                sprintf(str_oledbuf, "MENU:\n EXIT");
                            }
                            else
                            {
                                sprintf(str_oledbuf, "MENU:\n EXIT");
                            }
                            break; 
                        }
                        if (newKnob)
                        {
                            patchFxWriteFlag = patchFxWriteFlag ? false : true; // toggle On/Off
                        }
                        sprintf(str_oledbuf, "WR PCH FX:\n %s", patchFxWriteFlag ? "Write" : "Skip");
                        sprintf(str_buf1, "WR PCH FX:\n %s", patchFxWriteFlag ? "Write" : "Skip");
                        Serial8.println(str_buf1);
                    }
                    break;



                default:
                    break;
            }
            break;
        default:
            break;
    }

    if(newKnob != 0)
    {
//        sprintf(str_buf1, "knob = %d, knobButton = %d", newKnob, currentKnobButtonState );     
//        Serial8.println(str_buf1);
        // newKnob: 1 for CW, -1 for CCW
        newKnob = 0;
        knob.write(0);
        resetUITimeout();
    }

   if (UISM != SPLASH_SCREEN)
    {
        if (UISM == PATCH_SEL){
            display.setTextSize(2.0,2.0); // configuration of size of characters 
        }else{
            display.setTextSize(2.0); // configuration of size of characters 
        }
        display.setTextColor(WHITE);
        display.clearDisplay(); // erase display
        display.display(); // refresh display
        display.setCursor(7,5);
        display.println(str_oledbuf);
        display.display(); 
    }
}

void UITimeout(void)
{
        display.clearDisplay(); // erase display
        display.display(); 
        // TODO: resetUISM();
        ALREADY_TIMED_OUT = true;
}

void resetUITimeout(void)
{
    previousUITimeoutTime = currentUITimeoutTime;
    ALREADY_TIMED_OUT = false;
}

void readKnobAndKnobButton(void)
{
    int32_t newKnob_temp = 0;
    //--------------------------
    // Read knob and knobButton 
    //--------------------------
    newKnob_temp = knob.read()/4; //don't need 4X counting mode, so integer div by 4
    newKnob = newKnob_temp > 0 ? 1 : newKnob_temp < 0 ? -1 : 0; //don't need 4X counting mode, so integer div by 4
    knobButton.update();
    currentKnobButtonState = knobButton.read();
    if( !currentKnobButtonState && (newKnob != 0) ){ 
        knob.write(0); // reset knob position to zero
        newKnob = 0;
    }
    
    if( !longKnobButtonPressPending && !currentKnobButtonState && (knobButton.currentDuration() > longKnobButtonPressTime) )
    {
        longKnobButtonPressPending = true;
        sprintf(str_buf1, "Long knobButton Press" );     
        Serial8.println(str_buf1);
        resetUITimeout();
    }
    KnobButtonRising = knobButton.rose();

    if(KnobButtonRising)
    {
        KnobButtonRising = false;
        if(firstKnobButtonPress)
        {
           // firstKnowButtonPress is never a valid push so clear out the ButtonPress flags
           firstKnobButtonPress = false; 
           longKnobButtonPressPending = false;
           shortKnobButtonPress = false;
           longKnobButtonPress = false;
           return;
        }
        if(longKnobButtonPressPending)
        {
            longKnobButtonPressPending = false;
            longKnobButtonPress = true;
            shortKnobButtonPress = false;
        }
        else
        {
            longKnobButtonPress = false;
            shortKnobButtonPress = true;
        }
    }

    if( longKnobButtonPress || shortKnobButtonPress || newKnob != 0 )
    {
        resetUITimeout();
        updateUISM();  // updated User Interface State Machine
    }
}


void EEPROM_update(int addr, int val){  
   int temp_var;
   EEPROM.get(addr, temp_var);
   if( temp_var != val ){
      EEPROM.put(addr, val);
    }
}
