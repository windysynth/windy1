// MenuItemsListsFuncs.h
//
//#pragma once
#include <stdint.h>
#include <string>
#include "OledMenu.h"
#include "patches.h"


extern Adafruit_SSD1306 display;

extern int currentPatch;
extern int current_patchNumber;
extern bool patchLoaded[];

extern Encoder knob;
extern Bounce knobButton;
extern float longKnobButtonPressTime; 

//extern OledMenu myMenu;

extern MenuItem topMenu[]; 
extern MenuItem escapeMenu[];
extern MenuItem volAdjustMenu[]; 
extern bool volAdjustFun();
extern MenuItem patchSelectMenu[];
extern bool patchSelectFun();
extern MenuItem patchResetMenu[];
extern bool patchResetFun();
extern MenuItem patchCopyMenu[];
extern bool patchCopyFun();
extern bool patchCopyEditsFun();
extern MenuItem patchPasteMenu[];
extern bool patchPasteFun();
extern MenuItem patchSwapMenu[];
extern bool patchSwapFun();


extern MenuItem patchFxMenu[];
extern MenuItem fxCopyMenu[];
extern bool fxCopyFun();
extern MenuItem fxPasteMenu[];
extern bool fxPasteFun();
extern MenuItem fxSwapMenu[];
extern bool fxSwapFun();
extern MenuItem delayLevelMenu[];
extern bool delayLevelFun();
extern MenuItem delayTimeLMenu[];
extern bool delayTimeLFun();
extern MenuItem delayPongMenu[];
extern bool delayPongFun();
extern MenuItem delayFeedbackMenu[];
extern bool delayFeedbackFun();
extern MenuItem delayDampMenu[];
extern bool delayDampFun();
extern MenuItem reverbLevelMenu[];
extern bool reverbLevelFun();
extern MenuItem reverbTimeMenu[];
extern bool reverbTimeFun();
extern MenuItem reverbDensityMenu[];
extern bool reverbDensityFun();
extern MenuItem reverbDampMenu[];
extern bool reverbDampFun();
extern MenuItem chorusOnMenu[];
extern bool chorusOnFun();
extern MenuItem chorusDryMenu[];
extern bool chorusDryFun();
extern MenuItem chorusLfoFMenu[];
extern bool chorusLfoFFun();
extern MenuItem chorusFeedbackMenu[];
extern bool chorusFeedbackFun();
extern MenuItem chorusDelayMenu[];
extern bool chorusDelay1Fun();
extern MenuItem chorusModMenu[];
extern bool chorusMod1Fun();
extern MenuItem chorusWetMenu[];
extern bool chorusWet1Fun();
extern MenuItem chorusDelayMenu[];
extern bool chorusDelay2Fun();
extern MenuItem chorusModMenu[];
extern bool chorusMod2Fun();
extern MenuItem chorusWetMenu[];
extern bool chorusWet2Fun();

extern MenuItem patchOsc1Menu[];
extern MenuItem octOsc1Menu[];
extern bool octOsc1Fun();
extern MenuItem semiOsc1Menu[];
extern bool semiOsc1Fun();
extern MenuItem centsOsc1Menu[];
extern bool centsOsc1Fun();
extern MenuItem beatOsc1Menu[];
extern bool beatOsc1Fun();
extern MenuItem sawOsc1Menu[];
extern bool sawOsc1Fun();
extern MenuItem triOsc1Menu[];
extern bool triOsc1Fun();
extern MenuItem pulesOsc1Menu[];
extern bool pulseOsc1Fun();
extern MenuItem PWOsc1Menu[];
extern bool PWOsc1Fun();
extern MenuItem pwmFreqOsc1Menu[];
extern bool pwmFreqOsc1Fun();
extern MenuItem pwmDepthOsc1Menu[];
extern bool pwmDepthOsc1Fun();
extern MenuItem sweepTimeOsc1Menu[];
extern bool sweepTimeOsc1Fun();
extern MenuItem sweepDepthOsc1Menu[];
extern bool sweepDepthOsc1Fun();
extern MenuItem breathAttainOsc1Menu[];
extern bool breathAttainOsc1Fun();
extern MenuItem breathDepthOsc1Menu[];
extern bool breathDepthOsc1Fun();
extern MenuItem breathThresholdOsc1Menu[];
extern bool breathThresholdOsc1Fun();
extern MenuItem breathCurveOsc1Menu[];
extern bool breathCurveOsc1Fun();
extern MenuItem levelOsc1Menu[];
extern bool levelOsc1Fun();

extern MenuItem patchOsc2Menu[];
extern MenuItem octOsc2Menu[];
extern bool octOsc2Fun();
extern MenuItem semiOsc2Menu[];
extern bool semiOsc2Fun();
extern MenuItem centsOsc2Menu[];
extern bool centsOsc2Fun();
extern MenuItem beatOsc2Menu[];
extern bool beatOsc2Fun();
extern MenuItem sawOsc2Menu[];
extern bool sawOsc2Fun();
extern MenuItem triOsc2Menu[];
extern bool triOsc2Fun();
extern MenuItem pulesOsc2Menu[];
extern bool pulseOsc2Fun();
extern MenuItem PWOsc2Menu[];
extern bool PWOsc2Fun();
extern MenuItem pwmFreqOsc2Menu[];
extern bool pwmFreqOsc2Fun();
extern MenuItem pwmDepthOsc2Menu[];
extern bool pwmDepthOsc2Fun();
extern MenuItem sweepTimeOsc2Menu[];
extern bool sweepTimeOsc2Fun();
extern MenuItem sweepDepthOsc2Menu[];
extern bool sweepDepthOsc2Fun();
extern MenuItem breathAttainOsc2Menu[];
extern bool breathAttainOsc2Fun();
extern MenuItem breathDepthOsc2Menu[];
extern bool breathDepthOsc2Fun();
extern MenuItem breathThresholdOsc2Menu[];
extern bool breathThresholdOsc2Fun();
extern MenuItem breathCurveOsc2Menu[];
extern bool breathCurveOsc2Fun();
extern MenuItem levelOsc2Menu[];
extern bool levelOsc2Fun();
extern MenuItem xFadeOsc2Menu[];
extern bool xFadeOsc2Fun();

extern MenuItem patchOscFilter1Menu[];
extern MenuItem linkOscFiltersMenu[];
extern bool linkOscFiltersFun();
extern MenuItem modeOscFilter1Menu[];
extern bool modeOscFilter1Fun();
extern MenuItem freqOscFilter1Menu[];
extern bool freqOscFilter1Fun();
extern MenuItem qOscFilter1Menu[];
extern bool qOscFilter1Fun();
extern MenuItem keyFollowOscFilter1Menu[];
extern bool keyFollowOscFilter1Fun();
extern MenuItem breathModOscFilter1Menu[];
extern bool breathModOscFilter1Fun();
extern MenuItem breathCurveOscFilter1Menu[];
extern bool breathCurveOscFilter1Fun();
extern MenuItem lfoFreqOscFilter1Menu[];
extern bool lfoFreqOscFilter1Fun();
extern MenuItem lfoDepthOscFilter1Menu[];
extern bool lfoDepthOscFilter1Fun();
extern MenuItem lfoBreathOscFilter1Menu[];
extern bool lfoBreathOscFilter1Fun();
extern MenuItem lfoThreshOscFilter1Menu[];
extern bool lfoThreshOscFilter1Fun();
extern MenuItem PROGMEM sweepTimeOscFilter1Menu[];
extern bool sweepTimeOscFilter1Fun();
extern MenuItem sweepDepthOscFilter1Menu[];
extern bool sweepDepthOscFilter1Fun();

extern MenuItem patchOscFilter2Menu[];
extern MenuItem modeOscFilter2Menu[];
extern bool modeOscFilter2Fun();
extern MenuItem freqOscFilter2Menu[];
extern bool freqOscFilter2Fun();
extern MenuItem qOscFilter2Menu[];
extern bool qOscFilter2Fun();
extern MenuItem keyFollowOscFilter2Menu[];
extern bool keyFollowOscFilter2Fun();
extern MenuItem breathModOscFilter2Menu[];
extern bool breathModOscFilter2Fun();
extern MenuItem breathCurveOscFilter2Menu[];
extern bool breathCurveOscFilter2Fun();
extern MenuItem lfoFreqOscFilter2Menu[];
extern bool lfoFreqOscFilter2Fun();
extern MenuItem lfoDepthOscFilter2Menu[];
extern bool lfoDepthOscFilter2Fun();
extern MenuItem lfoBreathOscFilter2Menu[];
extern bool lfoBreathOscFilter2Fun();
extern MenuItem lfoThreshOscFilter2Menu[];
extern bool lfoThreshOscFilter2Fun();
extern MenuItem PROGMEM sweepTimeOscFilter2Menu[];
extern bool sweepTimeOscFilter2Fun();
extern MenuItem sweepDepthOscFilter2Menu[];
extern bool sweepDepthOscFilter2Fun();

extern MenuItem patchNoiseFilter3Menu[];
extern MenuItem linkNoiseFiltersMenu[];
extern bool linkNoiseFiltersFun();
extern MenuItem modeNoiseFilter3Menu[];
extern bool modeNoiseFilter3Fun();
extern MenuItem freqNoiseFilter3Menu[];
extern bool freqNoiseFilter3Fun();
extern MenuItem qNoiseFilter3Menu[];
extern bool qNoiseFilter3Fun();
extern MenuItem keyFollowNoiseFilter3Menu[];
extern bool keyFollowNoiseFilter3Fun();
extern MenuItem breathModNoiseFilter3Menu[];
extern bool breathModNoiseFilter3Fun();
extern MenuItem breathCurveNoiseFilter3Menu[];
extern bool breathCurveNoiseFilter3Fun();
extern MenuItem lfoFreqNoiseFilter3Menu[];
extern bool lfoFreqNoiseFilter3Fun();
extern MenuItem lfoDepthNoiseFilter3Menu[];
extern bool lfoDepthNoiseFilter3Fun();
extern MenuItem lfoBreathNoiseFilter3Menu[];
extern bool lfoBreathNoiseFilter3Fun();
extern MenuItem lfoThreshNoiseFilter3Menu[];
extern bool lfoThreshNoiseFilter3Fun();
extern MenuItem PROGMEM sweepTimeNoiseFilter3Menu[];
extern bool sweepTimeNoiseFilter3Fun();
extern MenuItem sweepDepthNoiseFilter3Menu[];
extern bool sweepDepthNoiseFilter3Fun();

extern MenuItem patchNoiseFilter4Menu[];
extern MenuItem modeNoiseFilter4Menu[];
extern bool modeNoiseFilter4Fun();
extern MenuItem freqNoiseFilter4Menu[];
extern bool freqNoiseFilter4Fun();
extern MenuItem qNoiseFilter4Menu[];
extern bool qNoiseFilter4Fun();
extern MenuItem keyFollowNoiseFilter4Menu[];
extern bool keyFollowNoiseFilter4Fun();
extern MenuItem breathModNoiseFilter4Menu[];
extern bool breathModNoiseFilter4Fun();
extern MenuItem breathCurveNoiseFilter4Menu[];
extern bool breathCurveNoiseFilter4Fun();
extern MenuItem lfoFreqNoiseFilter4Menu[];
extern bool lfoFreqNoiseFilter4Fun();
extern MenuItem lfoDepthNoiseFilter4Menu[];
extern bool lfoDepthNoiseFilter4Fun();
extern MenuItem lfoBreathNoiseFilter4Menu[];
extern bool lfoBreathNoiseFilter4Fun();
extern MenuItem lfoThreshNoiseFilter4Menu[];
extern bool lfoThreshNoiseFilter4Fun();
extern MenuItem PROGMEM sweepTimeNoiseFilter4Menu[];
extern bool sweepTimeNoiseFilter4Fun();
extern MenuItem sweepDepthNoiseFilter4Menu[];
extern bool sweepDepthNoiseFilter4Fun();

extern MenuItem patchNoiseMenu[];
extern MenuItem noiseTimeMenu[];
extern bool noiseTimeFun();
extern MenuItem noiseBreathMenu[];
extern bool noiseBreathFun();
extern MenuItem noiseLevelMenu[];
extern bool noiseLevelFun();

extern MenuItem patchFormantMenu[];
extern bool formantSelectFun();

extern MenuItem patchAmpMenu[];
extern bool patchAmpAdjustFun();

extern MenuItem patchCommonMenu[];
extern MenuItem keyTriggerMenu[];
extern bool keyTriggerFun();
extern MenuItem octButtonLevelMenu[];
extern bool octButtonLevelFun();
extern MenuItem bendRangeMenu[];
extern bool bendRangeFun();
extern MenuItem bendStepMenu[];
extern bool bendStepFun();

extern MenuItem systemAdjMenu[]; 
extern MenuItem auxInMenu[];
extern bool auxInFun();
extern MenuItem octaveMenu[];
extern bool octaveFun();
extern MenuItem semiMenu[];
extern bool semiFun();
extern MenuItem centsMenu[];
extern bool centsFun();
extern MenuItem breathCCMenu[];
extern bool breathCCFun();

extern MenuList listEscapeMenu;
extern MenuList listTopMenu;
extern MenuList listVolAdjustMenu;
extern MenuList listPatchSelectMenu;
extern MenuList listPatchResetMenu;
extern MenuList listPatchCopyMenu;
extern MenuList listPatchPasteMenu;
extern MenuList listPatchSwapMenu;
extern MenuList listPatchFxMenu;
extern MenuList listPatchOsc1Menu;
extern MenuList listPatchOsc2Menu;
extern MenuList listPatchOscFilter1Menu;
extern MenuList listPatchOscFilter2Menu;
extern MenuList listPatchNoiseFilter3Menu;
extern MenuList listPatchNoiseFilter4Menu;
extern MenuList listPatchNoiseMenu;
extern MenuList listPatchFormantMenu;
extern MenuList listPatchAmpMenu;
extern MenuList listPatchCommonMenu;

extern MenuList listSystemAdjMenu;

extern MenuList listFxCopyMenu;
extern MenuList listFxPasteMenu;
extern MenuList listFxSwapMenu;
extern MenuList listDelayLevelMenu;
extern MenuList listDelayTimeLMenu;
extern MenuList listDelayPongMenu;
extern MenuList listDelayFeedbackMenu;
extern MenuList listDelayDampMenu;
extern MenuList listReverbLevelMenu;
extern MenuList listReverbTimeMenu;
extern MenuList listReverbDensityMenu;
extern MenuList listReverbDampMenu;
extern MenuList listChorusOnMenu;
extern MenuList listChorusDryMenu;
extern MenuList listChorusLfoFMenu;
extern MenuList listChorusFeedbackMenu;
extern MenuList listChorusDelay1Menu;
extern MenuList listChorusMod1Menu;
extern MenuList listChorusWet1Menu;
extern MenuList listChorusDelay2Menu;
extern MenuList listChorusMod2Menu;
extern MenuList listChorusWet2Menu;

extern MenuList listOscOsc1Menu;
extern MenuList listSemiOsc1Menu;
extern MenuList listCentsOsc1Menu;
extern MenuList listBeatOsc1Menu;
extern MenuList listSawOsc1Menu;
extern MenuList listTriOsc1Menu;
extern MenuList listPulseOsc1Menu;
extern MenuList listPWOsc1Menu;
extern MenuList listPwmFreqOsc1Menu;
extern MenuList listPwmDepthOsc1Menu;
extern MenuList listSweepTimeOsc1Menu;
extern MenuList listSweepDepthOsc1Menu;
extern MenuList listBreathAttainOsc1Menu;
extern MenuList listBreathDepthOsc1Menu;
extern MenuList listBreathThresholdOsc1Menu;
extern MenuList listBreathCurveOsc1Menu;
extern MenuList listLevelOsc1Menu;

extern MenuList listOscOsc2Menu;
extern MenuList listSemiOsc2Menu;
extern MenuList listCentsOsc2Menu;
extern MenuList listBeatOsc2Menu;
extern MenuList listSawOsc2Menu;
extern MenuList listTriOsc2Menu;
extern MenuList listPulseOsc2Menu;
extern MenuList listPWOsc2Menu;
extern MenuList listPwmFreqOsc2Menu;
extern MenuList listPwmDepthOsc2Menu;
extern MenuList listSweepTimeOsc2Menu;
extern MenuList listSweepDepthOsc2Menu;
extern MenuList listBreathAttainOsc2Menu;
extern MenuList listBreathDepthOsc2Menu;
extern MenuList listBreathThresholdOsc2Menu;
extern MenuList listBreathCurveOsc2Menu;
extern MenuList listLevelOsc2Menu;
extern MenuList listXFadeOsc2Menu;

extern MenuList listLinkOscFiltersMenu;
extern MenuList listModeOscFilter1Menu;
extern MenuList listFreqOscFilter1Menu;
extern MenuList listQOscFilter1Menu;
extern MenuList listKeyFollowOscFilter1Menu;
extern MenuList listBreathModOscFilter1Menu;
extern MenuList listBreathCurveOscFilter1Menu;
extern MenuList listLfoFreqOscFilter1Menu;
extern MenuList listLfoDepthOscFilter1Menu;
extern MenuList listLfoBreathOscFilter1Menu;
extern MenuList listLfoThresholdOscFilter1Menu;
extern MenuList listSweepTimeOscFilter1Menu;
extern MenuList listSweepDepthOscFilter1Menu;

extern MenuList listModeOscFilter2Menu;
extern MenuList listFreqOscFilter2Menu;
extern MenuList listQOscFilter2Menu;
extern MenuList listKeyFollowOscFilter2Menu;
extern MenuList listBreathModOscFilter2Menu;
extern MenuList listBreathCurveOscFilter2Menu;
extern MenuList listLfoFreqOscFilter2Menu;
extern MenuList listLfoDepthOscFilter2Menu;
extern MenuList listLfoBreathOscFilter2Menu;
extern MenuList listLfoThresholdOscFilter2Menu;
extern MenuList listSweepTimeOscFilter2Menu;
extern MenuList listSweepDepthOscFilter2Menu;

extern MenuList listLinkNoiseFiltersMenu;
extern MenuList listModeNoiseFilter3Menu;
extern MenuList listFreqNoiseFilter3Menu;
extern MenuList listQNoiseFilter3Menu;
extern MenuList listKeyFollowNoiseFilter3Menu;
extern MenuList listBreathModNoiseFilter3Menu;
extern MenuList listBreathCurveNoiseFilter3Menu;
extern MenuList listLfoFreqNoiseFilter3Menu;
extern MenuList listLfoDepthNoiseFilter3Menu;
extern MenuList listLfoBreathNoiseFilter3Menu;
extern MenuList listLfoThresholdNoiseFilter3Menu;
extern MenuList listSweepTimeNoiseFilter3Menu;
extern MenuList listSweepDepthNoiseFilter3Menu;

extern MenuList listModeNoiseFilter4Menu;
extern MenuList listFreqNoiseFilter4Menu;
extern MenuList listQNoiseFilter4Menu;
extern MenuList listKeyFollowNoiseFilter4Menu;
extern MenuList listBreathModNoiseFilter4Menu;
extern MenuList listBreathCurveNoiseFilter4Menu;
extern MenuList listLfoFreqNoiseFilter4Menu;
extern MenuList listLfoDepthNoiseFilter4Menu;
extern MenuList listLfoBreathNoiseFilter4Menu;
extern MenuList listLfoThresholdNoiseFilter4Menu;
extern MenuList listSweepTimeNoiseFilter4Menu;
extern MenuList listSweepDepthNoiseFilter4Menu;

extern MenuList listNoiseTimeMenu;
extern MenuList listNoiseBreathMenu;
extern MenuList listNoiseLevelMenu;

extern MenuList listKeyTriggerMenu;
extern MenuList listOctButtonLevelMenu;
extern MenuList listBendRangeMenu;
extern MenuList listBendStepMenu;

extern MenuList listAuxInMenu;
extern MenuList listOctaveMenu;
extern MenuList listSemiMenu;
extern MenuList listCentsMenu;
extern MenuList listBreathCCMenu;

/*****************************
 * the definitions are in globals.cpp 
 ******************************
 * Define the functions you want your menu to call
 * They can be blocking or non-blocking
 * They should take no arguments and return a boolean
 * true if the function is finished and doesn't want to run again
 * false if the function is not done and wants to be called again
 ******************************
 *****************************/

//---------------------
// topMenu Functions
//---------------------

extern bool gotoVolAdjMenu();
extern bool gotoPatchSelectMenu();
extern bool gotoPatchResetMenu();
extern bool gotoPatchCopyMenu();
extern bool gotoPatchPasteMenu();
extern bool gotoPatchSwapMenu();
extern bool gotoPatchFxMenu();
extern bool gotoPatchOsc1Menu();
extern bool gotoPatchOsc2Menu();
extern bool gotoPatchOscFilter1Menu();
extern bool gotoPatchOscFilter2Menu();
extern bool gotoPatchNoiseFilter3Menu();
extern bool gotoPatchNoiseFilter4Menu();
extern bool gotoPatchNoiseMenu();
extern bool gotoPatchFormantMenu();
extern bool gotoPatchAmpMenu();
extern bool gotoPatchCommonMenu();

extern bool gotoSystemAdjMenu();

extern bool gotoTopMenu();
extern bool goUpOneMenu();


// patchFxMenu functions
extern bool gotoFxCopyMenu();
extern bool gotoFxPasteMenu();
extern bool gotoFxSwapMenu();
extern bool gotoDelayLevelMenu();
extern bool gotoDelayTimeLMenu();
extern bool gotoDelayPongMenu();
extern bool gotoDelayFeedbackMenu();
extern bool gotoDelayDampMenu();
extern bool gotoReverbLevelMenu();
extern bool gotoReverbTimeMenu();
extern bool gotoReverbDensityMenu();
extern bool gotoReverbDampMenu();
extern bool gotoChorusOnMenu();
extern bool gotoChorusDryMenu();
extern bool gotoChorusLfoFMenu();
extern bool gotoChorusFeedbackMenu();
extern bool gotoChorusDelay1Menu();
extern bool gotoChorusMod1Menu();
extern bool gotoChorusWet1Menu();
extern bool gotoChorusDelay2Menu();
extern bool gotoChorusMod2Menu();
extern bool gotoChorusWet2Menu();

// patchOsc1Menu functions
extern bool gotoOctOsc1Menu();
extern bool gotoSemiOsc1Menu();
extern bool gotoCentsOsc1Menu();
extern bool gotoBeatOsc1Menu();
extern bool gotoSawOsc1Menu();
extern bool gotoTriOsc1Menu();
extern bool gotoPulseOsc1Menu();
extern bool gotoPWOsc1Menu();
extern bool gotoPwmFreqOsc1Menu();
extern bool gotoPwmDepthOsc1Menu();
extern bool gotoSweepTimeOsc1Menu();
extern bool gotoSweepDepthOsc1Menu();
extern bool gotoBreathAttainOsc1Menu();
extern bool gotoBreathDepthOsc1Menu();
extern bool gotoBreathThresholdOsc1Menu();
extern bool gotoBreathCurveOsc1Menu();
extern bool gotoLevelOsc1Menu();

// patchOsc2Menu functions
extern bool gotoOctOsc2Menu();
extern bool gotoSemiOsc2Menu();
extern bool gotoCentsOsc2Menu();
extern bool gotoBeatOsc2Menu();
extern bool gotoSawOsc2Menu();
extern bool gotoTriOsc2Menu();
extern bool gotoPulseOsc2Menu();
extern bool gotoPWOsc2Menu();
extern bool gotoPwmFreqOsc2Menu();
extern bool gotoPwmDepthOsc2Menu();
extern bool gotoSweepTimeOsc2Menu();
extern bool gotoSweepDepthOsc2Menu();
extern bool gotoBreathAttainOsc2Menu();
extern bool gotoBreathDepthOsc2Menu();
extern bool gotoBreathThresholdOsc2Menu();
extern bool gotoBreathCurveOsc2Menu();
extern bool gotoLevelOsc2Menu();
extern bool gotoXFadeOsc2Menu();


// patchOscFilter1Menu Functions 
extern bool gotoLinkOscFiltersMenu();
extern bool gotoModeOscFilter1Menu();
extern bool gotoFreqOscFilter1Menu();
extern bool gotoQOscFilter1Menu();
extern bool gotoKeyFollowOscFilter1Menu();
extern bool gotoBreathModOscFilter1Menu();
extern bool gotoBreathCurveOscFilter1Menu();
extern bool gotoLfoFreqOscFilter1Menu();
extern bool gotoLfoDepthOscFilter1Menu();
extern bool gotoLfoBreathOscFilter1Menu();
extern bool gotoLfoThresholdOscFilter1Menu();
extern bool gotoSweepTimeOscFilter1Menu();
extern bool gotoSweepDepthOscFilter1Menu();

// patchOscFilter2Menu Functions
extern bool gotoModeOscFilter2Menu();
extern bool gotoFreqOscFilter2Menu();
extern bool gotoQOscFilter2Menu();
extern bool gotoKeyFollowOscFilter2Menu();
extern bool gotoBreathModOscFilter2Menu();
extern bool gotoBreathCurveOscFilter2Menu();
extern bool gotoLfoFreqOscFilter2Menu();
extern bool gotoLfoDepthOscFilter2Menu();
extern bool gotoLfoBreathOscFilter2Menu();
extern bool gotoLfoThresholdOscFilter2Menu();
extern bool gotoSweepTimeOscFilter2Menu();
extern bool gotoSweepDepthOscFilter2Menu();

// patchNoiseFilter3Menu Functions 
extern bool gotoLinkNoiseFiltersMenu();
extern bool gotoModeNoiseFilter3Menu();
extern bool gotoFreqNoiseFilter3Menu();
extern bool gotoQNoiseFilter3Menu();
extern bool gotoKeyFollowNoiseFilter3Menu();
extern bool gotoBreathModNoiseFilter3Menu();
extern bool gotoBreathCurveNoiseFilter3Menu();
extern bool gotoLfoFreqNoiseFilter3Menu();
extern bool gotoLfoDepthNoiseFilter3Menu();
extern bool gotoLfoBreathNoiseFilter3Menu();
extern bool gotoLfoThresholdNoiseFilter3Menu();
extern bool gotoSweepTimeNoiseFilter3Menu();
extern bool gotoSweepDepthNoiseFilter3Menu();

// patchNoiseFilter4Menu Functions
extern bool gotoModeNoiseFilter4Menu();
extern bool gotoFreqNoiseFilter4Menu();
extern bool gotoQNoiseFilter4Menu();
extern bool gotoKeyFollowNoiseFilter4Menu();
extern bool gotoBreathModNoiseFilter4Menu();
extern bool gotoBreathCurveNoiseFilter4Menu();
extern bool gotoLfoFreqNoiseFilter4Menu();
extern bool gotoLfoDepthNoiseFilter4Menu();
extern bool gotoLfoBreathNoiseFilter4Menu();
extern bool gotoLfoThresholdNoiseFilter4Menu();
extern bool gotoSweepTimeNoiseFilter4Menu();
extern bool gotoSweepDepthNoiseFilter4Menu();

// patchNoiseMenu Functions
extern bool gotoNoiseTimeMenu();
extern bool gotoNoiseBreathMenu();
extern bool gotoNoiseLevelMenu();

// patchCommonMenu Functions
extern bool gotoKeyTriggerMenu();
extern bool gotoOctButtonLevelMenu();
extern bool gotoBendRangeMenu();
extern bool gotoBendStepMenu();


// System Adj Menu functions
extern bool gotoAuxInMenu();
extern bool gotoOctaveMenu();
extern bool gotoSemiMenu();
extern bool gotoCentsMenu();
extern bool gotoBreathCCMenu();


