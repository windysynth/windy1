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

//#define NUMBER_OF_PATCHES 100
#define PATCHES_PER_BANK 128
#define NUMBER_OF_BANKS 1
#define NUMBER_OF_PATCHES (PATCHES_PER_BANK*NUMBER_OF_BANKS)

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
#define NRPN_MSB_SPARE  79
#define NRPN_MSB_SPARE_LENGTH 3
#define NRPN_MSB_NOISE_SYSPOS 0x98
#define NRPN_MSB_NOISE  80
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
#define CCNOISETIME 0
#define CCNOISEBREATHCURVE 1
#define CCNOISELEVEL 2
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
#define CCVIBRATOAMP 0
#define CCAMPLEVEL 1
#define CCOCTBUTTONLEVEL 2
#define CCEFFECTSCHORUSDELAY1 0
#define CCEFFECTSCHORUSMOD1 1
#define CCEFFECTSCHORUSWET1 2
#define CCEFFECTSCHORUSDELAY2 3
#define CCEFFECTSCHORUSMOD2 4
#define CCEFFECTSCHORUSWET2 5
#define CCEFFECTSCHORUSFEEDBACK 6
#define CCEFFECTSCHORUSLFOFREQ 7
#define CCEFFECTSCHORUSDRYLEVEL 8
#define CCEFFECTSDELAYTIME 0
#define CCEFFECTSDELAYFEEDBACK 1
#define CCEFFECTSDELAYDAMP 2
#define CCEFFECTSDELAYLEVEL 3
// CCEFFECTSDELAYSPARE used for "Pong" 
#define CCEFFECTSDELAYSPARE 4
#define CCEFFECTSREVERBSPARE 0
#define CCEFFECTSREVERBLEVEL 1
#define CCEFFECTSREVERBDENSEEARLY 2
#define CCEFFECTSREVERBTIME 3
#define CCEFFECTSREVERBDAMP 4

#define EFFECTGROUPCHORUS 0
#define EFFECTGROUPDELAY 1 
#define EFFECTGROUPREVERB 2
#define EFFECTGROUPCOMMON1 3

typedef struct __attribute__((packed)) Patch{
    char patch_string[32]; 
    uint8_t nrpn_msb_osc1[NRPN_MSB_OSC1_LENGTH];  // 18
    uint8_t nrpn_msb_osc2[NRPN_MSB_OSC2_LENGTH];  // 18
    uint8_t nrpn_msb_osc_filt1[NRPN_MSB_OSC_FILT1_LENGTH]; //12
    uint8_t nrpn_msb_osc_filt2[NRPN_MSB_OSC_FILT2_LENGTH]; //12
    uint8_t nrpn_msb_noise_filt3[NRPN_MSB_NOISE_FILT3_LENGTH]; //12
    uint8_t nrpn_msb_noise_filt4[NRPN_MSB_NOISE_FILT4_LENGTH]; //12
    uint8_t nrpn_msb_spare[NRPN_MSB_SPARE_LENGTH]; //3 
    uint8_t nrpn_msb_noise[NRPN_MSB_NOISE_LENGTH]; //3
    uint8_t nrpn_msb_common1[NRPN_MSB_COMMON1_LENGTH]; //10
    uint8_t nrpn_msb_common2[NRPN_MSB_COMMON2_LENGTH]; //3
    uint8_t nrpn_msb_chorus[NRPN_MSB_CHORUS_LENGTH];  // 9 
    uint8_t nrpn_msb_delay[NRPN_MSB_DELAY_LENGTH];    // 5 
    uint8_t nrpn_msb_reverb[NRPN_MSB_REVERB_LENGTH];   // 5
} patch_t; // 154 bytes

typedef struct __attribute__((packed)) Fx{
    char fx_string[32]; 
    uint8_t nrpn_msb_chorus[NRPN_MSB_CHORUS_LENGTH];  // 9 
    uint8_t nrpn_msb_delay[NRPN_MSB_DELAY_LENGTH];    // 5 
    uint8_t nrpn_msb_reverb[NRPN_MSB_REVERB_LENGTH];   // 5
    uint8_t nrpn_msb_common1[NRPN_MSB_COMMON1_LENGTH];   // 10
} fx_t;

uint8_t getFxValue(patch_t *patch, uint32_t effectGroup, uint32_t effectIdx);
void setFxValue(uint8_t value, patch_t *patch, uint32_t effectGroup, uint32_t effectIdx);
void patchToOctOsc1(patch_t *patch); //64,0,62,66,
void patchToSemiOsc1(patch_t *patch); //64,1,52,76,
void patchToFineOsc1(patch_t *patch); //64,2,14,114, -50 to +50 cents
void patchToBeatOsc1(patch_t *patch);  	//64,3,0,127,
void patchToSawOsc1(patch_t *patch); 	//64,5,0,127,
void patchToTriOsc1(patch_t *patch);	//64,6,0,127,
void patchToPulseOsc1(patch_t *patch); //64,7,0,127,
void patchToPwOsc1(patch_t *patch); //64,8,0,127,  GUI 0 = 50%
void patchToPwmFreqOsc1(patch_t *patch);  	//64,9,0,127,
void patchToPwmDepthOsc1(patch_t *patch); //64,10,0,127,  
void patchToSweepDepthOsc1(patch_t *patch); //64,11,0,127,
void patchToSweepTimeOsc1(patch_t *patch); //64,12,0,127,
void patchToBreathDepthOsc1(patch_t *patch);	//64,13,0,127,
void patchToBreathAttainOsc1(patch_t *patch); //64,14,0,127,
void patchToBreathOscCurveLines1(patch_t *patch);	//64,15,0,127,
void patchToBreathThreshOsc1(patch_t *patch); //64,16,0,127,
void patchToLevelOsc1(patch_t *patch); //64,17,0,127,
void patchToOctOsc2(patch_t *patch);  //65,0,62,66,
void patchToSemiOsc2(patch_t *patch); //64,1,52,76,
void patchToFineOsc2(patch_t *patch); //64,2,14,114, -50 to +50 cents
void patchToBeatOsc2(patch_t *patch);  	//64,3,0,127,
void patchToSawOsc2(patch_t *patch); 	//64,5,0,127,
void patchToTriOsc2(patch_t *patch);	//64,6,0,127,
void patchToPulseOsc2(patch_t *patch); //64,7,0,127,
void patchToPwOsc2(patch_t *patch); //64,8,0,127,  GUI 0 = 50%
void patchToPwmFreqOsc2(patch_t *patch);  	//64,9,0,127,
void patchToPwmDepthOsc2(patch_t *patch); //64,10,0,127,  
void patchToSweepDepthOsc2(patch_t *patch); //64,11,0,127,
void patchToSweepTimeOsc2(patch_t *patch); //64,12,0,127,
void patchToBreathDepthOsc2(patch_t *patch);	//64,13,0,127,
void patchToBreathAttainOsc2(patch_t *patch); //64,14,0,127,
void patchToBreathOscCurveLines2(patch_t *patch);	//64,15,0,127,
void patchToBreathThreshOsc2(patch_t *patch); //64,16,0,127,
void patchToLevelOsc2(patch_t *patch); //64,17,0,127,
void patchToModeOscFilter1(patch_t *patch); //72,0,0,4,//LP HP BP NTC OFF
void patchToFreqOscFilter1(patch_t *patch);
void patchToQFactorOscFilter1(patch_t *patch);	//72,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowOscFilter1(patch_t *patch); //72,3,52,88,// -12 to +24 num semi tones
void patchToBreathModOscFilter1(patch_t *patch);	//72,4,0,127,
void patchToLfoFreqOscFilter1(patch_t *patch);	//72,5,0,127,
void patchToLfoDepthOscFilter1(patch_t *patch); 	//72,6,0,127,
void patchToLfoBreathOscFilter1(patch_t *patch);  	//72,7,0,127,
void patchToLfoThreshOscFilter1(patch_t *patch);	//72,8,0,127,
void patchToSweepDepthOscFilter1(patch_t *patch);	//72,9,0,127,
void patchToSweepTimeOscFilter1(patch_t *patch); //72,10,0,127,
void patchToBreathOscFiltCurveLines1(patch_t *patch);  //72,11,0,127;
void patchToModeOscFilter2(patch_t *patch); //73,0,0,4,//LP HP BP NTC OFF
void patchToFreqOscFilter2(patch_t *patch);
void patchToQFactorOscFilter2(patch_t *patch);	//72,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowOscFilter2(patch_t *patch); //72,3,52,88,// -12 to +24 num semi tones
void patchToBreathModOscFilter2(patch_t *patch);	//72,4,0,127,
void patchToLfoFreqOscFilter2(patch_t *patch);	//72,5,0,127,
void patchToLfoDepthOscFilter2(patch_t *patch); 	//72,6,0,127,
void patchToLfoBreathOscFilter2(patch_t *patch);  	//72,7,0,127,
void patchToLfoThreshOscFilter2(patch_t *patch);	//72,8,0,127,
void patchToSweepDepthOscFilter2(patch_t *patch);	//72,9,0,127,
void patchToSweepTimeOscFilter2(patch_t *patch); //72,10,0,127,
void patchToBreathOscFiltCurveLines2(patch_t *patch);  //72,11,0,127,
void patchToModeNoiseFilter3(patch_t *patch); //74,0,0,4,//LP HP BP NTC OFF
void patchToFreqNoiseFilter3(patch_t *patch);
void patchToQFactorNoiseFilter3(patch_t *patch); //74,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowNoiseFilter3(patch_t *patch); //74,3,52,88,// -12 to +24 num semitones
void patchToBreathModNoiseFilter3(patch_t *patch);	//74,4,0,127,
void patchToLfoFreqNoiseFilter3(patch_t *patch);  	//74,5,0,127,
void patchToLfoDepthNoiseFilter3(patch_t *patch);  	//74,6,0,127,
void patchToLfoBreathNoiseFilter3(patch_t *patch);  	//74,7,0,127,
void patchToLfoThreshNoiseFilter3(patch_t *patch);  	//74,8,0,127,
void patchToSweepDepthNoiseFilter3(patch_t *patch);  	//74,9,0,127,
void patchToSweepTimeNoiseFilter3(patch_t *patch);  	//74,10,0,127,
void patchToBreathNoiseFiltCurveLines3(patch_t *patch);
void patchToModeNoiseFilter4(patch_t *patch); //74,0,0,4,//LP HP BP NTC OFF
void patchToFreqNoiseFilter4(patch_t *patch); 
void patchToQFactorNoiseFilter4(patch_t *patch); //74,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowNoiseFilter4(patch_t *patch); //74,3,52,88,// -12 to +24 num semitones
void patchToBreathModNoiseFilter4(patch_t *patch);	//74,4,0,127,
void patchToLfoFreqNoiseFilter4(patch_t *patch);  	//74,5,0,127,
void patchToLfoDepthNoiseFilter4(patch_t *patch);  	//74,6,0,127,
void patchToLfoBreathNoiseFilter4(patch_t *patch);  	//74,7,0,127,
void patchToLfoThreshNoiseFilter4(patch_t *patch);  	//74,8,0,127,
void patchToSweepDepthNoiseFilter4(patch_t *patch);  	//74,9,0,127,
void patchToSweepTimeNoiseFilter4(patch_t *patch);  	//74,10,0,127,
void patchToBreathNoiseFiltCurveLines4(patch_t *patch);  //74,11,0,127,
void patchToNoiseTime(patch_t *patch); //80,0,0,127,
void patchToNoiseBreathCurveLines(patch_t *patch); //80,1,0,127,
void patchToNoiseLevel(patch_t *patch);	//80,2,0,127,
void patchToBendRange(patch_t *patch); //81,0,0,12,// num semitones
void patchToBendStep(patch_t *patch);  	//81,1,0,1,//0=off 1=on
void patchToLinkOscFilters(patch_t *patch); //81,3,1,2,//1=Link on 2= Link off
void patchToLinkNoiseFilters(patch_t *patch); //81,4,1,2,
void patchToFormant(patch_t *patch); //81,5,0,2,//0=off 1=W.Wind 2=String
void patchToXFade(patch_t *patch); //81,6,0,1,//0=off 1=on
void patchToKeyTriggerSingle(patch_t *patch); //81,7,0,1,//0=Multi 1=Single
void patchTos81_8(patch_t *patch); //81,8,0,1,// ? 
void patchToChorusOn(patch_t *patch);  //81,9,0,1,// Chorus on off
void patchToVibratoAmp(patch_t *patch); //88,0,0,127, (bite tremelo amount)
void patchToAmpLevel(patch_t *patch); //88,1,0,127,
void patchToOctButtonLevel(patch_t *patch); //88,2,0,127,
void patchToEffectsChorusDelay1(patch_t *patch);   
void patchToEffectsChorusMod1(patch_t *patch); //112,1,0,127, (-50% to +50%)
void patchToEffectsChorusWet1(patch_t *patch); //112,2,0,127, neg vals are phase inverted
void patchToEffectsChorusDelay2(patch_t *patch); //112,3,0,127,
void patchToEffectsChorusMod2(patch_t *patch); //112,4,0,127,
void patchToEffectsChorusWet2(patch_t *patch); //112,5,0,127,
void patchToEffectsChorusFeedback(patch_t *patch); //112,6,0,127,
void patchToEffectsChorusLfoFreq(patch_t *patch); //112,7,0,127,
void patchToEffectsChorusDryLevel(patch_t *patch); //112,8,0,127,
void patchToEffectsDelayTimeL(patch_t *patch);	//113,0,0,127, 0 to 1270 ms
void patchToEffectsDelayFeedback(patch_t *patch); //113,1,0,127,
void patchToEffectsDelayDamp(patch_t *patch); //113,2,0,127,
void patchToEffectsDelayLevel(patch_t *patch);  //113,3,0,127,
void patchToEffectsDelayPong(patch_t *patch); //113,3,0,127,
void patchToEffectsReverbSpare(patch_t *patch); //114,1,0,127,
void patchToEffectsReverbLevel(patch_t *patch); //114,1,0,127,
void patchToEffectsReverbDenseEarly(patch_t *patch); //114,2,0,127, (using this for roomsize)
void patchToEffectsReverbTime(patch_t *patch); //114,3,10,50,//1000 to 5000 ms
void patchToEffectsReverbDamp(patch_t *patch);  	//114,4,54,74,//-10 to +10

void patchToSynthVariables(patch_t *patch);
void printCurveMidiData();
void printPatchValues();
void setCurrentPatch(int patchNumber);
void configureSD();
void savePatchSD(int i);
void saveCurrentPatchSD(int i); 
void saveCoppiedPatchSD(int i); 
void copyPatchBuffToPatchBuff(patch_t *dest, patch_t *sourc); 
void copyCurrentPatchToLoadedPatch(int patchNumber);
void copyLoadedPatchToCopyBuffer(int sourcePatchNumber);
void copyCurrentPatchToCopyBuffer(void);
void loadPatchSD(int i);
void loadAllPatches();




#endif
