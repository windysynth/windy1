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

//#define PSRAM_INSTALLED

#define PATCH_NUMBER_EEPROM_ADDR ( (int)0 )
#define VOL_EEPROM_ADDR ( (int)4 )
#define MIX_LINEIN_EEPROM_ADDR  ( (int)8 )
#define FINE_TUNE_CENTS_EEPROM_ADDR ( (int)12 )
#define TRANSPOSE_EEPROM_ADDR ( (int)16 )
#define OCTAVE_EEPROM_ADDR ( (int)20 )
#define BREATH_CC_EEPROM_ADDR ( (int)24 )

//------------ includes -------------------------------
//#include <MenuClass.h>
#include "MenuClass.h"
#include "OledMenu.h"
#include "globals.h"
#include "MenuItemsListsFuncs.h"
#include <USBHost_t36.h>
#include <MIDI.h>
#include "patches.h"
#include <EEPROM.h>

#include <OpenAudio_ArduinoLibrary.h> //for AudioConvert_I16toF32, AudioConvert_F32toI16, and AudioEffectGain_F32
#include <hexefx_audio_F32.h> // for AudioEffectPlateReverb_F32

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
//AudioAnalyzeRMS          rms_pink_Noise;     //xy=524.333251953125,916
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
AudioMixer4              mix_pwOsc1;     //xy=704.333251953125,209
AudioFilterStateVariable filterPreNoise; //xy=706.333251953125,933
//AudioAnalyzeRMS          rms_filterPreNoise;     //xy=524.333251953125,916
AudioSynthWaveformDc     dc_sweepDepthOsc1; //xy=713.333251953125,85
AudioSynthWaveformDc     dc_breathSweepOsc1; //xy=713.333251953125,116
AudioSynthWaveformDc     dc_breathSweepOsc2; //xy=731.333251953125,332
AudioSynthWaveformDc     dc_sweepDepthOsc2; //xy=734.333251953125,300
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
//AudioAnalyzeRMS          rms_multiply2;     //xy=524.333251953125,916
AudioEffectMultiply      mult_lfoDepthFilter4; //xy=883.333251953125,1306.0000610351562
AudioSynthWaveformDc     dc_breathNoiseFilter4; //xy=888.333251953125,1272.0000610351562
AudioSynthWaveformDc     dc_modOffsetNoiseFilter4; //xy=891.333251953125,1373.0000610351562
//AudioEffectWaveFolder    wavefolder_pwOsc1; //xy=903.333251953125,225
AudioSynthWaveformDc     dc_sweepDepthFilterSign4; //xy=899.333251953125,1229.0000610351562
AudioMixer4              mix_pitchModOsc1; //xy=935.333251953125,129
AudioEffectMultiply      sq_swpflt4;     //xy=948.333251953125,1193.0000610351562
AudioMixer4              mix_pitchModOsc2; //xy=952.333251953125,332
//AudioEffectWaveFolder    wavefolder_pwOsc2; //xy=970.333251953125,403
AudioSynthWaveformDc     dc_sweepDepthFilterSign2; //xy=975.333251953125,777
AudioEffectMultiply      sq_swpflt2;     //xy=1024.333251953125,744
AudioEffectMultiply      mult_lfoDepthFilter2; //xy=1038.333251953125,848
AudioSynthWaveformDc     dc_modOffsetOscFilter2; //xy=1038.333251953125,896
//AudioEffectWaveshaper    waveshape3;     //xy=1039.8500061035156,1101.8500061035156
AudioSynthWaveformDc     dc_breathOscFilter2; //xy=1042.333251953125,813
AudioMixer4              mix_levelNoise; //xy=1042.333251953125,1012
//AudioAnalyzeRMS          rms_mix_levelNoise;     //xy=524.333251953125,916
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
//AudioMixer4              mix_delayFeedback; //xy=1220.3331756591797,1630
AudioEffectMultiply      mlt_DepthFilterSign2; //xy=1237.333251953125,759
AudioMixer4              mix_fcModFilter2; //xy=1250.333251953125,872
AudioSynthWaveformDc     dc_breathThreshOsc2; //xy=1285.333251953125,264
AudioSynthWaveformDc     dc_breathThreshOsc1; //xy=1291.333251953125,70
AudioMixer4              mix_osc1;       //xy=1336.333251953125,158
//AudioEffectWaveshaper    waveshape1;     //xy=1335.6668090820312,659.6667785644531
AudioMixer4              mix_osc2;       //xy=1359.333251953125,336
AudioMixer4              mix_ntcFilter3; //xy=1360.333251953125,1096
//AudioAnalyzeRMS          rms_mix_ntcFilter3;     //xy=524.333251953125,916
//AudioEffectDelay         dly_delayEffects; //xy=1430.3332977294922,1629
AudioMixer4              mix_fcModFilter4_sweep; //xy=1433.333251953125,1211.0000610351562
//AudioFilterStateVariable filterPostDelayL; //xy=1477.9999237060547,1503.3333740234375
//AudioFilterStateVariable filterPostDelayR; //xy=1478.3332977294922,1755
AudioMixer4              mix_fcModFilter2_sweep; //xy=1497.333251953125,884
AudioMixer4              mix_xfade;      //xy=1505.333251953125,122
AudioFilterStateVariable filter1;        //xy=1553.333251953125,658
AudioEffectMultiply      mult_thCurveOsc2; //xy=1640.333251953125,298
//AudioEffectWaveshaper    waveshape4;     //xy=1644.8497924804688,1173.8501586914062
//AudioMixer4              mix_delayLevelR; //xy=1655.3334197998047,1844
//AudioMixer4              mix_delayLevelL; //xy=1661.3332977294922,1430.666748046875
AudioEffectMultiply      mult_thCurveOsc1; //xy=1674.333251953125,177
AudioMixer4              mix_ntcFilter1; //xy=1709.333251953125,652
//AudioEffectWaveshaper    waveshape2;     //xy=1728.8499755859375,920.8499908447266
AudioConvert_I16toF32           Int2FloatR; //xy=1769.949951171875,1688.6500244140625
AudioConvert_I16toF32           Int2FloatL;           //xy=1775.8499755859375,1622.8499755859375
AudioFilterStateVariable filter4;        //xy=1799.333251953125,1128
AudioMixer4              mix_oscLevels;  //xy=1861.333251953125,234
AudioEffectMultiply      mult_sqLimter; //xy=1523.1333618164062,471.8833312988281
AudioMixer4              mix_limiter; //xy=1897.716552734375,450.8833312988281
AudioEffectMultiply      mult_cubicLimter; //xy=1710.949951171875,461.8833312988281
AudioFilterStateVariable filter2;        //xy=1905.333251953125,924
AudioMixer4              mix_ntcFilter4; //xy=1961.333251953125,1116
//AudioAnalyzeRMS          rms_mix_ntcFilter4;     //xy=524.333251953125,916
AudioEffectPlateReverb_F32       verb_F32;       //xy=1981.6666259765625,1667.6666870117188
#ifdef PSRAM_INSTALLED
AudioEffectDelayStereo_F32       delay_F32 = AudioEffectDelayStereo_F32( (uint32_t)1270, true);       //xy=1981.6666259765625,1667.6666870117188
#else
AudioEffectDelayStereo_F32       delay_F32 = AudioEffectDelayStereo_F32( (uint32_t)200, false);       //xy=1981.6666259765625,1667.6666870117188
#endif
AudioEffectGain_F32     amp_extraGainR_F32;
AudioEffectGain_F32     amp_extraGainL_F32;
AudioMixer4_F32         mix_pongL_F32;
AudioMixer4_F32         mix_pongR_F32;
AudioMixer4              mix_ntcFilter2; //xy=2059.333251953125,914
AudioFilterFIR           fir_formant;    //xy=2111.333251953125,978
AudioMixer4              mix_Amp;        //xy=2131.333251953125,1127
AudioEffectEnvelope      env_squelch;
AudioEffectEnvelope      env_squelchR; //xy=2563.1497802734375,1767.050048828125
AudioEffectEnvelope      env_squelchL; //xy=2583.14990234375,1556.050048828125
AudioConvert_F32toI16           Float2IntR; //xy=2165.9503173828125,1731.6500244140625
AudioConvert_F32toI16           Float2IntL; //xy=2170.9503173828125,1609.6500244140625
AudioFilterStateVariable filter5;        //xy=2289.333251953125,1137
//AudioFilterStateVariable filterPreMixLPR; //xy=2347.3336181640625,1738
//AudioFilterStateVariable filterPreMixLPL; //xy=2357.3333740234375,1614
AudioFilterStateVariable filterPreMixHPR; //xy=2533.3336181640625,1732
AudioFilterStateVariable filterPreMixHPL; //xy=2538.333297729492,1606
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
//AudioConnection          patchCordTest1(pink_Noise, 0, rms_pink_Noise, 0 );
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
AudioConnection          patchCord46(filterPreNoise, 2, multiply2, 0);
//AudioConnection          patchCordTest3(filterPreNoise, 2, rms_filterPreNoise, 0);
AudioConnection          patchCord47(dc_sweepDepthOsc1, 0, mix_pitchModOsc1, 0);
AudioConnection          patchCord48(dc_breathSweepOsc1, 0, mix_pitchModOsc1, 1);
AudioConnection          patchCord49(dc_breathSweepOsc2, 0, mix_pitchModOsc2, 1);
AudioConnection          patchCord50(dc_sweepDepthOsc2, 0, mix_pitchModOsc2, 0);
AudioConnection          patchCord52(dc_beatOsc1, 0, mix_pitchModOsc1, 2);
AudioConnection          patchCord54(dc_beatOsc2, 0, mix_pitchModOsc2, 2);
AudioConnection          patchCord55(dc_sweepDepthFilter4, 0, sq_swpflt4, 0);
AudioConnection          patchCord56(dc_sweepDepthFilter4, 0, sq_swpflt4, 1);
//AudioConnection          patchCord57(mix_fcModFilter3_sweep, waveshape3);
AudioConnection          patchCord58(dc_sweepDepthFilter2, 0, sq_swpflt2, 0);
AudioConnection          patchCord59(dc_sweepDepthFilter2, 0, sq_swpflt2, 1);
AudioConnection          patchCord60(mix_fcModFilter1, 0, mix_fcModFilter1_sweep, 1);
AudioConnection          patchCord61(mlt_DepthFilterSign1, 0, mix_fcModFilter1_sweep, 0);
AudioConnection          patchCord62(dc_breathLfoFilter2, 0, mult_lfoDepthFilter2, 1);
AudioConnection          patchCord63(sine_lfoFilter2, 0, mult_lfoDepthFilter2, 0);
//AudioConnection          patchCord64(mix_chorus_dry, 0, mix_delayFeedback, 0);
//AudioConnection          patchCord65(mix_chorus_dry, 0, mix_delayLevelR, 0);
//AudioConnection          patchCord66(mix_chorus_dry, 0, mix_delayLevelL, 0);
AudioConnection          patchCord67(multiply2, 0, mix_levelNoise, 0);
//AudioConnection          patchCordTest4(multiply2, 0, rms_multiply2, 0);
//AudioConnection          patchCordTest2( mix_levelNoise, 0, rms_mix_levelNoise , 0);
AudioConnection          patchCord68(mult_lfoDepthFilter4, 0, mix_fcModFilter4, 1);
AudioConnection          patchCord69(dc_breathNoiseFilter4, 0, mix_fcModFilter4, 0);
AudioConnection          patchCord70(dc_modOffsetNoiseFilter4, 0, mix_fcModFilter4, 3);
AudioConnection          patchCord71(mix_pwOsc1, 0, wfmod_pulseOsc1, 1);
AudioConnection          patchCord72(dc_sweepDepthFilterSign4, 0, mlt_DepthFilterSign4, 1);
AudioConnection          patchCord73(mix_pitchModOsc1, 0, wfmod_sawOsc1, 0);
AudioConnection          patchCord74(mix_pitchModOsc1, 0, wfmod_triOsc1, 0);
AudioConnection          patchCord75(mix_pitchModOsc1, 0, wfmod_pulseOsc1, 0);
AudioConnection          patchCord76(sq_swpflt4, 0, mlt_DepthFilterSign4, 0);
AudioConnection          patchCord77(mix_pitchModOsc2, 0, wfmod_sawOsc2, 0);
AudioConnection          patchCord78(mix_pitchModOsc2, 0, wfmod_triOsc2, 0);
AudioConnection          patchCord79(mix_pitchModOsc2, 0, wfmod_pulseOsc2, 0);
AudioConnection          patchCord80(mix_pwOsc2, 0, wfmod_pulseOsc2, 1);
AudioConnection          patchCord81(dc_sweepDepthFilterSign2, 0, mlt_DepthFilterSign2, 1);
AudioConnection          patchCord82(sq_swpflt2, 0, mlt_DepthFilterSign2, 0);
AudioConnection          patchCord83(mult_lfoDepthFilter2, 0, mix_fcModFilter2, 1);
AudioConnection          patchCord84(dc_modOffsetOscFilter2, 0, mix_fcModFilter2, 3);
AudioConnection          patchCord85(mix_fcModFilter3_sweep, 0, filter3, 1);
AudioConnection          patchCord86(dc_breathOscFilter2, 0, mix_fcModFilter2, 0);
AudioConnection          patchCord87(mix_levelNoise, 0, filter3, 0);
AudioConnection          patchCord88(mix_levelNoise, 0, mix_ntcFilter3, 0);
//AudioConnection          patchCord89(mix_fcModFilter1_sweep, waveshape1);
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
//AudioConnection          patchCord101(mix_delayFeedback, dly_delayEffects);
AudioConnection          patchCord102(mlt_DepthFilterSign2, 0, mix_fcModFilter2_sweep, 0);
AudioConnection          patchCord103(mix_fcModFilter2, 0, mix_fcModFilter2_sweep, 1);
AudioConnection          patchCord104(dc_breathThreshOsc2, 0, mult_thCurveOsc2, 0);
AudioConnection          patchCord105(dc_breathThreshOsc2, 0, mix_xfade, 1);
AudioConnection          patchCord106(dc_breathThreshOsc1, 0, mix_xfade, 0);
AudioConnection          patchCord107(mix_osc1, 0, mult_thCurveOsc1, 1);
AudioConnection          patchCord108(mix_fcModFilter1_sweep, 0, filter1, 1);
AudioConnection          patchCord109(mix_osc2, 0, mult_thCurveOsc2, 1);
AudioConnection          patchCord110(mix_ntcFilter3, 0, mix_ntcFilter4, 0);
AudioConnection          patchCord111(mix_ntcFilter3, 0, filter4, 0);
//AudioConnection          patchCordTest5(mix_ntcFilter3, 0, rms_mix_ntcFilter3, 0);
//AudioConnection          patchCord112(dly_delayEffects, 0, filterPostDelayL, 0);
//AudioConnection          patchCord113(dly_delayEffects, 1, filterPostDelayR, 0);
//AudioConnection          patchCord114(mix_fcModFilter4_sweep, waveshape4);
//AudioConnection          patchCord115(filterPostDelayL, 0, mix_delayLevelL, 1);
//AudioConnection          patchCord116(filterPostDelayL, 0, mix_delayFeedback, 1);
//AudioConnection          patchCord117(filterPostDelayR, 0, mix_delayLevelR, 1);
//AudioConnection          patchCord118(filterPostDelayR, 0, mix_delayFeedback, 2);
//AudioConnection          patchCord119(mix_fcModFilter2_sweep, waveshape2);
AudioConnection          patchCord120(mix_xfade, 0, mult_thCurveOsc1, 0);
AudioConnection          patchCord121(filter1, 0, mix_ntcFilter1, 1);
AudioConnection          patchCord122(filter1, 1, mix_ntcFilter1, 2);
AudioConnection          patchCord123(filter1, 2, mix_ntcFilter1, 3);
AudioConnection          patchCord124(mult_thCurveOsc2, 0, mix_oscLevels, 1);
AudioConnection          patchCord125(mix_fcModFilter4_sweep, 0, filter4, 1);
//AudioConnection          patchCord126(mix_delayLevelR, Int2FloatR);
//AudioConnection          patchCord127(mix_delayLevelL, Int2FloatL);
AudioConnection          patchCord126(mix_chorus_dry, Int2FloatR);
AudioConnection          patchCord127(mix_chorus_dry, Int2FloatL);
AudioConnection          patchCord128(mult_thCurveOsc1, 0, mix_oscLevels, 0);
AudioConnection          patchCord129(mix_ntcFilter1, 0, filter2, 0);
AudioConnection          patchCord130(mix_ntcFilter1, 0, mix_ntcFilter2, 0);
AudioConnection          patchCord131(mix_fcModFilter2_sweep, 0, filter2, 1);
//AudioConnection_F32          patchCord132(Int2FloatR, 0, verb_F32, 1);
//AudioConnection_F32          patchCord133(Int2FloatL, 0, verb_F32, 0);
AudioConnection_F32          patchCord132(Int2FloatR, 0, delay_F32, 1);
AudioConnection_F32          patchCord133(Int2FloatL, 0, delay_F32, 0);
AudioConnection_F32          patchCord165(delay_F32, 1, verb_F32, 1);
AudioConnection_F32          patchCord166(delay_F32, 0, verb_F32, 0);  // 166 is max so far
AudioConnection          patchCord134(filter4, 0, mix_ntcFilter4, 1);
AudioConnection          patchCord135(filter4, 1, mix_ntcFilter4, 2);
AudioConnection          patchCord136(filter4, 2, mix_ntcFilter4, 3);
//AudioConnection          patchCord137(mix_oscLevels, 0, filter1, 0);
//AudioConnection          patchCord138(mix_oscLevels, 0, mix_ntcFilter1, 0);
AudioConnection          patchCord137(mix_oscLevels, 0, mult_sqLimter, 0);
AudioConnection          patchCord138(mix_oscLevels, 0, mult_sqLimter, 1);
AudioConnection          patchCord171(mix_oscLevels, 0, mix_limiter, 0);
AudioConnection          patchCord172(mix_oscLevels, 0, mix_limiter, 1);
AudioConnection          patchCord173(mix_oscLevels, 0, mult_cubicLimter, 0);
AudioConnection          patchCord174(mult_sqLimter, 0, mult_cubicLimter, 1);
AudioConnection          patchCord175(mult_cubicLimter, 0, mix_limiter, 2);
AudioConnection          patchCord176(mix_limiter, 0, mix_ntcFilter1, 0);
AudioConnection          patchCord177(mix_limiter, 0, filter1, 0);
AudioConnection          patchCord139(filter2, 0, mix_ntcFilter2, 1);
AudioConnection          patchCord140(filter2, 1, mix_ntcFilter2, 2);
AudioConnection          patchCord141(filter2, 2, mix_ntcFilter2, 3);
AudioConnection          patchCord142(mix_ntcFilter4, 0, mix_Amp, 1);
//AudioConnection          patchCordTest6(mix_ntcFilter4, 0, rms_mix_ntcFilter4, 1);
AudioConnection_F32          patchCord163(verb_F32, 1, amp_extraGainR_F32, 0);
AudioConnection_F32          patchCord164(verb_F32, 0, amp_extraGainL_F32, 0);
AudioConnection_F32          patchCord143(amp_extraGainR_F32, 0, mix_pongL_F32, 1);
AudioConnection_F32          patchCord144(amp_extraGainL_F32, 0, mix_pongL_F32, 0);
AudioConnection_F32          patchCord167(amp_extraGainR_F32, 0, mix_pongR_F32, 1);
AudioConnection_F32          patchCord168(amp_extraGainL_F32, 0, mix_pongR_F32, 0);
AudioConnection_F32          patchCord169(mix_pongR_F32, 0, Float2IntR, 0);
AudioConnection_F32          patchCord170(mix_pongL_F32, 0, Float2IntL, 0);
AudioConnection          patchCord145(mix_ntcFilter2, fir_formant);
//AudioConnection          patchCord146(fir_formant, 0, mix_Amp, 0);
AudioConnection          patchCord146(fir_formant, 0, filter5, 0);
AudioConnection          patchCord150(filter5, 2, mix_Amp, 0);
AudioConnection          patchCord147(mix_Amp, 0, env_squelch, 0);
//AudioConnection          patchCord148(Float2IntR, 0, filterPreMixLPR, 0);
//AudioConnection          patchCord149(Float2IntL, 0, filterPreMixLPL, 0);
//AudioConnection          patchCord151(filterPreMixLPR, 0, env_squelchR, 0);
//AudioConnection          patchCord152(filterPreMixLPL, 0, env_squelchL, 0);
AudioConnection          patchCord151(Float2IntR, 0, env_squelchR, 0);
AudioConnection          patchCord152(Float2IntL, 0, env_squelchL, 0);
AudioConnection          patchCord153(env_squelch, 0, mix_chorus_fb, 0);
AudioConnection          patchCord154(env_squelch, 0, mix_chorus_dry, 0);
AudioConnection          patchCord155(env_squelchR, 0, filterPreMixHPR, 0);
AudioConnection          patchCord156(env_squelchL, 0, filterPreMixHPL, 0);
AudioConnection          patchCord157(filterPreMixHPR, 2, mix_lineInR, 0);
AudioConnection          patchCord158(filterPreMixHPL, 2, mix_lineInL, 0);
AudioConnection          patchCord159(i2s2, 0, mix_lineInL, 1);
AudioConnection          patchCord160(i2s2, 1, mix_lineInR, 1);
AudioConnection          patchCord161(mix_lineInL, 0, i2s1, 0);
AudioConnection          patchCord162(mix_lineInR, 0, i2s1, 1);

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
// see OledMenu.h

//------- Encoder knob, button and UI state machine stuff----------------

const int knobButtonPin = 32;  // teensy4.1 pin
const int knobEncoderPin1 = 30;  // teensy4.1 pin
const int knobEncoderPin2 = 31;  // teensy4.1 pin
const uint32_t debounceDelay = 10; //  ms
Encoder knob(knobEncoderPin1, knobEncoderPin2);
Bounce knobButton(knobButtonPin, debounceDelay);


//const int patchNextButton = 29;
//uint32_t patchNextButtonPressedTime = 0;
//uint32_t knobButtonPressedTime = 0;
//int oldKnobButtonState = 0;

int currentKnobButtonState = 0;
bool longKnobButtonPressPending = false;
bool longKnobButtonPress = false;
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

// squelched after squelchRelease and before squelchRelease + squelchDelay
// synth variables update after updateSynthDelay which should occur while squelched
// squechAttack starts at updateSynthDelay + squelchDelay
const uint32_t readKnobInterval = 50;  // ms
const uint32_t updateSynthDelay = 70;  // ms > squelchRelease
const float squelchRelease = 30.0f;  // ms
const float squelchDelay = 60.0f;  // ms 
const float squelchAttack = 30.0f;  // ms
const uint32_t readRMSInterval = 1000;  // ms

//-------------- MIDI DIN connection ----------------------
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIs1);



//-------------- USB HOST MIDI Class Compliant --------------------------
USBHost myUSBHost;
//MIDIDevice midi_ho(myUSBHost);
MIDIDevice_BigBuffer midi_ho(myUSBHost);


// Number of samples in each delay line
// Allocate the delay lines for left and right channels


void setup() {
    // put your setup code here, to run once:
    
    //init str buffers
    sprintf(str_buf, version_str.c_str());
    sprintf(str_buf1, version_str.c_str());
    sprintf(str_oledbuf, splashScreen_str.c_str());

    //pinMode(patchNextButton, INPUT_PULLUP);
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
    
    //AudioMemory(540);  //TODO: how much AudioMemory do I need? (delay 2.9ms per block for 1270/2.9 = 438
    //AudioMemory_F32(20);
    AudioMemory(100);  //TODO: how much AudioMemory do I need? (delay 2.9ms per block for 1270/2.9 = 438
    AudioMemory_F32(200);
    AudioNoInterrupts();
    dc_pwOsc1.amplitude(0.0); // PW now set wfmod_pulseOsc1.pulsewidth_offset()
    sine_lfoOsc1.amplitude(PwmDepthOsc1);
    sine_lfoOsc1.frequency(PwmFreqOsc1);
    dc_pwOsc2.amplitude(0.0); //  PW now set wfmod_pulseOsc2.pulsewidth_offset()
    sine_lfoOsc2.amplitude(PwmDepthOsc2);
    sine_lfoOsc2.frequency(PwmFreqOsc2);
    mix_pwOsc1.gain(0,0.0);  
    mix_pwOsc1.gain(1,maxPwmDepthOsc);  
    mix_pwOsc2.gain(0,0.0);  
    mix_pwOsc2.gain(1,maxPwmDepthOsc); 

    mix_pitchModOsc1.gain(0, 1.0);  // use dc_sweepDepthOsc1 amplitude 
    mix_pitchModOsc1.gain(1, limitBreathSweepOsc1);  // use dc_breathSweepOsc1 amplitude  
    mix_pitchModOsc1.gain(2, 1.0);  // use dc_beatOsc1 amplitude 
    mix_pitchModOsc2.gain(0, 1.0);  // use dc_sweepDepthOsc1 amplitude 
    mix_pitchModOsc2.gain(1, limitBreathSweepOsc2);  // use dc_breathSweepOsc1 amplitude
    mix_pitchModOsc2.gain(2, 1.0);  // use dc_beatOsc1 amplitude
    dc_beatOsc1.amplitude(0.0); // TODO calculate beat number
    dc_beatOsc2.amplitude(0.0); // TODO calculate beat number

    //wfmod_sawOsc1.begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
    wfmod_sawOsc1.begin(WAVEFORM_SAWTOOTH);
    wfmod_sawOsc1.amplitude(SawOsc1);
    wfmod_sawOsc1.frequency(noteFreqOsc1);
    wfmod_sawOsc1.frequencyModulation(octaveControlOsc1);  // max freq mod +/- 4.0 octaves
    wfmod_triOsc1.begin(WAVEFORM_TRIANGLE_VARIABLE);
    wfmod_triOsc1.amplitude(TriOsc1);
    wfmod_triOsc1.frequency(noteFreqOsc1);
    wfmod_triOsc1.frequencyModulation(octaveControlOsc1);  // max freq mod +/- 4.0 octaves
    //wfmod_pulseOsc1.begin(WAVEFORM_BANDLIMIT_PULSE);
    wfmod_pulseOsc1.begin(WAVEFORM_PULSE);
    wfmod_pulseOsc1.amplitude(PulseOsc1);
    wfmod_pulseOsc1.frequency(noteFreqOsc1);
    wfmod_pulseOsc1.frequencyModulation(octaveControlOsc1);  // max freq mod +/- 4.0 octaves
    wfmod_pulseOsc1.pulsewidth_offset(PwOsc1); // ws added this to synth_waveform.cpp .h
    //wfmod_sawOsc2.begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
    wfmod_sawOsc2.begin(WAVEFORM_SAWTOOTH);
    wfmod_sawOsc2.amplitude(SawOsc2);
    wfmod_sawOsc2.frequency(noteFreqOsc2);
    wfmod_sawOsc2.frequencyModulation(octaveControlOsc2);  // max freq mod +/- 4.0 octaves
    wfmod_triOsc2.begin(WAVEFORM_TRIANGLE_VARIABLE);
    wfmod_triOsc2.amplitude(TriOsc2);
    wfmod_triOsc2.frequency(noteFreqOsc2);
    wfmod_triOsc2.frequencyModulation(octaveControlOsc2);  // max freq mod +/- 4.0 octaves
    //wfmod_pulseOsc2.begin(WAVEFORM_BANDLIMIT_PULSE);
    wfmod_pulseOsc2.begin(WAVEFORM_PULSE);
    wfmod_pulseOsc2.amplitude(PulseOsc2);
    wfmod_pulseOsc2.frequency(noteFreqOsc2);
    wfmod_pulseOsc2.frequencyModulation(octaveControlOsc2);  // max freq mod +/- 4.0 octaves
    wfmod_pulseOsc2.pulsewidth_offset(PwOsc2); // ws added this to synth_waveform.cpp .h
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
    filterPreNoise.octaveControl(octaveControlPreNoiseFilter); // sets range of control from mix_fcModFilter4 
    //filterPostDelayL.frequency(EffectsDelayDamp);
    //filterPostDelayL.resonance(0.707);
    //filterPostDelayR.frequency(EffectsDelayDamp);
    //filterPostDelayR.resonance(0.707);
    filterPreMixHPL.frequency(200);
    filterPreMixHPL.resonance(0.5);
    filterPreMixHPL.octaveControl(1.0);
    filterPreMixHPR.frequency(200);
    filterPreMixHPR.resonance(0.5);
    filterPreMixHPR.octaveControl(1.0);
//    filterPreMixLPL.frequency(20500);  // LP filter
//    filterPreMixLPL.resonance(0.707);
//    filterPreMixLPL.octaveControl(1.0);
//    filterPreMixLPR.frequency(20500);  // LP filter
//    filterPreMixLPR.resonance(2.0);
//    filterPreMixLPR.octaveControl(1.0);
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
    //cacluateHeadRoom();
    mix_oscLevels.gain(0, LevelOsc1*LevelOscN_HeadRoom);
    mix_oscLevels.gain(1, LevelOsc2*LevelOscN_HeadRoom);
    
    mix_limiter.gain(0, 1.0);
    mix_limiter.gain(1, LimiterAmount); 
    mix_limiter.gain(2, -LimiterAmount);

    mix_levelNoise.gain(0, 1.0); // pass thru for now
    //mix_levelNoise.gain(1, 1.0); // Debug path. 
    mix_breathTimeNoise.gain(0, 1.0); // TODO: set balance between breath and Time for noise  
    mix_breathTimeNoise.gain(1, 1.0); // TODO: set balance between breath and Time for nois

    //waveshape1.shape(WAVESHAPER_ARRAY,33);
    mix_fcModFilter1.gain(0, 1.0);  
    mix_fcModFilter1.gain(1, 1.0);  
    //mix_fcModFilter1.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter1.gain(2, 0.0);  
    mix_fcModFilter1.gain(3, modOffsetFilter1);  
    mix_fcModFilter1_sweep.gain(0, maxSweepDepthOscFilter);  
    mix_fcModFilter1_sweep.gain(1, 1.0);  
    mix_fcModFilter1_sweep.gain(2, 0.0);  
    mix_fcModFilter1_sweep.gain(3, 0.0);  

    //waveshape2.shape(WAVESHAPER_ARRAY,33);
    mix_fcModFilter2.gain(0, 1.0);  
    mix_fcModFilter2.gain(1, 1.0);  
    //mix_fcModFilter2.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter2.gain(2, 0.0);  
    mix_fcModFilter2.gain(3, modOffsetFilter2);  
    mix_fcModFilter2_sweep.gain(0, maxSweepDepthOscFilter);  
    mix_fcModFilter2_sweep.gain(1, 1.0);  
    mix_fcModFilter2_sweep.gain(2, 0.0);  
    mix_fcModFilter2_sweep.gain(3, 0.0);  

    //waveshape3.shape(WAVESHAPER_ARRAY,33);
    mix_fcModFilter3.gain(0, 1.0);  
    mix_fcModFilter3.gain(1, 1.0);  
    //mix_fcModFilter3.gain(2, maxSweepDepthFilter);  
    mix_fcModFilter3.gain(2, 0.0);  
    mix_fcModFilter3.gain(3, modOffsetFilter3);  
    mix_fcModFilter3_sweep.gain(0, maxSweepDepthOscFilter);  
    mix_fcModFilter3_sweep.gain(1, 1.0);  
    mix_fcModFilter3_sweep.gain(2, 0.0);  
    mix_fcModFilter3_sweep.gain(3, 0.0);  

    //waveshape4.shape(WAVESHAPER_ARRAY,33);
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
    mix_Amp.gain(1, 1.0f);  // 4000s AmpLevel doesn't control Noise Level

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


    //mix_delayFeedback.gain(0,1.0);
    //mix_delayFeedback.gain(1,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
    //mix_delayFeedback.gain(2,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
    //dly_delayEffects.delay(0, EffectsDelayTimeL);
    EffectsDelayTimeR = EffectsDelayTimeL*0.85f;
    //dly_delayEffects.delay(1, EffectsDelayTimeR);
    //dly_delayEffects.disable(2);
    //dly_delayEffects.disable(3);
    //dly_delayEffects.disable(4);
    //dly_delayEffects.disable(5);
    //dly_delayEffects.disable(6);
    //dly_delayEffects.disable(7);

    //freeverbs1.roomsize(EffectsReverbDenseEarly);
    //freeverbs1.damping(EffectsReverbDamp);

    // setup delay_F32 here
	//delay_F32.bypass_set(false); // delay off at start
    //delay_F32.time(1.0f); // 1.27sec and use psram
    //delay_F32.feedback(0.5); // 0 - 1.0 
    //delay_F32.mix(0.5);
    
	delay_F32.bypass_set(false); // delay off at start
    delay_F32.time(EffectsDelayTimeL); // 1.27sec and use psram
    delay_F32.feedback(EffectsDelayFeedback); // 0 - 1.0 
    delay_F32.mix(EffectsDelayLevel);
    //delay_F32.delay(number_of_samples); //  
    //delay_F32.inertia(0.5f); // 0 for fastest update - 1.0 for slowest update of delay time
    //delay_F32.treble(0.0f); // 0- 1.0 
    //delay_F32.treble_cut(0.0f); // 0- 1.0 
    //delay_F32.bass(0.0f); // 0- 1.0 
    //delay_F32.bass_cut(0.0f); // 0- 1.0 
    //delay_F32.mod_rateHz(0.0f); // 0- 16.0hz
    //delay_F32.mod_rate(0.0f); // 0- 1.0 
    //delay_F32.mix(maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));

 // setup verb_f32 here
	verb_F32.bypass_set(false);
    verb_F32.mix(EffectsReverbLevel); // 1.0 = full wet
    verb_F32.size(EffectsReverbTime); // sets reverb time (0.0 to 1.0f)
    verb_F32.diffusion(EffectsReverbDenseEarly); // lower values for more single repeats (echos) (0.0 to 1.0f)
    verb_F32.hidamp(EffectsReverbDamp);  // treble loss in reverb tails (0.0 to 1.0f)
    verb_F32.lodamp(0.0);  // bass loss in reverb tails (0.0 to 1.0f)
    verb_F32.lowpass(1.0f); // output lp filter (0.0 to 1.0f)
    verb_F32.hipass(0.0f);  // output hp filter (0.0 to 1.0f)
    //verb_F32.freezeBleedIn(tmp);
    //value = map(value, 0, 127, -12, 24);
    verb_F32.pitchSemitones(0);
    verb_F32.pitchMix(0.0f);
    verb_F32.shimmer(0.0f);
    //float value = map(0.0f, 0, 127, -12, 24);
    verb_F32.shimmerPitchSemitones(0.0f);	
    


    //mix_delayLevelL.gain(0,maxDelayLevel);
    //mix_delayLevelL.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));
    //mix_delayLevelR.gain(0,maxDelayLevel);
    //mix_delayLevelR.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));

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

    amp_extraGainR_F32.setGain(extraAmpFactor);
    amp_extraGainL_F32.setGain(extraAmpFactor);
    mix_pongL_F32.gain(0,0.5f*EffectsDelayPong + 0.5f); 
    mix_pongL_F32.gain(1,-0.5f*EffectsDelayPong + 0.5f);
    mix_pongR_F32.gain(0,-0.5f*EffectsDelayPong + 0.5f);
    mix_pongR_F32.gain(1,0.5f*EffectsDelayPong + 0.5f);
    
    //mix_lineInL.gain(0, volf(1.0-mix_lineinf));
    mix_lineInL.gain(0, volf);
    mix_lineInL.gain(1, mix_lineinf*extraLineInAmpFactor);
    mix_lineInL.gain(2, 0.0);
    mix_lineInL.gain(3, 0.0);
    //mix_lineInR.gain(0, volf(1.0-mix_lineinf));
    mix_lineInR.gain(0, volf);
    mix_lineInR.gain(1, mix_lineinf*extraLineInAmpFactor);
    mix_lineInR.gain(2, 0.0);
    mix_lineInR.gain(3, 0.0);



    sprintf(str_buf1, "loadPatchNumberEEPROM (%03d) ", eeprom_patchNumber );     
    Serial8.println(str_buf1);
   // sprintf(str_buf1, "current_patchNumber (%03d) ", current_patchNumber );     
   // Serial8.println(str_buf1);

    //if (patchLoaded[eeprom_patchNumber]){
        current_patchNumber = eeprom_patchNumber;
        //loadPatchSD(current_patchNumber);
        setCurrentPatch(current_patchNumber);
        patchToSynthVariables(&current_patch);
        sprintf(str_buf1, "current_patchNumber (%03d) ", current_patchNumber );     
        Serial8.println(str_buf1);
        printPatchValues();
    //}

    updateSynthVariablesFlag = true;
   // printPatchValues();   
    //PRINT_VALUES_FLAG = true;

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
        display.clearDisplay(); // erase display
        display.setTextSize(2); // configuration of size of characters 
        display.setTextColor(WHITE,BLACK);
        display.display(); // refresh display
        display.setCursor(0,5);
        display.println(str_oledbuf);
        display.display();
        //resetUITimeout();
    }

    myMenu.setCurrentMenu(&listTopMenu);
    Serial8.println(str_oledbuf);
    myMenu.resetButtonsAndKnobs(); // to clear spurios activity form power up
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
} // setup()

void loop() 
{   
    static unsigned long prevMilMenu = millis(); 
    static unsigned long prevMilUpdateSynth = millis(); 
    static unsigned long prevMilRMS = millis(); 
    static bool activeButtonOrKnob = false;
    currentMillis = millis();
    currentUITimeoutTime = currentMillis;

    /*
    if(currentMillis - prevMilRMS >= readRMSInterval) {
        prevMilRMS = currentMillis;
        if(rms_pink_Noise.available()){
            sprintf(str_buf1, "rms_pink_Noise: %5.3f", rms_pink_Noise.read() );
            Serial8.println(str_buf1);
        }
        if(rms_mix_levelNoise.available()){
            sprintf(str_buf1, "rms_mix_levelNoise: %5.3f", rms_mix_levelNoise.read() );
            Serial8.println(str_buf1);
        }
        if(rms_filterPreNoise.available()){
            sprintf(str_buf1, "rms_filterPreNoise: %5.3f", rms_filterPreNoise.read() );
            Serial8.println(str_buf1);
        }
        if(rms_multiply2.available()){
            sprintf(str_buf1, "rms_multiply2: %5.3f", rms_multiply2.read() );
            Serial8.println(str_buf1);
        }
        if(rms_mix_ntcFilter3.available()){
            sprintf(str_buf1, "rms_mix_ntcFilter3: %5.3f", rms_mix_ntcFilter3.read() );
            Serial8.println(str_buf1);
        }
        if(rms_mix_ntcFilter4.available()){
            sprintf(str_buf1, "rms_mix_ntcFilter4: %5.3f", rms_mix_ntcFilter4.read() );
            Serial8.println(str_buf1);
        }
    }
    */
    
    if(!activeButtonOrKnob) {
        activeButtonOrKnob = myMenu.checkButtonsAndKnobs();
    }
    if(currentMillis - prevMilMenu >= readKnobInterval) {  //every 50ms 
        prevMilMenu = currentMillis;
        if(activeButtonOrKnob | myMenu.reDoMenu) {
            myMenu.doMenu();
            activeButtonOrKnob = false;
        } else if(programChangeFlag){
           patchSelect();
           gotoPatchSelectMenu(); // to display the new patch selection
           myMenu.displayMenu();
           programChangeFlag = false;
        }
    }
    if (preUpdateSynthVariablesFlag){ // set in patchToSynthVariables()
        env_squelch.noteOff(); //start squelch audio (release)
        env_squelchL.noteOff(); //start squelch audio (release)
        env_squelchR.noteOff(); //start squelch audio (release)
        if(currentMillis - prevMilUpdateSynth >= updateSynthDelay) {
           preUpdateSynthVariablesFlag = false; 
           updateSynthVariablesFlag = true;
           env_squelch.noteOn(); //start un squelch audio (attack)
           env_squelchL.noteOn(); //start un squelch audio (attack)
           env_squelchR.noteOn(); //start un squelch audio (attack)
        } 
    } else { prevMilUpdateSynth = currentMillis;} 
  
    //-------------------------------------------------------
    //  Calculate some parameters
    //  that will be used to update the Audio System 
    //  during AudioNoInterrupts()
    //-------------------------------------------------------

    //-------------------------------------------------------
    //  Update SynthVariables if they changed
    //-------------------------------------------------------
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
            mix_xfade.gain(0, 1.0f);  // all Osc1
            mix_xfade.gain(1, 0.0f);    // no Osc2 
        }
        //cacluateHeadRoom();
        mix_oscLevels.gain(0, LevelOsc1*LevelOscN_HeadRoom);
        mix_oscLevels.gain(1, LevelOsc2*LevelOscN_HeadRoom);
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

        //freeverbs1.roomsize(EffectsReverbDenseEarly);
        //freeverbs1.damping(EffectsReverbDamp);
        verb_F32.mix(EffectsReverbLevel); // 1.0 = full wet
        verb_F32.size(EffectsReverbTime); // sets reverb time (0.0 to 1.0f)
        verb_F32.diffusion(EffectsReverbDenseEarly); // lower values for more single repeats (echos) (0.0 to 1.0f)
        verb_F32.hidamp(EffectsReverbDamp);  // treble loss in reverb tails (0.0 to 1.0f)

        //filterPostDelayL.frequency(EffectsDelayDamp);
        //filterPostDelayR.frequency(EffectsDelayDamp);
        mix_Amp_gain_0 =  AmpLevel*Amp_HeadRoom;

        //dly_delayEffects.delay(0, EffectsDelayTimeL);
        EffectsDelayTimeR = EffectsDelayTimeL*0.85f;  //TODO: create separate menu entries for left right disparity
        //dly_delayEffects.delay(1, EffectsDelayTimeR);  //TODO: adjust feedback gain lower as L/R times get similar
        //mix_delayFeedback.gain(1,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
        //mix_delayFeedback.gain(2,pow(EffectsDelayFeedback, gammaDelayFeedback)); // finer adjustment at low end
        //mix_delayLevelL.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));       //finer adjustment at low end
        //mix_delayLevelR.gain(1,maxDelayLevel*pow(EffectsDelayLevel, gammaDelayLevel));       //finer adjustment at low end
      delay_F32.time(EffectsDelayTimeL); // 1.27sec and use psram
      delay_F32.mix(EffectsDelayLevel);
      delay_F32.feedback(EffectsDelayFeedback); // 0 - 1.0 
        updateSynthVariablesFlag = false;
        //PRINT_VALUES_FLAG = true;
        PRINT_VALUES_FLAG = false;
    }
        // no reason to wait for these 
        mix_lineInLR_gain_0 = volf;
        mix_lineInLR_gain_1 = mix_lineinf*extraLineInAmpFactor;

    //-------------------------------------------------------
    //  Update Realtime Audio System
    //      Time to update variables that need to be changed 
    //      all at the same time.  
    //-------------------------------------------------------
    AudioNoInterrupts();
        mix_Amp.gain(0, mix_Amp_gain_0); 
        mix_Amp.gain(1, mix_Amp_gain_1);  // 4000s AmpLevel doesn't control Noise Level
        //mix_lineInL.gain(0, volf*extraAmpFactor*(1.0-mix_lineinf));
        mix_lineInL.gain(0, mix_lineInLR_gain_0);
        mix_lineInL.gain(1, mix_lineInLR_gain_1);
        //mix_lineInL.gain(2, 0.0);
        //mix_lineInL.gain(3, 0.0);
        //mix_lineInR.gain(0, volf*extraAmpFactor*(1.0-mix_lineinf));
        mix_lineInR.gain(0, mix_lineInLR_gain_0);
        mix_lineInR.gain(1, mix_lineInLR_gain_1);
        //mix_lineInR.gain(2, 0.0);
        //mix_lineInR.gain(3, 0.0);
    
    mix_pongL_F32.gain(0,0.5f*EffectsDelayPong + 0.5f); 
    mix_pongL_F32.gain(1,-0.5f*EffectsDelayPong + 0.5f);
    mix_pongR_F32.gain(0,-0.5f*EffectsDelayPong + 0.5f);
    mix_pongR_F32.gain(1,0.5f*EffectsDelayPong + 0.5f);

    noteNumberOsc1 = porta_step ? round(dc_portatime.read()*128.0) : dc_portatime.read()*128;
    noteNumberOsc1 = BendStep ? round( noteNumberOsc1 + BendRange * dc_pitchbend.read() )
                                     : noteNumberOsc1 + BendRange * dc_pitchbend.read();
    noteNumberOsc1 = noteNumberOsc1 + OctOsc1*12.0
                    + SemiOsc1+FineOsc1 + FineTuneCentsf + Transposef + Octavef;
    //noteNumberFilter1 = dc_portatimef.read()*128 + OctOsc1*12.0 + SemiOsc1+FineOsc1;
    //noteNumberFilter1 = dc_portatimef.read()*128 + SemiOsc1+FineOsc1;
    noteNumberFilter1 = dc_portatimef.read()*128.0f + FineTuneCentsf + Transposef + Octavef;
    noteNumberOsc2 = porta_step ? round(dc_portatime.read()*128.0) : dc_portatime.read()*128;
    noteNumberOsc2 = BendStep ? round( noteNumberOsc2 + BendRange * dc_pitchbend.read() )
                                     : noteNumberOsc2 + BendRange * dc_pitchbend.read();
    noteNumberOsc2 = noteNumberOsc2 + OctOsc2*12.0
                    + SemiOsc2+FineOsc2 + FineTuneCentsf + Transposef + Octavef;
    //noteNumberFilter2 = dc_portatimef.read()*128 + OctOsc2*12.0 + SemiOsc2+FineOsc2;
    //noteNumberFilter2 = dc_portatimef.read()*128 + SemiOsc2+FineOsc2;
    noteFreqOsc1 = 440.0 * pow(2, (noteNumberOsc1-69.0)/12 );  // 69 is note number for A4=440Hz
    noteFreqOsc1 = noteFreqOsc1+BeatOsc1; // BeatOsc1 is additive
    noteFreqOsc2 = 440.0 * pow(2, (noteNumberOsc2-69.0)/12 );  // 69 is note number for A4=440Hz
    noteFreqOsc2 = noteFreqOsc2+BeatOsc2; // BeatOsc2 is additive
    //noteFreqFilter5 = 440.0 * pow(2, (min(60,min(noteNumberOsc1,noteNumberOsc2))-69.0-12.0)/12 );  // always Oct below noteNumberOsc1 or 2 whichever is lower;  TODO: match 4000s
    //noteFreqFilter5 = 440.0 * pow(2, (min(60,min(noteNumberOsc1,noteNumberOsc2))-69.0)/12 );  // always Oct below noteNumberOsc1 or 2 whichever is lower;  TODO: match 4000s
    noteFreqFilter5 = 440.0 * pow(2, (min(noteNumberOsc1,noteNumberOsc2)-69.0)/12 );  // always at noteNumberOsc1 or 2 whichever is lower;  TODO: match 4000s
    //noteFreqFilter5 = 440.0 * pow(2, (noteNumberOsc1-69.0-12.0)/12 );  // always Oct below noteNumberOsc1 or 2 whichever is lower;  TODO: match 4000s
    keyfollowFilter1 = pow(2, (noteNumberFilter1-offsetNoteKeyfollow)*KeyFollowOscFilter1/144.0); //60 is C4   
    keyfollowFilter2 = pow(2, (noteNumberFilter1-offsetNoteKeyfollow)*KeyFollowOscFilter2/144.0); //60 is C4   
    keyfollowFilter3 = pow(2, (noteNumberFilter1-offsetNoteKeyfollowNoise)*KeyFollowNoiseFilter3/144.0); //60 is C4   
    keyfollowFilter4 = pow(2, (noteNumberFilter1-offsetNoteKeyfollowNoise)*KeyFollowNoiseFilter4/144.0); //60 is C4   
    keyfollowFilterPreNoise = pow(2, ( (noteNumberFilter1 < minPreNoiseNoteNumbr ? minPreNoiseNoteNumbr : noteNumberFilter1)- offsetNoteKeyfollowPreNoise )
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
    mix_fcModFilter1.gain(0, BreathModOscFilter1*FreqOscFilter1BModFactor); 
    mix_fcModFilter1.gain(3, modOffsetFilter1*FreqOscFilter1BModFactor); // modOffsetFilter1 = 1 in globals
    dc_modOffsetOscFilter1.amplitude(-BreathModOscFilter1);  // subtract off BM, 
    if (LinkOscFilters) 
    {
        filter2.frequency(clippedFreqFilter1);
        filter2.resonance(QFactorOscFilter1);   // Q factor
        mix_fcModFilter2.gain(0,  BreathModOscFilter1*FreqOscFilter1BModFactor); 
        mix_fcModFilter2.gain(3, modOffsetFilter1*FreqOscFilter1BModFactor); 
        ModeOscFilter2 = ModeOscFilter1;    
        dc_modOffsetOscFilter2.amplitude(-BreathModOscFilter1);
    }
    else
    {
        filter2.frequency(clippedFreqFilter2);
        filter2.resonance(QFactorOscFilter2);   // Q factor
        mix_fcModFilter2.gain(0,  BreathModOscFilter2*FreqOscFilter2BModFactor); 
        mix_fcModFilter2.gain(3, modOffsetFilter2*FreqOscFilter2BModFactor); 
        dc_modOffsetOscFilter2.amplitude(-BreathModOscFilter2);
    }
    filter3.frequency(clippedFreqFilter3);  
    filter3.resonance(QFactorNoiseFilter3);   // Q factor
    mix_fcModFilter3.gain(0,  BreathModNoiseFilter3*FreqNoiseFilter3BModFactor); 
    mix_fcModFilter3.gain(3, modOffsetFilter3*FreqNoiseFilter3BModFactor); 
    dc_modOffsetNoiseFilter3.amplitude(-BreathModNoiseFilter3);
    if (LinkNoiseFilters)
    {
        filter4.frequency(clippedFreqFilter3);
        filter4.resonance(QFactorNoiseFilter3);   // Q factor
        mix_fcModFilter4.gain(0,  BreathModNoiseFilter3*FreqNoiseFilter3BModFactor); 
        mix_fcModFilter4.gain(3, modOffsetFilter3*FreqNoiseFilter3BModFactor); 
        ModeNoiseFilter4 = ModeNoiseFilter3;    
        dc_modOffsetNoiseFilter4.amplitude(-BreathModNoiseFilter3);
    }
    else
    {
        filter4.frequency(clippedFreqFilter4);
        filter4.resonance(QFactorNoiseFilter4);   // Q factor
        mix_fcModFilter4.gain(0,  BreathModNoiseFilter4*FreqNoiseFilter4BModFactor); 
        mix_fcModFilter4.gain(3, modOffsetFilter4*FreqNoiseFilter4BModFactor); 
        dc_modOffsetNoiseFilter4.amplitude(-BreathModNoiseFilter4);
    }
    filterPreNoise.frequency(clippedFreqFilterPreNoise); 
    filter5.frequency(noteFreqFilter5); // HP filter post mix_Amp
    //filterPreMixHPL.frequency(noteFreqFilter5);
    //filterPreMixHPR.frequency(noteFreqFilter5);
    filterPreMixHPL.frequency(62.5f);
    filterPreMixHPR.frequency(62.5f);


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
    if (updateEpromFlag )
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
//void cacluateHeadRoom(){
//    LevelOscN_HeadRoom = 0.4f; //1.0f/1.0f;
//    extraAmpFactor = 7.5; //3.0f; 
//    Amp_HeadRoom = 1.0f;
//}

//TODO: replace this with all float version
//This function converts linear to expo, using gamma as the exponential amount
// example:  lin_to_log(data2, 127, breath_gamma)/127.0
float lin_to_log( int input, int input_max, float gamma) {
    float val = (float) ( pow ((float)input / (float)input_max, (float)gamma));
    val = limit(val,1.0,0.0) * (float)input_max ; 
    if(input>0.0f)
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
       //float gamma = pow( x, 1.0)*2.0; 
       //float gamma = pow( x, 1.0f-x*0.5f)*2.0f;
       float gamma;
//       if(input >= 64.0f/127.0f){
//         gamma = 4.0f*pow(x,2.0f)-8.0f*input+4.0f; 
//       } else {
         gamma = pow( x, 1.0)*2.0;  
//       }
    return gamma;
}


CurveLines gen_filter_curve_lines(float input) {
    // generate piece-wise linear curve from:
    // loSlope, midIntercept (slope = 1), hiSlope
    CurveLines fcl = {0.0f, 1.0f, 1.0f}; // {midIntercept, loSlope, hiSlope}
    if (input >= 0.5f){
        fcl.midIntercept = 2.0f*input-1.0f;
        fcl.loSlope = 0.01f*exp(9.21f*input);
        fcl.hiSlope = 63.5f*exp(-8.3f*input);
    } else {
        fcl.midIntercept = input - 0.5f;
        fcl.loSlope = 0.2f*exp(3.219f*input);
        fcl.hiSlope = 10.0f*exp(-4.6f*input);
    }
    return fcl;
}

CurveLines gen_osc_curve_lines(float input) {
    // generate piece-wise linear curve from:
    // loSlope, midIntercept (slope = 1), hiSlope
   // CurveLines fcl = {0.0f, 1.0f, 1.0f}; // {midIntercept, loSlope, hiSlope}
    float skewed_input = input >= 0.6f ? 5.0f/4.0f*(input-1.0f)+1.0f : 5.0f/6.0f*input;
    return gen_filter_curve_lines(skewed_input);
}

//CurveLines gen_noise_curve_lines(float input) {
//    float skewed_input = input >= 0.6 ? 5.0f/4.0f*(input-1.0f)+1.0f : 5.0f/6.0f*input;
//    return gen_filter_curve_lines(skewed_input);
//}

float piecewise_curve_func(float x, CurveLines fcl){
    // m*x+b for lo mid and high
    //if ( fcl.midIntercept == 0.0f ) { return x;}
    float loLine  = x*fcl.loSlope; 
    float midLine = x + fcl.midIntercept; 
    float hiLine  = fcl.hiSlope*(x - 1.0f) + 1.0f;
    if ( fcl.midIntercept > 0.0f ){
        return min(min(loLine,midLine),hiLine);
    } else {
        return max(max(loLine,midLine),hiLine);
    }
}

float log_pot_curve(float x, float Ymid) {
   // y = a*b^x - a; where b = (1/Ymid-1)^2, a = 1/(b-1);
   if (Ymid > 0.499f && Ymid < 0.501f) { return x; }
   float b = pow(1.0f/Ymid - 1.0f,2.0f);
   return (pow(b,x)-1.0f)/(b-1.0f);
}

float gen_noise_gamma(float input) {
       float x = 1.0-input; 
       float gamma = pow( x, 1.5)*5.0; 
    return gamma;
}

float gamma_func(float x, float gamma) {
    if (x == 0.0f)
        return 0.0f;
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
    if(breath < 0.0f)
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
    //if (ModeOscFilter1 !=modeFilter1_old)
    //{
    //    modeFilter1_old = ModeOscFilter1;
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
    //}   
    //if (ModeOscFilter2 !=modeFilter2_old)
    //{
    //    modeFilter2_old = ModeOscFilter2;
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
    //}   
    //if (ModeNoiseFilter3 !=modeFilter3_old)
    //{
    //    modeFilter3_old = ModeNoiseFilter3;
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
    //}  
    //if (ModeNoiseFilter4 !=modeFilter4_old)
    //{
    //    modeFilter4_old = ModeNoiseFilter4;
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
    //}    
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
                MIDIs1.sendAfterTouch(data1,data2,channel); 
                //MIDIs1.sendPolyPressure(data1,data2,channel); // deprecated, so use the overloaded one
        case midi_ho.AfterTouchChannel: //0xD0
            if (midi_from_host_flag)
                MIDIs1.sendAfterTouch(data1,channel); // TODO: add AT as method of breath control
        case midi_ho.ControlChange: //0xB0
            if (midi_from_host_flag)
                MIDIs1.sendControlChange(data1,data2,channel);
            switch (data1){
                case CC_MODULATION_WHEEL:
                case CC_BREATH: 
                case 3: 
                case CC_FOOT_PEDAL: 
                case CC_VOLUME:
                case CC_BALANCE: 
                case 9: 
                case CC_PAN:
                case CC_EXPRESSION:
                    if (data1 != (uint8_t)breath_cc )
                    {
                        break;
                    }
                    else
                    {
                        data2f = ((float)data2) * DIV127;
                        previousBreathf = lastBreathf;
                        lastBreathf = data2f;
                        dc_breathLfoFilter1_amp = lfoThresh(data2f,LfoThreshOscFilter1,LfoDepthOscFilter1,LfoBreathOscFilter1);
                        dc_breathLfoFilter2_amp = lfoThresh(data2f,LfoThreshOscFilter2,LfoDepthOscFilter2,LfoBreathOscFilter2);
                        dc_breathLfoFilter3_amp = lfoThresh(data2f,LfoThreshNoiseFilter3,LfoDepthNoiseFilter3,LfoBreathNoiseFilter3);
                        dc_breathLfoFilter4_amp = lfoThresh(data2f,LfoThreshNoiseFilter4,LfoDepthNoiseFilter4,LfoBreathNoiseFilter4);
                        dc_breathLfoFilter1.amplitude(dc_breathLfoFilter1_amp,dc_breathLfoFilter_rampTime);
                        dc_breathLfoFilter2.amplitude(dc_breathLfoFilter2_amp,dc_breathLfoFilter_rampTime);
                        dc_breathLfoFilter3.amplitude(dc_breathLfoFilter3_amp,dc_breathLfoFilter_rampTime);
                        dc_breathLfoFilter4.amplitude(dc_breathLfoFilter4_amp,dc_breathLfoFilter_rampTime);
                        dc_breathOscFilter1_amp = piecewise_curve_func(data2f, BreathOscFiltCurveLines1);
                        dc_breathOscFilter2_amp = piecewise_curve_func(data2f, BreathOscFiltCurveLines2);
                        dc_breathNoiseFilter3_amp = piecewise_curve_func(data2f, BreathNoiseFiltCurveLines3);
                        dc_breathNoiseFilter4_amp = piecewise_curve_func(data2f, BreathNoiseFiltCurveLines4);
                        //dc_breathNoise_amp = gamma_func(data2f,NoiseBreathCurve);
                        dc_breathNoise_amp = piecewise_curve_func(data2f,NoiseBreathCurveLines);
                        if(BreathAttainOsc1 > 0.0f)
                            dc_breathSweepOsc1.amplitude(BreathDepthOsc1*(1.0-limit(data2f/BreathAttainOsc1,1.0,-1.0)),
                                                        dc_breathSweepOscN_rampTime); // smoothing
                        else
                            dc_breathSweepOsc1.amplitude(0.0f);
                        if(BreathAttainOsc2 > 0.0f)
                            dc_breathSweepOsc2.amplitude(BreathDepthOsc2*(1.0-limit(data2f/BreathAttainOsc2,1.0,-1.0)),
                                                        dc_breathSweepOscN_rampTime); //  smoothing
                        else
                            dc_breathSweepOsc2.amplitude(0.0);

                        if(note_is_on)
                        {
                            //dc_breathThreshOsc1_amp = gamma_func(thresh( data2f,BreathThreshOsc1), BreathCurveOsc1);
                            //dc_breathThreshOsc2_amp = gamma_func(thresh( data2f,BreathThreshOsc2), BreathCurveOsc2);
                            dc_breathThreshOsc1_amp = piecewise_curve_func(thresh( data2f,BreathThreshOsc1), BreathOscCurveLines1);
                            float dynoMax = 35.0f;
                            float dynoIntercept = 75.0f;
                            float rampTimeDynOscOn1 = limit((dc_breathThreshOsc1_amp-lastBreathf)
                                                        *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime); 
                            dc_breathThreshOsc2_amp = piecewise_curve_func(thresh( data2f,BreathThreshOsc2), BreathOscCurveLines2);
                            float rampTimeDynOscOn2 = limit((dc_breathThreshOsc2_amp-lastBreathf)
                                                        *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime); 
                            dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathThreshOscN_rampTime);
                            dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,dc_breathThreshOscN_rampTime);      
                            //dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,rampTimeDynOscOn1);
                            //dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,rampTimeDynOscOn2);      
                            dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp,dc_breathFilterN_rampTime);
                            dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp,dc_breathFilterN_rampTime);
                            dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp,dc_breathFilterN_rampTime);
                            dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp,dc_breathFilterN_rampTime);
                            if( NoiseLevel > 0.0f )
                            {
                                float dynoMax = 40.0f;
                                float dynoIntercept = 90.0f;
                                float rampTimeDynNoiseOn = limit((dc_breathNoise_amp-lastBreathf)
                                                            *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime); 
                                dc_breathNoise.amplitude(dc_breathNoise_amp,dc_breathNoise_rampTime);
                                //dc_breathNoise.amplitude(dc_breathNoise_amp,rampTimeDynNoiseOn);
                            }
                        }
                        else // note is off
                        {
                            dc_breathThreshOsc1_amp = 0;
                            dc_breathThreshOsc2_amp = 0;
                            dc_breathNoise_amp = 0;
                            // TODO: make rampOffTime slower for BC higher values
                            float dynoMax = 35.0f;
                            float dynoIntercept = 75.0f;
                            float rampTimeDynOscOn1 = limit((previousBreathf-dc_breathThreshOsc1_amp)
                                                    *dynoIntercept,dynoMax,dc_breathOff_rampTime); 
                            float rampTimeDynOscOn2 = limit((previousBreathf-dc_breathThreshOsc1_amp)
                                                    *dynoIntercept,dynoMax,dc_breathOff_rampTime); 
                            float rampTimeDynNoiseOn = limit((previousBreathf-dc_breathNoise_amp)
                                                        *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime); 
                            //dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,rampTimeDynOscOn1);
                            //dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,rampTimeDynOscOn2);      
                            dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathOff_rampTime);
                            dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,dc_breathOff_rampTime);      
                            dc_breathOscFilter1.amplitude(0,dc_breathOff_rampTime);
                            dc_breathOscFilter2.amplitude(0,dc_breathOff_rampTime);
                            dc_breathNoiseFilter3.amplitude(0,dc_breathOff_rampTime);
                            dc_breathNoiseFilter4.amplitude(0,dc_breathOff_rampTime);
                            //dc_breathNoise.amplitude(0,rampTimeDynNoiseOn);
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
            if (data2 == 0) { noteOffFun(); break; }
            data2f = ((float)data2)* DIV127;
            if (lastBreathf <= 0.0f){ 
                previousBreathf = 0.0f;
                lastBreathf = data2f; 
            } 
            //  Only treat Note on Velocity as a Breath value if lastBreathf was zero
            dc_breathThreshOsc1_amp = piecewise_curve_func(thresh( lastBreathf,BreathThreshOsc1), BreathOscCurveLines1);
            dc_breathThreshOsc2_amp = piecewise_curve_func(thresh( lastBreathf,BreathThreshOsc2), BreathOscCurveLines2);
            dc_breathLfoFilter1_amp = lfoThresh(lastBreathf,LfoThreshOscFilter1,LfoDepthOscFilter1,LfoBreathOscFilter1);
            dc_breathLfoFilter2_amp = lfoThresh(lastBreathf,LfoThreshOscFilter2,LfoDepthOscFilter2,LfoBreathOscFilter2);
            dc_breathLfoFilter3_amp = lfoThresh(lastBreathf,LfoThreshNoiseFilter3,LfoDepthNoiseFilter3,LfoBreathNoiseFilter3);
            dc_breathLfoFilter4_amp = lfoThresh(lastBreathf,LfoThreshNoiseFilter4,LfoDepthNoiseFilter4,LfoBreathNoiseFilter4);
            dc_breathLfoFilter1.amplitude(dc_breathLfoFilter1_amp,dc_breathLfoFilter_rampTime);
            dc_breathLfoFilter2.amplitude(dc_breathLfoFilter2_amp,dc_breathLfoFilter_rampTime);
            dc_breathLfoFilter3.amplitude(dc_breathLfoFilter3_amp,dc_breathLfoFilter_rampTime);
            dc_breathLfoFilter4.amplitude(dc_breathLfoFilter4_amp,dc_breathLfoFilter_rampTime);
            dc_breathOscFilter1_amp = piecewise_curve_func(lastBreathf, BreathOscFiltCurveLines1);
            dc_breathOscFilter2_amp = piecewise_curve_func(lastBreathf, BreathOscFiltCurveLines2);
            dc_breathNoiseFilter3_amp = piecewise_curve_func(lastBreathf, BreathNoiseFiltCurveLines3);
            dc_breathNoiseFilter4_amp = piecewise_curve_func(lastBreathf, BreathNoiseFiltCurveLines4);
            dc_breathNoise_amp = piecewise_curve_func(lastBreathf,NoiseBreathCurveLines);
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
                float rampTimeShortening = 0.82; 
                float filterRampTimeShortening = 0.25; 
                float dynoMax = 35.0f;
                float dynoIntercept = 75.0f;
                // lower values of lastBreath get extended rampTime for breathThreshOsc<N>.amplitiude
                // (1.0-lastBreathf)*rampTimeExtensionFactor;
                //float rampTimeDynOscN = limit((dc_breathThreshOsc1_amp-lastBreathf)*dynoIntercept,dc_breathThreshOscN_rampTime,dynoMax); 
                float rampTimeDynOsc1 = limit((dc_breathThreshOsc1_amp-lastBreathf)*dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime); 
                float rampTimeDynOsc2 = limit((dc_breathThreshOsc2_amp-lastBreathf)*dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime); 
                dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathThreshOscN_rampTime*rampTimeShortening);
                dc_breathThreshOsc2.amplitude(dc_breathThreshOsc1_amp,dc_breathThreshOscN_rampTime*rampTimeShortening);
                //dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,rampTimeDynOsc1);      
                //dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,rampTimeDynOsc2);      
                dc_breathOscFilter1.amplitude(dc_breathOscFilter1_amp,dc_breathFilterN_rampTime*filterRampTimeShortening);
                dc_breathOscFilter2.amplitude(dc_breathOscFilter2_amp,dc_breathFilterN_rampTime*filterRampTimeShortening);
                dc_breathNoiseFilter3.amplitude(dc_breathNoiseFilter3_amp,dc_breathFilterN_rampTime*filterRampTimeShortening);
                dc_breathNoiseFilter4.amplitude(dc_breathNoiseFilter4_amp,dc_breathFilterN_rampTime*filterRampTimeShortening);
                if( NoiseLevel > 0.0f) { 
                    float dynoMax = 40.0f;
                    float dynoIntercept = 90.0f;
                    float rampTimeDynNoiseOn = limit((dc_breathNoise_amp-lastBreathf)
                                              *dynoIntercept,dynoMax,dc_breathThreshOscN_rampTime); 
                    dc_breathNoise.amplitude(dc_breathNoise_amp,dc_breathNoise_rampTime*rampTimeShortening); 
                    //dc_breathNoise.amplitude(dc_breathNoise_amp,rampTimeDynNoiseOn); 
                }
                if(BreathAttainOsc1 > 0.0f) {
                    dc_breathSweepOsc1.amplitude(BreathDepthOsc1*(1.0-limit(lastBreathf/BreathAttainOsc1,1.0,-1.0)),
                                                dc_breathSweepOscN_rampTime*rampTimeShortening); 
                }
                else {
                    dc_breathSweepOsc1.amplitude(0.0f);
                }
                if(BreathAttainOsc2 > 0.0f) {
                    dc_breathSweepOsc2.amplitude(BreathDepthOsc2*(1.0-limit(lastBreathf/BreathAttainOsc2,1.0,-1.0)),
                                                dc_breathSweepOscN_rampTime*rampTimeShortening); 
                }
                else {
                    dc_breathSweepOsc2.amplitude(0.0f);
                }

                if(SweepDepthOsc1 != 0.0f)
                {
                    dc_sweepDepthOsc1.amplitude(SweepDepthOsc1);
                    dc_sweepDepthOsc1.amplitude(0,SweepTimeOsc1);
                }
                if(SweepDepthOsc2 != 0.0f)
                {
                    dc_sweepDepthOsc2.amplitude(SweepDepthOsc2);
                    dc_sweepDepthOsc2.amplitude(0,SweepTimeOsc2);
                }
                // dc_beatOsc1.amplitude(BeatOsc1); // TODO calculate beat number
                // dc_beatOsc2.amplitude(BeatOsc2); // TODO calculate beat number
                if(NoiseLevel >0.0f)
                {
                    dc_timeNoise.amplitude(TimeNoiseAmp); // effectively set by pink_Noise.amplitude(NoiseLevel);
                    dc_timeNoise.amplitude(0.0f,NoiseTime); // ramp down the noise level
                }
                else
                {
                    dc_timeNoise.amplitude(0.0f);
                }
                // the sign signal now is aplitude as well, so the sq_swpfltN always runs between 1.0 and 0
                dc_sweepDepthFilterSign1.amplitude(SweepDepthOscFilter1);
                dc_sweepDepthFilter1.amplitude(1.0f);
                dc_sweepDepthFilter1.amplitude(0.0f,SweepTimeOscFilter1);
                if(LinkOscFilters)
                {
                    dc_sweepDepthFilterSign2.amplitude(SweepDepthOscFilter1);
                    dc_sweepDepthFilter2.amplitude(1.0f);
                    dc_sweepDepthFilter2.amplitude(0.0f,SweepTimeOscFilter1);
                } else {
                    dc_sweepDepthFilterSign2.amplitude(SweepDepthOscFilter2);
                    dc_sweepDepthFilter2.amplitude(1.0f);
                    dc_sweepDepthFilter2.amplitude(0.0f,SweepTimeOscFilter2);
                }

                dc_sweepDepthFilterSign3.amplitude(SweepDepthNoiseFilter3);
                dc_sweepDepthFilter3.amplitude(1.0f);
                dc_sweepDepthFilter3.amplitude(0.0f,SweepTimeNoiseFilter3);
                if(LinkNoiseFilters)
                {
                    dc_sweepDepthFilterSign4.amplitude(SweepDepthNoiseFilter3);
                    dc_sweepDepthFilter4.amplitude(1.0f);
                    dc_sweepDepthFilter4.amplitude(0.0f,SweepTimeNoiseFilter3);
                } else {
                    dc_sweepDepthFilterSign4.amplitude(SweepDepthNoiseFilter4);
                    dc_sweepDepthFilter4.amplitude(1.0f);
                    dc_sweepDepthFilter4.amplitude(0.0f,SweepTimeNoiseFilter4);
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
                if( NoiseLevel > 0.0f) { dc_breathNoise.amplitude(dc_breathNoise_amp,dc_breathNoise_rampTime); }
                if(BreathAttainOsc1 > 0.0f) {
                    dc_breathSweepOsc1.amplitude(BreathDepthOsc1*(1.0-limit(lastBreathf/BreathAttainOsc1,1.0,-1.0)),
                                                dc_breathSweepOscN_rampTime); 
                }
                else {
                    dc_breathSweepOsc1.amplitude(0.0f);
                }
                if(BreathAttainOsc2 > 0.0f) {
                    dc_breathSweepOsc2.amplitude(BreathDepthOsc2*(1.0-limit(lastBreathf/BreathAttainOsc2,1.0,-1.0)),
                                                dc_breathSweepOscN_rampTime); 
                }
                else {
                    dc_breathSweepOsc2.amplitude(0.0f);
                }
            }
            currentMidiNote = data1;
            note_is_on = true;
            break;

        case midi_ho.NoteOff:  //(type==0x80)
            if (midi_from_host_flag)
                MIDIs1.sendNoteOff(data1,data2,channel);
            noteOffFun();
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

void noteOffFun(void){
    if(data1==currentMidiNote)
    {
        note_is_on = false;
        fMidiNoteNorm = ((float)data1)/128.0;
        dc_portatime.amplitude(fMidiNoteNorm); // 
        dc_portatimef.amplitude(fMidiNoteNorm,portatimef_t); // 

        // in case no breath signal comes after NoteOff
        previousBreathf = lastBreathf;
        lastBreathf = 0.0f;
        dc_breathThreshOsc1_amp = 0;
        dc_breathThreshOsc2_amp = 0;
        dc_breathNoise_amp = 0;

        float dynoMax = 35.0f;
        float dynoIntercept = 75.0f;
        float rampTimeDynOscOff = limit((previousBreathf-dc_breathThreshOsc1_amp) //TODO: (previous breathThresh value - 0)
                                *dynoIntercept,dynoMax,dc_breathOff_rampTime); 
        float rampTimeDynNoiseOff = limit((previousBreathf-dc_breathNoise_amp)
                                *dynoIntercept,dynoMax,dc_breathOff_rampTime); 
        //dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,rampTimeDynOscOff);      
        //dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,rampTimeDynOscOff);      
        dc_breathThreshOsc1.amplitude(dc_breathThreshOsc1_amp,dc_breathOff_rampTime);
        // dc_breathThreshOscN_rampTime
        dc_breathThreshOsc2.amplitude(dc_breathThreshOsc2_amp,dc_breathOff_rampTime);
        dc_breathOscFilter1.amplitude(0,dc_breathOff_rampTime);
        dc_breathOscFilter2.amplitude(0,dc_breathOff_rampTime);
        dc_breathNoiseFilter3.amplitude(0,dc_breathOff_rampTime);
        dc_breathNoiseFilter4.amplitude(0,dc_breathOff_rampTime);
        //dc_breathNoise.amplitude(0,rampTimeDynNoiseOff);
        dc_breathNoise.amplitude(0,dc_breathOff_rampTime);
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


void EEPROM_update(int addr, int val){  
   int temp_var;
   EEPROM.get(addr, temp_var);
   if( temp_var != val ){
      EEPROM.put(addr, val);
    }
}
