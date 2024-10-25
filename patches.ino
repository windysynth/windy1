/************************************************************
  Windy1 USB Host Synth for Wind controllers
        MIT license
        Copyright (c) 2021 Tim King

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

****************************************************************/

#include "patches.h"
#include <SD.h>

    // 
void patchToSynthVariables(patch_t *patch) {
    OctOsc1 = (float)patch->nrpn_msb_osc1[CCOCTOSC1]-64.0;      //64,0,62,66,
    SemiOsc1 = (float)patch->nrpn_msb_osc1[CCSEMIOSC1]-64.0;     //64,1,52,76,
    FineOsc1 = ((float)patch->nrpn_msb_osc1[CCFINEOSC1]-64.0)/100.0;  	//64,2,14,114, -50 to +50 cents
    BeatOsc1 = ((float)patch->nrpn_msb_osc1[CCBEATOSC1]-64.0)/64.0;  	//64,3,0,127,
    SawOsc1 = (float)patch->nrpn_msb_osc1[CCSAWOSC1]*DIV127;  	//64,5,0,127,
    TriOsc1 = (float)patch->nrpn_msb_osc1[CCTRIOSC1]*DIV127;  	//64,6,0,127,
    PulseOsc1 = (float)patch->nrpn_msb_osc1[CCPULSEOSC1]*DIV127;  	//64,7,0,127,
    PwOsc1 = (float)patch->nrpn_msb_osc1[CCPWOSC1]*DIV127+PwOffsetOsc1;  	//64,8,0,127,  GUI 0 = 50%
    PwmFreqOsc1 = (float)patch->nrpn_msb_osc1[CCPWMFREQOSC1]*DIV127*maxPwmLfoFreq;  	//64,9,0,127,
    PwmDepthOsc1 = (float)patch->nrpn_msb_osc1[CCPWMDEPTHOSC1]*DIV127;  	//64,10,0,127,  
    SweepDepthOsc1 = ((float)patch->nrpn_msb_osc1[CCSWEEPDEPTHOSC1]-64.0)/64.0;  	//64,11,0,127,
    SweepTimeOsc1 = sweepTimeOscCurve( (float)patch->nrpn_msb_osc1[CCSWEEPTIMEOSC1]*DIV127);  	//64,12,0,127,
    BreathDepthOsc1 = ((float)patch->nrpn_msb_osc1[CCBREATHDEPTHOSC1]-64.0)/64.0;  	//64,13,0,127,
    BreathAttainOsc1 = (float)patch->nrpn_msb_osc1[CCBREATHATTAINOSC1]*DIV127;  	//64,14,0,127,
    BreathCurveOsc1 = gen_osc_gamma(((float)patch->nrpn_msb_osc1[CCBREATHCURVEOSC1])*DIV127);  	//64,15,0,127,
    BreathThreshOsc1 = (float)patch->nrpn_msb_osc1[CCBREATHTHRESHOSC1]*DIV127;  	//64,16,0,127,
    LevelOsc1 = (float)patch->nrpn_msb_osc1[CCLEVELOSC1]*DIV127;  		//64,17,0,127,
    OctOsc2 = (float)patch->nrpn_msb_osc2[CCOCTOSC2]-64.0;      //65,0,62,66,
    SemiOsc2 = (float)patch->nrpn_msb_osc2[CCSEMIOSC2]-64.0;     //65,1,52,76,
    FineOsc2 = ((float)patch->nrpn_msb_osc2[CCFINEOSC2]-64.0)/100;  	//65,2,14,114, -50 to +50 cents
    BeatOsc2 = ((float)patch->nrpn_msb_osc2[CCBEATOSC2]-64.0)/64.0;  	//65,3,0,127,
    SawOsc2 = (float)patch->nrpn_msb_osc2[CCSAWOSC2]*DIV127;  	//65,5,0,127,
    TriOsc2 = (float)patch->nrpn_msb_osc2[CCTRIOSC2]*DIV127;  	//65,6,0,127,
    PulseOsc2 = (float)patch->nrpn_msb_osc2[CCPULSEOSC2]*DIV127;  	//65,7,0,127, 
    PwOsc2 = (float)patch->nrpn_msb_osc2[CCPWOSC2]*DIV127 + PwOffsetOsc2;  	//65,8,0,127, GUI 0 = 50% duty
    PwmFreqOsc2 = (float)patch->nrpn_msb_osc2[CCPWMFREQOSC2]*DIV127*maxPwmLfoFreq;  	//65,9,0,127,
    PwmDepthOsc2 = (float)patch->nrpn_msb_osc2[CCPWMDEPTHOSC2]*DIV127;  	//65,10,0,127,
    SweepDepthOsc2 = ((float)patch->nrpn_msb_osc2[CCSWEEPDEPTHOSC2]-64.0)/64;  	//65,11,0,127,
    SweepTimeOsc2 = sweepTimeOscCurve((float)patch->nrpn_msb_osc2[CCSWEEPTIMEOSC2]*DIV127);  	//65,12,0,127,
    BreathDepthOsc2 = ((float)patch->nrpn_msb_osc2[CCBREATHDEPTHOSC2]-64.0)/64.0;  	//65,13,0,127,
    BreathAttainOsc2 = (float)patch->nrpn_msb_osc2[CCBREATHATTAINOSC2]*DIV127;  	//65,14,0,127,
    BreathCurveOsc2 = gen_osc_gamma(((float)patch->nrpn_msb_osc2[CCBREATHCURVEOSC2])*DIV127);  	//65,15,0,127,
    BreathThreshOsc2 = (float)patch->nrpn_msb_osc2[CCBREATHTHRESHOSC2]*DIV127;  	//65,16,0,127,
    LevelOsc2 = (float)patch->nrpn_msb_osc2[CCLEVELOSC2]*DIV127;  	//65,17,0,127,
    ModeOscFilter1 = (filterMode_t)patch->nrpn_msb_osc_filt1[CCMODEOSCFILTER1];  	//72,0,0,4,//LP HP BP NTC OFF
    // 69 is note number for A4=440Hz ;  	
    FreqOscFilter1 = 440.0*pow(2, ((float)patch->nrpn_msb_osc_filt1[CCFREQOSCFILTER1]-69.0)/12 );  //72,1,36,124,note# 36 to 124
    QFactorOscFilter1 = (float)patch->nrpn_msb_osc_filt1[CCQFACTOROSCFILTER1]/10.0;  	//72,2,5,127,"// 5=0.5, 127=12.7"
    KeyFollowOscFilter1 = ((float)patch->nrpn_msb_osc_filt1[CCKEYFOLLOWOSCFILTER1]-64.0); //72,3,52,88,// -12 to +24 num semi tones
    BreathModOscFilter1 = (float)patch->nrpn_msb_osc_filt1[CCBREATHMODOSCFILTER1]*DIV127;  	//72,4,0,127,
    LfoFreqOscFilter1 = pow((float)patch->nrpn_msb_osc_filt1[CCLFOFREQOSCFILTER1]*DIV127,2.0)*maxLfoFreqFilter1;  	//72,5,0,127,
    LfoDepthOscFilter1 = (float)patch->nrpn_msb_osc_filt1[CCLFODEPTHOSCFILTER1]*DIV127;  	//72,6,0,127,
    LfoBreathOscFilter1 = ((float)patch->nrpn_msb_osc_filt1[CCLFOBREATHOSCFILTER1]-64.0)/64.0;  	//72,7,0,127,
    LfoThreshOscFilter1 = (float)patch->nrpn_msb_osc_filt1[CCLFOTHRESHOSCFILTER1]*DIV127;  	//72,8,0,127,
    SweepDepthOscFilter1 = ((float)patch->nrpn_msb_osc_filt1[CCSWEEPDEPTHOSCFILTER1]-64.0)/64.0;  	//72,9,0,127,
    SweepTimeOscFilter1 = sweepTimeFilterCurve((float)patch->nrpn_msb_osc_filt1[CCSWEEPTIMEOSCFILTER1]*DIV127);  	//72,10,0,127,
    BreathCurveOscFilter1 = gen_filter_gamma(((float)patch->nrpn_msb_osc_filt1[CCBREATHCURVEOSCFILTER1])*DIV127);  	//72,11,0,127,
    ModeOscFilter2 = (filterMode_t)patch->nrpn_msb_osc_filt2[CCMODEOSCFILTER2];  	//73,0,0,4,//LP HP BP NTC OFF
    FreqOscFilter2 = 440.0*pow(2, ((float)patch->nrpn_msb_osc_filt2[CCFREQOSCFILTER2]-69.0)/12 ); //73,1,36,124,//Midi Note 36 to 124
    QFactorOscFilter2 = (float)patch->nrpn_msb_osc_filt2[CCQFACTOROSCFILTER2]/10.0;  	//73,2,5,127,"// 5=0.5, 127=12.7"
    KeyFollowOscFilter2 = ((float)patch->nrpn_msb_osc_filt2[CCKEYFOLLOWOSCFILTER2]-64.0);//73,3,52,88,// -12 to +24 num semi tones
    BreathModOscFilter2 = (float)patch->nrpn_msb_osc_filt2[CCBREATHMODOSCFILTER2]*DIV127;  	//73,4,0,127,
    LfoFreqOscFilter2 = pow((float)patch->nrpn_msb_osc_filt2[CCLFOFREQOSCFILTER2]*DIV127,2.0)*maxLfoFreqFilter2;  	//73,5,0,127,
    LfoDepthOscFilter2 = (float)patch->nrpn_msb_osc_filt2[CCLFODEPTHOSCFILTER2]*DIV127;  	//73,6,0,127,
    LfoBreathOscFilter2 = ((float)patch->nrpn_msb_osc_filt2[CCLFOBREATHOSCFILTER2]-64.0)/64.0;  	//73,7,0,127,
    LfoThreshOscFilter2 = (float)patch->nrpn_msb_osc_filt2[CCLFOTHRESHOSCFILTER2]*DIV127;  	//73,8,0,127,
    SweepDepthOscFilter2 = ((float)patch->nrpn_msb_osc_filt2[CCSWEEPDEPTHOSCFILTER2]-64.0)/64.0;  	//73,9,0,127,
    SweepTimeOscFilter2 = sweepTimeFilterCurve((float)patch->nrpn_msb_osc_filt2[CCSWEEPTIMEOSCFILTER2]*DIV127);  	//73,10,0,127,
    BreathCurveOscFilter2 = gen_filter_gamma(((float)patch->nrpn_msb_osc_filt2[CCBREATHCURVEOSCFILTER2])*DIV127);  	//73,11,0,127,
    ModeNoiseFilter3 = (filterMode_t)patch->nrpn_msb_noise_filt3[CCMODENOISEFILTER3];  	//74,0,0,4,//LP HP BP NTC OFF
    FreqNoiseFilter3 = 440.0*pow(2, ((float)patch->nrpn_msb_noise_filt3[CCFREQNOISEFILTER3]-69.0)/12 );//74,1,36,124,//Note# 36 to 124
    QFactorNoiseFilter3 = (float)patch->nrpn_msb_noise_filt3[CCQFACTORNOISEFILTER3]/10.0;  	//74,2,5,127,"// 5=0.5, 127=12.7"
    KeyFollowNoiseFilter3 = ((float)patch->nrpn_msb_noise_filt3[CCKEYFOLLOWNOISEFILTER3]-64.0);//74,3,52,88,// -12 to +24 num semitones
    BreathModNoiseFilter3 = (float)patch->nrpn_msb_noise_filt3[CCBREATHMODNOISEFILTER3]*DIV127;  	//74,4,0,127,
    LfoFreqNoiseFilter3 = pow((float)patch->nrpn_msb_noise_filt3[CCLFOFREQNOISEFILTER3]*DIV127,2.0)*maxLfoFreqFilter3;  	//74,5,0,127,
    LfoDepthNoiseFilter3 = (float)patch->nrpn_msb_noise_filt3[CCLFODEPTHNOISEFILTER3]*DIV127;  	//74,6,0,127,
    LfoBreathNoiseFilter3 = ((float)patch->nrpn_msb_noise_filt3[CCLFOBREATHNOISEFILTER3]-64.0)/64.0;  	//74,7,0,127,
    LfoThreshNoiseFilter3 = (float)patch->nrpn_msb_noise_filt3[CCLFOTHRESHNOISEFILTER3]*DIV127;  	//74,8,0,127,
    SweepDepthNoiseFilter3 = ((float)patch->nrpn_msb_noise_filt3[CCSWEEPDEPTHNOISEFILTER3]-64.0)/64.0;  	//74,9,0,127,
    SweepTimeNoiseFilter3 = sweepTimeFilterCurve((float)patch->nrpn_msb_noise_filt3[CCSWEEPTIMENOISEFILTER3]*DIV127);  	//74,10,0,127,
    BreathCurveNoiseFilter3 = gen_filter_gamma(((float)patch->nrpn_msb_noise_filt3[CCBREATHCURVENOISEFILTER3])*DIV127);  	//74,11,0,127,
    ModeNoiseFilter4 = (filterMode_t)patch->nrpn_msb_noise_filt4[CCMODENOISEFILTER4];  	//75,0,0,4,//LP HP BP NTC OFF
    FreqNoiseFilter4 = 440.0*pow(2, ((float)patch->nrpn_msb_noise_filt4[CCFREQNOISEFILTER4]-69.0)/12 ); //75,1,36,124,//Note# 36 to 124
    QFactorNoiseFilter4 = (float)patch->nrpn_msb_noise_filt4[CCQFACTORNOISEFILTER4]/10.0;  	//75,2,5,127,"// 5=0.5, 127=12.7"
    KeyFollowNoiseFilter4 = ((float)patch->nrpn_msb_noise_filt4[CCKEYFOLLOWNOISEFILTER4]-64.0);//75,3,52,88,// -12 to +24 num semitones
    BreathModNoiseFilter4 = (float)patch->nrpn_msb_noise_filt4[CCBREATHMODNOISEFILTER4]*DIV127;  	//75,4,0,127,
    LfoFreqNoiseFilter4 = pow((float)patch->nrpn_msb_noise_filt4[CCLFOFREQNOISEFILTER4]*DIV127,2.0)*maxLfoFreqFilter4;  	//75,5,0,127,
    LfoDepthNoiseFilter4 = (float)patch->nrpn_msb_noise_filt4[CCLFODEPTHNOISEFILTER4]*DIV127;  	//75,6,0,127,
    LfoBreathNoiseFilter4 = ((float)patch->nrpn_msb_noise_filt4[CCLFOBREATHNOISEFILTER4]-64.0)/64.0;  	//75,7,0,127,
    LfoThreshNoiseFilter4 = (float)patch->nrpn_msb_noise_filt4[CCLFOTHRESHNOISEFILTER4]*DIV127;  	//75,8,0,127,
    SweepDepthNoiseFilter4 = ((float)patch->nrpn_msb_noise_filt4[CCSWEEPDEPTHNOISEFILTER4]-64.0)/64.0;  	//75,9,0,127,
    SweepTimeNoiseFilter4 = sweepTimeFilterCurve((float)patch->nrpn_msb_noise_filt4[CCSWEEPTIMENOISEFILTER4]*DIV127);  	//75,10,0,127,
    BreathCurveNoiseFilter4 = gen_filter_gamma(((float)patch->nrpn_msb_noise_filt4[CCBREATHCURVENOISEFILTER4])*DIV127);  	//75,11,0,127,
    NoiseTime = noiseTimeCurve((float)patch->nrpn_msb_noise[CCNOISETIME]*DIV127);  	//80,0,0,127,
    NoiseBreathCurve = gen_noise_gamma(((float)(patch->nrpn_msb_noise[CCNOISEBREATHCURVE]))*DIV127);  	//80,1,0,127,
    NoiseLevel = pow((float)patch->nrpn_msb_noise[CCNOISELEVEL]*DIV127,.35)*maxNoiseLevel;  	//80,2,0,127,
    BendRange = (float)patch->nrpn_msb_common1[CCBENDRANGE];  	//81,0,0,12,// num semitones
    BendStep = (bool)patch->nrpn_msb_common1[CCBENDSTEP];  	//81,1,0,1,//0=off 1=on
    VibratoPitch = (float)patch->nrpn_msb_common1[CCVIBRATOPITCH]*DIV127;  	//81,2,0,127,  (this isn't necessary since bite sensor pb and plate pb are the same).
    LinkOscFilters = (bool)(patch->nrpn_msb_common1[CCLINKOSCFILTERS] % 2);  	//81,3,1,2,//1=Link on 2= Link off
    LinkNoiseFilters = (bool)(patch->nrpn_msb_common1[CCLINKNOISEFILTERS] % 2);  	//81,4,1,2,
    Formant = patch->nrpn_msb_common1[CCFORMANT];  	        //81,5,0,2,//0=off 1=W.Wind 2=String
    XFade = (bool)patch->nrpn_msb_common1[CCXFADE];  	        //81,6,0,1,//0=off 1=on
    KeyTriggerSingle = (bool)patch->nrpn_msb_common1[CCKEYTRIGGER];  	    //81,7,0,1,//0=Multi 1=Single
    s81_8 = (float)patch->nrpn_msb_common1[CC81_8];  	    //81,8,0,1,// ? 
    ChorusOn = (bool)patch->nrpn_msb_common1[CCCHORUSON];  	    //81,9,0,1,// Chorus on off
    VibratoAmp = (float)patch->nrpn_msb_common2[CCVIBRATOAMP]*DIV127;  	    //88,0,0,127, (bite tremelo amount)
    AmpLevel = (float)patch->nrpn_msb_common2[CCAMPLEVEL]*DIV127;  	    //88,1,0,127,
    OctButtonLevel = (float)patch->nrpn_msb_common2[CCOCTBUTTONLEVEL]*DIV127;  	    //88,2,0,127,
    // flange cuts delay in half, so mult by 2.0 here 
    EffectsChorusDelay1 = 2.0*44.1*(float)patch->nrpn_msb_chorus[CCEFFECTSCHORUSDELAY1];  //112,0,0,127, (0 to 127 ms)*44.1s/ms
    // 7.0ms*44.1s/ms for range of ChorusMod1 and 2
    EffectsChorusMod1 = (2.0*44.1)*((float)patch->nrpn_msb_chorus[CCEFFECTSCHORUSMOD1]-64.0)/64.0; //112,1,0,127, (-50% to +50%)
    EffectsChorusWet1 = ((float)patch->nrpn_msb_chorus[CCEFFECTSCHORUSWET1]-64.0)/64.0;  	//112,2,0,127, neg vals are phase inverted
    EffectsChorusDelay2 = 2.0*44.1*(float)patch->nrpn_msb_chorus[CCEFFECTSCHORUSDELAY2];  //112,3,0,127,
    EffectsChorusMod2 = (2.0*44.1)*((float)patch->nrpn_msb_chorus[CCEFFECTSCHORUSMOD2]-64.0)/64.0;  	//112,4,0,127,
    EffectsChorusWet2 = ((float)patch->nrpn_msb_chorus[CCEFFECTSCHORUSWET2]-64.0)/64.0;  	//112,5,0,127,
    EffectsChorusFeedback = ((float)patch->nrpn_msb_chorus[CCEFFECTSCHORUSFEEDBACK]-64.0)/64.0;//112,6,0,127,
    EffectsChorusLfoFreq = (float)patch->nrpn_msb_chorus[CCEFFECTSCHORUSLFOFREQ]/10.0; //112,7,0,127,
    EffectsChorusDryLevel = (float)patch->nrpn_msb_chorus[CCEFFECTSCHORUSDRYLEVEL]*DIV127;//112,8,0,127,
    EffectsDelayTimeL = (float)patch->nrpn_msb_delay[CCEFFECTSDELAYTIME]*10.0;  	//113,0,0,127, 0 to 1270 ms
    EffectsDelayFeedback = (float)patch->nrpn_msb_delay[CCEFFECTSDELAYFEEDBACK]*DIV127; //113,1,0,127,
    EffectsDelayDamp =maxDelayDamp*pow(2, -( (float)patch->nrpn_msb_delay[CCEFFECTSDELAYDAMP])/24.0 );  	//113,2,0,127,
    EffectsDelayLevel = (float)patch->nrpn_msb_delay[CCEFFECTSDELAYLEVEL]*DIV127;  	//113,3,0,127,
    EffectsDelaySpare = (float)patch->nrpn_msb_delay[CCEFFECTSDELAYSPARE]*DIV127;  	//113,3,0,127,
    EffectsReverbSpare = (float)patch->nrpn_msb_reverb[CCEFFECTSREVERBSPARE]*DIV127;  	//114,1,0,127,
    EffectsReverbLevel = (float)patch->nrpn_msb_reverb[CCEFFECTSREVERBLEVEL]*DIV127*maxReverbLevel;  	//114,1,0,127,
    EffectsReverbDenseEarly = (float)patch->nrpn_msb_reverb[CCEFFECTSREVERBDENSEEARLY]*DIV127*maxDenseEarly;  	//114,2,0,127, (using this for roomsize)
    EffectsReverbTime = (float)patch->nrpn_msb_reverb[CCEFFECTSREVERBTIME]*100;  	//114,3,10,50,//1000 to 5000 ms
    EffectsReverbDamp = ((float)patch->nrpn_msb_reverb[CCEFFECTSREVERBDAMP]-54)/20.0;  	//114,4,54,74,//-10 to +10
    updateSynthVariablesFlag = true;
}

void printCurveMidiData()
{
    Serial8.println(current_patch.patch_string);
    sprintf(str_buf1, "CCBREATHCURVEOSC1: %d", current_patch.nrpn_msb_osc1[CCBREATHCURVEOSC1]);      
    Serial8.println(str_buf1);
    sprintf(str_buf1, "CCBREATHCURVEOSC2: %d", current_patch.nrpn_msb_osc2[CCBREATHCURVEOSC2]);      
    Serial8.println(str_buf1);
    sprintf(str_buf1, "CCBREATHCURVEOSCFILTER1: %d", current_patch.nrpn_msb_osc_filt1[CCBREATHCURVEOSCFILTER1]);      
    Serial8.println(str_buf1);
    sprintf(str_buf1, "CCBREATHCURVEOSCFILTER2: %d", current_patch.nrpn_msb_osc_filt2[CCBREATHCURVEOSCFILTER2]);      
    Serial8.println(str_buf1);
    sprintf(str_buf1, "CCBREATHCURVENOISEFILTER3: %d", current_patch.nrpn_msb_noise_filt3[CCBREATHCURVENOISEFILTER3]);      
    Serial8.println(str_buf1);
    sprintf(str_buf1, "CCBREATHCURVENOISEFILTER4: %d", current_patch.nrpn_msb_noise_filt4[CCBREATHCURVENOISEFILTER4]);      
    Serial8.println(str_buf1);
    sprintf(str_buf1, "CCNOISEBREATHCURVE: %d", current_patch.nrpn_msb_noise[CCNOISEBREATHCURVE]);      
    Serial8.println(str_buf1);
}

void printPatchValues()
{
    Serial8.println(current_patch.patch_string);
    sprintf(str_buf1, "OctOsc1: %5.3f", OctOsc1);      
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SemiOsc1: %5.3f", SemiOsc1);     
    Serial8.println(str_buf1);
    sprintf(str_buf1, "FineOsc1: %5.3f", FineOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BeatOsc1: %5.3f", BeatOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SawOsc1: %5.3f", SawOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "TriOsc1: %5.3f", TriOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "PulseOsc1: %5.3f", PulseOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "PwOsc1: %5.3f", PwOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "PwmFreqOsc1: %5.3f", PwmFreqOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "PwmDepthOsc1: %5.3f", PwmDepthOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepDepthOsc1: %5.3f", SweepDepthOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepTimeOsc1: %5.3f", SweepTimeOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathDepthOsc1: %5.3f", BreathDepthOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathAttainOsc1: %5.3f", BreathAttainOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathCurveOsc1: %5.3f", BreathCurveOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathThreshOsc1: %5.3f", BreathThreshOsc1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LevelOsc1: %5.3f", LevelOsc1);  		
    Serial8.println(str_buf1);
    sprintf(str_buf1, "OctOsc2: %5.3f", OctOsc2);      
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SemiOsc2: %5.3f", SemiOsc2);     
    Serial8.println(str_buf1);
    sprintf(str_buf1, "FineOsc2: %5.3f", FineOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BeatOsc2: %5.3f", BeatOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SawOsc2: %5.3f", SawOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "TriOsc2: %5.3f", TriOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "PulseOsc2: %5.3f", PulseOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "PwOsc2: %5.3f", PwOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "PwmFreqOsc2: %5.3f", PwmFreqOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "PwmDepthOsc2: %5.3f", PwmDepthOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepDepthOsc2: %5.3f", SweepDepthOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepTimeOsc2: %5.3f", SweepTimeOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathDepthOsc2: %5.3f", BreathDepthOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathAttainOsc2: %5.3f", BreathAttainOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathCurveOsc2: %5.3f", BreathCurveOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathThreshOsc2: %5.3f", BreathThreshOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LevelOsc2: %5.3f", LevelOsc2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "ModeOscFilter1: %d", ModeOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "FreqOscFilter1: %5.3f", FreqOscFilter1);  
    Serial8.println(str_buf1);
    sprintf(str_buf1, "QFactorOscFilter1: %5.3f", QFactorOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "KeyFollowOscFilter1: %5.3f", KeyFollowOscFilter1); 
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathModOscFilter1: %5.3f", BreathModOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoFreqOscFilter1: %5.3f", LfoFreqOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoDepthOscFilter1: %5.3f", LfoDepthOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoBreathOscFilter1: %5.3f", LfoBreathOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoThreshOscFilter1: %5.3f", LfoThreshOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepDepthOscFilter1: %5.3f", SweepDepthOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepTimeOscFilter1: %5.3f", SweepTimeOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathCurveOscFilter1: %5.3f", BreathCurveOscFilter1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "ModeOscFilter2: %d", ModeOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "FreqOscFilter2: %5.3f", FreqOscFilter2); 
    Serial8.println(str_buf1);
    sprintf(str_buf1, "QFactorOscFilter2: %5.3f", QFactorOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "KeyFollowOscFilter2: %5.3f", KeyFollowOscFilter2);
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathModOscFilter2: %5.3f", BreathModOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoFreqOscFilter2: %5.3f", LfoFreqOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoDepthOscFilter2: %5.3f", LfoDepthOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoBreathOscFilter2: %5.3f", LfoBreathOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoThreshOscFilter2: %5.3f", LfoThreshOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepDepthOscFilter2: %5.3f", SweepDepthOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepTimeOscFilter2: %5.3f", SweepTimeOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathCurveOscFilter2: %5.3f", BreathCurveOscFilter2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "ModeNoiseFilter3: %d", ModeNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "FreqNoiseFilter3: %5.3f", FreqNoiseFilter3);
    Serial8.println(str_buf1);
    sprintf(str_buf1, "QFactorNoiseFilter3: %5.3f", QFactorNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "KeyFollowNoiseFilter3: %5.3f", KeyFollowNoiseFilter3);
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathModNoiseFilter3: %5.3f", BreathModNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoFreqNoiseFilter3: %5.3f", LfoFreqNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoDepthNoiseFilter3: %5.3f", LfoDepthNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoBreathNoiseFilter3: %5.3f", LfoBreathNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoThreshNoiseFilter3: %5.3f", LfoThreshNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepDepthNoiseFilter3: %5.3f", SweepDepthNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepTimeNoiseFilter3: %5.3f", SweepTimeNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathCurveNoiseFilter3: %5.3f", BreathCurveNoiseFilter3);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "ModeNoiseFilter4: %d", ModeNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "FreqNoiseFilter4: %5.3f", FreqNoiseFilter4); 
    Serial8.println(str_buf1);
    sprintf(str_buf1, "QFactorNoiseFilter4: %5.3f", QFactorNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "KeyFollowNoiseFilter4: %5.3f", KeyFollowNoiseFilter4);
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathModNoiseFilter4: %5.3f", BreathModNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoFreqNoiseFilter4: %5.3f", LfoFreqNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoDepthNoiseFilter4: %5.3f", LfoDepthNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoBreathNoiseFilter4: %5.3f", LfoBreathNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LfoThreshNoiseFilter4: %5.3f", LfoThreshNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepDepthNoiseFilter4: %5.3f", SweepDepthNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "SweepTimeNoiseFilter4: %5.3f", SweepTimeNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BreathCurveNoiseFilter4: %5.3f", BreathCurveNoiseFilter4);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "NoiseTime: %5.3f", NoiseTime);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "NoiseBreathCurve: %5.3f", NoiseBreathCurve);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "NoiseLevel: %5.3f", NoiseLevel);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BendRange: %5.3f", BendRange);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "BendStep: %d", BendStep);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "VibratoPitch: %5.3f", VibratoPitch);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LinkOscFilters: %d", LinkOscFilters);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "LinkNoiseFilters: %d", LinkNoiseFilters);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "Formant: %d", Formant);  	        
    Serial8.println(str_buf1);
    sprintf(str_buf1, "XFade: %d", XFade);  	        
    Serial8.println(str_buf1);
    sprintf(str_buf1, "KeyTriggerSingle: %d", KeyTriggerSingle);  	    
    Serial8.println(str_buf1);
    sprintf(str_buf1, "s81_8: %5.3f", s81_8);  	    
    Serial8.println(str_buf1);
    sprintf(str_buf1, "ChorusOn: %d", ChorusOn);  	    
    Serial8.println(str_buf1);
    sprintf(str_buf1, "VibratoAmp: %5.3f", VibratoAmp);  	    
    Serial8.println(str_buf1);
    sprintf(str_buf1, "AmpLevel: %5.3f", AmpLevel);  	    
    Serial8.println(str_buf1);
    sprintf(str_buf1, "OctButtonLevel: %5.3f", OctButtonLevel);  	    
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsChorusDelay1: %5.3f", EffectsChorusDelay1);  
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsChorusMod1: %5.3f", EffectsChorusMod1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsChorusWet1: %5.3f", EffectsChorusWet1);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsChorusDelay2: %5.3f", EffectsChorusDelay2);  
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsChorusMod2: %5.3f", EffectsChorusMod2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsChorusWet2: %5.3f", EffectsChorusWet2);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsChorusFeedback: %5.3f", EffectsChorusFeedback);
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsChorusLfoFreq: %5.3f", EffectsChorusLfoFreq); 
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsChorusDryLevel: %5.3f", EffectsChorusDryLevel);
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsDelayTimeL: %5.3f", EffectsDelayTimeL);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsDelayFeedback: %5.3f", EffectsDelayFeedback); 
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsDelayDamp: %5.3f", EffectsDelayDamp);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsDelayLevel: %5.3f", EffectsDelayLevel);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsDelaySpare: %5.3f", EffectsDelaySpare);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsReverbSpare: %5.3f", EffectsReverbSpare);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsReverbLevel: %5.3f", EffectsReverbLevel);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsReverbDenseEarly: %5.3f", EffectsReverbDenseEarly);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsReverbTime: %5.3f", EffectsReverbTime);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "EffectsReverbDamp: %5.3f", EffectsReverbDamp);  	
    Serial8.println(str_buf1);
    sprintf(str_buf1, "dc_breathThreshOsc1: %5.3f", dc_breathThreshOsc1.read());
    Serial8.println(str_buf1);
} 


void setCurrentPatch(int patchNumber)
{
    uint8_t i = 0;
    for(i = 0; i<31; i++)
        current_patch.patch_string[i] = loadedPatches[patchNumber].patch_string[i];
    for( i = 0; i<NRPN_MSB_OSC1_LENGTH; i++)
       current_patch.nrpn_msb_osc1[i] = loadedPatches[patchNumber].nrpn_msb_osc1[i];
    for( i = 0; i<NRPN_MSB_OSC2_LENGTH; i++)
       current_patch.nrpn_msb_osc2[i] = loadedPatches[patchNumber].nrpn_msb_osc2[i];
    for( i = 0; i<NRPN_MSB_OSC_FILT1_LENGTH; i++)
       current_patch.nrpn_msb_osc_filt1[i] = loadedPatches[patchNumber].nrpn_msb_osc_filt1[i];
    for( i = 0; i<NRPN_MSB_OSC_FILT2_LENGTH; i++)
       current_patch.nrpn_msb_osc_filt2[i] = loadedPatches[patchNumber].nrpn_msb_osc_filt2[i];
    for( i = 0; i<NRPN_MSB_NOISE_FILT3_LENGTH; i++)
       current_patch.nrpn_msb_noise_filt3[i] = loadedPatches[patchNumber].nrpn_msb_noise_filt3[i];
    for( i = 0; i<NRPN_MSB_NOISE_FILT4_LENGTH; i++)
       current_patch.nrpn_msb_noise_filt4[i] = loadedPatches[patchNumber].nrpn_msb_noise_filt4[i];
    for( i = 0; i<NRPN_MSB_SPARE_LENGTH; i++)
       current_patch.nrpn_msb_spare[i] = loadedPatches[patchNumber].nrpn_msb_spare[i];
    for( i = 0; i<NRPN_MSB_NOISE_LENGTH; i++)
       current_patch.nrpn_msb_noise[i] = loadedPatches[patchNumber].nrpn_msb_noise[i];
    for( i = 0; i<NRPN_MSB_COMMON1_LENGTH; i++)
       current_patch.nrpn_msb_common1[i] = loadedPatches[patchNumber].nrpn_msb_common1[i];
    for( i = 0; i<NRPN_MSB_COMMON2_LENGTH; i++)
       current_patch.nrpn_msb_common2[i] = loadedPatches[patchNumber].nrpn_msb_common2[i];
    for( i = 0; i<NRPN_MSB_CHORUS_LENGTH; i++)
       current_patch.nrpn_msb_chorus[i] = loadedPatches[patchNumber].nrpn_msb_chorus[i];
    for( i = 0; i<NRPN_MSB_DELAY_LENGTH; i++)
       current_patch.nrpn_msb_delay[i] = loadedPatches[patchNumber].nrpn_msb_delay[i];
    for( i = 0; i<NRPN_MSB_REVERB_LENGTH; i++)
       current_patch.nrpn_msb_reverb[i] = loadedPatches[patchNumber].nrpn_msb_reverb[i];
}

void copyCurrentPatchToLoadedPatch(int patchNumber)
{
    uint8_t i = 0;
    for(i = 0; i<31; i++)
       loadedPatches[patchNumber].patch_string[i] = current_patch.patch_string[i];
    for( i = 0; i<NRPN_MSB_OSC1_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_osc1[i] = current_patch.nrpn_msb_osc1[i];
    for( i = 0; i<NRPN_MSB_OSC2_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_osc2[i] = current_patch.nrpn_msb_osc2[i];
    for( i = 0; i<NRPN_MSB_OSC_FILT1_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_osc_filt1[i] = current_patch.nrpn_msb_osc_filt1[i];
    for( i = 0; i<NRPN_MSB_OSC_FILT2_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_osc_filt2[i] = current_patch.nrpn_msb_osc_filt2[i];
    for( i = 0; i<NRPN_MSB_NOISE_FILT3_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_noise_filt3[i] = current_patch.nrpn_msb_noise_filt3[i];
    for( i = 0; i<NRPN_MSB_NOISE_FILT4_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_noise_filt4[i] = current_patch.nrpn_msb_noise_filt4[i];
    for( i = 0; i<NRPN_MSB_SPARE_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_spare[i] = current_patch.nrpn_msb_spare[i];
    for( i = 0; i<NRPN_MSB_NOISE_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_noise[i] = current_patch.nrpn_msb_noise[i];
    for( i = 0; i<NRPN_MSB_COMMON1_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_common1[i] = current_patch.nrpn_msb_common1[i];
    for( i = 0; i<NRPN_MSB_COMMON2_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_common2[i] = current_patch.nrpn_msb_common2[i];
    for( i = 0; i<NRPN_MSB_CHORUS_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_chorus[i] = current_patch.nrpn_msb_chorus[i];
    for( i = 0; i<NRPN_MSB_DELAY_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_delay[i] = current_patch.nrpn_msb_delay[i];
    for( i = 0; i<NRPN_MSB_REVERB_LENGTH; i++)
       loadedPatches[patchNumber].nrpn_msb_reverb[i] = current_patch.nrpn_msb_reverb[i];
}


//---------------------
// SD card access
//---------------------

    void configureSD() {
      char str_buf_sd[16];
      //SPI.setMOSI(11); 
      //SPI.setSCK(13);
      //if (!(SD.begin(10))) // chipSelect = 10
      if (!(SD.begin(BUILTIN_SDCARD))) // chipSelect = built-in uses default?
      {  
          Serial8.println("Unable to access the SD card");
      //    loadPatchEEPROM();
      }
      for(int i=0;i < NUMBER_OF_PATCHES ;i++) {
        sprintf(str_buf_sd, "%03d.PAT", i );     
        if(SD.exists(str_buf_sd)) {
          dataFile = SD.open(str_buf_sd);
          dataFile.read(&loadedPatches[i], sizeof(loadedPatches[0]));
          dataFile.close();
          patchLoaded[i] = true;
        } else {
          patchLoaded[i] = false;
        }
      }
    }

    void savePatchSD(int i) {
      char str_buf_sd[16];
      // loadedPatches[i] = createPatch();
      if (!patchLoaded[i]) {
        sprintf(str_buf1,"Patch: %03d not loaded. Nothing to save.", i);
        Serial8.println(str_buf1);
        return;   
      } 
      sprintf(str_buf_sd, "%03d.PAT", i );     
      if (SD.exists(str_buf_sd)) {
        SD.remove(str_buf_sd);
      }
      dataFile = SD.open(str_buf_sd, FILE_WRITE);
      if(!dataFile) {
        Serial8.println("Could not create file");
        //TODO:  hook this up:  wavplayer.play("err.wav");
      } else {
        Serial8.print("size: ");
        Serial8.print(sizeof(loadedPatches[0]));
        Serial8.println();
        dataFile.write((byte*)&loadedPatches[i], sizeof(loadedPatches[0]));
//        Serial8.write((byte*)&loadedPatches[i], sizeof(loadedPatches[0]));
        dataFile.close();
        Serial8.print("Written");
      }
    }

    void saveCurrentPatchSD(int i) { // i is patch number to save it
      char str_buf_sd[16];
      // loadedPatches[i] = createPatch();
      if (!patchLoaded[i]) {
        sprintf(str_buf1,"Patch: %03d not loaded. can't upsate.", i);
        Serial8.println(str_buf1);
        return;   
      } 
      sprintf(str_buf_sd, "%03d.PAT", i );     
      if (SD.exists(str_buf_sd)) {
        SD.remove(str_buf_sd);
      }
      dataFile = SD.open(str_buf_sd, FILE_WRITE);
      if(!dataFile) {
        Serial8.println("Could not create file");
        //TODO:  hook this up:  wavplayer.play("err.wav");
      } else {
        Serial8.print("size: ");
        Serial8.print(sizeof(loadedPatches[0]));
        Serial8.println();
        dataFile.write((byte*)&current_patch, sizeof(loadedPatches[0]));
//        Serial8.write((byte*)&current_patch, sizeof(loadedPatches[0]));
        dataFile.close();
        sprintf(str_buf1,"current_patch written into %03d.PAT", i);
        copyCurrentPatchToLoadedPatch(i);
        Serial8.println(str_buf1);
      }
    }


    void loadPatchSD(int i) {
      char str_buf_sd[16];
      if(patchLoaded[i]) {
        Serial8.println("patch alreadey loaded");
        setCurrentPatch(i);
        Serial8.println("setCurrentPatch");
        patchToSynthVariables(&current_patch);
        Serial8.println("patchToSynthVariables");
      } else {
        sprintf(str_buf_sd, "%03d.PAT", i );     
        dataFile = SD.open(str_buf_sd);
        if(!dataFile) {
          Serial8.println("Could not load file");
          //TODO: hook this up wavplayer.play("err.wav");
        } else {
          int bytes = dataFile.read(&loadedPatches[i], sizeof(loadedPatches[0]));
          dataFile.close();
          Serial8.print("Loaded:"); Serial8.println(bytes, DEC);
          patchLoaded[i] = true;
          setCurrentPatch(i);
          patchToSynthVariables(&current_patch);
        }
      }
    }

    void loadAllPatches() {
      char str_buf_sd[16];
      int n_loaded = 0;
      for(int i=0;i < NUMBER_OF_PATCHES ;i++) {
        sprintf(str_buf_sd, "%03d.PAT", i );     
        if(SD.exists(str_buf_sd)) {
          dataFile = SD.open(str_buf_sd);
          dataFile.read(&loadedPatches[i], sizeof(loadedPatches[0]));
          dataFile.close();
          patchLoaded[i] = true;
          n_loaded++;
        } else {
          patchLoaded[i] = false;
          sprintf(str_buf1,"Patch: %03d does not exist. Nothing to load.", i);
          Serial8.println(str_buf1);
        }
      }
          sprintf(str_buf1,"loaded %03d patches.", n_loaded);
          Serial8.println(str_buf1);
    }

float noiseTimeCurve(float noiseTimePercent) {
    float a = 0.0f;
    float b = 1.0f;
    if (noiseTimePercent <= 0){
        a = 0.0f;
        b = 0.0f;
    } else if (noiseTimePercent < 0.7f){
         a = 0.0564f * 1000.0f; // 1000 for time in mS
         b = 2.75f;
    } else if (noiseTimePercent < 0.9f) {
        a = 0.0105f * 1000.0f; // 1000 for time in mS
        b = 5.2f;
    } else {
        a = 0.0001f * 1000.0f; // 1000 for time in mS
        b = 10.4f;
    }
    float y = a*exp(b*noiseTimePercent);
    return y;
}

float sweepTimeFilterCurve(float sweepTime) {
    float a = 0.0f;
    float b = 1.0f;
    if (sweepTime <= 0){
        a = 0.0f;
        b = 0.0f;
    } else if (sweepTime < 0.7f){
         a = 0.0103f * 1000.0f; // 1000 for time in mS
         b = 4.1647f;
    } else if (sweepTime < 0.9f) {
        a = 0.0021f * 1000.0f; // 1000 for time in mS
        b = 6.4453f;
    } else {
        a = 0.0000003f * 1000.0f; // 1000 for time in mS
        b = 16.27f;
    }
    float y = a*exp(b*sweepTime);
    return y;
}

float sweepTimeOscCurve(float sweepTime){
    // separate func in case it needs to be different
    // but just call the FilterCurve one for now 
    return sweepTimeFilterCurve(sweepTime);
}


