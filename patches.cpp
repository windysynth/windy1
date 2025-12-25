/************************************************************
  Windy1 USB Host Synth for Wind controllers
        MIT license
        Copyright (c) 2021 Tim King

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

****************************************************************/
#include "globals.h"
#include "patches.h"
#include "menuSetup.h"
#include <SD.h>

void setDestination()
{
  // Called when entering the Paste menu to prepare/initialize for destination selection
  // Add your setup code here
  paste_patchNumber = current_patchNumber;
}

void nprnToDisplayChorus(char *dispValBuf, uint8_t *val)
{
  if (val == NULL)
    return;
  switch (*val)
  {
  case 0:
    sprintf(dispValBuf, "    Off    ");
    break;
  case 1:
    sprintf(dispValBuf, "  Mono(4k) ");
    break;
  case 2:
    sprintf(dispValBuf, "  Stereo   ");
    break;
  default:
    sprintf(dispValBuf, " er_Chorus");
    break;
  }
}
void nprnToDisplayOnOff(char *dispValBuf, uint8_t *val) // 0=Off 1=On
{
  if (val == NULL)
    return;
  if (*val % 2 == 0) // Filter Link values: 1 = on, 2 = off
    sprintf(dispValBuf, "   Off    ");
  else
    sprintf(dispValBuf, "   On     ");
}
void nprnToDisplayPatchGlobal(char *dispValBuf, uint8_t *val) // 0=Global 1=Patch
{
  if (val == NULL)
    return;
  if (*val == 0)
    sprintf(dispValBuf, "   Global ");
  else
    sprintf(dispValBuf, "   Patch  ");
}
void nprnToDisplayKeyTrigger(char *dispValBuf, uint8_t *val) // 0=Mult 1=Single
{
  if (val == NULL)
    return;
  if (*val == 0)
    sprintf(dispValBuf, "   Multi  ");
  else
    sprintf(dispValBuf, "   Single ");
}
void nprnToDisplay03d(char *dispValBuf, uint8_t *val) // 000 to 127
{
  if (val == NULL)
    return;
  sprintf(dispValBuf, "   %03d    ", *val);
}
void nprnToDisplay03dm(char *dispValBuf, uint8_t *val) // -64 to +63
{
  if (val == NULL)
    return;
  int8_t sval = (int8_t)(*val) - 64;
  sprintf(dispValBuf, "   %+03d    ", sval);
}
void nprnToDisplay03dmS(char *dispValBuf, uint8_t *val) // 0 to 127 mS
{
  if (val == NULL)
    return;
  sprintf(dispValBuf, "   %03d mS ", *val);
}
void nprnToDisplay04d10x(char *dispValBuf, uint8_t *val) // 0 to 1270
{
  if (val == NULL)
    return;
  int16_t bigval = (int16_t)(*val) * 10;
  sprintf(dispValBuf, "  %04d    ", bigval);
}
void nprnToDisplay04d100x(char *dispValBuf, uint8_t *val) // 1000 to 5000
{
  if (val == NULL)
    return;
  int16_t bigval = (int16_t)(*val) * 100;
  sprintf(dispValBuf, "  %04d    ", bigval);
}
void nprnToDisplay03dFlip(char *dispValBuf, uint8_t *val) // 127 to 0
{
  if (val == NULL)
    return;
  uint8_t flipval = 127 - *val;
  sprintf(dispValBuf, "   %03d    ", flipval);
}
void nprnToDisplay2p1fHzDiv10(char *dispValBuf, uint8_t *val) //  val/10.0 Hz
{
  if (val == NULL)
    return;
  float fval = (float)(*val) / 10.0f;
  sprintf(dispValBuf, "  %04.1f   ", fval);
}
void nprnToDisplayFilterMode(char *dispValBuf, uint8_t *val) // 0=LP 1=HP 2=BP 3=NTC 4=Thru
{
  if (val == NULL)
    return;
  switch ((filterMode_t)*val)
  {
  case LP:
    sprintf(dispValBuf, "    LP     ");
    break;
  case HP:
    sprintf(dispValBuf, "    HP     ");
    break;
  case BP:
    sprintf(dispValBuf, "    BP     ");
    break;
  case NTC:
    sprintf(dispValBuf, "    NTC    ");
    break;
  case THRU:
    sprintf(dispValBuf, "    Thru   ");
    break;
  default:
    sprintf(dispValBuf, " er_Fltmode");
    break;
  }
}
void nprnToDisplayFilterFreqHz(char *dispValBuf, uint8_t *val)
{
  if (val == NULL)
    return;
  float FreqFilterNN = (float)(*val);
  float FreqFilter = 440.0 * pow(2, (FreqFilterNN - 69.0) / 12);
  sprintf(dispValBuf, " %5.1f Hz", FreqFilter);
}
void nprnToDisplayFormant(char *dispValBuf, uint8_t *val)
{
  if (val == NULL)
    return;
  switch (*val)
  {
  case 0:
    sprintf(dispValBuf, "    Off    ");
    break;
  case 1:
    sprintf(dispValBuf, "  WoodWind ");
    break;
  case 2:
    sprintf(dispValBuf, "  String   ");
    break;
  default:
    sprintf(dispValBuf, " er_Formant"); // TODO: fix, powers up to here for some reason
    break;
  }
}
void nprnToDisplayCurrentPatch(char *dispValBuf, uint8_t *val)
{
  if (val == NULL)
    return;
  sprintf(dispValBuf, "%s", current_patch.patch_string);
}
void nprnToDisplayDest(char *dispValBuf, uint8_t *val)
{
  if (val == NULL)
    return;
  sprintf(dispValBuf, "%03d   ", *val + 1);
  // sprintf(dispValBuf, "%d: %s", *val + 1, namebuf);
}

//--- synth params funcs not part of patch
void volAdjustFun()
{
  volf = ((float)vol) / 100.0f;
  volf = (volf * volf) * 2.0f;
};
void auxInAdjustFun()
{
  mix_lineinf = ((float)mix_linein) / 100.0f;
  mix_lineinf = (mix_lineinf * mix_lineinf) * 2.0f;
};
void octaveAdjustFun()
{
  Octavef = (((float)Octave) - 64.0f) * 12.0f;
};
void semiAdjustFun()
{
  Transposef = (float)Transpose - 64.0f;
} // 64,1,52,76,
void centsAdjustFun()
{
  FineTuneCentsf = ((float)FineTuneCents - 64.0f) / 100.0f;
} // 64,2,14,114,-50 to +50 cents
void breathccAdjustFun()
{
  breath_cc = breath_cc == 6 && breath_cc_last == 5 ? 7 : breath_cc; // skip 6 if last was 5
  breath_cc = breath_cc == 6 && breath_cc_last == 7 ? 5 : breath_cc; // skip 6 if last was 7
  breath_cc_last = breath_cc;
} // 64,2,14,114,-50 to +50 cents
void fxSourcToSynth()
{
  // updateSynthVariablesFlag = true; //updating patch and effects (TODO: only effects?)
  patchToSynthVariables();
}
void patchToDummyFunc()
{
  // dummy function for menu items that don't have a patchToSynth function
}
/*
    compress_oscLevels.setAttack( 0.10f);
    compress_oscLevels.setRelease( 20.0f);
    compress_oscLevels.setRatio( 12.0f);
    compress_oscLevels.setThreshold( -16.0f);
    compress_oscLevels.setKnee( 8.0f);
    compress_oscLevels.setMakeupGain(14.0f);
    compress_oscLevels.setSideChain(0);
*/
void compAttackAdjustFun()
{
  comp_paramsf[CCCOMPATTACK] = ((float)comp_params[CCCOMPATTACK]) / 10.0f; // 0.1ms to 12.7ms
  updateCalibrationFlag = true;
}
void compReleaseAdjustFun()
{
  comp_paramsf[CCCOMPRELEASE] = (float)(comp_params[CCCOMPRELEASE]); // 1ms to 127ms
  updateCalibrationFlag = true;
}
void compRatioAdjustFun()
{
  comp_paramsf[CCCOMPRATIO] = ((float)comp_params[CCCOMPRATIO]) / 10.0f; // 0.1 to 12.7
  updateCalibrationFlag = true;
}
void compThreshAdjustFun()
{
  comp_paramsf[CCCOMPTHRESHOLD] = (float)(comp_params[CCCOMPTHRESHOLD] - 64); // -40 to 0 dB
  updateCalibrationFlag = true;
}
void compKneeAdjustFun()
{
  comp_paramsf[CCCOMPKNEE] = (float)(comp_params[CCCOMPKNEE]); // 0 to 40 dB
  updateCalibrationFlag = true;
}
void compMUGainAdjustFun()
{
  comp_paramsf[CCCOMPMAKEUPGAIN] = (float)(comp_params[CCCOMPMAKEUPGAIN]); // 0 to 40 db
  updateCalibrationFlag = true;
}
void nnBmodAdjustFun()
{
  // call functions that use NNBModCal
  patchToFreqOscFilter1();
  patchToFreqOscFilter2();
  patchToFreqNoiseFilter3();
  patchToFreqNoiseFilter4();
}

//--------patchTo<SynthParam> functions--------
void patchToOctOsc1()
{
  OctOsc1 = (float)current_patch.nrpn_msb_osc1[CCOCTOSC1] - 64.0f;
} // 64,0,62,66,
void patchToSemiOsc1()
{
  SemiOsc1 = (float)current_patch.nrpn_msb_osc1[CCSEMIOSC1] - 64.0f;
} // 64,1,52,76,
void patchToFineOsc1()
{
  FineOsc1 = ((float)current_patch.nrpn_msb_osc1[CCFINEOSC1] - 64.0f) / 100.0f;
} // 64,2,14,114,-50 to +50 cents
void patchToBeatOsc1()
{
  BeatOsc1 = ((float)current_patch.nrpn_msb_osc1[CCBEATOSC1] - 64.0f) / 64.0f * BeatMax;
} // 64,3,0,127, -5.33hz to +5.33hz
void patchToSawOsc1()
{
  SawOsc1 = log_pot_curve((float)current_patch.nrpn_msb_osc1[CCSAWOSC1] * DIV127, logPotYmidWaveN);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,5,0,127,
void patchToTriOsc1()
{
  TriOsc1 = log_pot_curve((float)current_patch.nrpn_msb_osc1[CCTRIOSC1] * DIV127, logPotYmidWaveN);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,6,0,127,
void patchToPulseOsc1()
{
  PulseOsc1 = log_pot_curve((float)current_patch.nrpn_msb_osc1[CCPULSEOSC1] * DIV127, logPotYmidWaveN);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,7,0,127,
void patchToPwOsc1()
{
  PwOsc1 = (float)current_patch.nrpn_msb_osc1[CCPWOSC1] * DIV127 + PwOffsetOsc1;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,8,0,127,  GUI 0 = 50%
void patchToPwmFreqOsc1()
{
  PwmFreqOsc1 = (float)current_patch.nrpn_msb_osc1[CCPWMFREQOSC1] * DIV127 * maxPwmLfoFreq;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,9,0,127,
void patchToPwmDepthOsc1()
{
  PwmDepthOsc1 = (float)current_patch.nrpn_msb_osc1[CCPWMDEPTHOSC1] * DIV127;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,10,0,127,
void patchToSweepDepthOsc1()
{
  SweepDepthOsc1 = 0.5f * ((float)current_patch.nrpn_msb_osc1[CCSWEEPDEPTHOSC1] - 64.0) / 64.0;
} // 64,11,0,127,
void patchToSweepTimeOsc1()
{
  SweepTimeOsc1 = sweepTimeOscCurve((float)current_patch.nrpn_msb_osc1[CCSWEEPTIMEOSC1] * DIV127);
} // 64,12,0,127,
void patchToBreathDepthOsc1()
{
  BreathDepthOsc1 = ((float)current_patch.nrpn_msb_osc1[CCBREATHDEPTHOSC1] - 64.0) / 64.0;
} // 64,13,0,127,
void patchToBreathAttainOsc1()
{
  BreathAttainOsc1 = (float)current_patch.nrpn_msb_osc1[CCBREATHATTAINOSC1] * DIV127;
} // 64,14,0,127,
// BreathCurveOsc1 = gen_osc_gamma(((float)current_patch.nrpn_msb_osc1[CCBREATHCURVEOSC1])*DIV127);  	//64,15,0,127,
void patchToBreathOscCurveLines1()
{
  BreathOscCurveLines1 = gen_osc_curve_lines(((float)current_patch.nrpn_msb_osc1[CCBREATHCURVEOSC1]) * DIV127);
} // 64,15,0,127,
void patchToBreathThreshOsc1()
{
  BreathThreshOsc1 = (float)current_patch.nrpn_msb_osc1[CCBREATHTHRESHOSC1] * DIV127;
} // 64,16,0,127,
void patchToLevelOsc1()
{
  LevelOsc1 = log_pot_curve((float)current_patch.nrpn_msb_osc1[CCLEVELOSC1] * DIV127, logPotYmidLevelOscN);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,17,0,127,
void patchToOctOsc2()
{
  OctOsc2 = (float)current_patch.nrpn_msb_osc2[CCOCTOSC2] - 64.0f;
} // 65,0,62,66,
void patchToSemiOsc2()
{
  SemiOsc2 = (float)current_patch.nrpn_msb_osc2[CCSEMIOSC2] - 64.0f;
} // 64,1,52,76,
void patchToFineOsc2()
{
  FineOsc2 = ((float)current_patch.nrpn_msb_osc2[CCFINEOSC2] - 64.0f) / 100.0f;
} // 64,2,14,114, -50 to +50 cents
void patchToBeatOsc2()
{
  BeatOsc2 = ((float)current_patch.nrpn_msb_osc2[CCBEATOSC2] - 64.0f) / 64.0f * BeatMax;
} // 64,3,0,127,
void patchToSawOsc2()
{
  SawOsc2 = log_pot_curve((float)current_patch.nrpn_msb_osc2[CCSAWOSC2] * DIV127, logPotYmidWaveN);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,5,0,127,
void patchToTriOsc2()
{
  TriOsc2 = log_pot_curve((float)current_patch.nrpn_msb_osc2[CCTRIOSC2] * DIV127, logPotYmidWaveN);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,6,0,127,
void patchToPulseOsc2()
{
  PulseOsc2 = log_pot_curve((float)current_patch.nrpn_msb_osc2[CCPULSEOSC2] * DIV127, logPotYmidWaveN);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,7,0,127,
void patchToPwOsc2()
{
  PwOsc2 = (float)current_patch.nrpn_msb_osc2[CCPWOSC2] * DIV127 + PwOffsetOsc2;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,8,0,127,  GUI 0 = 50%
void patchToPwmFreqOsc2()
{
  PwmFreqOsc2 = (float)current_patch.nrpn_msb_osc2[CCPWMFREQOSC2] * DIV127 * maxPwmLfoFreq;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,9,0,127,
void patchToPwmDepthOsc2()
{
  PwmDepthOsc2 = (float)current_patch.nrpn_msb_osc2[CCPWMDEPTHOSC2] * DIV127;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,10,0,127,
void patchToSweepDepthOsc2()
{
  SweepDepthOsc2 = 0.5f * ((float)current_patch.nrpn_msb_osc2[CCSWEEPDEPTHOSC2] - 64.0f) / 64.0f;
} // 64,11,0,127,
void patchToSweepTimeOsc2()
{
  SweepTimeOsc2 = sweepTimeOscCurve((float)current_patch.nrpn_msb_osc2[CCSWEEPTIMEOSC2] * DIV127);
} // 64,12,0,127,
void patchToBreathDepthOsc2()
{
  BreathDepthOsc2 = ((float)current_patch.nrpn_msb_osc2[CCBREATHDEPTHOSC2] - 64.0) / 64.0;
} // 64,13,0,127,
void patchToBreathAttainOsc2()
{
  BreathAttainOsc2 = (float)current_patch.nrpn_msb_osc2[CCBREATHATTAINOSC2] * DIV127;
} // 64,14,0,127,
// BreathCurveOsc2 = gen_osc_gamma(((float)current_patch.nrpn_msb_osc2[CCBREATHCURVEOSC2])*DIV127);  	//64,15,0,127,
void patchToBreathOscCurveLines2()
{
  BreathOscCurveLines2 = gen_osc_curve_lines(((float)current_patch.nrpn_msb_osc2[CCBREATHCURVEOSC2]) * DIV127);
} // 64,15,0,127,
void patchToBreathThreshOsc2()
{
  BreathThreshOsc2 = (float)current_patch.nrpn_msb_osc2[CCBREATHTHRESHOSC2] * DIV127;
} // 64,16,0,127,
void patchToLevelOsc2()
{
  LevelOsc2 = log_pot_curve((float)current_patch.nrpn_msb_osc2[CCLEVELOSC2] * DIV127, logPotYmidLevelOscN);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 64,17,0,127,
void patchToModeOscFilter1()
{
  ModeOscFilter1 = (filterMode_t)current_patch.nrpn_msb_osc_filt1[CCMODEOSCFILTER1];
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 72,0,0,4,//LP HP BP NTC OFF
void patchToFreqOscFilter1()
{
  // 69 is note number for A4=440Hz ;
  float FreqOscFilter1NN = (float)current_patch.nrpn_msb_osc_filt1[CCFREQOSCFILTER1]; // 72,1,36,124,note# 36(c2) to 124(e9 = 10,548Hz)
  //(when slider = NNBModCal (in globals.cpp), BModFactor is 1.0), using full range set by octaveControlFilter1
  FreqOscFilter1BModFactor = FreqOscFilter1NN / (float)NNBModCal;
  FreqOscFilter1 = 440.0f * pow(2, (FreqOscFilter1NN - 69.0) / 12);
  // 72,1,36,124,note# 36(c2) to 124(e9)
}
void patchToQFactorOscFilter1()
{
  QFactorOscFilter1 = (float)current_patch.nrpn_msb_osc_filt1[CCQFACTOROSCFILTER1] / 10.0f;
} // 72,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowOscFilter1()
{
  KeyFollowOscFilter1 = ((float)current_patch.nrpn_msb_osc_filt1[CCKEYFOLLOWOSCFILTER1] - 64.0f);
} // 72,3,52,88,// -12 to +24 num semi tones
void patchToBreathModOscFilter1()
{
  BreathModOscFilter1 = (float)current_patch.nrpn_msb_osc_filt1[CCBREATHMODOSCFILTER1] * DIV127;
} // 72,4,0,127,
void patchToLfoFreqOscFilter1()
{
  LfoFreqOscFilter1 = pow((float)current_patch.nrpn_msb_osc_filt1[CCLFOFREQOSCFILTER1] * DIV127, 2.0f) * maxLfoFreqFilter1;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 72,5,0,127,
void patchToLfoDepthOscFilter1()
{
  LfoDepthOscFilter1 = (float)current_patch.nrpn_msb_osc_filt1[CCLFODEPTHOSCFILTER1] * DIV127;
} // 72,6,0,127,
void patchToLfoBreathOscFilter1()
{
  LfoBreathOscFilter1 = ((float)current_patch.nrpn_msb_osc_filt1[CCLFOBREATHOSCFILTER1] - 64.0f) / 64.0f;
} // 72,7,0,127,
void patchToLfoThreshOscFilter1()
{
  LfoThreshOscFilter1 = (float)current_patch.nrpn_msb_osc_filt1[CCLFOTHRESHOSCFILTER1] * DIV127;
} // 72,8,0,127,
void patchToSweepDepthOscFilter1()
{
  SweepDepthOscFilter1 = ((float)current_patch.nrpn_msb_osc_filt1[CCSWEEPDEPTHOSCFILTER1] - 64.0f) / 64.0f;
} // 72,9,0,127,
void patchToSweepTimeOscFilter1()
{
  SweepTimeOscFilter1 = sweepTimeFilterCurve((float)current_patch.nrpn_msb_osc_filt1[CCSWEEPTIMEOSCFILTER1] * DIV127);
} // 72,10,0,127,
// BreathCurveOscFilter1 = gen_filter_gamma(((float)current_patch.nrpn_msb_osc_filt1[CCBREATHCURVEOSCFILTER1])*DIV127);  	//72,11,0,127,
void patchToBreathOscFiltCurveLines1()
{
  BreathOscFiltCurveLines1 = gen_filter_curve_lines(((float)current_patch.nrpn_msb_osc_filt1[CCBREATHCURVEOSCFILTER1]) * DIV127);
} // 72,11,0,127,
void patchToModeOscFilter2()
{
  ModeOscFilter2 = (filterMode_t)current_patch.nrpn_msb_osc_filt2[CCMODEOSCFILTER2];
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 73,0,0,4,//LP HP BP NTC OFF
void patchToFreqOscFilter2()
{
  // 69 is note number for A4=440Hz ;
  float FreqOscFilter2NN = (float)current_patch.nrpn_msb_osc_filt2[CCFREQOSCFILTER2]; // 72,1,36,124,note# 36(c2) to 124(e9)
  FreqOscFilter2BModFactor = FreqOscFilter2NN / (float)NNBModCal;                     //(when slider = 96 (c7), BModFactor is 1.0)
  FreqOscFilter2 = 440.0f * pow(2, (FreqOscFilter2NN - 69.0f) / 12.0f);
  // 72,1,36,124,note# 36(c2) to 124(e9)
}
void patchToQFactorOscFilter2()
{
  QFactorOscFilter2 = (float)current_patch.nrpn_msb_osc_filt2[CCQFACTOROSCFILTER2] / 10.0;
} // 72,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowOscFilter2()
{
  KeyFollowOscFilter2 = ((float)current_patch.nrpn_msb_osc_filt2[CCKEYFOLLOWOSCFILTER2] - 64.0f);
} // 72,3,52,88,// -12 to +24 num semi tones
void patchToBreathModOscFilter2()
{
  BreathModOscFilter2 = (float)current_patch.nrpn_msb_osc_filt2[CCBREATHMODOSCFILTER2] * DIV127;
} // 72,4,0,127,
void patchToLfoFreqOscFilter2()
{
  LfoFreqOscFilter2 = pow((float)current_patch.nrpn_msb_osc_filt2[CCLFOFREQOSCFILTER2] * DIV127, 2.0f) * maxLfoFreqFilter2;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 72,5,0,127,
void patchToLfoDepthOscFilter2()
{
  LfoDepthOscFilter2 = (float)current_patch.nrpn_msb_osc_filt2[CCLFODEPTHOSCFILTER2] * DIV127;
} // 72,6,0,127,
void patchToLfoBreathOscFilter2()
{
  LfoBreathOscFilter2 = ((float)current_patch.nrpn_msb_osc_filt2[CCLFOBREATHOSCFILTER2] - 64.0f) / 64.0f;
} // 72,7,0,127,
void patchToLfoThreshOscFilter2()
{
  LfoThreshOscFilter2 = (float)current_patch.nrpn_msb_osc_filt2[CCLFOTHRESHOSCFILTER2] * DIV127;
} // 72,8,0,127,
void patchToSweepDepthOscFilter2()
{
  SweepDepthOscFilter2 = ((float)current_patch.nrpn_msb_osc_filt2[CCSWEEPDEPTHOSCFILTER2] - 64.0f) / 64.0f;
} // 72,9,0,127,
void patchToSweepTimeOscFilter2()
{
  SweepTimeOscFilter2 = sweepTimeFilterCurve((float)current_patch.nrpn_msb_osc_filt2[CCSWEEPTIMEOSCFILTER2] * DIV127);
} // 72,10,0,127,
// BreathCurveOscFilter2 = gen_filter_gamma(((float)current_patch.nrpn_msb_osc_filt2[CCBREATHCURVEOSCFILTER2])*DIV127);  	//72,11,0,127,
void patchToBreathOscFiltCurveLines2()
{
  BreathOscFiltCurveLines2 = gen_filter_curve_lines(((float)current_patch.nrpn_msb_osc_filt2[CCBREATHCURVEOSCFILTER2]) * DIV127);
} // 72,11,0,127,
void patchToModeNoiseFilter3()
{
  ModeNoiseFilter3 = (filterMode_t)current_patch.nrpn_msb_noise_filt3[CCMODENOISEFILTER3];
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 74,0,0,4,//LP HP BP NTC OFF
void patchToFreqNoiseFilter3()
{
  float FreqNoiseFilter3NN = (float)current_patch.nrpn_msb_noise_filt3[CCFREQNOISEFILTER3]; // 72,1,36,124,note# 36(c2) to 124(e9)
  FreqNoiseFilter3BModFactor = FreqNoiseFilter3NN / (float)NNBModCal;
  FreqNoiseFilter3 = 440.0f * pow(2, (FreqNoiseFilter3NN - 69.0f) / 12.0f);
  // 72,1,36,124,note# 36(c2) to 124(e9)
}
void patchToQFactorNoiseFilter3()
{
  QFactorNoiseFilter3 = (float)current_patch.nrpn_msb_noise_filt3[CCQFACTORNOISEFILTER3] / 10.0;
} // 74,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowNoiseFilter3()
{
  KeyFollowNoiseFilter3 = ((float)current_patch.nrpn_msb_noise_filt3[CCKEYFOLLOWNOISEFILTER3] - 64.0);
} // 74,3,52,88,// -12 to +24 num semitones
void patchToBreathModNoiseFilter3()
{
  BreathModNoiseFilter3 = (float)current_patch.nrpn_msb_noise_filt3[CCBREATHMODNOISEFILTER3] * DIV127;
} // 74,4,0,127,
void patchToLfoFreqNoiseFilter3()
{
  LfoFreqNoiseFilter3 = pow((float)current_patch.nrpn_msb_noise_filt3[CCLFOFREQNOISEFILTER3] * DIV127, 2.0) * maxLfoFreqFilter3;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 74,5,0,127,
void patchToLfoDepthNoiseFilter3()
{
  LfoDepthNoiseFilter3 = (float)current_patch.nrpn_msb_noise_filt3[CCLFODEPTHNOISEFILTER3] * DIV127;
} // 74,6,0,127,
void patchToLfoBreathNoiseFilter3()
{
  LfoBreathNoiseFilter3 = ((float)current_patch.nrpn_msb_noise_filt3[CCLFOBREATHNOISEFILTER3] - 64.0) / 64.0;
} // 74,7,0,127,
void patchToLfoThreshNoiseFilter3()
{
  LfoThreshNoiseFilter3 = (float)current_patch.nrpn_msb_noise_filt3[CCLFOTHRESHNOISEFILTER3] * DIV127;
} // 74,8,0,127,
void patchToSweepDepthNoiseFilter3()
{
  SweepDepthNoiseFilter3 = ((float)current_patch.nrpn_msb_noise_filt3[CCSWEEPDEPTHNOISEFILTER3] - 64.0) / 64.0;
} // 74,9,0,127,
void patchToSweepTimeNoiseFilter3()
{
  SweepTimeNoiseFilter3 = sweepTimeFilterCurve((float)current_patch.nrpn_msb_noise_filt3[CCSWEEPTIMENOISEFILTER3] * DIV127);
} // 74,10,0,127,
// BreathCurveNoiseFilter3 = gen_filter_gamma(((float)current_patch.nrpn_msb_noise_filt3[CCBREATHCURVENOISEFILTER3])*DIV127);  	//74,11,0,127,
void patchToBreathNoiseFiltCurveLines3()
{
  BreathNoiseFiltCurveLines3 = gen_filter_curve_lines(((float)current_patch.nrpn_msb_noise_filt3[CCBREATHCURVENOISEFILTER3]) * DIV127);
} // 74,11,0,127,
void patchToModeNoiseFilter4()
{
  ModeNoiseFilter4 = (filterMode_t)current_patch.nrpn_msb_noise_filt4[CCMODENOISEFILTER4];
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()

} // 74,0,0,4,//LP HP BP NTC OFF
void patchToFreqNoiseFilter4()
{
  float FreqNoiseFilter4NN = (float)current_patch.nrpn_msb_noise_filt4[CCFREQNOISEFILTER4]; // 72,1,36,124,note# 36(c2) to 124(e9)
  FreqNoiseFilter4BModFactor = FreqNoiseFilter4NN / (float)NNBModCal;
  FreqNoiseFilter4 = 440.0f * pow(2, (FreqNoiseFilter4NN - 69.0f) / 12.0f);
  // 72,1,36,124,note# 36(c2) to 124(e9)
}
void patchToQFactorNoiseFilter4()
{
  QFactorNoiseFilter4 = (float)current_patch.nrpn_msb_noise_filt4[CCQFACTORNOISEFILTER4] / 10.0f;
} // 74,2,5,127,"// 5=0.5, 127=12.7"
void patchToKeyFollowNoiseFilter4()
{
  KeyFollowNoiseFilter4 = ((float)current_patch.nrpn_msb_noise_filt4[CCKEYFOLLOWNOISEFILTER4] - 64.0f);
} // 74,3,52,88,// -12 to +24 num semitones
void patchToBreathModNoiseFilter4()
{
  BreathModNoiseFilter4 = (float)current_patch.nrpn_msb_noise_filt4[CCBREATHMODNOISEFILTER4] * DIV127;
} // 74,4,0,127,
void patchToLfoFreqNoiseFilter4()
{
  LfoFreqNoiseFilter4 = pow((float)current_patch.nrpn_msb_noise_filt4[CCLFOFREQNOISEFILTER4] * DIV127, 2.0f) * maxLfoFreqFilter4;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 74,5,0,127,
void patchToLfoDepthNoiseFilter4()
{
  LfoDepthNoiseFilter4 = (float)current_patch.nrpn_msb_noise_filt4[CCLFODEPTHNOISEFILTER4] * DIV127;
} // 74,6,0,127,
void patchToLfoBreathNoiseFilter4()
{
  LfoBreathNoiseFilter4 = ((float)current_patch.nrpn_msb_noise_filt4[CCLFOBREATHNOISEFILTER4] - 64.0f) / 64.0f;
} // 74,7,0,127,
void patchToLfoThreshNoiseFilter4()
{
  LfoThreshNoiseFilter4 = (float)current_patch.nrpn_msb_noise_filt4[CCLFOTHRESHNOISEFILTER4] * DIV127;
} // 74,8,0,127,
void patchToSweepDepthNoiseFilter4()
{
  SweepDepthNoiseFilter4 = ((float)current_patch.nrpn_msb_noise_filt4[CCSWEEPDEPTHNOISEFILTER4] - 64.0f) / 64.0f;
} // 74,9,0,127,
void patchToSweepTimeNoiseFilter4()
{
  SweepTimeNoiseFilter4 = sweepTimeFilterCurve((float)current_patch.nrpn_msb_noise_filt4[CCSWEEPTIMENOISEFILTER4] * DIV127);
} // 74,10,0,127,
// BreathCurveNoiseFilter4 = gen_filter_gamma(((float)current_patch.nrpn_msb_noise_filt4[CCBREATHCURVENOISEFILTER4])*DIV127);  	//74,11,0,127,
void patchToBreathNoiseFiltCurveLines4()
{
  BreathNoiseFiltCurveLines4 = gen_filter_curve_lines(((float)current_patch.nrpn_msb_noise_filt4[CCBREATHCURVENOISEFILTER4]) * DIV127);
} // 74,11,0,127,
void patchToNoiseTime()
{
  NoiseTime = noiseTimeCurve((float)current_patch.nrpn_msb_noise[CCNOISETIME] * DIV127);
} // 80,0,0,127,
// NoiseBreathCurve = gen_noise_gamma(((float)(current_patch.nrpn_msb_noise[CCNOISEBREATHCURVE]))*DIV127);  	//80,1,0,127,
void patchToNoiseBreathCurveLines()
{
  NoiseBreathCurveLines = gen_osc_curve_lines(((float)(current_patch.nrpn_msb_noise[CCNOISEBREATHCURVE])) * DIV127);
} // 80,1,0,127,
void patchToNoiseLevel()
{
  NoiseLevel = (float)current_patch.nrpn_msb_noise[CCNOISELEVEL] * DIV127 * maxNoiseLevel;
  //  if ((float)current_patch.nrpn_msb_common2[CCAMPLEVEL] * DIV127 > 0.005f)
  //    NoiseLevel = NoiseLevel * (AmpLevel / ((float)current_patch.nrpn_msb_common2[CCAMPLEVEL] * DIV127));

  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 80,2,0,127,
void patchToBendRange()
{
  BendRange = (float)current_patch.nrpn_msb_common1[CCBENDRANGE];
} // 81,0,0,12,// num semitones
void patchToBendStep()
{
  BendStep = (bool)current_patch.nrpn_msb_common1[CCBENDSTEP];
} // 81,1,0,1,//0=off 1=on
void patchToVibratoPitch()
{
  VibratoPitch = (float)current_patch.nrpn_msb_common1[CCVIBRATOPITCH] * DIV127;
} // 81,2,0,127,  (this isn't necessary since bite sensor pb and plate pb are the same).
void patchToLinkOscFilters()
{
  LinkOscFilters = (bool)(current_patch.nrpn_msb_common1[CCLINKOSCFILTERS] % 2);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 81,3,1,2,//1=Link on 2= Link off
void patchToLinkNoiseFilters()
{
  LinkNoiseFilters = (bool)(current_patch.nrpn_msb_common1[CCLINKNOISEFILTERS] % 2);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 81,4,1,2,
void patchToFormant()
{
  Formant = current_patch.nrpn_msb_common1[CCFORMANT];
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 81,5,0,2,//0=off 1=W.Wind 2=String
void patchToXFade()
{
  XFade = (bool)current_patch.nrpn_msb_common1[CCXFADE];
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 81,6,0,1,//0=off 1=on
void patchToKeyTriggerSingle()
{
  KeyTriggerSingle = (bool)current_patch.nrpn_msb_common1[CCKEYTRIGGER];
} // 81,7,0,1,//0=Multi 1=Single
void patchTos81_8()
{
  s81_8 = (float)current_patch.nrpn_msb_common1[CC81_8];
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 81,8,0,1,// ?
void patchToChorusOn()
{
  ChorusOn = (uint8_t)*getFxValue(EFFECTGROUPCOMMON1, CCCHORUSON);
  // 81,9,0,2,// Chorus off, mono(4k), stereo
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToVibratoAmp()
{
  VibratoAmp = (float)current_patch.nrpn_msb_common2[CCVIBRATOAMP] * DIV127;
} // 88,0,0,127, (bite tremelo amount)
void patchToAmpLevel()
{
  // AmpLevel = amp_curve( (float)current_patch.nrpn_msb_common2[CCAMPLEVEL]*DIV127 );
  AmpLevel = (float)current_patch.nrpn_msb_common2[CCAMPLEVEL] * DIV127;
  patchToNoiseLevel();
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
} // 88,1,0,127,
// void patchToOctButtonLevel()
//{
//   OctButtonLevel = (float)current_patch.nrpn_msb_common2[CCOCTBUTTONLEVEL] * DIV127;
// } // 88,2,0,127,
void patchToEffectsChorusDelay1()
{
  // flange cuts delay in half, so mult by 2.0 here
  // 7.0ms*44.1s/ms for range of ChorusMod1 and 2
  EffectsChorusDelay1 = 2.0f * 44.1f * (float)*getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY1);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsChorusMod1()
{
  EffectsChorusMod1 = (2.0f * 44.1f) * ((float)*getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD1) - 64.0f) / 64.0f;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsChorusWet1()
{
  EffectsChorusWet1 = ((float)*getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET1) - 64.0f) / 64.0f;
  // 112,2,0,127, neg vals are phase inverted
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsChorusDelay2()
{
  EffectsChorusDelay2 = 2.0f * 44.1f * (float)*getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY2);
  // 112,3,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsChorusMod2()
{
  EffectsChorusMod2 = (2.0f * 44.1f) * ((float)*getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD2) - 64.0) / 64.0;
  // 112,4,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsChorusWet2()
{
  EffectsChorusWet2 = ((float)*getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET2) - 64.0f) / 64.0f;
  // 112,5,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsChorusFeedback()
{
  EffectsChorusFeedback = ((float)*getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSFEEDBACK) - 64.0f) / 64.0f;
  // 112,6,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsChorusLfoFreq()
{
  EffectsChorusLfoFreq = (float)*getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSLFOFREQ) / 10.0f;
  // 112,7,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsChorusDryLevel()
{
  EffectsChorusDryLevel = (float)*getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSDRYLEVEL) * DIV127;
  // 112,8,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsDelayTimeL()
{
  // AudioEffectDelayStereo_F32::time() squares the number (for some reason), so i'll sqrt here first
  EffectsDelayTimeL = (float)*getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYTIME) * DIV127;
  EffectsDelayTimeL = sqrt(EffectsDelayTimeL);
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsDelayFeedback()
{
  EffectsDelayFeedback = (float)*getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYFEEDBACK) * DIV127;
  // 113,1,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsDelayDamp()
{
  EffectsDelayDamp = (float)*getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYDAMP) * DIV127;
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsDelayLevel()
{
  EffectsDelayLevel = (float)*getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYLEVEL) * DIV127 * 0.5f;
  // 113,3,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsDelayPong()
{
  EffectsDelayPong = (float)(127 - *getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYSPARE)) * DIV127;
  // 113,4,0,127, (using spare cc for pong)
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsReverbSpare()
{
  EffectsReverbSpare = (float)*getFxValue(EFFECTGROUPREVERB, CCEFFECTSREVERBSPARE) * DIV127;
  // 114,1,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsReverbLevel()
{
  EffectsReverbLevel = (float)*getFxValue(EFFECTGROUPREVERB, CCEFFECTSREVERBLEVEL) * DIV127 * maxReverbLevel;
  // 114,1,0,127,
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsReverbDenseEarly()
{
  EffectsReverbDenseEarly = (float)*getFxValue(EFFECTGROUPREVERB, CCEFFECTSREVERBDENSEEARLY) * DIV127 * maxDenseEarly;
  // 114,2,0,127, (using this for roomsize)
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
// EffectsReverbTime is from 1000ms to 5000ms, but the Plate reverb takes 0.0 to 1.0 as inputs, 0.1 to 0.6 is approximately 1sec to 5sec.
void patchToEffectsReverbTime()
{
  EffectsReverbTime = (float)*getFxValue(EFFECTGROUPREVERB, CCEFFECTSREVERBTIME) / 100.0f + 0.1f;
  // 114,3,10,50,//1000 to 5000 ms
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag; // if not called from patchToSynthVariables()
}
void patchToEffectsReverbDamp()
{
  EffectsReverbDamp = ((float)*getFxValue(EFFECTGROUPREVERB, CCEFFECTSREVERBDAMP) - 54.0f) / 20.0f; // 114,4,54,74,//-10 to +10
  updateSynthVariablesFlag = !preUpdateSynthVariablesFlag;                                          // if not called from patchToSynthVariables()
}

void patchToSynthVariables()
{
  // set first to also use as switch in "patchTo<param>()"
  // so we don't update values twice in here.
  preUpdateSynthVariablesFlag = true;
  patchToOctOsc1();              // 64,0,62,66,
  patchToSemiOsc1();             // 64,1,52,76,
  patchToFineOsc1();             // 64,2,14,114, -50 to +50 cents
  patchToBeatOsc1();             // 64,3,0,127,
  patchToSawOsc1();              // 64,5,0,127,
  patchToTriOsc1();              // 64,6,0,127,
  patchToPulseOsc1();            // 64,7,0,127,
  patchToPwOsc1();               // 64,8,0,127,  GUI 0 = 50%
  patchToPwmFreqOsc1();          // 64,9,0,127,
  patchToPwmDepthOsc1();         // 64,10,0,127,
  patchToSweepDepthOsc1();       // 64,11,0,127,
  patchToSweepTimeOsc1();        // 64,12,0,127,
  patchToBreathDepthOsc1();      // 64,13,0,127,
  patchToBreathAttainOsc1();     // 64,14,0,127,
  patchToBreathOscCurveLines1(); // 64,15,0,127,
  patchToBreathThreshOsc1();     // 64,16,0,127,
  patchToLevelOsc1();            // 64,17,0,127,
  patchToOctOsc2();              // 65,0,62,66,
  patchToSemiOsc2();             // 64,1,52,76,
  patchToFineOsc2();             // 64,2,14,114, -50 to +50 cents
  patchToBeatOsc2();             // 64,3,0,127,
  patchToSawOsc2();              // 64,5,0,127,
  patchToTriOsc2();              // 64,6,0,127,
  patchToPulseOsc2();            // 64,7,0,127,
  patchToPwOsc2();               // 64,8,0,127,  GUI 0 = 50%
  patchToPwmFreqOsc2();          // 64,9,0,127,
  patchToPwmDepthOsc2();         // 64,10,0,127,
  patchToSweepDepthOsc2();       // 64,11,0,127,
  patchToSweepTimeOsc2();        // 64,12,0,127,
  patchToBreathDepthOsc2();      // 64,13,0,127,
  patchToBreathAttainOsc2();     // 64,14,0,127,
  patchToBreathOscCurveLines2(); // 64,15,0,127,
  patchToBreathThreshOsc2();     // 64,16,0,127,
  patchToLevelOsc2();            // 64,17,0,127,
  patchToModeOscFilter1();       // 72,0,0,4,//LP HP BP NTC OFF
  patchToFreqOscFilter1();
  patchToQFactorOscFilter1();        // 72,2,5,127,"// 5=0.5, 127=12.7"
  patchToKeyFollowOscFilter1();      // 72,3,52,88,// -12 to +24 num semi tones
  patchToBreathModOscFilter1();      // 72,4,0,127,
  patchToLfoFreqOscFilter1();        // 72,5,0,127,
  patchToLfoDepthOscFilter1();       // 72,6,0,127,
  patchToLfoBreathOscFilter1();      // 72,7,0,127,
  patchToLfoThreshOscFilter1();      // 72,8,0,127,
  patchToSweepDepthOscFilter1();     // 72,9,0,127,
  patchToSweepTimeOscFilter1();      // 72,10,0,127,
  patchToBreathOscFiltCurveLines1(); // 72,11,0,127;
  patchToModeOscFilter2();           // 73,0,0,4,//LP HP BP NTC OFF
  patchToFreqOscFilter2();
  patchToQFactorOscFilter2();        // 72,2,5,127,"// 5=0.5, 127=12.7"
  patchToKeyFollowOscFilter2();      // 72,3,52,88,// -12 to +24 num semi tones
  patchToBreathModOscFilter2();      // 72,4,0,127,
  patchToLfoFreqOscFilter2();        // 72,5,0,127,
  patchToLfoDepthOscFilter2();       // 72,6,0,127,
  patchToLfoBreathOscFilter2();      // 72,7,0,127,
  patchToLfoThreshOscFilter2();      // 72,8,0,127,
  patchToSweepDepthOscFilter2();     // 72,9,0,127,
  patchToSweepTimeOscFilter2();      // 72,10,0,127,
  patchToBreathOscFiltCurveLines2(); // 72,11,0,127,
  patchToModeNoiseFilter3();         // 74,0,0,4,//LP HP BP NTC OFF
  patchToFreqNoiseFilter3();
  patchToQFactorNoiseFilter3();    // 74,2,5,127,"// 5=0.5, 127=12.7"
  patchToKeyFollowNoiseFilter3();  // 74,3,52,88,// -12 to +24 num semitones
  patchToBreathModNoiseFilter3();  // 74,4,0,127,
  patchToLfoFreqNoiseFilter3();    // 74,5,0,127,
  patchToLfoDepthNoiseFilter3();   // 74,6,0,127,
  patchToLfoBreathNoiseFilter3();  // 74,7,0,127,
  patchToLfoThreshNoiseFilter3();  // 74,8,0,127,
  patchToSweepDepthNoiseFilter3(); // 74,9,0,127,
  patchToSweepTimeNoiseFilter3();  // 74,10,0,127,
  patchToBreathNoiseFiltCurveLines3();
  patchToModeNoiseFilter4(); // 74,0,0,4,//LP HP BP NTC OFF
  patchToFreqNoiseFilter4();
  patchToQFactorNoiseFilter4();        // 74,2,5,127,"// 5=0.5, 127=12.7"
  patchToKeyFollowNoiseFilter4();      // 74,3,52,88,// -12 to +24 num semitones
  patchToBreathModNoiseFilter4();      // 74,4,0,127,
  patchToLfoFreqNoiseFilter4();        // 74,5,0,127,
  patchToLfoDepthNoiseFilter4();       // 74,6,0,127,
  patchToLfoBreathNoiseFilter4();      // 74,7,0,127,
  patchToLfoThreshNoiseFilter4();      // 74,8,0,127,
  patchToSweepDepthNoiseFilter4();     // 74,9,0,127,
  patchToSweepTimeNoiseFilter4();      // 74,10,0,127,
  patchToBreathNoiseFiltCurveLines4(); // 74,11,0,127,
  patchToNoiseTime();                  // 80,0,0,127,
  patchToNoiseBreathCurveLines();      // 80,1,0,127,
  patchToNoiseLevel();                 // 80,2,0,127,
  patchToBendRange();                  // 81,0,0,12,// num semitones
  patchToBendStep();                   // 81,1,0,1,//0=off 1=on
  // patchToVibratoPitch( );  //81,2,0,127,  (this isn't necessary since bite sensor pb and plate pb are the same).
  patchToLinkOscFilters();   // 81,3,1,2,//1=Link on 2= Link off
  patchToLinkNoiseFilters(); // 81,4,1,2,
  patchToFormant();          // 81,5,0,2,//0=off 1=W.Wind 2=String
  patchToXFade();            // 81,6,0,1,//0=off 1=on
  patchToKeyTriggerSingle(); // 81,7,0,1,//0=Multi 1=Single
  patchTos81_8();            // 81,8,0,1,// ?
  patchToChorusOn();         // 81,9,0,1,// Chorus on off
  patchToVibratoAmp();       // 88,0,0,127, (bite tremelo amount)
  patchToAmpLevel();         // 88,1,0,127,
  // patchToOctButtonLevel();   // 88,2,0,127,
  patchToEffectsChorusDelay1();
  patchToEffectsChorusMod1();       // 112,1,0,127, (-50% to +50%)
  patchToEffectsChorusWet1();       // 112,2,0,127, neg vals are phase inverted
  patchToEffectsChorusDelay2();     // 112,3,0,127,
  patchToEffectsChorusMod2();       // 112,4,0,127,
  patchToEffectsChorusWet2();       // 112,5,0,127,
  patchToEffectsChorusFeedback();   // 112,6,0,127,
  patchToEffectsChorusLfoFreq();    // 112,7,0,127,
  patchToEffectsChorusDryLevel();   // 112,8,0,127,
  patchToEffectsDelayTimeL();       // 113,0,0,127, 0 to 1270 ms
  patchToEffectsDelayFeedback();    // 113,1,0,127,
  patchToEffectsDelayDamp();        // 113,2,0,127,
  patchToEffectsDelayLevel();       // 113,3,0,127,
  patchToEffectsDelayPong();        // 113,3,0,127,
  patchToEffectsReverbSpare();      // 114,1,0,127,
  patchToEffectsReverbLevel();      // 114,1,0,127,
  patchToEffectsReverbDenseEarly(); // 114,2,0,127, (using this for roomsize)
  patchToEffectsReverbTime();       // 114,3,10,50,//1000 to 5000 ms
  patchToEffectsReverbDamp();       // 114,4,54,74,//-10 to +10
  // updateSynthVariablesFlag = true;
}

#ifdef USEALL_PATCHFUNCS
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
  sprintf(str_buf1, "EffectsDelayPong: %5.3f", EffectsDelayPong);
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
#endif

void setCurrentPatch(int patchNumber)
{
  if (!patchLoaded[patchNumber])
  {
    sprintf(str_buf1, "Patch: %03d not loaded. Can't copy", patchNumber);
    Serial8.println(str_buf1);
    return;
  }
  // sprintf(str_buf1, "current_patch set to: %03d ", patchNumber);
  // Serial8.println(str_buf1);
  memcpy((byte *)&current_patch, (byte *)&loadedPatches[patchNumber], sizeof(current_patch));
}

void copyCurrentPatchToLoadedPatch(int patchNumber)
{
  if (!patchLoaded[patchNumber])
  {
    sprintf(str_buf1, "Patch: %03d not loaded. Can't copy", patchNumber);
    Serial8.println(str_buf1);
    return;
  }
  memcpy((byte *)&loadedPatches[patchNumber], (byte *)&current_patch, sizeof(loadedPatches[patchNumber]));
}

void copyPatchBuffToPatchBuff(patch_t *destPatch, patch_t *sourcePatch)
{
  memcpy((byte *)destPatch, (byte *)sourcePatch, sizeof(*destPatch));
}

#ifdef USEALL_PATCHFUNCS
//---------------------
// SD card access
//---------------------
#endif
#ifdef USELOAD_PATCHFUNCS
void configureSD()
{
  char str_buf_sd[16];
  if (!(SD.begin(BUILTIN_SDCARD))) // chipSelect = built-in uses default?
  {
    Serial8.println("Unable to access the SD card");
    //    loadPatchEEPROM();
  }
  for (int i = 0; i < NUMBER_OF_PATCHES; i++)
  {
    sprintf(str_buf_sd, "%03d.PAT", i);
    if (SD.exists(str_buf_sd))
    {
      dataFile = SD.open(str_buf_sd);
      dataFile.read(&loadedPatches[i], sizeof(loadedPatches[0]));
      dataFile.close();
      patchLoaded[i] = true;
    }
    else
    {
      patchLoaded[i] = false;
    }
  }
}

void savePatchSD(int i)
{
  char str_buf_sd[16];
  // loadedPatches[i] = createPatch();
  if (!patchLoaded[i])
  {
    sprintf(str_buf1, "Patch: %03d not loaded. Nothing to save.", i);
    Serial8.println(str_buf1);
    return;
  }
  sprintf(str_buf_sd, "%03d.PAT", i);
  if (SD.exists(str_buf_sd))
  {
    SD.remove(str_buf_sd);
  }
  dataFile = SD.open(str_buf_sd, FILE_WRITE);
  if (!dataFile)
  {
    Serial8.println("Could not create file");
    // TODO:  hook this up:  wavplayer.play("err.wav");
  }
  else
  {
    Serial8.print("size: ");
    Serial8.print(sizeof(loadedPatches[0]));
    Serial8.println();
    dataFile.write((byte *)&loadedPatches[i], sizeof(loadedPatches[0]));
    //        Serial8.write((byte*)&loadedPatches[i], sizeof(loadedPatches[0]));
    dataFile.close();
    Serial8.print("Written");
  }
}

void saveGlobalFxSD()
{
  char str_buf_sd[16];
  if (!globalFxLoaded)
  {
    sprintf(str_buf1, "Fx: %03d not loaded. Nothing to save.", 0);
    Serial8.println(str_buf1);
    return;
  }
  sprintf(str_buf_sd, "%03d.FX", 0);
  if (SD.exists(str_buf_sd))
  {
    SD.remove(str_buf_sd);
  }
  dataFile = SD.open(str_buf_sd, FILE_WRITE);
  if (!dataFile)
  {
    Serial8.println("Could not create file");
    // TODO:  hook this up:  wavplayer.play("err.wav");
  }
  else
  {
    copyCurrentFxToCopyBufferFx();
    Serial8.print("size: ");
    Serial8.print(sizeof(copy_buffer_fx));
    Serial8.println();
    dataFile.write((byte *)&copy_buffer_fx, sizeof(copy_buffer_fx));
    dataFile.close();
    Serial8.print("Written");
    loadGlobalFxSD();
  }
}

void saveCurrentPatchSD(int i)
{ // i is patch number to save it
  char str_buf_sd[16];
  // loadedPatches[i] = createPatch();
  if (!patchLoaded[i])
  {
    sprintf(str_buf1, "Patch: %03d not loaded. can't upsate.", i);
    Serial8.println(str_buf1);
    return;
  }
  sprintf(str_buf_sd, "%03d.PAT", i);
  if (SD.exists(str_buf_sd))
  {
    SD.remove(str_buf_sd);
  }
  dataFile = SD.open(str_buf_sd, FILE_WRITE);
  if (!dataFile)
  {
    Serial8.println("Could not create file");
    // TODO:  hook this up:  wavplayer.play("err.wav");
  }
  else
  {
    Serial8.print("size: ");
    Serial8.print(sizeof(loadedPatches[0]));
    Serial8.println();
    dataFile.write((byte *)&current_patch, sizeof(loadedPatches[0]));
    dataFile.close();
    sprintf(str_buf1, "current_patch written into %03d.PAT", i);
    copyCurrentPatchToLoadedPatch(i);
    Serial8.println(str_buf1);
  }
}

void saveCoppiedPatchSD(int i)
{ // i is patch number to save it
  char str_buf_sd[16];
  // loadedPatches[i] = createPatch();
  if (!patchLoaded[i])
  {
    sprintf(str_buf1, "Patch: %03d not loaded. can't update.", i);
    Serial8.println(str_buf1);
    return;
  }
  sprintf(str_buf_sd, "%03d.PAT", i);
  if (SD.exists(str_buf_sd))
  {
    SD.remove(str_buf_sd);
  }
  dataFile = SD.open(str_buf_sd, FILE_WRITE);
  if (!dataFile)
  {
    Serial8.println("Could not create file");
    // TODO:  hook this up:  wavplayer.play("err.wav");
  }
  else
  {
    Serial8.print("size: ");
    Serial8.print(sizeof(loadedPatches[0]));
    Serial8.println();
    dataFile.write((byte *)&copy_buffer_patch, sizeof(loadedPatches[0]));
    dataFile.close();
    sprintf(str_buf1, "copy_buffer_patch written into %03d.PAT", i);
    // copyPatchBuffToPatchBuff(&current_patch, &copy_buffer_patch);
    // void setCurrentPatch(int patchNumber)
    Serial8.println(str_buf1);
  }
}

void saveSwappedPatchSD(int i)
{ // i is patch number to save it
  char str_buf_sd[16];
  // loadedPatches[i] = createPatch();
  if (!patchLoaded[i])
  {
    sprintf(str_buf1, "Patch: %03d not loaded. can't update.", i);
    Serial8.println(str_buf1);
    return;
  }
  sprintf(str_buf_sd, "%03d.PAT", i);
  if (SD.exists(str_buf_sd))
  {
    SD.remove(str_buf_sd);
  }
  dataFile = SD.open(str_buf_sd, FILE_WRITE);
  if (!dataFile)
  {
    Serial8.println("Could not create file");
    // TODO:  hook this up:  wavplayer.play("err.wav");
  }
  else
  {
    Serial8.print("size: ");
    Serial8.print(sizeof(loadedPatches[0]));
    Serial8.println();
    dataFile.write((byte *)&swap_buffer_patch, sizeof(loadedPatches[0]));
    dataFile.close();
    sprintf(str_buf1, "copy_buffer_patch written into %03d.PAT", i);
    copyPatchBuffToPatchBuff(&current_patch, &swap_buffer_patch);
    Serial8.println(str_buf1);
  }
}
#endif

void copyLoadedPatchToCopyBuffer(int sourcePatchNumber)
{
  if (!patchLoaded[sourcePatchNumber])
  {
    sprintf(str_buf1, "Patch: %03d not loaded. Can't copy", sourcePatchNumber);
    Serial8.println(str_buf1);
    return;
  }
  memcpy((byte *)&copy_buffer_patch, (byte *)&loadedPatches[sourcePatchNumber], sizeof(copy_buffer_patch));
}
void copyLoadedPatchToSwapBuffer(int sourcePatchNumber)
{
  if (!patchLoaded[sourcePatchNumber])
  {
    sprintf(str_buf1, "Patch: %03d not loaded. Can't copy", sourcePatchNumber);
    Serial8.println(str_buf1);
    return;
  }
  memcpy((byte *)&swap_buffer_patch, (byte *)&loadedPatches[sourcePatchNumber], sizeof(swap_buffer_patch));
}
void copyCurrentPatchToCopyBuffer(void)
{
  memcpy((byte *)&copy_buffer_patch, (byte *)&current_patch, sizeof(copy_buffer_patch));
}
void copyCurrentFxToCopyBufferFx(void)
{
  memcpy((byte *)&copy_buffer_fx.nrpn_msb_chorus[0], (byte *)&current_patch.nrpn_msb_chorus[0],
         NRPN_MSB_CHORUS_LENGTH + NRPN_MSB_DELAY_LENGTH + NRPN_MSB_REVERB_LENGTH);
  memcpy((byte *)&copy_buffer_fx.nrpn_msb_common1[0], (byte *)&current_patch.nrpn_msb_common1[0],
         NRPN_MSB_COMMON1_LENGTH);
}
void copyCopyBufferFxToPatch(int patchNumber)
{
  memcpy((byte *)&loadedPatches[patchNumber].nrpn_msb_chorus[0],
         (byte *)&copy_buffer_fx.nrpn_msb_chorus[0],
         NRPN_MSB_CHORUS_LENGTH + NRPN_MSB_DELAY_LENGTH + NRPN_MSB_REVERB_LENGTH);
  memcpy((byte *)&loadedPatches[patchNumber].nrpn_msb_common1[0],
         (byte *)&copy_buffer_fx.nrpn_msb_common1[0],
         NRPN_MSB_COMMON1_LENGTH);
}

#ifdef USELOAD_PATCHFUNCS
void loadPatchSD(int i)
{
  char str_buf_sd[16];
  //  if(patchLoaded[i]) {
  //    Serial8.println("patch alreadey loaded");
  //    setCurrentPatch(i);
  //    Serial8.println("setCurrentPatch");
  //    patchToSynthVariables(&current_patch);
  //    Serial8.println("patchToSynthVariables");
  //} else {
  sprintf(str_buf_sd, "%03d.PAT", i);
  dataFile = SD.open(str_buf_sd);
  if (!dataFile)
  {
    Serial8.println("Could not load file");
    // TODO: hook this up wavplayer.play("err.wav");
  }
  else
  {
    int bytes = dataFile.read(&loadedPatches[i], sizeof(loadedPatches[0]));
    dataFile.close();
    Serial8.print("Loaded:");
    Serial8.println(bytes, DEC);
    patchLoaded[i] = true;
    setCurrentPatch(i);
    // patchToSynthVariables(&current_patch);
    patchToSynthVariables();
  }
  //}
}

void loadAllPatches()
{
  char str_buf_sd[16];
  int n_loaded = 0;
  for (int i = 0; i < NUMBER_OF_PATCHES; i++)
  {
    sprintf(str_buf_sd, "%03d.PAT", i);
    if (SD.exists(str_buf_sd))
    {
      dataFile = SD.open(str_buf_sd);
      dataFile.read(&loadedPatches[i], sizeof(loadedPatches[0]));
      dataFile.close();
      patchLoaded[i] = true;
      n_loaded++;
    }
    else
    {
      sprintf(str_buf1, "Patch: %03d does not exist. create one from copy_buffer_patch", i);
      Serial8.println(str_buf1);
      dataFile = SD.open(str_buf_sd, FILE_WRITE);
      if (!dataFile)
      {
        Serial8.println("Could not create file");
        // TODO:  hook this up:  wavplayer.play("err.wav");
        patchLoaded[i] = false;
      }
      else
      {
        Serial8.print("size: ");
        Serial8.print(sizeof(loadedPatches[0]));
        Serial8.println();
        dataFile.write((byte *)&copy_buffer_patch, sizeof(loadedPatches[0]));
        dataFile.close();
        sprintf(str_buf1, "copy_buffer_patch written into %03d.PAT", i);
        copyPatchBuffToPatchBuff(&loadedPatches[i], &copy_buffer_patch);
        patchLoaded[i] = true;
        Serial8.println(str_buf1);
      }
    }
  }
  sprintf(str_buf1, "loaded %03d patches.", n_loaded);
  Serial8.println(str_buf1);
}

void loadGlobalFxSD()
{
  char str_buf_sd[16];
  sprintf(str_buf_sd, "%03d.FX", 0); // 0.FX is global fx
  if (SD.exists(str_buf_sd))
  {
    dataFile = SD.open(str_buf_sd);
    dataFile.read((byte *)&global_buffer_fx, sizeof(global_buffer_fx));
    dataFile.close();
    globalFxLoaded = true;
  }
  else
  {
    sprintf(str_buf1, "Fx: %03d does not exist. create one from global_buffer_fx", 0);
    Serial8.println(str_buf1);
    dataFile = SD.open(str_buf_sd, FILE_WRITE);
    if (!dataFile)
    {
      Serial8.println("Could not create file");
      // TODO:  hook this up:  wavplayer.play("err.wav");
      globalFxLoaded = false;
    }
    else
    {
      Serial8.print("size: ");
      Serial8.print(sizeof(global_buffer_fx));
      Serial8.println();
      dataFile.write((byte *)&global_buffer_fx, sizeof(global_buffer_fx));
      dataFile.close();
      sprintf(str_buf1, "global_buffer_fx written into %03d.FX", 0);
      globalFxLoaded = true;
      Serial8.println(str_buf1);
    }
  }
}
#endif

float noiseTimeCurve(float noiseTimePercent)
{
  float a = 0.0f;
  float b = 1.0f;
  if (noiseTimePercent <= 0.0f)
  {
    a = 0.0f;
    b = 0.0f;
  }
  else if (noiseTimePercent < 0.7f)
  {
    a = 0.0564f * 1000.0f; // 1000 for time in mS
    b = 2.75f;
  }
  else if (noiseTimePercent < 0.9f)
  {
    a = 0.0105f * 1000.0f; // 1000 for time in mS
    b = 5.2f;
  }
  else
  {
    a = 0.0001f * 1000.0f; // 1000 for time in mS
    b = 10.4f;
  }
  float y = a * exp(b * noiseTimePercent);
  return y;
}

float sweepTimeFilterCurve(float sweepTime)
{
  float a = 0.0f;
  float b = 1.0f;
  if (sweepTime <= 0.0f)
  {
    // a = 0.0f;
    a = 10.0f; // minumum time is 10mS
    b = 0.0f;
  }
  else if (sweepTime < 0.7f)
  {
    a = 0.0103f * 1000.0f; // 1000 for time in mS
    b = 4.1647f;
  }
  else if (sweepTime < 0.9f)
  {
    a = 0.0021f * 1000.0f; // 1000 for time in mS
    b = 6.4453f;
  }
  else
  {
    a = 0.0000003f * 1000.0f; // 1000 for time in mS
    b = 16.27f;
  }
  float y = a * exp(b * sweepTime);
  return y;
}

float sweepTimeOscCurve(float sweepTime)
{
  float a = 0.0f;
  float b = 1.0f;
  if (sweepTime <= 0.0f)
  {
    // a = 0.0f;
    a = 30.0f; // minumum time is 30ms
    b = 0.0f;
  }
  else if (sweepTime < 0.7f)
  {
    a = 0.035f * 1000.0f; // 1000 for time in mS
    b = 3.25f;
  }
  else if (sweepTime < 0.9f)
  {
    a = 0.0040f * 1000.0f; // 1000 for time in mS
    b = 6.38f;
  }
  else
  {
    a = 0.00005f * 1000.0f; // 1000 for time in mS
    b = 11.2f;
  }
  float y = a * exp(b * sweepTime);
  return y;
}

void patchSelect()
{
  current_patchNumber = programChangeData % NUMBER_OF_PATCHES;
  // load the patch
  if (patchLoaded[current_patchNumber])
  {
    setCurrentPatch(current_patchNumber);
    patchToSynthVariables();
  }
  else
  {
    loadPatchSD(current_patchNumber); // also does setCurrentPatch and patchToSynthVariables
  }
}

void patchSelectFromMenu()
{
  if (patchLoaded[current_patchNumber])
  {
    setCurrentPatch(current_patchNumber);
    patchToSynthVariables();
  }
  else
  {
    loadPatchSD(current_patchNumber); // also does setCurrentPatch and patchToSynthVariables
  }
}

void pasteDestSelect()
{
  if (!patchLoaded[paste_patchNumber])
  {
    return;
  }
  if (paste_patchNumber != paste_patchNumber_prev)
  {
    namingFinishedFlag = false;
    paste_patchNumber_prev = paste_patchNumber;
  }
  if (!namingFinishedFlag)
    strcpy(namebuf, loadedPatches[paste_patchNumber].patch_string);
}

void swapDestSelect()
{
  if (!patchLoaded[swap_patchNumber])
  {
    return;
  }
  if (swap_patchNumber != swap_patchNumber_prev)
  {
    namingFinishedFlag = false;
    swap_patchNumber_prev = swap_patchNumber;
  }
  if (!namingFinishedFlag)
    strcpy(namebuf, loadedPatches[swap_patchNumber].patch_string);
}

uint8_t *getFxValue(uint32_t effectGroup, uint32_t effectIdx)
{
  sprintf(str_buf1, "fxSourcPatch: %d", fxSourcePatch);
  if (fxSourcePatch)
  {
    switch (effectGroup)
    {
    case EFFECTGROUPCHORUS:
      return &current_patch.nrpn_msb_chorus[effectIdx];
    case EFFECTGROUPREVERB:
      return &current_patch.nrpn_msb_reverb[effectIdx];
    case EFFECTGROUPDELAY:
      return &current_patch.nrpn_msb_delay[effectIdx];
    case EFFECTGROUPCOMMON1:
      return &current_patch.nrpn_msb_common1[effectIdx];
    default:
      return nullptr;
      sprintf(str_buf1, "invalid Patch effectGroup: %lu", effectGroup);
    }
  }
  else
  {
    switch (effectGroup)
    {
    case EFFECTGROUPCHORUS:
      return &global_buffer_fx.nrpn_msb_chorus[effectIdx];
    case EFFECTGROUPREVERB:
      return &global_buffer_fx.nrpn_msb_reverb[effectIdx];
    case EFFECTGROUPDELAY:
      return &global_buffer_fx.nrpn_msb_delay[effectIdx];
    case EFFECTGROUPCOMMON1:
      return &global_buffer_fx.nrpn_msb_common1[effectIdx];
    default:
      sprintf(str_buf1, "invalid Global effectGroup: %lu", effectGroup);
      return nullptr;
    }
  }

  Serial8.println(str_buf1);
  return nullptr; // should never get here.
}
void setFxValue(uint8_t value, patch_t *patch, uint32_t effectGroup, uint32_t effectIdx)
{
  if (fxSourcePatch)
  {
    switch (effectGroup)
    {
    case EFFECTGROUPCHORUS:
      patch->nrpn_msb_chorus[effectIdx] = value;
    case EFFECTGROUPREVERB:
      patch->nrpn_msb_reverb[effectIdx] = value;
    case EFFECTGROUPDELAY:
      patch->nrpn_msb_delay[effectIdx] = value;
    case EFFECTGROUPCOMMON1:
      patch->nrpn_msb_common1[effectIdx] = value;
    }
  }
  else
  {
    switch (effectGroup)
    {
    case EFFECTGROUPCHORUS:
      global_buffer_fx.nrpn_msb_chorus[effectIdx] = value;
    case EFFECTGROUPREVERB:
      global_buffer_fx.nrpn_msb_reverb[effectIdx] = value;
    case EFFECTGROUPDELAY:
      global_buffer_fx.nrpn_msb_delay[effectIdx] = value;
    case EFFECTGROUPCOMMON1:
      global_buffer_fx.nrpn_msb_common1[effectIdx] = value;
    }
  }
}

template <typename T>
T wrap(T value, T min, T max)
{
  const T range = max - min + 1;
  if (value < min)
  {
    value += range * ((min - value + range - 1) / range);
  }
  else if (value > max)
  {
    value -= range * ((value - max + range - 1) / range);
  }
  return value;
}
