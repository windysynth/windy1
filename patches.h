/************************************************************
  Windy1 USB Host Synth for Wind controllers
        MIT license
        Copyright (c) 2021 Tim King

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

****************************************************************/

#ifndef patches_h_
#define patches_h_
#include <Arduino.h>

// #define NUMBER_OF_PATCHES 100
#define PATCHES_PER_BANK 128
#define NUMBER_OF_BANKS 1
#define NUMBER_OF_PATCHES (PATCHES_PER_BANK * NUMBER_OF_BANKS)
#define USEALL_PATCHFUNCS
#define USELOAD_PATCHFUNCS

#define PATCH_STRING_SYSPOS 0x0C
#define PATCH_STRING_LENGTH 31
#define NRPN_MSB_OSC1_SYSPOS 0x2C
#define NRPN_MSB_OSC1 64
#define NRPN_MSB_OSC1_LENGTH 18
#define NRPN_MSB_OSC2_SYSPOS 0x41
#define NRPN_MSB_OSC2 65
#define NRPN_MSB_OSC2_LENGTH 18
#define NRPN_MSB_OSC_FILT1_SYSPOS 0x56
#define NRPN_MSB_OSC_FILT1 72
#define NRPN_MSB_OSC_FILT1_LENGTH 12
#define NRPN_MSB_OSC_FILT2_SYSPOS 0x65
#define NRPN_MSB_OSC_FILT2 73
#define NRPN_MSB_OSC_FILT2_LENGTH 12
#define NRPN_MSB_NOISE_FILT3_SYSPOS 0x74
#define NRPN_MSB_NOISE_FILT3 74
#define NRPN_MSB_NOISE_FILT3_LENGTH 12
#define NRPN_MSB_NOISE_FILT4_SYSPOS 0x83
#define NRPN_MSB_NOISE_FILT4 75
#define NRPN_MSB_NOISE_FILT4_LENGTH 12
#define NRPN_MSB_SPARE_SYSPOS 0x92
#define NRPN_MSB_SPARE 79
#define NRPN_MSB_SPARE_LENGTH 3
#define NRPN_MSB_NOISE_SYSPOS 0x98
#define NRPN_MSB_NOISE 80
#define NRPN_MSB_NOISE_LENGTH 3
#define NRPN_MSB_COMMON1_SYSPOS 0x9E
#define NRPN_MSB_COMMON1 81
#define NRPN_MSB_COMMON1_LENGTH 10
#define NRPN_MSB_COMMON2_SYSPOS 0xAB
#define NRPN_MSB_COMMON2 88
#define NRPN_MSB_COMMON2_LENGTH 3
#define NRPN_MSB_CHORUS_SYSPOS 0xB1
#define NRPN_MSB_CHORUS 112
#define NRPN_MSB_CHORUS_LENGTH 9
#define NRPN_MSB_DELAY_SYSPOS 0xBD
#define NRPN_MSB_DELAY 113
#define NRPN_MSB_DELAY_LENGTH 5
#define NRPN_MSB_REVERB_SYSPOS 0xC5
#define NRPN_MSB_REVERB 114
#define NRPN_MSB_REVERB_LENGTH 5

#define NRPN_MSB_COMP 115
#define NRPN_MSB_COMP_LENGTH 7

// uint8_t nrpn_msb_osc1[NRPN_MSB_OSC1_LENGTH];               // 18
#define CCOCTOSC1 0
#define CCSEMIOSC1 1
#define CCFINEOSC1 2
#define CCBEATOSC1 3
#define CCSAWOSC1 5
#define CCTRIOSC1 6
#define CCPULSEOSC1 7
#define CCPWOSC1 8
#define CCPWMFREQOSC1 9
#define CCPWMDEPTHOSC1 10
#define CCSWEEPDEPTHOSC1 11
#define CCSWEEPTIMEOSC1 12
#define CCBREATHDEPTHOSC1 13
#define CCBREATHATTAINOSC1 14
#define CCBREATHCURVEOSC1 15
#define CCBREATHTHRESHOSC1 16
#define CCLEVELOSC1 17
// uint8_t nrpn_msb_osc2[NRPN_MSB_OSC2_LENGTH];               // 18
#define CCOCTOSC2 0
#define CCSEMIOSC2 1
#define CCFINEOSC2 2
#define CCBEATOSC2 3
#define CCSAWOSC2 5
#define CCTRIOSC2 6
#define CCPULSEOSC2 7
#define CCPWOSC2 8
#define CCPWMFREQOSC2 9
#define CCPWMDEPTHOSC2 10
#define CCSWEEPDEPTHOSC2 11
#define CCSWEEPTIMEOSC2 12
#define CCBREATHDEPTHOSC2 13
#define CCBREATHATTAINOSC2 14
#define CCBREATHCURVEOSC2 15
#define CCBREATHTHRESHOSC2 16
#define CCLEVELOSC2 17
// uint8_t nrpn_msb_osc_filt1[NRPN_MSB_OSC_FILT1_LENGTH];     // 12
#define CCMODEOSCFILTER1 0
#define CCFREQOSCFILTER1 1
#define CCQFACTOROSCFILTER1 2
#define CCKEYFOLLOWOSCFILTER1 3
#define CCBREATHMODOSCFILTER1 4
#define CCLFOFREQOSCFILTER1 5
#define CCLFODEPTHOSCFILTER1 6
#define CCLFOBREATHOSCFILTER1 7
#define CCLFOTHRESHOSCFILTER1 8
#define CCSWEEPDEPTHOSCFILTER1 9
#define CCSWEEPTIMEOSCFILTER1 10
#define CCBREATHCURVEOSCFILTER1 11
// uint8_t nrpn_msb_osc_filt2[NRPN_MSB_OSC_FILT2_LENGTH];     // 12
#define CCMODEOSCFILTER2 0
#define CCFREQOSCFILTER2 1
#define CCQFACTOROSCFILTER2 2
#define CCKEYFOLLOWOSCFILTER2 3
#define CCBREATHMODOSCFILTER2 4
#define CCLFOFREQOSCFILTER2 5
#define CCLFODEPTHOSCFILTER2 6
#define CCLFOBREATHOSCFILTER2 7
#define CCLFOTHRESHOSCFILTER2 8
#define CCSWEEPDEPTHOSCFILTER2 9
#define CCSWEEPTIMEOSCFILTER2 10
#define CCBREATHCURVEOSCFILTER2 11
// uint8_t nrpn_msb_noise_filt3[NRPN_MSB_NOISE_FILT3_LENGTH]; // 12
#define CCMODENOISEFILTER3 0
#define CCFREQNOISEFILTER3 1
#define CCQFACTORNOISEFILTER3 2
#define CCKEYFOLLOWNOISEFILTER3 3
#define CCBREATHMODNOISEFILTER3 4
#define CCLFOFREQNOISEFILTER3 5
#define CCLFODEPTHNOISEFILTER3 6
#define CCLFOBREATHNOISEFILTER3 7
#define CCLFOTHRESHNOISEFILTER3 8
#define CCSWEEPDEPTHNOISEFILTER3 9
#define CCSWEEPTIMENOISEFILTER3 10
#define CCBREATHCURVENOISEFILTER3 11
// uint8_t nrpn_msb_noise_filt4[NRPN_MSB_NOISE_FILT4_LENGTH]; // 12
#define CCMODENOISEFILTER4 0
#define CCFREQNOISEFILTER4 1
#define CCQFACTORNOISEFILTER4 2
#define CCKEYFOLLOWNOISEFILTER4 3
#define CCBREATHMODNOISEFILTER4 4
#define CCLFOFREQNOISEFILTER4 5
#define CCLFODEPTHNOISEFILTER4 6
#define CCLFOBREATHNOISEFILTER4 7
#define CCLFOTHRESHNOISEFILTER4 8
#define CCSWEEPDEPTHNOISEFILTER4 9
#define CCSWEEPTIMENOISEFILTER4 10
#define CCBREATHCURVENOISEFILTER4 11
// uint8_t nrpn_msb_spare[NRPN_MSB_SPARE_LENGTH];             // 3
// uint8_t nrpn_msb_noise[NRPN_MSB_NOISE_LENGTH];             // 3
#define CCNOISETIME 0
#define CCNOISEBREATHCURVE 1
#define CCNOISELEVEL 2
// uint8_t nrpn_msb_common1[NRPN_MSB_COMMON1_LENGTH];         // 10
#define CCBENDRANGE 0
#define CCBENDSTEP 1
#define CCVIBRATOPITCH 2
#define CCLINKOSCFILTERS 3
#define CCLINKNOISEFILTERS 4
#define CCFORMANT 5
#define CCXFADE 6
#define CCKEYTRIGGER 7
#define CC81_8 8
#define CCCHORUSON 9
// uint8_t nrpn_msb_common2[NRPN_MSB_COMMON2_LENGTH];         // 3
#define CCVIBRATOAMP 0
#define CCAMPLEVEL 1
#define CCOCTBUTTONLEVEL 2
// uint8_t nrpn_msb_chorus[NRPN_MSB_CHORUS_LENGTH];           // 9
#define CCEFFECTSCHORUSDELAY1 0
#define CCEFFECTSCHORUSMOD1 1
#define CCEFFECTSCHORUSWET1 2
#define CCEFFECTSCHORUSDELAY2 3
#define CCEFFECTSCHORUSMOD2 4
#define CCEFFECTSCHORUSWET2 5
#define CCEFFECTSCHORUSFEEDBACK 6
#define CCEFFECTSCHORUSLFOFREQ 7
#define CCEFFECTSCHORUSDRYLEVEL 8
// uint8_t nrpn_msb_delay[NRPN_MSB_DELAY_LENGTH];             // 5
#define CCEFFECTSDELAYTIME 0
#define CCEFFECTSDELAYFEEDBACK 1
#define CCEFFECTSDELAYDAMP 2
#define CCEFFECTSDELAYLEVEL 3
// CCEFFECTSDELAYSPARE used for "Pong"
#define CCEFFECTSDELAYSPARE 4
// uint8_t nrpn_msb_reverb[NRPN_MSB_REVERB_LENGTH];           // 5
#define CCEFFECTSREVERBSPARE 0
#define CCEFFECTSREVERBLEVEL 1
#define CCEFFECTSREVERBDENSEEARLY 2
#define CCEFFECTSREVERBTIME 3
#define CCEFFECTSREVERBDAMP 4

#define EFFECTGROUPCHORUS 0
#define EFFECTGROUPDELAY 1
#define EFFECTGROUPREVERB 2
#define EFFECTGROUPCOMMON1 3

#define CCCOMPATTACK 0
#define CCCOMPRELEASE 1
#define CCCOMPRATIO 2
#define CCCOMPTHRESHOLD 3
#define CCCOMPKNEE 4
#define CCCOMPMAKEUPGAIN 5
#define CCCOMPSIDECHAIN 6

typedef struct __attribute__((packed)) Patch
{
    char patch_string[32];
    uint8_t nrpn_msb_osc1[NRPN_MSB_OSC1_LENGTH];               // 18
    uint8_t nrpn_msb_osc2[NRPN_MSB_OSC2_LENGTH];               // 18
    uint8_t nrpn_msb_osc_filt1[NRPN_MSB_OSC_FILT1_LENGTH];     // 12
    uint8_t nrpn_msb_osc_filt2[NRPN_MSB_OSC_FILT2_LENGTH];     // 12
    uint8_t nrpn_msb_noise_filt3[NRPN_MSB_NOISE_FILT3_LENGTH]; // 12
    uint8_t nrpn_msb_noise_filt4[NRPN_MSB_NOISE_FILT4_LENGTH]; // 12
    uint8_t nrpn_msb_spare[NRPN_MSB_SPARE_LENGTH];             // 3
    uint8_t nrpn_msb_noise[NRPN_MSB_NOISE_LENGTH];             // 3
    uint8_t nrpn_msb_common1[NRPN_MSB_COMMON1_LENGTH];         // 10
    uint8_t nrpn_msb_common2[NRPN_MSB_COMMON2_LENGTH];         // 3
    uint8_t nrpn_msb_chorus[NRPN_MSB_CHORUS_LENGTH];           // 9
    uint8_t nrpn_msb_delay[NRPN_MSB_DELAY_LENGTH];             // 5
    uint8_t nrpn_msb_reverb[NRPN_MSB_REVERB_LENGTH];           // 5
} patch_t;                                                     // 154 bytes

typedef struct __attribute__((packed)) Fx
{
    char fx_string[32];
    uint8_t nrpn_msb_chorus[NRPN_MSB_CHORUS_LENGTH];   // 9
    uint8_t nrpn_msb_delay[NRPN_MSB_DELAY_LENGTH];     // 5
    uint8_t nrpn_msb_reverb[NRPN_MSB_REVERB_LENGTH];   // 5
    uint8_t nrpn_msb_common1[NRPN_MSB_COMMON1_LENGTH]; // 10
} fx_t;

typedef struct __attribute__((packed)) Ext
{
    char ext_string[32];
    uint8_t nrpn_msb_compressor[NRPN_MSB_COMP_LENGTH]; // 7
} ext_t;

typedef enum
{
    LP,
    HP,
    BP,
    NTC,
    THRU
} filterMode_t;

uint8_t *getFxValue(uint32_t effectGroup, uint32_t effectIdx);
void setFxValue(uint8_t value, patch_t *patch, uint32_t effectGroup, uint32_t effectIdx);

void setDestination();

void nprnToDisplayChorus(char *dispValBuf, uint8_t *val);       // 0=Off 1=Mono(4k) 2=Stereo
void nprnToDisplayOnOff(char *dispValBuf, uint8_t *val);        // 0=Off 1=On
void nprnToDisplayPatchGlobal(char *dispValBuf, uint8_t *val);  // 0=Global 1=Patch
void nprnToDisplayKeyTrigger(char *dispValBuf, uint8_t *val);   // 0=Mult 1=Single
void nprnToDisplay03d(char *dispValBuf, uint8_t *val);          // 000 to 127
void nprnToDisplay03dm(char *dispValBuf, uint8_t *val);         // -64 to +63
void nprnToDisplay03dmS(char *dispValBuf, uint8_t *val);        // 0 to 127 mS
void nprnToDisplay04d10x(char *dispValBuf, uint8_t *val);       // 0 to 1270
void nprnToDisplay04d100x(char *dispValBuf, uint8_t *val);      // 1000 to 5000
void nprnToDisplay03dFlip(char *dispValBuf, uint8_t *val);      // 127 to 0
void nprnToDisplay2p1fHzDiv10(char *dispValBuf, uint8_t *val);  //  val/10.0 Hz
void nprnToDisplayFilterMode(char *dispValBuf, uint8_t *val);   // 0=LP 1=HP 2=BP 3=NTC 4=Thru
void nprnToDisplayFilterFreqHz(char *dispValBuf, uint8_t *val); //
void nprnToDisplayFormant(char *dispValBuf, uint8_t *val);      // 0=Off, 1=Woodwind, 2=String
void nprnToDisplayCurrentPatch(char *dispValBuf, uint8_t *val); //
void nprnToDisplayDest(char *dispValBuf, uint8_t *val);

void volAdjustFun();
void auxInAdjustFun();
void octaveAdjustFun();
void semiAdjustFun();
void centsAdjustFun();
void breathccAdjustFun();
void patchToDummyFunc();
void fxSourcToSynth();
void compAttackAdjustFun();
void compReleaseAdjustFun();
void compRatioAdjustFun();
void compThreshAdjustFun();
void compKneeAdjustFun();
void compMUGainAdjustFun();
void nnBmodAdjustFun();
void patchToOctOsc1();              // 64,0,62,66,
void patchToSemiOsc1();             // 64,1,52,76,
void patchToFineOsc1();             // 64,2,14,114, -50 to +50 cents
void patchToBeatOsc1();             // 64,3,0,127,
void patchToSawOsc1();              // 64,5,0,127,
void patchToTriOsc1();              // 64,6,0,127,
void patchToPulseOsc1();            // 64,7,0,127,
void patchToPwOsc1();               // 64,8,0,127,  GUI 0 = 50%
void patchToPwmFreqOsc1();          // 64,9,0,127,
void patchToPwmDepthOsc1();         // 64,10,0,127,
void patchToSweepDepthOsc1();       // 64,11,0,127,
void patchToSweepTimeOsc1();        // 64,12,0,127,
void patchToBreathDepthOsc1();      // 64,13,0,127,
void patchToBreathAttainOsc1();     // 64,14,0,127,
void patchToBreathOscCurveLines1(); // 64,15,0,127,
void patchToBreathThreshOsc1();     // 64,16,0,127,
void patchToLevelOsc1();            // 64,17,0,127,
void patchToOctOsc2();              // 65,0,62,66,
void patchToSemiOsc2();             // 64,1,52,76,
void patchToFineOsc2();             // 64,2,14,114, -50 to +50 cents
void patchToBeatOsc2();             // 64,3,0,127,
void patchToSawOsc2();              // 64,5,0,127,
void patchToTriOsc2();              // 64,6,0,127,
void patchToPulseOsc2();            // 64,7,0,127,
void patchToPwOsc2();               // 64,8,0,127,  GUI 0 = 50%
void patchToPwmFreqOsc2();          // 64,9,0,127,
void patchToPwmDepthOsc2();         // 64,10,0,127,
void patchToSweepDepthOsc2();       // 64,11,0,127,
void patchToSweepTimeOsc2();        // 64,12,0,127,
void patchToBreathDepthOsc2();      // 64,13,0,127,
void patchToBreathAttainOsc2();     // 64,14,0,127,
void patchToBreathOscCurveLines2(); // 64,15,0,127,
void patchToBreathThreshOsc2();     // 64,16,0,127,
void patchToLevelOsc2();            // 64,17,0,127,
void patchToModeOscFilter1();       // 72,0,0,4,//LP HP BP NTC OFF
void patchToFreqOscFilter1();
void patchToQFactorOscFilter1();        // 72,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowOscFilter1();      // 72,3,52,88,// -12 to +24 num semi tones
void patchToBreathModOscFilter1();      // 72,4,0,127,
void patchToLfoFreqOscFilter1();        // 72,5,0,127,
void patchToLfoDepthOscFilter1();       // 72,6,0,127,
void patchToLfoBreathOscFilter1();      // 72,7,0,127,
void patchToLfoThreshOscFilter1();      // 72,8,0,127,
void patchToSweepDepthOscFilter1();     // 72,9,0,127,
void patchToSweepTimeOscFilter1();      // 72,10,0,127,
void patchToBreathOscFiltCurveLines1(); // 72,11,0,127;
void patchToModeOscFilter2();           // 73,0,0,4,//LP HP BP NTC OFF
void patchToFreqOscFilter2();
void patchToQFactorOscFilter2();        // 72,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowOscFilter2();      // 72,3,52,88,// -12 to +24 num semi tones
void patchToBreathModOscFilter2();      // 72,4,0,127,
void patchToLfoFreqOscFilter2();        // 72,5,0,127,
void patchToLfoDepthOscFilter2();       // 72,6,0,127,
void patchToLfoBreathOscFilter2();      // 72,7,0,127,
void patchToLfoThreshOscFilter2();      // 72,8,0,127,
void patchToSweepDepthOscFilter2();     // 72,9,0,127,
void patchToSweepTimeOscFilter2();      // 72,10,0,127,
void patchToBreathOscFiltCurveLines2(); // 72,11,0,127,
void patchToModeNoiseFilter3();         // 74,0,0,4,//LP HP BP NTC OFF
void patchToFreqNoiseFilter3();
void patchToQFactorNoiseFilter3();    // 74,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowNoiseFilter3();  // 74,3,52,88,// -12 to +24 num semitones
void patchToBreathModNoiseFilter3();  // 74,4,0,127,
void patchToLfoFreqNoiseFilter3();    // 74,5,0,127,
void patchToLfoDepthNoiseFilter3();   // 74,6,0,127,
void patchToLfoBreathNoiseFilter3();  // 74,7,0,127,
void patchToLfoThreshNoiseFilter3();  // 74,8,0,127,
void patchToSweepDepthNoiseFilter3(); // 74,9,0,127,
void patchToSweepTimeNoiseFilter3();  // 74,10,0,127,
void patchToBreathNoiseFiltCurveLines3();
void patchToModeNoiseFilter4(); // 74,0,0,4,//LP HP BP NTC OFF
void patchToFreqNoiseFilter4();
void patchToQFactorNoiseFilter4();        // 74,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowNoiseFilter4();      // 74,3,52,88,// -12 to +24 num semitones
void patchToBreathModNoiseFilter4();      // 74,4,0,127,
void patchToLfoFreqNoiseFilter4();        // 74,5,0,127,
void patchToLfoDepthNoiseFilter4();       // 74,6,0,127,
void patchToLfoBreathNoiseFilter4();      // 74,7,0,127,
void patchToLfoThreshNoiseFilter4();      // 74,8,0,127,
void patchToSweepDepthNoiseFilter4();     // 74,9,0,127,
void patchToSweepTimeNoiseFilter4();      // 74,10,0,127,
void patchToBreathNoiseFiltCurveLines4(); // 74,11,0,127,
void patchToNoiseTime();                  // 80,0,0,127,
void patchToNoiseBreathCurveLines();      // 80,1,0,127,
void patchToNoiseLevel();                 // 80,2,0,127,
void patchToBendRange();                  // 81,0,0,12,// num semitones
void patchToBendStep();                   // 81,1,0,1,//0=off 1=on
void patchToLinkOscFilters();             // 81,3,1,2,//1=Link on 2= Link off
void patchToLinkNoiseFilters();           // 81,4,1,2,
void patchToFormant();                    // 81,5,0,2,//0=off 1=W.Wind 2=String
void patchToXFade();                      // 81,6,0,1,//0=off 1=on
void patchToKeyTriggerSingle();           // 81,7,0,1,//0=Multi 1=Single
void patchTos81_8();                      // 81,8,0,1,// ?
void patchToChorusOn();                   // 81,9,0,1,// Chorus on off
void patchToVibratoAmp();                 // 88,0,0,127, (bite tremelo amount)
void patchToAmpLevel();                   // 88,1,0,127,
// void patchToOctButtonLevel();             // 88,2,0,127,
void patchToEffectsChorusDelay1();
void patchToEffectsChorusMod1();       // 112,1,0,127, (-50% to +50%)
void patchToEffectsChorusWet1();       // 112,2,0,127, neg vals are phase inverted
void patchToEffectsChorusDelay2();     // 112,3,0,127,
void patchToEffectsChorusMod2();       // 112,4,0,127,
void patchToEffectsChorusWet2();       // 112,5,0,127,
void patchToEffectsChorusFeedback();   // 112,6,0,127,
void patchToEffectsChorusLfoFreq();    // 112,7,0,127,
void patchToEffectsChorusDryLevel();   // 112,8,0,127,
void patchToEffectsDelayTimeL();       // 113,0,0,127, 0 to 1270 ms
void patchToEffectsDelayFeedback();    // 113,1,0,127,
void patchToEffectsDelayDamp();        // 113,2,0,127,
void patchToEffectsDelayLevel();       // 113,3,0,127,
void patchToEffectsDelayPong();        // 113,3,0,127,
void patchToEffectsReverbSpare();      // 114,1,0,127,
void patchToEffectsReverbLevel();      // 114,1,0,127,
void patchToEffectsReverbDenseEarly(); // 114,2,0,127, (using this for roomsize)
void patchToEffectsReverbTime();       // 114,3,10,50,//1000 to 5000 ms
void patchToEffectsReverbDamp();       // 114,4,54,74,//-10 to +10

void patchSelect();
void patchSelectFromMenu();
void pasteDestSelect();
void swapDestSelect();
void patchToSynthVariables();

#ifdef USEALL_PATCHFUNCS
void printCurveMidiData();
void printPatchValues();
#endif
#ifdef USELOAD_PATCHFUNCS
void setCurrentPatch(int patchNumber);
void configureSD();
void savePatchSD(int i);
void saveGlobalFxSD();
void saveCurrentPatchSD(int i);
void saveCoppiedPatchSD(int i);
void saveSwappedPatchSD(int i);
#endif
void copyPatchBuffToPatchBuff(patch_t *dest, patch_t *sourc);
void copyCurrentPatchToLoadedPatch(int patchNumber);
void copyLoadedPatchToCopyBuffer(int sourcePatchNumber);
void copyLoadedPatchToSwapBuffer(int sourcePatchNumber);
void copyCurrentPatchToCopyBuffer(void);
void copyCurrentFxToCopyBufferFx(void);
void copyCopyBufferFxToPatch(int patchNumber);
#ifdef USELOAD_PATCHFUNCS
void loadPatchSD(int i);
void loadAllPatches();
void loadGlobalFxSD();
#endif
float sweepTimeOscCurve(float sweepTime);
float sweepTimeFilterCurve(float sweepTime);
float noiseTimeCurve(float noiseTimePercent);


#endif
