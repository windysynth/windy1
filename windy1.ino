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
// to work with an EWI5000 EWIUSB initially & EWISOLO in future
// The patches are editable with the Vyzex EWI4000S editor software
// and uses the Teensy Audio Library
//
// There are a couple of tweeks to the Teensy Audio Library
// First, to use WAVEFORM_BANDLIMIT_SAWTOOTH with the AudioSynthWaveformModulated
// you need at least Teensyduino 1.54
// Next, I had to modify C:\Arduino\hardware\teensy\avr\libraries\Audio\filter_variable.cpp
//  uncommented line 42 to #define IMPROVE_HIGH_FREQUENCY_ACCURACY
//  changed constant on line 131
//		//if (fmult > 5378279) fmult = 5378279;
//		if (fmult > 4205728) fmult = 4205728; // Tims hack max fmult slightly > 1.0
//		//if (fmult > 4194303) fmult = 4194303; // Tims hack max fmult = 1.0
//  This hack keeps the filters from producing really harsh white noise when
//  the fmult gets too high because of the combnation of:
//  frequency(),  e.g above 113
//  resonance(),  e.g 0.707
//  octaveControl(),  e.g 7.0
//  Frequency Control input, e.g close to 1.0 
//  basically if F = Fc*2^(signal*octaves) > 14,744Hz you will get a full volume, random noise 
//  out of the filter if you don't make this hack.
//  https://forum.pjrc.com/threads/67358-AudioFilterStateVariable-unwanted-noise

#define CC_MODULATION_WHEEL               1
#define CC_BREATH                         2
#define CC_PORTA_TIME                     5
#define CC_NRPN_DATA_ENTRY                6
#define CC_NRPN_LSB                       98
#define CC_NRPN_MSB                       99

//------------some define statements --------------------
// #define FREQQ (440)

//------------ includes -------------------------------
#include <Bounce.h>
#include <USBHost_t36.h>
#include <MIDI.h>
#include "patches.h"
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
AudioSynthWaveformDc     dc_timeNoise;   //xy=86.5,979.9999389648438
AudioSynthWaveformDc     dc_breathLfoFilter3; //xy=90.5,1191.9999389648438
AudioSynthWaveformSine   sine_lfoFilter3; //xy=105.5,1158.9999389648438
AudioSynthWaveformDc     dc_portatimef; //xy=111.19999694824219,138.1999969482422
AudioSynthWaveformDc     dc_portatime;   //xy=116.19999694824219,105
AudioSynthWaveformDc     dc_pitchbend;   //xy=120.19999694824219,72
AudioSynthWaveformDc     dc_sweepDepthFilter3; //xy=117.2000732421875,1039.9998168945312
AudioSynthWaveformDc     dc_breathNoise; //xy=249.20001220703125,942.1999816894531
AudioEffectMultiply      sq_timeNoise; //xy=249.20001220703125,977.7999877929688
AudioSynthWaveformDc     dc_sweepDepthFilterSign3; //xy=266.8500061035156,1075.4498901367188
AudioSynthNoiseWhite     pink_Noise;         //xy=271.20001220703125,892.199951171875
AudioMixer4              mix_dcStub;     //xy=286.1999969482422,115
AudioSynthWaveformDc     dc_modOffsetNoiseFilter3; //xy=297.29998779296875,1226.199951171875
AudioEffectMultiply      mult_lfoDepthFilter3; //xy=302.5,1187.9999389648438
AudioSynthWaveformDc     dc_breathNoiseFilter3; //xy=304.5,1151.199951171875
AudioEffectMultiply      sq_swpflt3; //xy=315.4000549316406,1036.7997436523438
AudioSynthWaveformDc     dc_breathLfoFilter1; //xy=396.20001220703125,635
AudioSynthWaveformSine   sine_lfoFilter1; //xy=407.20001220703125,609
AudioMixer4              mix_breathTimeNoise; //xy=439.79998779296875,972.1999816894531
AudioFilterStateVariable filterPreNoise; //xy=453.20001220703125,909.199951171875
AudioSynthWaveformDc     dc_sweepDepthOsc1; //xy=460.1999969482422,61
AudioSynthWaveformDc     dc_breathSweepOsc1; //xy=460.1999969482422,92
AudioSynthWaveformDc     dc_sweepDepthFilter1; //xy=475.20001220703125,509.0000305175781
AudioSynthWaveformDc     dc_breathSweepOsc2; //xy=478.1999969482422,308
AudioSynthWaveformDc     dc_sweepDepthOsc2; //xy=481.1999969482422,276
AudioSynthWaveformDc     dc_beatOsc1;    //xy=488.1999969482422,124
AudioSynthWaveformDc     dc_beatOsc2;    //xy=508.1999969482422,340
AudioEffectMultiply      mlt_DepthFilterSign3; //xy=513.8499755859375,1041.4498901367188
AudioMixer4              mix_fcModFilter3; //xy=517.4999084472656,1176
AudioSynthWaveformSine   sine_lfoOsc1;   //xy=540.1999969482422,199
AudioSynthWaveformDc     dc_pwOsc1;      //xy=542.1999969482422,162
AudioSynthWaveformSine   sine_lfoOsc2;   //xy=543.2000122070312,413.0000305175781
AudioSynthWaveformDc     dc_pwOsc2;      //xy=558.1999969482422,375
AudioSynthWaveformDc     dc_modOffsetOscFilter1; //xy=592,664.2000122070312
AudioEffectMultiply      mult_lfoDepthFilter1; //xy=600.2000122070312,631
AudioSynthWaveformDc     dc_sweepDepthFilterSign1; //xy=601.0499877929688,543.8499755859375
AudioSynthWaveformDc     dc_breathOscFilter1;      //xy=603.2000122070312,599
AudioEffectMultiply      multiply2;      //xy=631.2000122070312,969.9999389648438
AudioEffectMultiply      sq_swpflt1; //xy=662.4000244140625,507.8000183105469
AudioMixer4              mix_pitchModOsc1; //xy=682.1999969482422,105
AudioMixer4              mix_pwOsc1;     //xy=699.1999969482422,186
AudioMixer4              mix_pitchModOsc2; //xy=699.1999969482422,308
AudioMixer4              mix_delayFeedback; //xy=695.199951171875,1569.99951171875
AudioSynthWaveformDc     dc_breathLfoFilter4; //xy=699.2000122070312,1277.9999389648438
AudioSynthWaveformSine   sine_lfoFilter4; //xy=706.2000122070312,1240.9999389648438
AudioMixer4              mix_pwOsc2;     //xy=715.1999969482422,399
AudioSynthWaveformDc     dc_sweepDepthFilter4; //xy=734.199951171875,1123
AudioMixer4              mix_fcModFilter3_sweep; //xy=734.5999755859375,1064.199951171875
AudioMixer4              mix_levelNoise; //xy=789.2000122070312,988.9999389648438
AudioMixer4              mix_fcModFilter1; //xy=843.2000122070312,642
AudioEffectMultiply      mlt_DepthFilterSign1; //xy=853.0499877929688,543.8499755859375
AudioSynthWaveformDc     dc_sweepDepthFilterSign4; //xy=868.8499755859375,1161.449951171875
AudioFilterStateVariable filterPostDelay; //xy=870.2000732421875,1696.1997680664062
AudioSynthWaveformModulated wfmod_triOsc1;  //xy=887.1999969482422,129
AudioSynthWaveformModulated wfmod_sawOsc1;  //xy=893.1999969482422,97
AudioSynthWaveformModulated wfmod_pulseOsc1; //xy=893.1999969482422,160
AudioSynthWaveformDc     dc_breathLfoFilter2; //xy=890.199951171875,937.9999389648438
AudioEffectDelay         dly_delayEffects; //xy=889.199951171875,1568.9996337890625
AudioSynthWaveformSine   sine_lfoFilter2; //xy=893.199951171875,902.9999389648438
AudioSynthWaveformModulated wfmod_triOsc2;  //xy=902.1999969482422,313
AudioEffectMultiply      mult_lfoDepthFilter4; //xy=902.199951171875,1258.9999389648438
AudioSynthWaveformDc     dc_sweepDepthFilter2; //xy=904.199951171875,812.9999389648438
AudioSynthWaveformModulated wfmod_sawOsc2;  //xy=907.1999969482422,282
AudioSynthWaveformDc     dc_breathNoiseFilter4; //xy=907.199951171875,1224.199951171875
AudioSynthWaveformDc     dc_modOffsetNoiseFilter4; //xy=910,1325.199951171875
AudioSynthWaveformModulated wfmod_pulseOsc2; //xy=915.1999969482422,345
AudioEffectMultiply      sq_swpflt4; //xy=917.4000244140625,1125.7999267578125
AudioFilterStateVariable filter3;        //xy=942.2000122070312,1073.9999389648438
AudioSynthWaveformDc     dc_breathThreshOsc2; //xy=1032.2000122070312,240.1999969482422
AudioSynthWaveformDc     dc_breathThreshOsc1;            //xy=1038.2000122070312,46.19999694824219
AudioSynthWaveformDc     dc_sweepDepthFilterSign2; //xy=1048.050048828125,844.8499145507812
AudioMixer4              mix_osc1;       //xy=1083.1999969482422,134
AudioSynthWaveformDc     dc_modOffsetOscFilter2; //xy=1088,1010.199951171875
AudioMixer4              mix_fcModFilter1_sweep; //xy=1091.2000732421875,638
AudioEffectMultiply      mult_lfoDepthFilter2; //xy=1094.199951171875,947.9999389648438
AudioEffectMultiply      sq_swpflt2; //xy=1097.4000244140625,811.7999267578125
AudioSynthWaveformDc     dc_breathOscFilter2; //xy=1098.199951171875,912.199951171875
AudioMixer4              mix_osc2;       //xy=1106.1999969482422,312
AudioMixer4              mix_ntcFilter3; //xy=1107.199951171875,1072.9999389648438
AudioMixer4              mix_delayLevel; //xy=1110.199951171875,1498.999755859375
AudioEffectMultiply      mlt_DepthFilterSign4; //xy=1111.8499755859375,1131.449951171875
AudioMixer4              mix_fcModFilter4; //xy=1162.199951171875,1280.9999389648438
AudioEffectChorus        chor_chorusEffects; //xy=1217.1999816894531,1702.999755859375
AudioMixer4              mix_xfade;      //xy=1252.1999969482422,98
AudioEffectReverb        rvb_reverb;        //xy=1248.199951171875,1648.1997680664062
AudioEffectFreeverb      freeverb1;      //xy=1261.199951171875,1544.1998291015625
AudioFilterStateVariable filter1;        //xy=1300.199951171875,634
AudioEffectMultiply      mlt_DepthFilterSign2; //xy=1310.050048828125,826.8499145507812
AudioMixer4              mix_fcModFilter2; //xy=1320.199951171875,983.9999389648438
AudioMixer4              mix_fcModFilter4_sweep; //xy=1366.1999053955078,1135.7999267578125
AudioEffectMultiply      mult_thCurveOsc2; //xy=1387.1999969482422,274
AudioMixer4              mix_fcModFilter2_sweep; //xy=1413.4000244140625,914.9999389648438
AudioEffectMultiply      mult_thCurveOsc1; //xy=1421.1999969482422,153
AudioMixer4              mix_reverb; //xy=1416.5999755859375,1550.1998901367188
AudioEffectGranular      granular1;      //xy=1441.1999969482422,1696.1998748779297
AudioMixer4              mix_ntcFilter1; //xy=1456.199951171875,628
AudioEffectFlange        flange1;        //xy=1512.1999969482422,1653.1998748779297
AudioFilterStateVariable filter4;        //xy=1546.1999053955078,1104.9999389648438
AudioMixer4              mix_oscLevels;  //xy=1608.199951171875,210
AudioFilterStateVariable filter2;        //xy=1616.199951171875,905.9999389648438
AudioFilterStateVariable filterPreMixLP; //xy=1618.699951171875,1485.4500122070312
AudioInputI2S            i2s2;           //xy=1640.199951171875,1610.1997680664062
AudioMixer4              mix_ntcFilter4; //xy=1708.199951171875,1092.9999389648438
AudioMixer4              mix_lineIn; //xy=1790.199951171875,1562.5999145507812
AudioMixer4              mix_ntcFilter2; //xy=1806.199951171875,890.9999389648438
AudioFilterFIR           fir_formant;    //xy=1858.199951171875,954.9999694824219
AudioMixer4              mix_Amp;        //xy=1878.199951171875,1103.9999389648438
AudioFilterStateVariable filterPreMixHP; //xy=1956.199951171875,1535.2000122070312
AudioFilterStateVariable filter5; //xy=2036.699951171875,1113.4498901367188
AudioOutputI2S           i2s1;           //xy=2143.199951171875,1555.9999389648438
AudioConnection          patchCord1(dc_timeNoise, 0, sq_timeNoise, 0);
AudioConnection          patchCord2(dc_timeNoise, 0, sq_timeNoise, 1);
AudioConnection          patchCord3(dc_breathLfoFilter3, 0, mult_lfoDepthFilter3, 1);
AudioConnection          patchCord4(sine_lfoFilter3, 0, mult_lfoDepthFilter3, 0);
AudioConnection          patchCord5(dc_portatimef, 0, mix_dcStub, 3);
AudioConnection          patchCord6(dc_portatime, 0, mix_dcStub, 2);
AudioConnection          patchCord7(dc_pitchbend, 0, mix_dcStub, 1);
AudioConnection          patchCord8(dc_sweepDepthFilter3, 0, sq_swpflt3, 0);
AudioConnection          patchCord9(dc_sweepDepthFilter3, 0, sq_swpflt3, 1);
AudioConnection          patchCord10(dc_breathNoise, 0, mix_breathTimeNoise, 0);
AudioConnection          patchCord11(sq_timeNoise, 0, mix_breathTimeNoise, 1);
AudioConnection          patchCord12(dc_sweepDepthFilterSign3, 0, mlt_DepthFilterSign3, 1);
AudioConnection          patchCord13(pink_Noise, 0, filterPreNoise, 0);
AudioConnection          patchCord14(dc_modOffsetNoiseFilter3, 0, mix_fcModFilter3, 3);
AudioConnection          patchCord15(mult_lfoDepthFilter3, 0, mix_fcModFilter3, 1);
AudioConnection          patchCord16(dc_breathNoiseFilter3, 0, mix_fcModFilter3, 0);
AudioConnection          patchCord17(sq_swpflt3, 0, mlt_DepthFilterSign3, 0);
AudioConnection          patchCord18(dc_breathLfoFilter1, 0, mult_lfoDepthFilter1, 1);
AudioConnection          patchCord19(sine_lfoFilter1, 0, mult_lfoDepthFilter1, 0);
AudioConnection          patchCord20(mix_breathTimeNoise, 0, multiply2, 1);
AudioConnection          patchCord21(filterPreNoise, 2, multiply2, 0);
AudioConnection          patchCord22(dc_sweepDepthOsc1, 0, mix_pitchModOsc1, 0);
AudioConnection          patchCord23(dc_breathSweepOsc1, 0, mix_pitchModOsc1, 1);
AudioConnection          patchCord24(dc_sweepDepthFilter1, 0, sq_swpflt1, 0);
AudioConnection          patchCord25(dc_sweepDepthFilter1, 0, sq_swpflt1, 1);
AudioConnection          patchCord26(dc_breathSweepOsc2, 0, mix_pitchModOsc2, 1);
AudioConnection          patchCord27(dc_sweepDepthOsc2, 0, mix_pitchModOsc2, 0);
AudioConnection          patchCord28(dc_beatOsc1, 0, mix_pitchModOsc1, 2);
AudioConnection          patchCord29(dc_beatOsc2, 0, mix_pitchModOsc2, 2);
AudioConnection          patchCord30(mlt_DepthFilterSign3, 0, mix_fcModFilter3_sweep, 0);
AudioConnection          patchCord31(mix_fcModFilter3, 0, mix_fcModFilter3_sweep, 1);
AudioConnection          patchCord32(sine_lfoOsc1, 0, mix_pwOsc1, 1);
AudioConnection          patchCord33(dc_pwOsc1, 0, mix_pwOsc1, 0);
AudioConnection          patchCord34(sine_lfoOsc2, 0, mix_pwOsc2, 1);
AudioConnection          patchCord35(dc_pwOsc2, 0, mix_pwOsc2, 0);
AudioConnection          patchCord36(dc_modOffsetOscFilter1, 0, mix_fcModFilter1, 3);
AudioConnection          patchCord37(mult_lfoDepthFilter1, 0, mix_fcModFilter1, 1);
AudioConnection          patchCord38(dc_sweepDepthFilterSign1, 0, mlt_DepthFilterSign1, 1);
AudioConnection          patchCord39(dc_breathOscFilter1, 0, mix_fcModFilter1, 0);
AudioConnection          patchCord40(multiply2, 0, mix_levelNoise, 0);
AudioConnection          patchCord41(sq_swpflt1, 0, mlt_DepthFilterSign1, 0);
AudioConnection          patchCord42(mix_pitchModOsc1, 0, wfmod_sawOsc1, 0);
AudioConnection          patchCord43(mix_pitchModOsc1, 0, wfmod_triOsc1, 0);
AudioConnection          patchCord44(mix_pitchModOsc1, 0, wfmod_pulseOsc1, 0);
AudioConnection          patchCord45(mix_pwOsc1, 0, wfmod_pulseOsc1, 1);
AudioConnection          patchCord46(mix_pitchModOsc2, 0, wfmod_sawOsc2, 0);
AudioConnection          patchCord47(mix_pitchModOsc2, 0, wfmod_triOsc2, 0);
AudioConnection          patchCord48(mix_pitchModOsc2, 0, wfmod_pulseOsc2, 0);
AudioConnection          patchCord49(mix_delayFeedback, dly_delayEffects);
AudioConnection          patchCord50(dc_breathLfoFilter4, 0, mult_lfoDepthFilter4, 1);
AudioConnection          patchCord51(sine_lfoFilter4, 0, mult_lfoDepthFilter4, 0);
AudioConnection          patchCord52(mix_pwOsc2, 0, wfmod_pulseOsc2, 1);
AudioConnection          patchCord53(dc_sweepDepthFilter4, 0, sq_swpflt4, 0);
AudioConnection          patchCord54(dc_sweepDepthFilter4, 0, sq_swpflt4, 1);
AudioConnection          patchCord55(mix_fcModFilter3_sweep, 0, filter3, 1);
AudioConnection          patchCord56(mix_levelNoise, 0, filter3, 0);
AudioConnection          patchCord57(mix_levelNoise, 0, mix_ntcFilter3, 0);
AudioConnection          patchCord58(mix_fcModFilter1, 0, mix_fcModFilter1_sweep, 1);
AudioConnection          patchCord59(mlt_DepthFilterSign1, 0, mix_fcModFilter1_sweep, 0);
AudioConnection          patchCord60(dc_sweepDepthFilterSign4, 0, mlt_DepthFilterSign4, 1);
AudioConnection          patchCord61(filterPostDelay, 0, mix_delayLevel, 1);
AudioConnection          patchCord62(filterPostDelay, 0, mix_delayFeedback, 1);
AudioConnection          patchCord63(wfmod_triOsc1, 0, mix_osc1, 1);
AudioConnection          patchCord64(wfmod_sawOsc1, 0, mix_osc1, 0);
AudioConnection          patchCord65(wfmod_pulseOsc1, 0, mix_osc1, 2);
AudioConnection          patchCord66(dc_breathLfoFilter2, 0, mult_lfoDepthFilter2, 1);
AudioConnection          patchCord67(dly_delayEffects, 7, filterPostDelay, 0);
AudioConnection          patchCord68(sine_lfoFilter2, 0, mult_lfoDepthFilter2, 0);
AudioConnection          patchCord69(wfmod_triOsc2, 0, mix_osc2, 1);
AudioConnection          patchCord70(mult_lfoDepthFilter4, 0, mix_fcModFilter4, 1);
AudioConnection          patchCord71(dc_sweepDepthFilter2, 0, sq_swpflt2, 0);
AudioConnection          patchCord72(dc_sweepDepthFilter2, 0, sq_swpflt2, 1);
AudioConnection          patchCord73(wfmod_sawOsc2, 0, mix_osc2, 0);
AudioConnection          patchCord74(dc_breathNoiseFilter4, 0, mix_fcModFilter4, 0);
AudioConnection          patchCord75(dc_modOffsetNoiseFilter4, 0, mix_fcModFilter4, 3);
AudioConnection          patchCord76(wfmod_pulseOsc2, 0, mix_osc2, 2);
AudioConnection          patchCord77(sq_swpflt4, 0, mlt_DepthFilterSign4, 0);
AudioConnection          patchCord78(filter3, 0, mix_ntcFilter3, 1);
AudioConnection          patchCord79(filter3, 1, mix_ntcFilter3, 2);
AudioConnection          patchCord80(filter3, 2, mix_ntcFilter3, 3);
AudioConnection          patchCord81(dc_breathThreshOsc2, 0, mult_thCurveOsc2, 0);
AudioConnection          patchCord82(dc_breathThreshOsc2, 0, mix_xfade, 1);
AudioConnection          patchCord83(dc_breathThreshOsc1, 0, mix_xfade, 0);
AudioConnection          patchCord84(dc_sweepDepthFilterSign2, 0, mlt_DepthFilterSign2, 1);
AudioConnection          patchCord85(mix_osc1, 0, mult_thCurveOsc1, 1);
AudioConnection          patchCord86(dc_modOffsetOscFilter2, 0, mix_fcModFilter2, 3);
AudioConnection          patchCord87(mix_fcModFilter1_sweep, 0, filter1, 1);
AudioConnection          patchCord88(mult_lfoDepthFilter2, 0, mix_fcModFilter2, 1);
AudioConnection          patchCord89(sq_swpflt2, 0, mlt_DepthFilterSign2, 0);
AudioConnection          patchCord90(dc_breathOscFilter2, 0, mix_fcModFilter2, 0);
AudioConnection          patchCord91(mix_osc2, 0, mult_thCurveOsc2, 1);
AudioConnection          patchCord92(mix_ntcFilter3, 0, filter4, 0);
AudioConnection          patchCord93(mix_ntcFilter3, 0, mix_ntcFilter4, 0);
AudioConnection          patchCord94(mix_delayLevel, freeverb1);
AudioConnection          patchCord95(mix_delayLevel, 0, mix_reverb, 0);
AudioConnection          patchCord96(mlt_DepthFilterSign4, 0, mix_fcModFilter4_sweep, 0);
AudioConnection          patchCord97(mix_fcModFilter4, 0, mix_fcModFilter4_sweep, 1);
AudioConnection          patchCord98(mix_xfade, 0, mult_thCurveOsc1, 0);
AudioConnection          patchCord99(freeverb1, 0, mix_reverb, 1);
AudioConnection          patchCord100(filter1, 0, mix_ntcFilter1, 1);
AudioConnection          patchCord101(filter1, 1, mix_ntcFilter1, 2);
AudioConnection          patchCord102(filter1, 2, mix_ntcFilter1, 3);
AudioConnection          patchCord103(mlt_DepthFilterSign2, 0, mix_fcModFilter2_sweep, 0);
AudioConnection          patchCord104(mix_fcModFilter2, 0, mix_fcModFilter2_sweep, 1);
AudioConnection          patchCord105(mix_fcModFilter4_sweep, 0, filter4, 1);
AudioConnection          patchCord106(mult_thCurveOsc2, 0, mix_oscLevels, 1);
AudioConnection          patchCord107(mix_fcModFilter2_sweep, 0, filter2, 1);
AudioConnection          patchCord108(mult_thCurveOsc1, 0, mix_oscLevels, 0);
AudioConnection          patchCord109(mix_reverb, 0, filterPreMixLP, 0);
AudioConnection          patchCord110(mix_ntcFilter1, 0, filter2, 0);
AudioConnection          patchCord111(mix_ntcFilter1, 0, mix_ntcFilter2, 0);
AudioConnection          patchCord112(filter4, 0, mix_ntcFilter4, 1);
AudioConnection          patchCord113(filter4, 1, mix_ntcFilter4, 2);
AudioConnection          patchCord114(filter4, 2, mix_ntcFilter4, 3);
AudioConnection          patchCord115(mix_oscLevels, 0, filter1, 0);
AudioConnection          patchCord116(mix_oscLevels, 0, mix_ntcFilter1, 0);
AudioConnection          patchCord117(filter2, 0, mix_ntcFilter2, 1);
AudioConnection          patchCord118(filter2, 1, mix_ntcFilter2, 2);
AudioConnection          patchCord119(filter2, 2, mix_ntcFilter2, 3);
AudioConnection          patchCord120(filterPreMixLP, 0, mix_lineIn, 0);
AudioConnection          patchCord121(filterPreMixLP, 1, mix_lineIn, 1);
AudioConnection          patchCord122(i2s2, 0, mix_lineIn, 2);
AudioConnection          patchCord123(i2s2, 1, mix_lineIn, 3);
AudioConnection          patchCord124(mix_ntcFilter4, 0, mix_Amp, 1);
AudioConnection          patchCord125(mix_lineIn, 0, filterPreMixHP, 0);
AudioConnection          patchCord126(mix_ntcFilter2, fir_formant);
AudioConnection          patchCord127(fir_formant, 0, mix_Amp, 0);
AudioConnection          patchCord128(mix_Amp, 0, filter5, 0);
AudioConnection          patchCord129(filterPreMixHP, 2, i2s1, 0);
AudioConnection          patchCord130(filterPreMixHP, 2, i2s1, 1);
AudioConnection          patchCord131(filter5, 2, mix_delayFeedback, 0);
AudioConnection          patchCord132(filter5, 2, mix_delayLevel, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=250.1999969482422,172
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
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     33 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);


//------- Encoder knob, button and UI state machine stuff----------------

Encoder knob(30, 31);
int32_t newKnob = 0;
const int knobButtonPin = 32;
const int patchNextButton = 29;
const uint32_t debounceDelay = 75; // 50 ms
uint32_t patchNextButtonPressedTime = 0;
uint32_t knobButtonPressedTime = 0;
Bounce knobButton(knobButtonPin, debounceDelay);
int currentKnobButtonState = 0;
int oldKnobButtonState = 0;
bool longKnobButtonPressPending = false;
bool longKnobButtonPress = false;
int longKnobButtonPressTime = 500;  // milliiseconds
bool shortKnobButtonPress = false;
bool firstKnobButtonPress = true;
bool KnobButtonRising = false;
bool submenu_active = false;
const uint32_t UITimeoutInterval = 7000;  // milliseconds
typedef enum {SPLASH_SCREEN, VOL_ADJ, PATCH_SEL, MENU} uism_t;
uism_t UISM = SPLASH_SCREEN;
typedef enum {MENU_EXIT, MENU_MIX, MENU_TUNING} menusm_t;
// TODO: typedef enum {MENU_EXIT, MENU_MIX, MENU_TUNING, MENU_TRANS, MENU_OCT } menusm_t;
menusm_t MENUSM = MENU_EXIT;


//const uint32_t gatherSensorInterval = 50;  // milliseconds
const uint32_t readKnobInterval = 50;  // milliseconds


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
char str_buf[64] ={"version: .18"};
char str_buf1[64] ={"version: .18"};
char str_oledbuf[64] ={"Windy 1, ver: .18"};
bool PRINT_VALUES_FLAG = true;
char version_str[] = {"Windy 1, ver: .18"};


// globals for loop control
uint32_t previousMillis = 0;
uint32_t currentMillis = 0;  
uint32_t previousUsageTime = 0;
uint32_t previousUITimeoutTime = 0;
uint32_t currentUITimeoutTime = 0;
uint32_t previousDebugPrintTime = 0;

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
float dc_breathFilterN_rampTime = 8.0;
float dc_breathNoise_amp = 0.0;
float dc_breathThreshOsc1_amp = 0.0;
float dc_breathThreshOsc2_amp = 0.0;
float dc_breathThreshNoise_amp = 0.0;
float dc_breathThreshOscN_rampTime = 8.0;
float dc_breathLfoFilter1_amp = 0.0;
float dc_breathLfoFilter2_amp = 0.0;
float dc_breathLfoFilter3_amp = 0.0;
float dc_breathLfoFilter4_amp = 0.0;
float dc_portatime_val = 0.0;
float dc_portatime_range = 500.0/12.0; // number of ms when _val is 1.0
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
int32_t vol = 75;
int32_t eeprom_vol = 75;
float volf = 1.5;
int32_t mix_linein = 0;
int32_t eeprom_mix_linein = 0;
float mix_lineinf = 0.0;
int32_t FineTuneCents = 0;  	// 100 cents per halfstep, FineTune Control +/- 100
int32_t eeprom_FineTuneCents = 0;
float FineTuneCentsf = 0.0;  	

// synth variable limits and references
float maxPwmLfoFreq = 10.0;         // 4000s is 10 Hz at 100%
float maxPwmDepthOsc = 0.60;        // 4000s is +/- 30% at 100% depth
float maxSweepTimeOsc = 1500.0;       // TODO: set this to match 4000s
float sweepTimeOscGamma = 4.0;       // TODO: adjust this to match 4000s
float maxSweepTimeOscFilter = 2000.0; // 255.0;
float sweepTimeOscFilterGamma = 4.0;       // TODO: adjust this to match 4000s
float maxSweepTimeNoiseFilter = 1000.0;
float sweepTimeNoiseFilterGamma = 4.0;       // TODO: adjust this to match 4000s
float maxSweepDepthFilter = 1.0;   // TODO: set this to match 4000s
float maxLfoFreqFilter1 = 50.0;
float maxLfoFreqFilter2 = 50.0;
float maxLfoFreqFilter3 = 50.0;
float maxLfoFreqFilter4 = 50.0;
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
float octaveControlFilter1 = 7.0;
float octaveControlFilter2 = 7.0;
float octaveControlFilter3 = 7.0;
float octaveControlFilter4 = 7.0;
float octaveControlFilter5 = 0.0;
float offsetNoteKeyfollow = 86.0;  // 84 = C6, 72 = C5, 60 = C4
float minPreNoiseNoteNumbr = 60.0;  // 84 = C6, 72 = C5, 60 = C4  4000s noise stops changing below about C4
const uint32_t updatedAudioSystemInterval = 1;  // milliseconds
float freqOscFilterOctaveOffset  = 0.0;    // use 3 to divide FreqOscFilterN by 2^3 to allow modulation to go from -1 to +3/7 
//float fOFOFactor = pow(2,-freqOscFilterOctaveOffset); //TODO: fix this 
float fOFOFactor = 1.0; 
//float fOFOHack = freqOscFilterOctaveOffset/octaveControlFilter1;
float fOFOHack = 0;
float maxFilterFreq = 12000.0; 
float minFilterFreq = 65.4; // min note number 36
float minFilterPreNoiseFreq = 261.63; // middle C (C4)
float maxDelayDamp = 3000.0; //TODO: find out correct value
float maxDelayLevel = 0.15; //TODO: find out correct value
float gammaDelayLevel = 3.0; //TODO: find out correct value
float gammaDelayFeedback = 1.5; //TODO: find out correct value
float maxTimeNoise = 2000;  // 1000 ms
float TimeNoiseGamma = 4.0;  
float maxNoiseLevel = 1.0; //0.23;  
float minGamma = 0.1;
float maxGamma = 2.0;
float maxReverbLevel = 0.20;
float maxDenseEarly = 0.5; 
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
#define CHORUS_DELAY_LENGTH (16*AUDIO_BLOCK_SAMPLES)
// Allocate the delay lines for left and right channels
short l_delayline[CHORUS_DELAY_LENGTH];
short r_delayline[CHORUS_DELAY_LENGTH];
int n_chorus = 1;


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
float KeyFollowPreNoiseFilter = 4;  // TODO: match 4000s
float keyfollowFilterPreNoise = 1.0; 
float FreqPreNoiseFilter = 3000.0; // TODO: match 4000s
float NoiseTime = 0.0;  	//80,0,0,127,
float TimeNoiseAmp = 0.25;  	//80,0,0,127,
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
float EffectsDelayTime = 0;  	//113,0,0,127,
float EffectsDelayFeedback = 0; //113,1,0,127,
float EffectsDelayDamp = 10000;  	//113,2,0,127,
float EffectsDelayLevel = 0;  	//113,3,0,127,
float EffectsDelaySpare = 1.0;  	//113,3,0,127,
float EffectsReverbSpare = 1.0;  	//114,1,0,127,
float EffectsReverbLevel = 0;  	//114,1,0,127,
float EffectsReverbDenseEarly = 0;  	//114,2,0,127,
float EffectsReverbTime = 0;  	//114,3,10,50,//1.0 to 5.0 sec
float EffectsReverbDamp = 0;  	//114,4,54,74,//-10 to +10

filterMode_t modeFilter1_old = THRU;
filterMode_t modeFilter2_old = THRU;
filterMode_t modeFilter3_old = THRU;
filterMode_t modeFilter4_old = THRU;

void setup() {
  // put your setup code here, to run once:
  pinMode(patchNextButton, INPUT_PULLUP);
  pinMode(knobButtonPin, INPUT_PULLUP);
  //Serial1.begin(1000000);
  Serial8.begin(1000000);
  Serial.begin(1000000);
  configureSD();
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.80);
  AudioMemory(540);  //TODO: how much AudioMemory do I need? (delay 2.9ms per block for 1270/2.9 = 438
 // sgtl5000_1.audioPostProcessorEnable();
 // //sgtl5000_1.surroundSoundEnable();
// //sgtl5000_1.surroundSound(7, 3);
//  sgtl5000_1.adcHighPassFilterDisable();
//  sgtl5000_1.eqSelect(3);//5-band eq
//  sgtl5000_1.eqBands(eq_1, eq_2, eq_3, eq_4, eq_5);
sgtl5000_1.lineOutLevel(13);  //3.16V p-p
sgtl5000_1.lineInLevel(0);    // 3.12V p-p
  AudioNoInterrupts();
  dc_pwOsc1.amplitude(PwOsc1); // default 50% dutycycle
  sine_lfoOsc1.amplitude(PwmDepthOsc1);
  sine_lfoOsc1.frequency(PwmFreqOsc1);
  dc_pwOsc2.amplitude(PwOsc2); // default 50% dutycycle
  sine_lfoOsc2.amplitude(PwmDepthOsc2);
  sine_lfoOsc2.frequency(PwmFreqOsc2);
  mix_pwOsc1.gain(0,1.0);  // use dc_pwOsc1.ammplitude to adjust
  mix_pwOsc1.gain(1,maxPwmDepthOsc);  // 
  mix_pwOsc2.gain(0,1.0);  // use dc_pwOsc1.ammplitude to adjust
  mix_pwOsc2.gain(1,maxPwmDepthOsc);  //
  
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
  filterPostDelay.frequency(EffectsDelayDamp);
  filterPostDelay.resonance(0.707);
  filterPreMixHP.frequency(100);
  filterPreMixHP.resonance(0.707);
  filterPreMixLP.frequency(15000);  // LP filter
  filterPreMixLP.resonance(0.707);
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

    // TODO: copy to wherever patch changes are made
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
    mix_osc1.gain(1, 1.0);
    mix_osc1.gain(2, 1.0);
    mix_osc1.gain(3, 0.0);
    mix_osc2.gain(0, 1.0);
    mix_osc2.gain(1, 1.0);
    mix_osc2.gain(2, 1.0);
    mix_osc2.gain(3, 0.0);
    mix_oscLevels.gain(0, LevelOsc1);
    mix_oscLevels.gain(1, LevelOsc2);
    mix_levelNoise.gain(0, 1.0); // pass thru for now
    //mix_levelNoise.gain(1, 1.0); // Debug path. 
    mix_breathTimeNoise.gain(0, 1.0); // TODO: set balance between breath and Time for noise  
    mix_breathTimeNoise.gain(1, 1.0); // TODO: set balance between breath and Time for nois

    mix_fcModFilter1.gain(0, 1.0);  
    mix_fcModFilter1.gain(1, 1.0);  
    //mix_fcModFilter1.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter1.gain(2, 0.0);  
    mix_fcModFilter1.gain(3, modOffsetFilter1);  
    mix_fcModFilter1_sweep.gain(0, maxSweepDepthFilter);  
    mix_fcModFilter1_sweep.gain(1, 1.0);  
    mix_fcModFilter1_sweep.gain(2, 0.0);  
    mix_fcModFilter1_sweep.gain(3, 0.0);  

    mix_fcModFilter2.gain(0, 1.0);  
    mix_fcModFilter2.gain(1, 1.0);  
    //mix_fcModFilter2.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter2.gain(2, 0.0);  
    mix_fcModFilter2.gain(3, modOffsetFilter2);  
    mix_fcModFilter2_sweep.gain(0, maxSweepDepthFilter);  
    mix_fcModFilter2_sweep.gain(1, 1.0);  
    mix_fcModFilter2_sweep.gain(2, 0.0);  
    mix_fcModFilter2_sweep.gain(3, 0.0);  

    mix_fcModFilter3.gain(0, 1.0);  
    mix_fcModFilter3.gain(1, 1.0);  
    //mix_fcModFilter3.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter3.gain(2, 0.0);  
    mix_fcModFilter3.gain(3, modOffsetFilter3);  
    mix_fcModFilter3_sweep.gain(0, maxSweepDepthFilter);  
    mix_fcModFilter3_sweep.gain(1, 1.0);  
    mix_fcModFilter3_sweep.gain(2, 0.0);  
    mix_fcModFilter3_sweep.gain(3, 0.0);  

    mix_fcModFilter4.gain(0, 1.0);  
    mix_fcModFilter4.gain(1, 1.0);  
    //mix_fcModFilter4.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter4.gain(2, 0.0);  
    mix_fcModFilter4.gain(3, modOffsetFilter4);  
    mix_fcModFilter4_sweep.gain(0, maxSweepDepthFilter);  
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

    mix_Amp.gain(0, AmpLevel);
    mix_Amp.gain(1, 1.0);  // 4000s AmpLevel doesn't control Noise Level
   
 
    chor_chorusEffects.begin(l_delayline,CHORUS_DELAY_LENGTH,n_chorus); 
    mix_delayFeedback.gain(0,1.0);
    mix_delayFeedback.gain(1,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
    dly_delayEffects.delay(0, 0);
    dly_delayEffects.delay(7, EffectsDelayTime);
    dly_delayEffects.disable(1);
    dly_delayEffects.disable(2);
    dly_delayEffects.disable(3);
    dly_delayEffects.disable(4);
    dly_delayEffects.disable(5);
    dly_delayEffects.disable(6);

    freeverb1.roomsize(EffectsReverbDenseEarly);
    freeverb1.damping(EffectsReverbDamp);
//    rvb_reverb.reverbTime(EffectsReverbTime);
    mix_delayLevel.gain(0,maxDelayLevel);
    mix_delayLevel.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));

  Serial.println(str_buf);
 // Serial1.println(str_buf1);
  Serial8.println(str_buf1);
  delay(200);

  myUSBHost.begin();
 
  loadAllPatches();
  loadPatchNumberEEPROM();
    sprintf(str_buf1, "read eeprom_patchNumber (%03d)", eeprom_patchNumber );     
    Serial8.println(str_buf1);
  if(eeprom_patchNumber < 0 || eeprom_patchNumber >= NUMBER_OF_PATCHES)
  {
      sprintf(str_buf1, "loadPatchNumberEEPROM (%03d) not within 0 to NUMBER_OF_PATCHES-1. overwrite with 000.", eeprom_patchNumber ); 
      Serial8.println(str_buf1);
      eeprom_patchNumber = 0;
      savePatchNumberEEPROM();
  }

    
    loadFineTuneCentsEEPROM();
    sprintf(str_buf1, "read eeprom_FineTuneCents (%03ld)", eeprom_FineTuneCents );     
    Serial8.println(str_buf1);
    if(eeprom_FineTuneCents < -100 || eeprom_FineTuneCents > 100)
    {
        eeprom_FineTuneCents = 0;
        saveFineTuneCentsEEPROM();
    }
    FineTuneCents = eeprom_FineTuneCents; 
    FineTuneCentsf =  ((float)FineTuneCents)/100.0f;

    loadMixLineinEEPROM();
    sprintf(str_buf1, "read eeprom_mix_linein (%03ld)", eeprom_mix_linein );     
    Serial8.println(str_buf1);
    if(eeprom_mix_linein <0 || eeprom_mix_linein > 100)
    {
        eeprom_mix_linein = 0;
        saveMixLineinEEPROM();
    }
    mix_linein = eeprom_mix_linein; 
    mix_lineinf =  ((float)mix_linein)/100.0f;

    loadVolEEPROM();
    sprintf(str_buf1, "read eeprom_vol (%03ld)", eeprom_vol );     
    Serial8.println(str_buf1);
    if(eeprom_vol <0 || eeprom_vol > 100)
    {
        eeprom_vol = 0;
        saveVolEEPROM();
    }
    vol = eeprom_vol;
    //volf = ((float)vol)/50.0f;
    volf = ((float)vol)/100.0f;
    volf = (volf*volf)*2.0f;

    mix_lineIn.gain(0, volf*extraAmpFactor*(1.0-mix_lineinf));
    mix_lineIn.gain(1, 0.0);
    mix_lineIn.gain(2, volf*extraLineInAmpFactor*mix_lineinf);
    mix_lineIn.gain(3, volf*extraLineInAmpFactor*mix_lineinf);


  sprintf(str_buf1, "loadPatchNumberEEPROM (%03d) ", eeprom_patchNumber );     
  Serial8.println(str_buf1);
  sprintf(str_buf1, "current_patchNumber (%03d) ", current_patchNumber );     
  Serial8.println(str_buf1);
  if (patchLoaded[eeprom_patchNumber]){
      current_patchNumber = eeprom_patchNumber;
      sprintf(str_buf1, "current_patchNumber (%03d) ", current_patchNumber );     
      Serial8.println(str_buf1);
      setCurrentPatch(current_patchNumber);
      patchToSynthVariables(&current_patch);
  }
  patchToSynthVariables(&current_patch);
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

    display.setTextSize(1); // configuration of size of caracters caractères
    display.setTextColor(WHITE);
    display.clearDisplay(); // erase display
    display.display(); // refresh display
    display.setCursor(1,5);
    display.println(str_oledbuf);
    display.display();
    resetUITimeout();
  }


}

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
        // gatherSensorsData();
        // print somee stuff for debugging
        // sprintf(str_buf, "%d,%d", fslpPressure, fslpPosition);
        // Serial.println(str_buf);
        // readPot();
        readKnobAndKnobButton();
    }
    if (currentUITimeoutTime - previousUITimeoutTime >= UITimeoutInterval)
    {
        UITimeout();
    }
  
    //-------------------------------------------------------
    //  Calculate some parameters
    //  that will be used to update the Audio System 
    //  during AudioNoInterrupts()
    //-------------------------------------------------------
    if (0) {
        if(BreathAttainOsc1 > 0.0)
            dc_breathSweepOsc1.amplitude(BreathDepthOsc1*(1.0-limit(dc_breathThreshOsc1.read()/BreathAttainOsc1,1.0,-1.0)),2.0); // 2ms fudge filter
        else
            dc_breathSweepOsc1.amplitude(0.0);

        if(BreathAttainOsc2 > 0.0)
            dc_breathSweepOsc2.amplitude(BreathDepthOsc2*(1.0-limit(dc_breathThreshOsc2.read()/BreathAttainOsc2,1.0,-1.0)),2.0); // 2ms fudge filter
        else
            dc_breathSweepOsc2.amplitude(0.0);
    }

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
        mix_oscLevels.gain(0, LevelOsc1);
        mix_oscLevels.gain(1, LevelOsc2);
        dc_pwOsc1.amplitude(PwOsc1); // default 50% dutycycle
        sine_lfoOsc1.amplitude(PwmDepthOsc1);
        sine_lfoOsc1.frequency(PwmFreqOsc1);
        dc_pwOsc2.amplitude(PwOsc2); // default 50% dutycycle
        sine_lfoOsc2.amplitude(PwmDepthOsc2);
        sine_lfoOsc2.frequency(PwmFreqOsc2);
        sine_lfoFilter1.frequency(LfoFreqOscFilter1);    
        sine_lfoFilter2.frequency(LfoFreqOscFilter2);    
        sine_lfoFilter3.frequency(LfoFreqNoiseFilter3);    
        sine_lfoFilter4.frequency(LfoFreqNoiseFilter4);    
        wfmod_sawOsc1.amplitude(SawOsc1);
        wfmod_triOsc1.amplitude(TriOsc1);
        wfmod_pulseOsc1.amplitude(PulseOsc1);
        wfmod_sawOsc2.amplitude(SawOsc2);
        wfmod_triOsc2.amplitude(TriOsc2);
        wfmod_pulseOsc2.amplitude(PulseOsc2);
        pink_Noise.amplitude(NoiseLevel);  //
        freeverb1.roomsize(EffectsReverbDenseEarly);
        freeverb1.damping(EffectsReverbDamp);
        filterPostDelay.frequency(EffectsDelayDamp);
//        rvb_reverb.reverbTime(EffectsReverbTime);
        //mix_reverb.gain(0, 1.0-EffectsReverbLevel);
        mix_reverb.gain(0, 1.0);
        mix_reverb.gain(1, EffectsReverbLevel);
        mix_Amp.gain(0, AmpLevel);
        mix_Amp.gain(1, 1.0);  // 4000s AmpLevel doesn't control Noise Level
        mix_lineIn.gain(0, volf*extraAmpFactor*(1.0-mix_lineinf));
        mix_lineIn.gain(1, 0.0);
        mix_lineIn.gain(2, volf*extraLineInAmpFactor*mix_lineinf);
        mix_lineIn.gain(3, volf*extraLineInAmpFactor*mix_lineinf);
        dly_delayEffects.delay(7, EffectsDelayTime);
        mix_delayFeedback.gain(1,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
        mix_delayLevel.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));       //finer adjustment at low end
        updateSynthVariablesFlag = false;
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
                    + SemiOsc1+FineOsc1 + FineTuneCentsf;
    //noteNumberFilter1 = dc_portatimef.read()*128 + OctOsc1*12.0 + SemiOsc1+FineOsc1;
    noteNumberFilter1 = dc_portatimef.read()*128 + SemiOsc1+FineOsc1;
    noteNumberOsc2 = porta_step ? round(dc_portatime.read()*128.0) : dc_portatime.read()*128;
    noteNumberOsc2 = BendStep ? round( noteNumberOsc2 + BendRange * dc_pitchbend.read() )
                                     : noteNumberOsc2 + BendRange * dc_pitchbend.read();
    noteNumberOsc2 = noteNumberOsc2 + OctOsc2*12.0
                    + SemiOsc2+FineOsc2 + FineTuneCentsf;
    //noteNumberFilter2 = dc_portatimef.read()*128 + OctOsc2*12.0 + SemiOsc2+FineOsc2;
    noteNumberFilter2 = dc_portatimef.read()*128 + SemiOsc2+FineOsc2;
    noteFreqOsc1 = 440.0 * pow(2, (noteNumberOsc1-69.0)/12 );  // 69 is note number for A4=440Hz
    noteFreqOsc2 = 440.0 * pow(2, (noteNumberOsc2-69.0)/12 );  // 69 is note number for A4=440Hz
    //noteFreqFilter5 = 440.0 * pow(2, (noteNumberOsc1-69.0-18.0)/12 );  // always Oct below noteNumberOsc1  TODO: match 4000s
    noteFreqFilter5 = 440.0 * pow(2, (noteNumberOsc1-69.0-12.0)/12 );  // always Oct below noteNumberOsc1  TODO: match 4000s
    dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathThreshOscN_rampTime);
    dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,dc_breathThreshOscN_rampTime);      
    keyfollowFilter1 = pow(2, (noteNumberFilter1-offsetNoteKeyfollow)*KeyFollowOscFilter1/144.0); //72 is C5   
    keyfollowFilter2 = pow(2, (noteNumberFilter2-offsetNoteKeyfollow)*KeyFollowOscFilter2/144.0); //72 is C5 
    keyfollowFilter3 = pow(2, (noteNumberFilter1-offsetNoteKeyfollow)*KeyFollowNoiseFilter3/144.0); //72 is C5
    keyfollowFilter4 = pow(2, (noteNumberFilter1-offsetNoteKeyfollow)*KeyFollowNoiseFilter4/144.0); //72 is C5 
    keyfollowFilterPreNoise = pow(2, ( (noteNumberFilter1 < minPreNoiseNoteNumbr ? minPreNoiseNoteNumbr : noteNumberFilter1)- offsetNoteKeyfollow )
                                *KeyFollowPreNoiseFilter/144.0); //72 is C5 
    wfmod_sawOsc1.frequency(noteFreqOsc1);
    wfmod_triOsc1.frequency(noteFreqOsc1);
    wfmod_pulseOsc1.frequency(noteFreqOsc1);
    wfmod_sawOsc2.frequency(noteFreqOsc2);
    wfmod_triOsc2.frequency(noteFreqOsc2);
    wfmod_pulseOsc2.frequency(noteFreqOsc2);
    // octaveControlFilter<N> can only go +/-7 Octaves and we need -10, so we hack the Freq fOFOFacter down 3 octaves and change
    // modulation to go from -7 to +3 octaves.
    clippedFreqFilter1 = (keyfollowFilter1*FreqOscFilter1*fOFOFactor < minFilterFreq)  // fOFOFactor needs fOFOHack below
                        ? minFilterFreq : (keyfollowFilter1*FreqOscFilter1*fOFOFactor < maxFilterFreq) 
                        ? keyfollowFilter1*FreqOscFilter1*fOFOFactor : maxFilterFreq;  
    clippedFreqFilter2 = (keyfollowFilter2*FreqOscFilter2*fOFOFactor < minFilterFreq)  
                        ? minFilterFreq : (keyfollowFilter2*FreqOscFilter2*fOFOFactor < maxFilterFreq) 
                        ? keyfollowFilter2*FreqOscFilter2*fOFOFactor : maxFilterFreq;  
    clippedFreqFilter3 = (keyfollowFilter3*FreqNoiseFilter3*fOFOFactor < minFilterFreq)  
                        ? minFilterFreq : (keyfollowFilter3*FreqNoiseFilter3*fOFOFactor < maxFilterFreq) 
                        ? keyfollowFilter3*FreqNoiseFilter3*fOFOFactor : maxFilterFreq;  
    clippedFreqFilter4 = (keyfollowFilter4*FreqNoiseFilter4*fOFOFactor < minFilterFreq)  
                        ? minFilterFreq : (keyfollowFilter4*FreqNoiseFilter4*fOFOFactor < maxFilterFreq) 
                        ? keyfollowFilter4*FreqNoiseFilter4*fOFOFactor : maxFilterFreq;  
    clippedFreqFilterPreNoise = keyfollowFilterPreNoise*FreqPreNoiseFilter < minFilterPreNoiseFreq // TODO: add offset or factor relative to current note freq
                        ? minFilterPreNoiseFreq : keyfollowFilterPreNoise*FreqPreNoiseFilter < maxFilterFreq
                        ? keyfollowFilterPreNoise*FreqPreNoiseFilter : maxFilterFreq;
    filter1.frequency(clippedFreqFilter1);  
    filter1.resonance(QFactorOscFilter1);   // Q factor
    mix_fcModFilter1.gain(0,fOFOHack + BreathModOscFilter1); // allow breath to go from 0 to Hack-BM (fOFOHack needs fOFOFactor above)
    mix_fcModFilter1.gain(3, modOffsetFilter1); 
    dc_modOffsetOscFilter1.amplitude(-BreathModOscFilter1);  // subtract off BM, 
    if (LinkOscFilters) 
    {
        filter2.frequency(clippedFreqFilter1);
        filter2.resonance(QFactorOscFilter1);   // Q factor
        mix_fcModFilter2.gain(0, fOFOHack + BreathModOscFilter1); 
        mix_fcModFilter2.gain(3, modOffsetFilter1); 
        ModeOscFilter2 = ModeOscFilter1;    
        dc_modOffsetOscFilter2.amplitude(-BreathModOscFilter1);
    }
    else
    {
        filter2.frequency(clippedFreqFilter2);
        filter2.resonance(QFactorOscFilter2);   // Q factor
        mix_fcModFilter2.gain(0, fOFOHack + BreathModOscFilter2); 
        mix_fcModFilter2.gain(3, modOffsetFilter2); 
        dc_modOffsetOscFilter2.amplitude(-BreathModOscFilter2);
    }
    filter3.frequency(clippedFreqFilter3);  
    filter3.resonance(QFactorNoiseFilter3);   // Q factor
    mix_fcModFilter3.gain(0, fOFOHack + BreathModNoiseFilter3); 
    mix_fcModFilter3.gain(3, modOffsetFilter3); 
    dc_modOffsetNoiseFilter3.amplitude(-BreathModNoiseFilter3);
    if (LinkNoiseFilters)
    {
        filter4.frequency(clippedFreqFilter3);
        filter4.resonance(QFactorNoiseFilter3);   // Q factor
        mix_fcModFilter4.gain(0, fOFOHack + BreathModNoiseFilter3); 
        mix_fcModFilter4.gain(3, modOffsetFilter4); 
        ModeNoiseFilter4 = ModeNoiseFilter3;    
        dc_modOffsetNoiseFilter4.amplitude(-BreathModNoiseFilter3);
    }
    else
    {
        filter4.frequency(clippedFreqFilter4);
        filter4.resonance(QFactorNoiseFilter4);   // Q factor
        mix_fcModFilter4.gain(0, fOFOHack + BreathModNoiseFilter4); 
        mix_fcModFilter4.gain(3, modOffsetFilter4); 
        dc_modOffsetNoiseFilter4.amplitude(-BreathModNoiseFilter4);
    }
    filterPreNoise.frequency(clippedFreqFilterPreNoise); 
    filter5.frequency(noteFreqFilter5); // HP filter post mix_Amp
    filterPreMixHP.frequency(noteFreqFilter5);


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
    
    if(1) //if(millis() - previousDebugPrintTime >=500)
    {
        if(PRINT_VALUES_FLAG)
        {
            printPatchValues();   
            PRINT_VALUES_FLAG = false;
            // sprintf(str_buf1, "clippedFreqFilterPreNoise: %5.3f ", clippedFreqFilterPreNoise );     
            Serial8.println(str_buf1);
        }
        //previousDebugPrintTime = millis();
        //sprintf(str_buf1, "foo: %5.3f", foo );      
        //Serial8.println(str_buf1); 
    }


    //-------------------------------------------------------
    //  Read and process incomming Midi 
    //-------------------------------------------------------
    while ( midi_ho.read() ) 
    {
        processMIDI(); // process midi coming from the ewi
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
                            ||(eeprom_vol != vol) ))
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
            updatePatchNumberEEPROM();
            eeprom_vol = vol;
            sprintf(str_buf1, "Writing vol (%03ld) to EEPROM", eeprom_vol );     
            Serial8.println(str_buf1);
            updateVolEEPROM();
            eeprom_mix_linein = mix_linein;
            sprintf(str_buf1, "Writing mix_linein (%03ld) to EEPROM", eeprom_mix_linein );     
            Serial8.println(str_buf1);
            updateMixLineinEEPROM();
            eeprom_FineTuneCents = FineTuneCents;
            sprintf(str_buf1, "Writing FineTuneCents (%03ld) to EEPROM", eeprom_FineTuneCents );     
            Serial8.println(str_buf1);
            updateFineTuneCentsEEPROM();
            updateEpromFlag = false;
        }
    }

 
}

//-------------------
// TODO: gather sensor data for other sensors
//--------------------
// void gatherSensorsData(void){}


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
      //float gamma = pow( x, 2.5)*12.0; 
    return gamma;
}

float gen_filter_gamma(float input) {
       float x = 1.0-input; 
      // float gamma = pow( x, 2.75)*12.8; 
       float gamma = pow( x, 1.0)*2.0; 
    return gamma;
}

float gen_noise_gamma(float input) {
       float x = 1.0-input; 
      // float gamma = pow( x, 2.75)*12.8; 
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
        return (x < th) ? limit( depth*breath*(x/th - 1.0),1.0,0.0) : 0.0;
    }
    else
    {
        return (x > th) ? limit( depth*breath*(x-th)/(1.0-th),1.0,0.0) : 0.0;
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
                mix_ntcFilter1.gain(3,0.0);  // HP
                break;
            case LP:
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
void processMIDI(void) 
{
    // Note: callback method was too slow
    //      see https://www.pjrc.com/teensy/td_midi.html
      type =       midi_ho.getType();
      data1 =      midi_ho.getData1();
      data2 =      midi_ho.getData2();
      channel =    midi_ho.getChannel();
     // const uint8_t *sys = midi_ho.getSysExArray();
     // sprintf(str_buf, "type: %d, data1: %d, data2: %d, channel: %d", type,data1, data2, channel);
     // Serial.println(str_buf);

    switch (type) 
    {
        case midi_ho.ControlChange: //0xB0
            switch (data1){
                case CC_BREATH:
                    data2f = ((float)data2) * DIV127;
                    dc_breathLfoFilter1_amp = lfoThresh(data2f,LfoThreshOscFilter1,LfoDepthOscFilter1,LfoBreathOscFilter1);
                    dc_breathLfoFilter2_amp = lfoThresh(data2f,LfoThreshOscFilter2,LfoDepthOscFilter2,LfoBreathOscFilter2);
                    dc_breathLfoFilter3_amp = lfoThresh(data2f,LfoThreshNoiseFilter3,LfoDepthNoiseFilter3,LfoBreathNoiseFilter3);
                    dc_breathLfoFilter4_amp = lfoThresh(data2f,LfoThreshNoiseFilter4,LfoDepthNoiseFilter4,LfoBreathNoiseFilter4);
                    dc_breathLfoFilter1.amplitude(dc_breathLfoFilter1_amp,6);
                    dc_breathLfoFilter2.amplitude(dc_breathLfoFilter2_amp,6);
                    dc_breathLfoFilter3.amplitude(dc_breathLfoFilter3_amp,6);
                    dc_breathLfoFilter4.amplitude(dc_breathLfoFilter4_amp,6);
                    //dc_breathOscFilter1_amp = lin_to_log(data2, 127, BreathCurveOscFilter1)*DIV127;
                    dc_breathOscFilter1_amp = gamma_func(data2f, BreathCurveOscFilter1);
                    //dc_breathOscFilter2_amp = lin_to_log(data2, 127, BreathCurveOscFilter2)*DIV127;
                    dc_breathOscFilter2_amp = gamma_func(data2f, BreathCurveOscFilter2);
                    //dc_breathNoiseFilter3_amp = lin_to_log(data2, 127, BreathCurveNoiseFilter3)*DIV127;
                    dc_breathNoiseFilter3_amp = gamma_func(data2f, BreathCurveNoiseFilter3);
                    //dc_breathNoiseFilter4_amp = lin_to_log(data2, 127, BreathCurveNoiseFilter4)*DIV127;
                    dc_breathNoiseFilter4_amp = gamma_func(data2f, BreathCurveNoiseFilter4);
                    //dc_breathNoise_amp = lin_to_log(data2, 127, NoiseBreathCurve)*DIV127;
                    //dc_breathNoise_amp = limit(pow(data2f,limit(NoiseBreathCurve,maxGamma,minGamma)),1.0,0.0);
                    dc_breathNoise_amp = gamma_func(data2f,NoiseBreathCurve);
                    //dc_breathNoise_amp = data2f*NoiseBreathCurve;
                    if(BreathAttainOsc1 > 0.0)
                        dc_breathSweepOsc1.amplitude(BreathDepthOsc1*(1.0-limit(data2f/BreathAttainOsc1,1.0,-1.0)),6.0); // 6ms smoothing
                    else
                        dc_breathSweepOsc1.amplitude(0.0);
                    if(BreathAttainOsc2 > 0.0)
                        dc_breathSweepOsc2.amplitude(BreathDepthOsc2*(1.0-limit(data2f/BreathAttainOsc2,1.0,-1.0)),6.0); // 6ms smoothing
                    else
                        dc_breathSweepOsc2.amplitude(0.0);

                    if(note_is_on)
                    {
                        //dc_breathThreshOsc1_amp = pow(thresh( data2f,BreathThreshOsc1), BreathCurveOsc1);
                        dc_breathThreshOsc1_amp = gamma_func(thresh( data2f,BreathThreshOsc1), BreathCurveOsc1);
                        //dc_breathThreshOsc2_amp = pow(thresh( data2f,BreathThreshOsc2), BreathCurveOsc2);
                        dc_breathThreshOsc2_amp = gamma_func(thresh( data2f,BreathThreshOsc2), BreathCurveOsc2);
                        //dc_breath.amplitude(dc_breath_amp,2);
                        dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp,dc_breathFilterN_rampTime);
                        dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp,dc_breathFilterN_rampTime);
                        dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp,dc_breathFilterN_rampTime);
                        dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp,dc_breathFilterN_rampTime);
                        //if( (NoiseLevel > 0) && (NoiseTime == 0 || NoiseTime >= maxTimeNoise/2.0) )
                        if( NoiseLevel > 0 )
                        {
                            dc_breathNoise.amplitude(dc_breathNoise_amp,6);
                        }
                    }
                    else
                    {
                        dc_breathThreshOsc1_amp = 0;
                        dc_breathThreshOsc2_amp = 0;
                        //dc_breath.amplitude(0,6);   // slower decay when note is off (no 'thwap')
                        dc_breathOscFilter1.amplitude(0,6);
                        dc_breathOscFilter2.amplitude(0,6);
                        dc_breathNoiseFilter3.amplitude(0,6);
                        dc_breathNoiseFilter4.amplitude(0,6);
                        dc_breathNoise.amplitude(0,6);
                    }
                    break;
                case CC_PORTA_TIME:
                  dc_portatime_val = ((float)data2)*DIV127;    // PortamentoTime MSB
                  break;
            }
            break;     

        case midi_ho.NoteOn: //(type==0x90)
          //  TODO: create amplitude transition between legato notes
            data2f = ((float)data2)* DIV127;
            //dc_breathThreshOsc1_amp = pow(thresh( data2f,BreathThreshOsc1), BreathCurveOsc1);
            dc_breathThreshOsc1_amp = gamma_func(thresh( data2f,BreathThreshOsc1), BreathCurveOsc1);
            //dc_breathThreshOsc2_amp = pow(thresh( data2f,BreathThreshOsc2), BreathCurveOsc2);
            dc_breathThreshOsc2_amp = gamma_func(thresh( data2f,BreathThreshOsc2), BreathCurveOsc2);
            //dc_breathThreshNoise_amp = pow( data2f , NoiseBreathCurve);
           // dc_breathNoise_amp = lin_to_log(data2, 127, NoiseBreathCurve)*DIV127;
            //dc_breathNoise_amp = limit(pow(data2f,limit(NoiseBreathCurve,maxGamma,minGamma)),1.0,0.0);
            dc_breathNoise_amp = gamma_func(data2f,NoiseBreathCurve);
            //dc_breathNoise_amp = data2f*NoiseBreathCurve;
            dc_breathLfoFilter1_amp = lfoThresh(data2f,LfoThreshOscFilter1,LfoDepthOscFilter1,LfoBreathOscFilter1);
            dc_breathLfoFilter2_amp = lfoThresh(data2f,LfoThreshOscFilter2,LfoDepthOscFilter2,LfoBreathOscFilter2);
            dc_breathLfoFilter3_amp = lfoThresh(data2f,LfoThreshNoiseFilter3,LfoDepthNoiseFilter3,LfoBreathNoiseFilter3);
            dc_breathLfoFilter4_amp = lfoThresh(data2f,LfoThreshNoiseFilter4,LfoDepthNoiseFilter4,LfoBreathNoiseFilter4);
            dc_breathLfoFilter1.amplitude(dc_breathLfoFilter1_amp,6);
            dc_breathLfoFilter2.amplitude(dc_breathLfoFilter2_amp,6);
            dc_breathLfoFilter3.amplitude(dc_breathLfoFilter3_amp,6);
            dc_breathLfoFilter4.amplitude(dc_breathLfoFilter4_amp,6);
            //dc_breathOscFilter1_amp = lin_to_log(data2, 127, BreathCurveOscFilter1)*DIV127;
            dc_breathOscFilter1_amp = gamma_func(data2f, BreathCurveOscFilter1);
            dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp,dc_breathFilterN_rampTime);
            //dc_breathOscFilter2_amp = lin_to_log(data2, 127, BreathCurveOscFilter2)*DIV127;
            dc_breathOscFilter2_amp = gamma_func(data2f, BreathCurveOscFilter2);
            dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp,dc_breathFilterN_rampTime);
            //dc_breathNoiseFilter3_amp = lin_to_log(data2, 127, BreathCurveNoiseFilter3)*DIV127;
            dc_breathNoiseFilter3_amp = gamma_func(data2f, BreathCurveNoiseFilter3);
            dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp,dc_breathFilterN_rampTime);
            //dc_breathNoiseFilter4_amp = lin_to_log(data2, 127, BreathCurveNoiseFilter4)*DIV127;
            dc_breathNoiseFilter4_amp = gamma_func(data2f, BreathCurveNoiseFilter4);
            dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp,dc_breathFilterN_rampTime);
            dc_breathNoise.amplitude(dc_breathNoise_amp,6);
            fMidiNoteNorm = ((float)data1)/128.0;
            fMidiNoteNorm_diff = abs( (float)(data1 - currentMidiNote));
            if(BreathAttainOsc1 > 0.0)
                dc_breathSweepOsc1.amplitude(BreathDepthOsc1*(1.0-limit(data2f/BreathAttainOsc1,1.0,-1.0)),6.0); // 2ms fudge filter
            else
                dc_breathSweepOsc1.amplitude(0.0);
            if(BreathAttainOsc2 > 0.0)
                dc_breathSweepOsc2.amplitude(BreathDepthOsc2*(1.0-limit(data2f/BreathAttainOsc2,1.0,-1.0)),6.0); // 2ms fudge filter
            else
                dc_breathSweepOsc2.amplitude(0.0);

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
                    //dc_timeNoise.amplitude(1.0); // effectively set by pink_Noise.amplitude(NoiseLevel);
                    dc_timeNoise.amplitude(TimeNoiseAmp); // effectively set by pink_Noise.amplitude(NoiseLevel);
                    dc_timeNoise.amplitude(0.0,NoiseTime); // ramp down the noise level
                }
                else
                {
                    dc_timeNoise.amplitude(0.0);
                }
                if(SweepDepthOscFilter1 > 0.0)
                {
                    dc_sweepDepthFilterSign1.amplitude(1.0);
                    dc_sweepDepthFilter1.amplitude(SweepDepthOscFilter1);
                    dc_sweepDepthFilter1.amplitude(0,SweepTimeOscFilter1);
                }
                else if(SweepDepthOscFilter1 < 0.0)
                {
                    dc_sweepDepthFilterSign1.amplitude(-1.0);
                    dc_sweepDepthFilter1.amplitude(SweepDepthOscFilter1);
                    dc_sweepDepthFilter1.amplitude(0,SweepTimeOscFilter1);
                }
                if(SweepDepthOscFilter2 >0.0)
                {
                    dc_sweepDepthFilterSign2.amplitude(1.0);
                    dc_sweepDepthFilter2.amplitude(SweepDepthOscFilter2);
                    dc_sweepDepthFilter2.amplitude(0,SweepTimeOscFilter2);
                }
                else if(SweepDepthOscFilter2 < 0.0)
                {
                    dc_sweepDepthFilterSign2.amplitude(-1.0);
                    dc_sweepDepthFilter2.amplitude(SweepDepthOscFilter2);
                    dc_sweepDepthFilter2.amplitude(0,SweepTimeOscFilter2);
                }
                if(SweepDepthNoiseFilter3 >0.0)
                {
                    dc_sweepDepthFilterSign3.amplitude(1.0);
                    dc_sweepDepthFilter3.amplitude(SweepDepthNoiseFilter3);
                    dc_sweepDepthFilter3.amplitude(0,SweepTimeNoiseFilter3);
                }
                else if(SweepDepthNoiseFilter3 < 0.0)
                {
                    dc_sweepDepthFilterSign3.amplitude(-1.0);
                    dc_sweepDepthFilter3.amplitude(SweepDepthNoiseFilter3);
                    dc_sweepDepthFilter3.amplitude(0,SweepTimeNoiseFilter3);
                }
                if(SweepDepthNoiseFilter4 > 0.0)
                {
                    dc_sweepDepthFilterSign4.amplitude(1.0);
                    dc_sweepDepthFilter4.amplitude(SweepDepthNoiseFilter4);
                    dc_sweepDepthFilter4.amplitude(0,SweepTimeNoiseFilter4);
                }
                else if(SweepDepthNoiseFilter4 < 0.0)
                {
                    dc_sweepDepthFilterSign4.amplitude(-1.0);
                    dc_sweepDepthFilter4.amplitude(SweepDepthNoiseFilter4);
                    dc_sweepDepthFilter4.amplitude(0,SweepTimeNoiseFilter4);
                }
            }
            currentMidiNote = data1;
            note_is_on = true;
            break;

        case midi_ho.NoteOff:  //(type==0x80)
            if(data1==currentMidiNote)
            {
                note_is_on = false;
                fMidiNoteNorm = ((float)data1)/128.0;
                dc_portatime.amplitude(fMidiNoteNorm); // 
                dc_portatimef.amplitude(fMidiNoteNorm,portatimef_t); // 
            }
            break;

        case midi_ho.PitchBend:
            dc_pitchbend.amplitude((data1+data2*128.0-8192.0)*DIV8192, dc_pitchbend_ramp_rate);
            //dc_pitchbend_value = data1+data2*128.0-8192.0;
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
                sprintf(str_oledbuf, "Patch: %03d", current_patchNumber+1);
                break; 
            }
            if(longKnobButtonPress)
            {
                longKnobButtonPress = false;
                UISM = MENU;
                MENUSM = MENU_EXIT;
                sprintf(str_oledbuf, "MENU: EXIT");
                resetUITimeout();
                break; 
            }
            if (newKnob)
            {
                vol = vol + newKnob;
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
                sprintf(str_oledbuf, "MENU: EXIT");
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
                Serial8.println(current_patch.patch_string);
                if (updateEpromFlag)
                {
                    eepromCurrentMillis = millis();
                    eepromPreviousMillis = eepromCurrentMillis; // reset timer every knob turn 
                }
                sprintf(str_oledbuf, "Patch: %03d", current_patchNumber+1);
            }
                
            break;
        case  MENU: 
            switch (MENUSM)   // TODO: add MENU_TRANS and MENU_OCT
            { 
                case MENU_EXIT:
                    if(!submenu_active)
                    {
                        if (newKnob > 0)
                        {
                            MENUSM = MENU_MIX;
                            sprintf(str_oledbuf, "MENU: MIX");
                            break; 
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_TUNING;
                            sprintf(str_oledbuf, "MENU: TUNING");
                            break; 
                        }
                        else
                        {
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU: EXIT");
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
                            sprintf(str_oledbuf, "MENU: TUNING");
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU: EXIT");
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
                            sprintf(str_oledbuf, "MENU: MIX");
                        }
                    }
                    else
                    {
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU: EXIT");
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
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU: EXIT");
                        }
                        else if(newKnob < 0) 
                        {
                            MENUSM = MENU_MIX;
                            sprintf(str_oledbuf, "MENU: MIX");
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
                            sprintf(str_oledbuf, "MENU: TUNING");
                        }
                    }
                    else 
                    {
                        if(shortKnobButtonPress)
                        {
                            submenu_active = false;
                            shortKnobButtonPress = false;
                            MENUSM = MENU_EXIT;
                            sprintf(str_oledbuf, "MENU: EXIT");
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
        display.setTextSize(1.5); // configuration of size of caracters caractères
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
}

void resetUITimeout(void)
{
    previousUITimeoutTime = currentUITimeoutTime;
}

void readKnobAndKnobButton(void)
{
    //--------------------------
    // Read knob and knobButton 
    //--------------------------
    newKnob = knob.read()/4; //don't need 4X counting mode, so integer div by 4
    knobButton.update();
    currentKnobButtonState = knobButton.read();
    if( !longKnobButtonPressPending && !currentKnobButtonState && (knobButton.duration() > longKnobButtonPressTime) )
    {
        longKnobButtonPressPending = true;
        sprintf(str_buf1, "Long knobButton Press" );     
        Serial8.println(str_buf1);
        resetUITimeout();
    }
    KnobButtonRising = knobButton.risingEdge();

    if(KnobButtonRising)
    {
        KnobButtonRising = false;
        if(firstKnobButtonPress)
        {
           firstKnobButtonPress = false; 
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
