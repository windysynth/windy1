#include <algorithm>
#include "OledMenu.h"
#include "globals.h"
#include "MenuItemsListsFuncs.h"
#include "patches.h"

extern AudioEffectEnvelope      env_squelch;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

/*****************************
 ******************************
 * Setup the menu as an array of MenuItem
 * Create a MenuList and an instance of your 
 * menu class
 ******************************
 *****************************/

MenuItem PROGMEM topMenu[18] = {
    { "VolAdj  " , gotoVolAdjMenu   }
  , { "PatchSel" , gotoPatchSelectMenu   }
  , { "PatchRst" , gotoPatchResetMenu   }
  , { "PatchCpy" , gotoPatchCopyMenu   }
  , { "PatchPst" , gotoPatchPasteMenu   }
  , { "PatchSwp" , gotoPatchSwapMenu   }
  , { "PatchFx " , gotoPatchFxMenu   }
  , { "Osc1    " , gotoPatchOsc1Menu   }
  , { "Osc2    " , gotoPatchOsc2Menu   }
  , { "OscFilt1" , gotoPatchOscFilter1Menu   }
  , { "OscFilt2" , gotoPatchOscFilter2Menu   }
  , { "Nz Filt1" , gotoPatchNoiseFilter1Menu   }
  , { "Nz Filt2" , gotoPatchNoiseFilter2Menu   }
  , { "Noise   " , gotoPatchNoiseMenu   }
  , { "Formant " , gotoPatchFormantMenu   }
  , { "Amp     " , gotoPatchAmpMenu   }
  , { "P Common" , gotoPatchCommonMenu   }
  , { "System  " , gotoSystemAdjMenu   }
};

MenuItem PROGMEM volAdjustMenu[1] = { 
    { "Volume:\n ", volAdjustFun    }
};
bool volAdjustFun() {
 if(myMenu.updateLeafValue){
    vol += myMenu.updateLeafValue;
    vol = vol < 0 ? 0 : vol >= 100 ? 100 : vol;
    volf = ((float)vol)/100.0f;
    volf = (volf*volf)*2.0f;
    //preUpdateSynthVariablesFlag = true; // squelch while change vol 
    sprintf(myMenu.str_oledbuf,"  %d   ", vol);
    Serial8.println(F("volAdjustFun: updated vol "));
    return true;
 }
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  Serial8.println(F("volAdjustFun: goto TopMenu"));
  //myMenu.setCurrentMenu(&listTopMenu);
  gotoTopMenu();
  return true;
}
MenuItem PROGMEM patchSelectMenu[1] = {
    { "Patch:\n " , patchSelectFun   }
};
// patchSelectMenu function
bool patchSelectFun() {
 if(myMenu.updateLeafValue){
    current_patchNumber += myMenu.updateLeafValue;
    if(current_patchNumber > NUMBER_OF_PATCHES-1 ){
        current_patchNumber = 0; 
    }
    else if(current_patchNumber < 0 ) {
        current_patchNumber = NUMBER_OF_PATCHES-1; 
    } 
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
    ps.setCharAt( ps.indexOf(' '), '\n'); // TODO: spaces till end of line then \n
    sprintf(myMenu.str_oledbuf, "%03d\n%s", current_patchNumber+1, ps.c_str() );
    Serial8.println(current_patch.patch_string);

    if (updateEpromFlag)
    {
        eepromCurrentMillis = millis();
        eepromPreviousMillis = eepromCurrentMillis; // reset timer every knob turn 
    }
    return true;
 }
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  Serial8.println(F("patchSelectFun: goto TopMenu"));
  //myMenu.setCurrentMenu(&listTopMenu);
  gotoTopMenu();
  return true;
}
MenuItem PROGMEM patchResetMenu[2] = {
    { "Back    " , goUpOneMenu   }
   ,{ "Reset   " , patchResetFun   }
};
bool patchResetFun(){ return goUpOneMenu(); }
MenuItem PROGMEM patchCopyMenu[2] = {
    { "Back    " , goUpOneMenu   }
   ,{ "Copy    " , patchCopyFun   }
};
bool patchCopyFun(){ return goUpOneMenu(); }
MenuItem PROGMEM patchPasteMenu[2] = {
    { "Back    " , goUpOneMenu   }
   ,{ "Paste   " , patchPasteFun   }
};
bool patchPasteFun(){ return goUpOneMenu(); }
MenuItem PROGMEM patchSwapMenu[2] = {
    { "Back:\n " , goUpOneMenu   }
   ,{ "Swap:\n " , patchSwapFun   }
};
bool patchSwapFun(){ return goUpOneMenu(); }

MenuItem PROGMEM patchFxMenu[23] = {
    { "Back    " , goUpOneMenu   }
   ,{ "FxCopy  " , gotoFxCopyMenu   }
   ,{ "FxPaste " , gotoFxPasteMenu   }
   ,{ "FxSwap  " , gotoFxSwapMenu   }
   ,{ "DlyLevel" , gotoDelayLevelMenu   }
   ,{ "DlyTimeL" , gotoDelayTimeLMenu   }
   ,{ "DlyTimeR" , gotoDelayTimeRMenu   }
   ,{ "DlyFB   " , gotoDelayFeedbackMenu   }
   ,{ "DlyDamp " , gotoDelayDampMenu   }
   ,{ "RvbLevel" , gotoReverbLevelMenu   }
   ,{ "RvbTime " , gotoReverbTimeMenu   }
   ,{ "RvbDens " , gotoReverbDensityMenu   }
   ,{ "RvbDamp " , gotoReverbDampMenu   }
   ,{ "ChrsOn  " , gotoChorusOnMenu   }
   ,{ "ChrsDry " , gotoChorusDryMenu   }
   ,{ "ChrsLfoF" , gotoChorusLfoFMenu   }
   ,{ "ChrsFB  " , gotoChorusFeedbackMenu   }
   ,{ "ChrsDly1" , gotoChorusDelay1Menu   }
   ,{ "ChrsMod1" , gotoChorusMod1Menu   }
   ,{ "ChrsWet1" , gotoChorusWet1Menu   }
   ,{ "ChrsDly2" , gotoChorusDelay2Menu   }
   ,{ "ChrsMod2" , gotoChorusMod2Menu   }
   ,{ "ChrsWet2" , gotoChorusWet2Menu   }
};

MenuItem PROGMEM fxCopyMenu[2] = {
    { "Back    " , goUpOneMenu   }
   ,{ "CopyFx  " , fxCopyFun   }
};
bool fxCopyFun(){ return goUpOneMenu(); }
MenuItem PROGMEM fxPasteMenu[2] = {
    { "Back    " , goUpOneMenu   }
   ,{ "PasteFx " , fxPasteFun   }
};
bool fxPasteFun(){ return goUpOneMenu(); }
MenuItem PROGMEM fxSwapMenu[2] = {
    { "Back:\n " , goUpOneMenu   }
   ,{ "SwapFx:\n " , fxSwapFun   }
};
bool fxSwapFun(){ return goUpOneMenu(); }
MenuItem PROGMEM delayLevelMenu[1] = {
    { "Dly Lev:\n " , delayLevelFun   }
};
bool delayLevelFun(){ return goUpOneMenu(); }
MenuItem PROGMEM delayTimeLMenu[1] = {
    { "DlyTimeL\n " , delayTimeLFun   }
};
bool delayTimeLFun(){ 
 if(myMenu.updateLeafValue){
     int delayTimeTemp = current_patch.nrpn_msb_delay[CCEFFECTSDELAYTIME];
     delayTimeTemp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_delay[CCEFFECTSDELAYTIME] = std::clamp(delayTimeTemp,0, 127);
     patchToSynthVariables(&current_patch);
     sprintf(myMenu.str_oledbuf,"  %04d    ", 10*current_patch.nrpn_msb_delay[CCEFFECTSDELAYTIME]);
     return true;
 }
 Serial8.println(F("delayLevelLFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM delayTimeRMenu[1] = {
    { "DlyTimeR\n " , delayTimeRFun   }
};
bool delayTimeRFun(){ 
     if(myMenu.updateLeafValue){
         return true;
     }
    Serial8.println(F("delayLevelRFun calls goUpOneMenu"));
    return goUpOneMenu(); 
}
MenuItem PROGMEM delayFeedbackMenu[1] = {
    { "Dly FB: \n " , delayFeedbackFun   }
};
bool delayFeedbackFun(){ return goUpOneMenu(); }
MenuItem PROGMEM delayDampMenu[1] = {
    { "DlyDamp:\n " , delayDampFun   }
};
bool delayDampFun(){ return goUpOneMenu(); }
MenuItem PROGMEM reverbLevelMenu[1] = {
    { "Rvb Lev:\n " , reverbLevelFun   }
};
bool reverbLevelFun(){ return goUpOneMenu(); }
MenuItem PROGMEM reverbTimeMenu[1] = {
    { "RvbTime:\n " , reverbTimeFun   }
};
bool reverbTimeFun(){ return goUpOneMenu(); }
MenuItem PROGMEM reverbDensityMenu[1] = {
    { "RvbDens:\n " , reverbDensityFun   }
};
bool reverbDensityFun(){ return goUpOneMenu(); }
MenuItem PROGMEM reverbDampMenu[1] = {
    { "RvbDamp:\n " , reverbDampFun   }
};
bool reverbDampFun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusOnMenu[1] = {
    { "Chrs On:\n " , chorusOnFun   }
};
bool chorusOnFun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusDryMenu[1] = {
    { "ChrsDry:\n " , chorusDryFun   }
};
bool chorusDryFun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusLfoFMenu[1] = {
    { "ChsLfoF:\n " , chorusLfoFFun   }
};
bool chorusLfoFFun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusFeedbackMenu[1] = {
    { "Chrs FB:\n " , chorusFeedbackFun   }
};
bool chorusFeedbackFun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusDelay1Menu[1] = {
    { "ChsDly1:\n " , chorusDelay1Fun   }
};
bool chorusDelay1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusMod1Menu[1] = {
    { "ChsMod1:\n " , chorusMod1Fun   }
};
bool chorusMod1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusWet1Menu[1] = {
    { "ChsWet1:\n " , chorusWet1Fun   }
};
bool chorusWet1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusDelay2Menu[1] = {
    { "ChsDly2:\n " , chorusDelay2Fun   }
};
bool chorusDelay2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusMod2Menu[1] = {
    { "ChsMod2:\n " , chorusMod2Fun   }
};
bool chorusMod2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM chorusWet2Menu[1] = {
    { "ChsWet2:\n " , chorusWet2Fun   }
};
bool chorusWet2Fun(){ return goUpOneMenu(); }

MenuItem PROGMEM patchOsc1Menu[18] = {
    { "Back    " , goUpOneMenu   }
   ,{ "OctOsc1 " , gotoOctOsc1Menu   }
   ,{ "SemiOsc1" , gotoSemiOsc1Menu   }
   ,{ "CentOsc1" , gotoCentsOsc1Menu   }
   ,{ "BeatOsc1" , gotoBeatOsc1Menu   }
   ,{ "SawOsc1 " , gotoSawOsc1Menu   }
   ,{ "TriOsc1 " , gotoTriOsc1Menu   }
   ,{ "PulsOsc1" , gotoPulseOsc1Menu   }
   ,{ "PWOsc1  " , gotoPWOsc1Menu   }
   ,{ "PwmFOsc1" , gotoPwmFreqOsc1Menu   }
   ,{ "PwmDOsc1" , gotoPwmDepthOsc1Menu   }
   ,{ "SwpTOsc1" , gotoSweepTimeOsc1Menu   }
   ,{ "SwpDOsc1" , gotoSweepDepthOsc1Menu   }
   ,{ "BAtnOsc1" , gotoBreathAttainOsc1Menu   }
   ,{ "BDptOsc1" , gotoBreathDepthOsc1Menu   }
   ,{ "BThrOsc1" , gotoBreathThresholdOsc1Menu   }
   ,{ "BCrvOsc1" , gotoBreathCurveOsc1Menu   }
   ,{ "LevlOsc1" , gotoLevelOsc1Menu   }
};

MenuItem PROGMEM octOsc1Menu[1] = {
    { "OctOsc1:\n " , octOsc1Fun   }  // TODO: 
};
bool octOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM semiOsc1Menu[1] = {
    { "SemiOs1:\n " , semiOsc1Fun   }
};
bool semiOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM centsOsc1Menu[1] = {
    { "CentOs1:\n " , centsOsc1Fun   }
};
bool centsOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM beatOsc1Menu[1] = {
    { "BeatOs1:\n " , beatOsc1Fun   }
};
bool beatOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM sawOsc1Menu[1] = {
    { "SawOsc1:\n " , sawOsc1Fun   }
};
bool sawOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM triOsc1Menu[1] = {
    { "TriOsc1:\n " , triOsc1Fun   }
};
bool triOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM pulesOsc1Menu[1] = {
    { "PlsOsc1:\n " , pulseOsc1Fun   }
};
bool pulseOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM PWOsc1Menu[1] = {
    { "PW Osc1:\n " , PWOsc1Fun   }
};
bool PWOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM pwmFreqOsc1Menu[1] = {
    { "PwmFOs1:\n " , pwmFreqOsc1Fun   }
};
bool pwmFreqOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM pwmDepthOsc1Menu[1] = {
    { "PwmDOs1:\n " , pwmDepthOsc1Fun   }
};
bool pwmDepthOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM sweepTimeOsc1Menu[1] = {
    { "SwpTOs1:\n " , sweepTimeOsc1Fun   }
};
bool sweepTimeOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM sweepDepthOsc1Menu[1] = {
    { "SwpDOs1:\n " , sweepDepthOsc1Fun   }
};
bool sweepDepthOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM breathAttainOsc1Menu[1] = {
    { "BAtnOs1:\n " , breathAttainOsc1Fun   }
};
bool breathAttainOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM breathDepthOsc1Menu[1] = {
    { "BDptOs1:\n " , breathDepthOsc1Fun   }
};
bool breathDepthOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM breathThresholdOsc1Menu[1] = {
    { "BthrOs1:\n " , breathThresholdOsc1Fun   }
};
bool breathThresholdOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM breathCurveOsc1Menu[1] = {
    { "BCrvOs1:\n " , breathCurveOsc1Fun   }
};
bool breathCurveOsc1Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM levelOsc1Menu[1] = {
    { "LevOsc1:\n " , levelOsc1Fun   }
};
bool levelOsc1Fun(){ return goUpOneMenu(); }

MenuItem PROGMEM patchOsc2Menu[19] = {
    { "Back    " , goUpOneMenu   }
   ,{ "OctOsc2 " , gotoOctOsc2Menu   }
   ,{ "SemiOsc2" , gotoSemiOsc2Menu   }
   ,{ "CentOsc2" , gotoCentsOsc2Menu   }
   ,{ "BeatOsc2" , gotoBeatOsc2Menu   }
   ,{ "SawOsc2 " , gotoSawOsc2Menu   }
   ,{ "TriOsc2 " , gotoTriOsc2Menu   }
   ,{ "PulsOsc2" , gotoPulseOsc2Menu   }
   ,{ "PWOsc2  " , gotoPWOsc2Menu   }
   ,{ "PwmFOsc2" , gotoPwmFreqOsc2Menu   }
   ,{ "PwmDOsc2" , gotoPwmDepthOsc2Menu   }
   ,{ "SwpTOsc2" , gotoSweepTimeOsc2Menu   }
   ,{ "SwpDOsc2" , gotoSweepDepthOsc2Menu   }
   ,{ "BAtnOsc2" , gotoBreathAttainOsc2Menu   }
   ,{ "BDptOsc2" , gotoBreathDepthOsc2Menu   }
   ,{ "BThrOsc2" , gotoBreathThresholdOsc2Menu   }
   ,{ "BCrvOsc2" , gotoBreathCurveOsc2Menu   }
   ,{ "LevlOsc2" , gotoLevelOsc2Menu   }
   ,{ "XFadOsc2" , gotoXFadeOsc2Menu   }
};

MenuItem PROGMEM octOsc2Menu[1] = {
    { "OctOsc2:\n " , octOsc2Fun   }
};
bool octOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM semiOsc2Menu[1] = {
    { "SemiOs2:\n " , semiOsc2Fun   }
};
bool semiOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM centsOsc2Menu[1] = {
    { "CentOs2:\n " , centsOsc2Fun   }
};
bool centsOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM beatOsc2Menu[1] = {
    { "BeatOs2:\n " , beatOsc2Fun   }
};
bool beatOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM sawOsc2Menu[1] = {
    { "SawOsc2:\n " , sawOsc2Fun   }
};
bool sawOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM triOsc2Menu[1] = {
    { "TriOsc2:\n " , triOsc2Fun   }
};
bool triOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM pulesOsc2Menu[1] = {
    { "PlsOsc2:\n " , pulseOsc2Fun   }
};
bool pulseOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM PWOsc2Menu[1] = {
    { "PW Osc2:\n " , PWOsc2Fun   }
};
bool PWOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM pwmFreqOsc2Menu[1] = {
    { "PwmFOs2:\n " , pwmFreqOsc2Fun   }
};
bool pwmFreqOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM pwmDepthOsc2Menu[1] = {
    { "PwmDOs2:\n " , pwmDepthOsc2Fun   }
};
bool pwmDepthOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM sweepTimeOsc2Menu[1] = {
    { "SwpTOs2:\n " , sweepTimeOsc2Fun   }
};
bool sweepTimeOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM sweepDepthOsc2Menu[1] = {
    { "SwpDOs2:\n " , sweepDepthOsc2Fun   }
};
bool sweepDepthOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM breathAttainOsc2Menu[1] = {
    { "BAtnOs2:\n " , breathAttainOsc2Fun   }
};
bool breathAttainOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM breathDepthOsc2Menu[1] = {
    { "BDptOs1:\n " , breathDepthOsc2Fun   }
};
bool breathDepthOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM breathThresholdOsc2Menu[1] = {
    { "BthrOs2:\n " , breathThresholdOsc2Fun   }
};
bool breathThresholdOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM breathCurveOsc2Menu[1] = {
    { "BCrvOs2:\n " , breathCurveOsc2Fun   }
};
bool breathCurveOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM levelOsc2Menu[1] = {
    { "LevOsc2:\n " , levelOsc2Fun   }
};
bool levelOsc2Fun(){ return goUpOneMenu(); }
MenuItem PROGMEM xFadeOsc2Menu[1] = {
    { "XFadOs2:\n " , xFadeOsc2Fun   }
};
bool xFadeOsc2Fun(){ return goUpOneMenu(); }

MenuItem PROGMEM patchOscFilter1Menu[14] = { 
    { "Back    " , goUpOneMenu   }
   ,{ "Link OFs" , gotoLinkOscFiltersMenu   }
   ,{ "Mode OF1" , gotoModeOscFilter1Menu   }
   ,{ "Freq OF1" , gotoFreqOscFilter1Menu   }
   ,{ "Q OF1   " , gotoQOscFilter1Menu   }
   ,{ "KeyFlOF1" , gotoKeyFollowOscFilter1Menu   }
   ,{ "BrModOF1" , gotoBreathModOscFilter1Menu   }
   ,{ "BrCrvOF1" , gotoBreathCurveOscFilter1Menu   }
   ,{ "LfoF OF1" , gotoLfoFreqOscFilter1Menu   }
   ,{ "LfoD OF1" , gotoLfoDepthOscFilter1Menu   }
   ,{ "LfoBrOF1" , gotoLfoBreathOscFilter1Menu   }
   ,{ "LfoThOF1" , gotoLfoThresholdOscFilter1Menu   }
   ,{ "SwpT OF1" , gotoSweepTimeOscFilter1Menu   }
   ,{ "SwpD OF1" , gotoSweepDepthOscFilter1Menu   }
};

MenuItem PROGMEM linkOscFiltersMenu[1] = {
    { "LinkOFs:\n " , linkOscFiltersFun   }
};
bool linkOscFiltersFun() { return goUpOneMenu(); }
MenuItem PROGMEM modeOscFilter1Menu[1] = {
    { "ModeOF1:\n " , modeOscFilter1Fun   }
};
bool modeOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM freqOscFilter1Menu[1] = {
    { "FreqOF1:\n " , freqOscFilter1Fun   }
};
bool freqOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM qOscFilter1Menu[1] = {
    { "Q OscF1:\n " , qOscFilter1Fun   }
};
bool qOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM keyFollowOscFilter1Menu[1] = {
    { "KeyFOF1:\n " , keyFollowOscFilter1Fun   }
};
bool keyFollowOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM breathModOscFilter1Menu[1] = {
    { "BrMdOF1:\n " , breathModOscFilter1Fun   }
};
bool breathModOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM breathCurveOscFilter1Menu[1] = {
    { "BrCvOF1:\n " , breathCurveOscFilter1Fun   }
};
bool breathCurveOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoFreqOscFilter1Menu[1] = {
    { "LfoFOF1:\n " , lfoFreqOscFilter1Fun   }
};
bool lfoFreqOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoDepthOscFilter1Menu[1] = {
    { "LfoDOF1:\n " , lfoDepthOscFilter1Fun   }
};
bool lfoDepthOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoBreathOscFilter1Menu[1] = {
    { "LfoBOF1:\n " , lfoBreathOscFilter1Fun   }
};
bool lfoBreathOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoThreshOscFilter1Menu[1] = {
    { "LfoTOF1:\n " , lfoThreshOscFilter1Fun   }
};
bool lfoThreshOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM sweepTimeOscFilter1Menu[1] = {
    { "SwpTOF1:\n " , sweepTimeOscFilter1Fun   }
};
bool sweepTimeOscFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM sweepDepthOscFilter1Menu[1] = {
    { "SwpDOF1:\n " , sweepDepthOscFilter1Fun   }
};
bool sweepDepthOscFilter1Fun() { return goUpOneMenu(); }

MenuItem PROGMEM patchOscFilter2Menu[13] = {
    { "Back    " , goUpOneMenu   }
   ,{ "Mode OF2" , gotoModeOscFilter2Menu   }
   ,{ "Freq OF2" , gotoFreqOscFilter2Menu   }
   ,{ "Q OF2   " , gotoQOscFilter2Menu   }
   ,{ "KeyFlOF2" , gotoKeyFollowOscFilter2Menu   }
   ,{ "BrModOF2" , gotoBreathModOscFilter2Menu   }
   ,{ "BrCrvOF2" , gotoBreathCurveOscFilter2Menu   }
   ,{ "LfoF OF2" , gotoLfoFreqOscFilter2Menu   }
   ,{ "LfoD OF2" , gotoLfoDepthOscFilter2Menu   }
   ,{ "LfoBrOF2" , gotoLfoBreathOscFilter2Menu   }
   ,{ "LfoThOF2" , gotoLfoThresholdOscFilter2Menu   }
   ,{ "SwpT OF2" , gotoSweepTimeOscFilter2Menu   }
   ,{ "SwpD OF2" , gotoSweepDepthOscFilter2Menu   }
};

MenuItem PROGMEM modeOscFilter2Menu[1] = {
    { "ModeOF2:\n " , modeOscFilter2Fun   }
};
bool modeOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM freqOscFilter2Menu[1] = {
    { "FreqOF2:\n " , freqOscFilter2Fun   }
};
bool freqOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM qOscFilter2Menu[1] = {
    { "Q OscF2:\n " , qOscFilter2Fun   }
};
bool qOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM keyFollowOscFilter2Menu[1] = {
    { "KeyFOF2:\n " , keyFollowOscFilter2Fun   }
};
bool keyFollowOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM breathModOscFilter2Menu[1] = {
    { "BrMdOF2:\n " , breathModOscFilter2Fun   }
};
bool breathModOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM breathCurveOscFilter2Menu[1] = {
    { "BrCvOF2:\n " , breathCurveOscFilter2Fun   }
};
bool breathCurveOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoFreqOscFilter2Menu[1] = {
    { "LfoFOF2:\n " , lfoFreqOscFilter2Fun   }
};
bool lfoFreqOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoDepthOscFilter2Menu[1] = {
    { "LfoDOF2:\n " , lfoDepthOscFilter2Fun   }
};
bool lfoDepthOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoBreathOscFilter2Menu[1] = {
    { "LfoBOF2:\n " , lfoBreathOscFilter2Fun   }
};
bool lfoBreathOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoThreshOscFilter2Menu[1] = {
    { "LfoTOF2:\n " , lfoThreshOscFilter2Fun   }
};
bool lfoThreshOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM sweepTimeOscFilter2Menu[1] = {
    { "SwpTOF2:\n " , sweepTimeOscFilter2Fun   }
};
bool sweepTimeOscFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM sweepDepthOscFilter2Menu[1] = {
    { "SwpDOF2:\n " , sweepDepthOscFilter2Fun   }
};
bool sweepDepthOscFilter2Fun() { return goUpOneMenu(); }

MenuItem PROGMEM patchNoiseFilter1Menu[14] = { 
    { "Back    " , goUpOneMenu   }
   ,{ "Link NFs" , gotoLinkNoiseFiltersMenu   }
   ,{ "Mode NF1" , gotoModeNoiseFilter1Menu   }
   ,{ "Freq NF1" , gotoFreqNoiseFilter1Menu   }
   ,{ "Q NF1   " , gotoQNoiseFilter1Menu   }
   ,{ "KeyFlNF1" , gotoKeyFollowNoiseFilter1Menu   }
   ,{ "BrModNF1" , gotoBreathModNoiseFilter1Menu   }
   ,{ "BrCrvNF1" , gotoBreathCurveNoiseFilter1Menu   }
   ,{ "LfoF NF1" , gotoLfoFreqNoiseFilter1Menu   }
   ,{ "LfoD NF1" , gotoLfoDepthNoiseFilter1Menu   }
   ,{ "LfoBrNF1" , gotoLfoBreathNoiseFilter1Menu   }
   ,{ "LfoThNF1" , gotoLfoThresholdNoiseFilter1Menu   }
   ,{ "SwpT NF1" , gotoSweepTimeNoiseFilter1Menu   }
   ,{ "SwpD NF1" , gotoSweepDepthNoiseFilter1Menu   }
};

MenuItem PROGMEM linkNoiseFiltersMenu[1] = {
    { "LinkNFs:\n " , linkNoiseFiltersFun   }
};
bool linkNoiseFiltersFun() { return goUpOneMenu(); }
MenuItem PROGMEM modeNoiseFilter1Menu[1] = {
    { "ModeNF1:\n " , modeNoiseFilter1Fun   }
};
bool modeNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM freqNoiseFilter1Menu[1] = {
    { "FreqNF1:\n " , freqNoiseFilter1Fun   }
};
bool freqNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM qNoiseFilter1Menu[1] = {
    { "Q NoiF1:\n " , qNoiseFilter1Fun   }
};
bool qNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM keyFollowNoiseFilter1Menu[1] = {
    { "KeyFNF1:\n " , keyFollowNoiseFilter1Fun   }
};
bool keyFollowNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM breathModNoiseFilter1Menu[1] = {
    { "BrMdNF1:\n " , breathModNoiseFilter1Fun   }
};
bool breathModNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM breathCurveNoiseFilter1Menu[1] = {
    { "BrCvNF1:\n " , breathCurveNoiseFilter1Fun   }
};
bool breathCurveNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoFreqNoiseFilter1Menu[1] = {
    { "LfoFNF1:\n " , lfoFreqNoiseFilter1Fun   }
};
bool lfoFreqNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoDepthNoiseFilter1Menu[1] = {
    { "LfoDNF1:\n " , lfoDepthNoiseFilter1Fun   }
};
bool lfoDepthNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoBreathNoiseFilter1Menu[1] = {
    { "LfoBNF1:\n " , lfoBreathNoiseFilter1Fun   }
};
bool lfoBreathNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoThreshNoiseFilter1Menu[1] = {
    { "LfoTNF1:\n " , lfoThreshNoiseFilter1Fun   }
};
bool lfoThreshNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM sweepTimeNoiseFilter1Menu[1] = {
    { "SwpTNF1:\n " , sweepTimeNoiseFilter1Fun   }
};
bool sweepTimeNoiseFilter1Fun() { return goUpOneMenu(); }
MenuItem PROGMEM sweepDepthNoiseFilter1Menu[1] = {
    { "SwpDNF1:\n " , sweepDepthNoiseFilter1Fun   }
};
bool sweepDepthNoiseFilter1Fun() { return goUpOneMenu(); }

MenuItem PROGMEM patchNoiseFilter2Menu[13] = {
    { "Back    " , goUpOneMenu   }
   ,{ "Mode NF2" , gotoModeNoiseFilter2Menu   }
   ,{ "Freq NF2" , gotoFreqNoiseFilter2Menu   }
   ,{ "Q NF2   " , gotoQNoiseFilter2Menu   }
   ,{ "KeyFlNF2" , gotoKeyFollowNoiseFilter2Menu   }
   ,{ "BrModNF2" , gotoBreathModNoiseFilter2Menu   }
   ,{ "BrCrvNF2" , gotoBreathCurveNoiseFilter2Menu   }
   ,{ "LfoF NF2" , gotoLfoFreqNoiseFilter2Menu   }
   ,{ "LfoD NF2" , gotoLfoDepthNoiseFilter2Menu   }
   ,{ "LfoBrNF2" , gotoLfoBreathNoiseFilter2Menu   }
   ,{ "LfoThNF2" , gotoLfoThresholdNoiseFilter2Menu   }
   ,{ "SwpT NF2" , gotoSweepTimeNoiseFilter2Menu   }
   ,{ "SwpD NF2" , gotoSweepDepthNoiseFilter2Menu   }
};

MenuItem PROGMEM modeNoiseFilter2Menu[1] = {
    { "ModeNF2:\n " , modeNoiseFilter2Fun   }
};
bool modeNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM freqNoiseFilter2Menu[1] = {
    { "FreqNF2:\n " , freqNoiseFilter2Fun   }
};
bool freqNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM qNoiseFilter2Menu[1] = {
    { "Q NoiF2:\n " , qNoiseFilter2Fun   }
};
bool qNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM keyFollowNoiseFilter2Menu[1] = {
    { "KeyFNF2:\n " , keyFollowNoiseFilter2Fun   }
};
bool keyFollowNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM breathModNoiseFilter2Menu[1] = {
    { "BrMdNF2:\n " , breathModNoiseFilter2Fun   }
};
bool breathModNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM breathCurveNoiseFilter2Menu[1] = {
    { "BrCvNF2:\n " , breathCurveNoiseFilter2Fun   }
};
bool breathCurveNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoFreqNoiseFilter2Menu[1] = {
    { "LfoFNF2:\n " , lfoFreqNoiseFilter2Fun   }
};
bool lfoFreqNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoDepthNoiseFilter2Menu[1] = {
    { "LfoDNF2:\n " , lfoDepthNoiseFilter2Fun   }
};
bool lfoDepthNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoBreathNoiseFilter2Menu[1] = {
    { "LfoBNF2:\n " , lfoBreathNoiseFilter2Fun   }
};
bool lfoBreathNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM lfoThreshNoiseFilter2Menu[1] = {
    { "LfoTNF2:\n " , lfoThreshNoiseFilter2Fun   }
};
bool lfoThreshNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM sweepTimeNoiseFilter2Menu[1] = {
    { "SwpTNF2:\n " , sweepTimeNoiseFilter2Fun   }
};
bool sweepTimeNoiseFilter2Fun() { return goUpOneMenu(); }
MenuItem PROGMEM sweepDepthNoiseFilter2Menu[1] = {
    { "SwpDNF2:\n " , sweepDepthNoiseFilter2Fun   }
};
bool sweepDepthNoiseFilter2Fun() { return goUpOneMenu(); }

MenuItem PROGMEM patchNoiseMenu[4] = {
    { "Back    " , goUpOneMenu   }
   ,{ "Nz Time " , gotoNoiseTimeMenu   }
   ,{ "NzBreath" , gotoNoiseBreathMenu   }
   ,{ "Nz Level" , gotoNoiseLevelMenu   }
};

MenuItem PROGMEM noiseTimeMenu[1] = {
    { "Nz Time:\n " , noiseTimeFun   }
};
bool noiseTimeFun() { return goUpOneMenu(); }
MenuItem PROGMEM noiseBreathMenu[1] = {
    { "NzBreth:\n " , noiseBreathFun   }
};
bool noiseBreathFun() { return goUpOneMenu(); }
MenuItem PROGMEM noiseLevelMenu[1] = {
    { "NzLevel:\n " , noiseLevelFun   }
};
bool noiseLevelFun() { return goUpOneMenu(); }

MenuItem PROGMEM patchFormantMenu[1] = {
    { "Formant:\n ", formantSelectFun    }
};
bool formantSelectFun() { return goUpOneMenu(); }

MenuItem PROGMEM patchAmpMenu[1] = {
    { "Patch\n Amp:\n ", patchAmpAdjustFun    }
};
bool patchAmpAdjustFun() { return goUpOneMenu(); }

MenuItem PROGMEM patchCommonMenu[5] = {
    { "Back    " , goUpOneMenu   }
  , { "Key Trig" , gotoKeyTriggerMenu   }
  , { "OctBtnLv" , gotoOctButtonLevelMenu   }
  , { "BendRnge" , gotoBendRangeMenu   }
  , { "BendStep" , gotoBendStepMenu   }
//  , { "VibPitch" , gotoVibratoPitchCommonMenu   }
//  , { "Vib Amp " , gotoVibratoAmpCommonMenu   }
};
MenuItem PROGMEM keyTriggerMenu[1] = {
    { "KeyTrig:\n ", keyTriggerFun    }
};
bool keyTriggerFun() { return goUpOneMenu(); }
MenuItem PROGMEM octButtonLevelMenu[1] = {
    { "OctBtnL:\n ", octButtonLevelFun    }
};
bool octButtonLevelFun() { return goUpOneMenu(); }
MenuItem PROGMEM bendRangeMenu[1] = {
    { "BendRng:\n ", bendRangeFun    }
};
bool bendRangeFun() { return goUpOneMenu(); }
MenuItem PROGMEM bendStepMenu[1] = {
    { "BendStp:\n ", bendStepFun    }
};
bool bendStepFun() { return goUpOneMenu(); }

MenuItem PROGMEM systemAdjMenu[6] = {
    { "Back    " , goUpOneMenu   }
  , { "AuxIn   " , gotoAuxInMenu   }
  , { "Octave  " , gotoOctaveMenu   }
  , { "Semi    " , gotoSemiMenu   }
  , { "Cents   " , gotoCentsMenu   }
  , { "BreathCC" , gotoBreathCCMenu   }
};
MenuItem PROGMEM auxInMenu[1] = {
    { "Aux In: \n ", auxInFun    }
};
bool auxInFun() { 
     if(myMenu.updateLeafValue){
    mix_linein += myMenu.updateLeafValue;
    mix_linein = mix_linein < 0 ? 0 : mix_linein >= 100 ? 100 : mix_linein;
    mix_lineinf = ((float)mix_linein)/100.0f;
    mix_lineinf = (mix_lineinf*mix_lineinf)*2.0f;
    //preUpdateSynthVariablesFlag = true; // squelch while change vol 
    sprintf(myMenu.str_oledbuf,"  %d   ", mix_linein);
    Serial8.println(F("mixLineIn: updated mix_linein "));    
    return true; 
 }
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  Serial8.println(F("volAdjustFun: goto TopMenu"));
  //myMenu.setCurrentMenu(&listTopMenu);
  return goUpOneMenu(); 
}
MenuItem PROGMEM octaveMenu[1] = {
    { "Octave: \n ", octaveFun    }
};
bool octaveFun() { 
    return goUpOneMenu(); 
}
MenuItem PROGMEM semiMenu[1] = {
    { "Semiton:\n ", semiFun    }
};
bool semiFun() { return goUpOneMenu(); }
MenuItem PROGMEM centsMenu[1] = {
    { "Cents:  \n ", centsFun    }
};
bool centsFun() { return goUpOneMenu(); }
MenuItem PROGMEM breathCCMenu[1] = {
    { "BrethCC:\n ", breathCCFun    }
};
bool breathCCFun() { return goUpOneMenu(); }

MenuList listTopMenu(topMenu, 18);
MenuList listVolAdjustMenu(volAdjustMenu, 1);
MenuList listPatchSelectMenu(patchSelectMenu, 1);
MenuList listPatchResetMenu(patchResetMenu, 2);
MenuList listPatchCopyMenu(patchCopyMenu, 2);
MenuList listPatchPasteMenu(patchPasteMenu, 2);
MenuList listPatchSwapMenu(patchSwapMenu, 2);
MenuList listPatchFxMenu(patchFxMenu, 23);
MenuList listPatchOsc1Menu(patchOsc1Menu, 18);
MenuList listPatchOsc2Menu(patchOsc2Menu, 19);
MenuList listPatchOscFilter1Menu(patchOscFilter1Menu, 14);
MenuList listPatchOscFilter2Menu(patchOscFilter2Menu, 13);
MenuList listPatchNoiseFilter1Menu(patchNoiseFilter1Menu, 14);
MenuList listPatchNoiseFilter2Menu(patchNoiseFilter2Menu, 13);
MenuList listPatchNoiseMenu(patchNoiseMenu, 4);
MenuList listPatchFormantMenu(patchFormantMenu, 1);
MenuList listPatchAmpMenu(patchAmpMenu, 1);
MenuList listPatchCommonMenu(patchCommonMenu, 5);

MenuList listSystemAdjMenu(systemAdjMenu, 6);

MenuList listFxCopyMenu(fxCopyMenu,2);
MenuList listFxPasteMenu(fxPasteMenu,2);
MenuList listFxSwapMenu(fxSwapMenu,2);
MenuList listDelayLevelMenu(delayLevelMenu,1);
MenuList listDelayTimeLMenu(delayTimeLMenu,1);
MenuList listDelayTimeRMenu(delayTimeRMenu,1);
MenuList listDelayFeedbackMenu(delayFeedbackMenu,1);
MenuList listDelayDampMenu(delayDampMenu,1);
MenuList listReverbLevelMenu(reverbLevelMenu,1);
MenuList listReverbTimeMenu(reverbTimeMenu,1);
MenuList listReverbDensityMenu(reverbDensityMenu,1);
MenuList listReverbDampMenu(reverbDampMenu,1);
MenuList listChorusOnMenu(chorusOnMenu,1);
MenuList listChorusDryMenu(chorusDryMenu,1);
MenuList listChorusLfoFMenu(chorusLfoFMenu,1);
MenuList listChorusFeedbackMenu(chorusFeedbackMenu,1);
MenuList listChorusDelay1Menu(chorusDelay1Menu,1);
MenuList listChorusMod1Menu(chorusMod1Menu,1);
MenuList listChorusWet1Menu(chorusWet1Menu,1);
MenuList listChorusDelay2Menu(chorusDelay2Menu,1);
MenuList listChorusMod2Menu(chorusMod2Menu,1);
MenuList listChorusWet2Menu(chorusWet2Menu,1);

MenuList listOscOsc1Menu(octOsc1Menu, 1);
MenuList listSemiOsc1Menu(semiOsc1Menu, 1);
MenuList listCentsOsc1Menu(centsOsc1Menu, 1);
MenuList listBeatOsc1Menu(beatOsc1Menu, 1);
MenuList listSawOsc1Menu(sawOsc1Menu, 1);
MenuList listTriOsc1Menu(triOsc1Menu, 1);
MenuList listPulseOsc1Menu(pulesOsc1Menu, 1);
MenuList listPWOsc1Menu(PWOsc1Menu, 1);
MenuList listPwmFreqOsc1Menu(pwmFreqOsc1Menu, 1);
MenuList listPwmDepthOsc1Menu(pwmDepthOsc1Menu, 1);
MenuList listSweepTimeOsc1Menu(sweepTimeOsc1Menu, 1);
MenuList listSweepDepthOsc1Menu(sweepDepthOsc1Menu, 1);
MenuList listBreathAttainOsc1Menu(breathAttainOsc1Menu, 1);
MenuList listBreathDepthOsc1Menu(breathDepthOsc1Menu, 1);
MenuList listBreathThresholdOsc1Menu(breathThresholdOsc1Menu, 1);
MenuList listBreathCurveOsc1Menu(breathCurveOsc1Menu, 1);
MenuList listLevelOsc1Menu(levelOsc1Menu, 1);

MenuList listOscOsc2Menu(octOsc2Menu, 1);
MenuList listSemiOsc2Menu(semiOsc2Menu, 1);
MenuList listCentsOsc2Menu(centsOsc2Menu, 1);
MenuList listBeatOsc2Menu(beatOsc2Menu, 1);
MenuList listSawOsc2Menu(sawOsc2Menu, 1);
MenuList listTriOsc2Menu(triOsc2Menu, 1);
MenuList listPulseOsc2Menu(pulesOsc2Menu, 1);
MenuList listPWOsc2Menu(PWOsc2Menu, 1);
MenuList listPwmFreqOsc2Menu(pwmFreqOsc2Menu, 1);
MenuList listPwmDepthOsc2Menu(pwmDepthOsc2Menu, 1);
MenuList listSweepTimeOsc2Menu(sweepTimeOsc2Menu, 1);
MenuList listSweepDepthOsc2Menu(sweepDepthOsc2Menu, 1);
MenuList listBreathAttainOsc2Menu(breathAttainOsc2Menu, 1);
MenuList listBreathDepthOsc2Menu(breathDepthOsc2Menu, 1);
MenuList listBreathThresholdOsc2Menu(breathThresholdOsc2Menu, 1);
MenuList listBreathCurveOsc2Menu(breathCurveOsc2Menu, 1);
MenuList listLevelOsc2Menu(levelOsc2Menu, 1);
MenuList listXFadeOsc2Menu(xFadeOsc2Menu, 1);

MenuList listLinkOscFiltersMenu(linkOscFiltersMenu, 1);
MenuList listModeOscFilter1Menu(modeOscFilter1Menu, 1);
MenuList listFreqOscFilter1Menu(freqOscFilter1Menu, 1);
MenuList listQOscFilter1Menu(qOscFilter1Menu, 1);
MenuList listKeyFollowOscFilter1Menu(keyFollowOscFilter1Menu, 1);
MenuList listBreathModOscFilter1Menu(breathModOscFilter1Menu, 1);
MenuList listBreathCurveOscFilter1Menu(breathCurveOscFilter1Menu, 1);
MenuList listLfoFreqOscFilter1Menu(lfoFreqOscFilter1Menu, 1);
MenuList listLfoDepthOscFilter1Menu(lfoDepthOscFilter1Menu, 1);
MenuList listLfoBreathOscFilter1Menu(lfoBreathOscFilter1Menu, 1);
MenuList listLfoThresholdOscFilter1Menu(lfoThreshOscFilter1Menu, 1);
MenuList listSweepTimeOscFilter1Menu(sweepTimeOscFilter1Menu, 1);
MenuList listSweepDepthOscFilter1Menu(sweepDepthOscFilter1Menu, 1);

MenuList listModeOscFilter2Menu(modeOscFilter2Menu, 1);
MenuList listFreqOscFilter2Menu(freqOscFilter2Menu, 1);
MenuList listQOscFilter2Menu(qOscFilter2Menu, 1);
MenuList listKeyFollowOscFilter2Menu(keyFollowOscFilter2Menu, 1);
MenuList listBreathModOscFilter2Menu(breathModOscFilter2Menu, 1);
MenuList listBreathCurveOscFilter2Menu(breathCurveOscFilter2Menu, 1);
MenuList listLfoFreqOscFilter2Menu(lfoFreqOscFilter2Menu, 1);
MenuList listLfoDepthOscFilter2Menu(lfoDepthOscFilter2Menu, 1);
MenuList listLfoBreathOscFilter2Menu(lfoBreathOscFilter2Menu, 1);
MenuList listLfoThresholdOscFilter2Menu(lfoThreshOscFilter2Menu, 1);
MenuList listSweepTimeOscFilter2Menu(sweepTimeOscFilter2Menu, 1);
MenuList listSweepDepthOscFilter2Menu(sweepDepthOscFilter2Menu, 1);

MenuList listLinkNoiseFiltersMenu(linkNoiseFiltersMenu, 1);
MenuList listModeNoiseFilter1Menu(modeNoiseFilter1Menu, 1);
MenuList listFreqNoiseFilter1Menu(freqNoiseFilter1Menu, 1);
MenuList listQNoiseFilter1Menu(qNoiseFilter1Menu, 1);
MenuList listKeyFollowNoiseFilter1Menu(keyFollowNoiseFilter1Menu, 1);
MenuList listBreathModNoiseFilter1Menu(breathModNoiseFilter1Menu, 1);
MenuList listBreathCurveNoiseFilter1Menu(breathCurveNoiseFilter1Menu, 1);
MenuList listLfoFreqNoiseFilter1Menu(lfoFreqNoiseFilter1Menu, 1);
MenuList listLfoDepthNoiseFilter1Menu(lfoDepthNoiseFilter1Menu, 1);
MenuList listLfoBreathNoiseFilter1Menu(lfoBreathNoiseFilter1Menu, 1);
MenuList listLfoThresholdNoiseFilter1Menu(lfoThreshNoiseFilter1Menu, 1);
MenuList listSweepTimeNoiseFilter1Menu(sweepTimeNoiseFilter1Menu, 1);
MenuList listSweepDepthNoiseFilter1Menu(sweepDepthNoiseFilter1Menu, 1);

MenuList listModeNoiseFilter2Menu(modeNoiseFilter2Menu, 1);
MenuList listFreqNoiseFilter2Menu(freqNoiseFilter2Menu, 1);
MenuList listQNoiseFilter2Menu(qNoiseFilter2Menu, 1);
MenuList listKeyFollowNoiseFilter2Menu(keyFollowNoiseFilter2Menu, 1);
MenuList listBreathModNoiseFilter2Menu(breathModNoiseFilter2Menu, 1);
MenuList listBreathCurveNoiseFilter2Menu(breathCurveNoiseFilter2Menu, 1);
MenuList listLfoFreqNoiseFilter2Menu(lfoFreqNoiseFilter2Menu, 1);
MenuList listLfoDepthNoiseFilter2Menu(lfoDepthNoiseFilter2Menu, 1);
MenuList listLfoBreathNoiseFilter2Menu(lfoBreathNoiseFilter2Menu, 1);
MenuList listLfoThresholdNoiseFilter2Menu(lfoThreshNoiseFilter2Menu, 1);
MenuList listSweepTimeNoiseFilter2Menu(sweepTimeNoiseFilter2Menu, 1);
MenuList listSweepDepthNoiseFilter2Menu(sweepDepthNoiseFilter2Menu, 1);

MenuList listNoiseTimeMenu(noiseTimeMenu, 1);
MenuList listNoiseBreathMenu(noiseBreathMenu, 1);
MenuList listNoiseLevelMenu(noiseLevelMenu, 1);

MenuList listKeyTriggerMenu(keyTriggerMenu, 1);
MenuList listOctButtonLevelMenu(octButtonLevelMenu, 1);
MenuList listBendRangeMenu(bendRangeMenu, 1);
MenuList listBendStepMenu(bendStepMenu, 1);

MenuList listAuxInMenu(auxInMenu, 1);
MenuList listOctaveMenu(octaveMenu, 1);
MenuList listSemiMenu(semiMenu, 1);
MenuList listCentsMenu(centsMenu, 1);
MenuList listBreathCCMenu(breathCCMenu, 1);

/*****************************
 * prototypes go in globals.h
 ******************************
 * Define the functions you want your menu to call
 * They can be blocking or non-blocking
 * They should take no arguments and return a bool
 * true if the function is finished and doesn't want to run again
 * false if the function is not done and wants to be called again
 ******************************
 *****************************/

//extern OledMenu myMenu; // not sure why this is needed here and in globals.h


//---------------------
// topMenu Functions
//---------------------

bool gotoVolAdjMenu() {
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  myMenu.setCurrentMenu(&listVolAdjustMenu);
  sprintf(myMenu.str_oledbuf,"  %d   ", vol);
  display.println(myMenu.str_oledbuf);
  return true;
}
bool gotoPatchSelectMenu() {
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  myMenu.setCurrentMenu(&listPatchSelectMenu);
  String ps( current_patch.patch_string );
  ps.setCharAt( ps.indexOf(' '), '\n'); // TODO: spaces till end of line then \n
  sprintf(myMenu.str_oledbuf, "%03d\n%s", current_patchNumber+1, ps.c_str() );
  display.println(myMenu.str_oledbuf);
  Serial8.println(myMenu.str_oledbuf);
  return true;
}
bool gotoPatchResetMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchResetMenu);
  return true;
}
bool gotoPatchCopyMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchCopyMenu);
  return true;
}
bool gotoPatchPasteMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchPasteMenu);
    return true;
}
bool gotoPatchSwapMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchSwapMenu);
    return true;
}
bool gotoPatchFxMenu(){
  //myMenu.previousMenu = myMenu.currentMenu;
  //myMenu.previousItemIndex = myMenu.currentItemIndex;
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchFxMenu);
    return true;
}
bool gotoPatchOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchOsc1Menu);
    return true;
}
bool gotoPatchOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchOsc2Menu);
    return true;
}
bool gotoPatchOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchOscFilter1Menu);
    return true;
}
bool gotoPatchOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchOscFilter2Menu);
    return true;
}
bool gotoPatchNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchNoiseFilter1Menu);
    return true;
}
bool gotoPatchNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchNoiseFilter2Menu);
    return true;
}
bool gotoPatchNoiseMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchNoiseMenu);
    return true;
}
bool gotoPatchFormantMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchFormantMenu);
    return true;
}
bool gotoPatchAmpMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchAmpMenu);
    return true;
}
bool gotoPatchCommonMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchAmpMenu);
  myMenu.setCurrentMenu(&listPatchCommonMenu);
    return true;
}

bool gotoSystemAdjMenu() {
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSystemAdjMenu);
  return true;
}

bool goUpOneMenu() {
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  //myMenu.setCurrentMenu(myMenu.previousMenu);
  //myMenu.currentItemIndex = myMenu.previousItemIndex;
  if(!myMenu.previousMenuStack.empty() 
          && !myMenu.previousItemIndexStack.empty()){
      myMenu.setCurrentMenu(myMenu.previousMenuStack.top());
      myMenu.currentItemIndex = myMenu.previousItemIndexStack.top();
      myMenu.previousMenuStack.pop();
      myMenu.previousItemIndexStack.pop();
  }
  return true;
}

bool gotoTopMenu() {
  while(!myMenu.previousMenuStack.empty()){
      myMenu.previousMenuStack.pop();
  }
  while(!myMenu.previousItemIndexStack.empty()){
      myMenu.previousItemIndexStack.pop();
  }
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  myMenu.setCurrentMenu(&listTopMenu);
  return true;
}




// patchFxMenu functions
bool gotoFxCopyMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFxCopyMenu);
  return true;
}
bool gotoFxPasteMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFxPasteMenu);
  return true;
}
bool gotoFxSwapMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFxSwapMenu);
  return true;
}
bool gotoDelayLevelMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayLevelMenu);
  return true;
}
bool gotoDelayTimeLMenu(){
  display.clearDisplay();
  display.display(); // refresh display
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayTimeLMenu);
  sprintf(myMenu.str_oledbuf,"  %04d    ", 10*current_patch.nrpn_msb_delay[CCEFFECTSDELAYTIME]);
  display.println(myMenu.str_oledbuf);
  return true;
}
bool gotoDelayTimeRMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayTimeRMenu);
  return true;
}
bool gotoDelayFeedbackMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayFeedbackMenu);
  return true;
}
bool gotoDelayDampMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayDampMenu);
  return true;
}
bool gotoReverbLevelMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listReverbLevelMenu);
  return true;
}
bool gotoReverbTimeMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listReverbTimeMenu);
  return true;
}
bool gotoReverbDensityMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listReverbDensityMenu);
  return true;
}
bool gotoReverbDampMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listReverbDampMenu);
  return true;
}
bool gotoChorusOnMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusOnMenu);
  return true;
}
bool gotoChorusDryMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusDryMenu);
  return true;
}
bool gotoChorusLfoFMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusLfoFMenu);
  return true;
}
bool gotoChorusFeedbackMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusFeedbackMenu);
  return true;
}
bool gotoChorusDelay1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusDelay1Menu);
  return true;
}
bool gotoChorusMod1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusMod1Menu);
  return true;
}
bool gotoChorusWet1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusWet1Menu);
  return true;
}
bool gotoChorusDelay2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusDelay2Menu);
  return true;
}
bool gotoChorusMod2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusMod2Menu);
  return true;
}
bool gotoChorusWet2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusWet2Menu);
  return true;
}

// patchOsc1Menu functions
bool gotoOctOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listOscOsc1Menu);
  return true;
}
bool gotoSemiOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSemiOsc1Menu);
  return true;
}
bool gotoCentsOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listCentsOsc1Menu);
  return true;
}
bool gotoBeatOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBeatOsc1Menu);
  return true;
}
bool gotoSawOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSawOsc1Menu);
  return true;
}
bool gotoTriOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listTriOsc1Menu);
  return true;
}
bool gotoPulseOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPulseOsc1Menu);
  return true;
}
bool gotoPWOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPWOsc1Menu);
  return true;
}
bool gotoPwmFreqOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPwmFreqOsc1Menu);
  return true;
}
bool gotoPwmDepthOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPwmDepthOsc1Menu);
  return true;
}
bool gotoSweepTimeOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeOsc1Menu);
  return true;
}
bool gotoSweepDepthOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthOsc1Menu);
  return true;
}
bool gotoBreathAttainOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathAttainOsc1Menu);
  return true;
}
bool gotoBreathDepthOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathDepthOsc1Menu);
  return true;
}
bool gotoBreathThresholdOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathThresholdOsc1Menu);
  return true;
}
bool gotoBreathCurveOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveOsc1Menu);
  return true;
}
bool gotoLevelOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLevelOsc1Menu);
  return true;
}

// patchOsc2Menu functions
//

bool gotoOctOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listOscOsc2Menu);
  return true;
}
bool gotoSemiOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSemiOsc2Menu);
  return true;
}
bool gotoCentsOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listCentsOsc2Menu);
  return true;
}
bool gotoBeatOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBeatOsc2Menu);
  return true;
}
bool gotoSawOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSawOsc2Menu);
  return true;
}
bool gotoTriOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listTriOsc2Menu);
  return true;
}
bool gotoPulseOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPulseOsc2Menu);
  return true;
}
bool gotoPWOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPWOsc2Menu);
  return true;
}
bool gotoPwmFreqOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPwmFreqOsc2Menu);
  return true;
}
bool gotoPwmDepthOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPwmDepthOsc2Menu);
  return true;
}
bool gotoSweepTimeOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeOsc2Menu);
  return true;
}
bool gotoSweepDepthOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthOsc2Menu);
  return true;
}
bool gotoBreathAttainOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathAttainOsc2Menu);
  return true;
}
bool gotoBreathDepthOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathDepthOsc2Menu);
  return true;
}
bool gotoBreathThresholdOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathThresholdOsc2Menu);
  return true;
}
bool gotoBreathCurveOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveOsc2Menu);
  return true;
}
bool gotoLevelOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLevelOsc2Menu);
  return true;
}
bool gotoXFadeOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listXFadeOsc2Menu);
  return true;
}


// patchOscFilter1Menu Functions 
bool gotoLinkOscFiltersMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLinkOscFiltersMenu);
  return true;
}
bool gotoModeOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listModeOscFilter1Menu);
  return true;
}
bool gotoFreqOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFreqOscFilter1Menu);
  return true;
}
bool gotoQOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listQOscFilter1Menu);
  return true;
}
bool gotoKeyFollowOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyFollowOscFilter1Menu);
  return true;
}
bool gotoBreathModOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathModOscFilter1Menu);
  return true;
}
bool gotoBreathCurveOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveOscFilter1Menu);
  return true;
}
bool gotoLfoFreqOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoFreqOscFilter1Menu);
  return true;
}
bool gotoLfoDepthOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoDepthOscFilter1Menu);
  return true;
}
bool gotoLfoBreathOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoBreathOscFilter1Menu);
  return true;
}
bool gotoLfoThresholdOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoThresholdOscFilter1Menu);
  return true;
}
bool gotoSweepTimeOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeOscFilter1Menu);
  return true;
}
bool gotoSweepDepthOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthOscFilter1Menu);
  return true;
}

// patchOscFilter2Menu Functions
bool gotoModeOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listModeOscFilter2Menu);
  return true;
}
bool gotoFreqOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFreqOscFilter2Menu);
  return true;
}
bool gotoQOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listQOscFilter2Menu);
  return true;
}
bool gotoKeyFollowOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyFollowOscFilter2Menu);
  return true;
}
bool gotoBreathModOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathModOscFilter2Menu);
  return true;
}
bool gotoBreathCurveOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveOscFilter2Menu);
  return true;
}
bool gotoLfoFreqOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoFreqOscFilter2Menu);
  return true;
}
bool gotoLfoDepthOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoDepthOscFilter2Menu);
  return true;
}
bool gotoLfoBreathOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoBreathOscFilter2Menu);
  return true;
}
bool gotoLfoThresholdOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoThresholdOscFilter2Menu);
  return true;
}
bool gotoSweepTimeOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeOscFilter2Menu);
  return true;
}
bool gotoSweepDepthOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthOscFilter2Menu);
  return true;
}


// patchNoiseFilter1Menu Functions 
bool gotoLinkNoiseFiltersMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLinkNoiseFiltersMenu);
  return true;
}
bool gotoModeNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listModeNoiseFilter1Menu);
  return true;
}
bool gotoFreqNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFreqNoiseFilter1Menu);
  return true;
}
bool gotoQNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listQNoiseFilter1Menu);
  return true;
}
bool gotoKeyFollowNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyFollowNoiseFilter1Menu);
  return true;
}
bool gotoBreathModNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathModNoiseFilter1Menu);
  return true;
}
bool gotoBreathCurveNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveNoiseFilter1Menu);
  return true;
}
bool gotoLfoFreqNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoFreqNoiseFilter1Menu);
  return true;
}
bool gotoLfoDepthNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoDepthNoiseFilter1Menu);
  return true;
}
bool gotoLfoBreathNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoBreathNoiseFilter1Menu);
  return true;
}
bool gotoLfoThresholdNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoThresholdNoiseFilter1Menu);
  return true;
}
bool gotoSweepTimeNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeNoiseFilter1Menu);
  return true;
}
bool gotoSweepDepthNoiseFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthNoiseFilter1Menu);
  return true;
}

// patchNoiseFilter2Menu Functions
bool gotoModeNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listModeNoiseFilter2Menu);
  return true;
}
bool gotoFreqNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFreqNoiseFilter2Menu);
  return true;
}
bool gotoQNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listQNoiseFilter2Menu);
  return true;
}
bool gotoKeyFollowNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyFollowNoiseFilter2Menu);
  return true;
}
bool gotoBreathModNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathModNoiseFilter2Menu);
  return true;
}
bool gotoBreathCurveNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveNoiseFilter2Menu);
  return true;
}
bool gotoLfoFreqNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoFreqNoiseFilter2Menu);
  return true;
}
bool gotoLfoDepthNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoDepthNoiseFilter2Menu);
  return true;
}
bool gotoLfoBreathNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoBreathNoiseFilter2Menu);
  return true;
}
bool gotoLfoThresholdNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoThresholdNoiseFilter2Menu);
  return true;
}
bool gotoSweepTimeNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeNoiseFilter2Menu);
  return true;
}
bool gotoSweepDepthNoiseFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthNoiseFilter2Menu);
  return true;
}

// patchNoiseMenu Functions
bool gotoNoiseTimeMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listNoiseTimeMenu);
  return true;
}
bool gotoNoiseBreathMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listNoiseBreathMenu);
  return true;
}
bool gotoNoiseLevelMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listNoiseLevelMenu);
  return true;
}

bool gotoKeyTriggerMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyTriggerMenu);
  return true;
}
bool gotoOctButtonLevelMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listOctButtonLevelMenu);
  return true;
}
bool gotoBendRangeMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBendRangeMenu);
  return true;
}
bool gotoBendStepMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBendStepMenu);
  return true;
}





// systemAdjMenu Menu functions
bool gotoAuxInMenu() {
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  myMenu.setCurrentMenu(&listAuxInMenu);
  sprintf(myMenu.str_oledbuf,"  %d   ", mix_linein);
  display.println(myMenu.str_oledbuf);
  return true;
}

bool gotoOctaveMenu() {
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  myMenu.setCurrentMenu(&listOctaveMenu);
  sprintf(myMenu.str_oledbuf,"  %d   ", Octave);
  display.println(myMenu.str_oledbuf);
  return true;
}

bool gotoSemiMenu() {
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  myMenu.setCurrentMenu(&listSemiMenu);
  sprintf(myMenu.str_oledbuf,"  %d   ", Transpose);
  display.println(myMenu.str_oledbuf);
  return true;
}

bool gotoCentsMenu() {
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  myMenu.setCurrentMenu(&listCentsMenu);
  sprintf(myMenu.str_oledbuf,"  %d   ", FineTuneCents);
  display.println(myMenu.str_oledbuf);
  return true;
}

bool gotoBreathCCMenu() {
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  display.clearDisplay(); // erase display
  display.display(); // refresh display
  myMenu.setCurrentMenu(&listBreathCCMenu);
  //TODO: handle Channel Aftertouch
  sprintf(myMenu.str_oledbuf,"  %d   ", breath_cc);
  display.println(myMenu.str_oledbuf);
  return true;
}



