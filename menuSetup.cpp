/****************************************************************
  menuSetup.cpp - Menu definitions for Windy1 synthesizer project
  --------------------------------------------------------------
  This file defines the menu structure and items for the Windy1
  synthesizer project using the MenuSystem library.

  Created by Your Name, Date.
  --------------------------------------------------------------
  License: MIT License
**************************************************************/

#include "MenuSystem.h"
#include "menuSetup.h"
#include "globals.h"

// We'll allocate menus in setup to avoid static init order problems
Menu *topMenuPtr = nullptr;
// Menu *patchSelectMenuPtr = nullptr;
Menu *patchPasteMenuPtr = nullptr;
Menu *patchSwapMenuPtr = nullptr;
Menu *patchFxMenuPtr = nullptr;
Menu *saveGlobalFxMenuPtr = nullptr;
Menu *patchOsc1MenuPtr = nullptr;
Menu *patchOsc2MenuPtr = nullptr;
Menu *patchOscFilt1MenuPtr = nullptr;
Menu *patchOscFilt2MenuPtr = nullptr;
Menu *patchNzFilt3MenuPtr = nullptr;
Menu *patchNzFilt4MenuPtr = nullptr;
Menu *patchNoiseMenuPtr = nullptr;
// Menu *patchFormantMenuPtr = nullptr;
// Menu *patchAmpMenuPtr = nullptr;
Menu *patchCommonMenuPtr = nullptr;
Menu *systemMenuPtr = nullptr;
Menu *calibrationMenuPtr = nullptr;

// menu system instance
MenuSystem *ms;

// helpers to create items
MenuItem makeValueItem(const char *title, PatchToSynthFunc p, void *valPtr, double min, double max, double step, ValueToDispFunc valToDisp)
{
    MenuItem m;
    m.title = title;
    m.type = MenuItemType::Value;
    // store pointer as uint8_t*
    m.u.value = MenuSystem::makeValue((uint8_t *)valPtr, p, min, max, step, valToDisp);
    return m;
}
MenuItem makeActionItem(const char *title, ActionFunc f)
{
    MenuItem m;
    m.title = title;
    m.type = MenuItemType::Action;
    m.u.action = f;
    return m;
}
MenuItem makeSubItem(const char *title, Menu *sm)
{
    MenuItem m;
    m.title = title;
    m.type = MenuItemType::SubMenu;
    m.u.submenu = sm;
    return m;
}
MenuItem makeNamingItem(const char *title, char *buf, uint8_t maxlen, NamingFinishFunc f, NamingAbortFunc a)
{
    MenuItem m;
    m.title = title;
    m.type = MenuItemType::Naming;
    m.u.naming = MenuSystem::makeNaming(buf, maxlen, f, a);
    return m;
}

// fill arrays
MenuItem topMenuItems[] = {
    //{"PatchSel...", MenuItemType::SubMenu, {.submenu = nullptr}},              // submenu pointer will be assigned in setup
    makeValueItem("", patchSelectFromMenu, &current_patchNumber, 0, 127, 129, nprnToDisplayCurrentPatch), // 129 = 1 with wrap around
    makeValueItem("Volume:", volAdjustFun, &vol, 0, 100, 4, nprnToDisplay03d),
    {"PatchPaste...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"PatchSwap...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"Fx...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"Osc1...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"Osc2...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"OscFilt1...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"OscFilt2...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"NoiseFilt1...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"NoiseFilt2...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"Noise...", MenuItemType::SubMenu, {.submenu = nullptr}},
    //{"Formant", MenuItemType::SubMenu, {.submenu = nullptr}},
    makeValueItem("Formant:", patchToFormant, &current_patch.nrpn_msb_common1[CCFORMANT], 0, 2, 1, nprnToDisplayFormant),
    //{"PatchAmp", MenuItemType::SubMenu, {.submenu = nullptr}},
    makeValueItem("PatchAmp:", patchToAmpLevel, &current_patch.nrpn_msb_common2[CCAMPLEVEL], 0, 127, 1, nprnToDisplay03d),
    {"PatchCommon...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"System...", MenuItemType::SubMenu, {.submenu = nullptr}},
    {"Calibration...", MenuItemType::SubMenu, {.submenu = nullptr}},
};
// MenuItem patchSelectMenuItems[] = {
//     makeActionItem("Exit w/o Save", goUpOne),
//     makeNamingItem("Name", namebuf, 20, namingFinished, namingAborted), // TODO: max Name width based on font
// };
MenuItem patchPasteMenuItems[] = {
    makeActionItem("Exit w/o pasting", goUpOne),
    makeValueItem("Destination:", pasteDestSelect, &paste_patchNumber, 0, 127, 129, nprnToDisplayDest), // 129 = 1 with wrap around
    makeNamingItem(namebuf, namebuf, 20, namingFinished, namingAborted),                                // TODO: max Name width based on font
    makeActionItem("PASTE", pastePatchToDestination),
};

MenuItem patchSwapMenuItems[] = {
    makeActionItem("Exit w/o swapping", goUpOne),
    makeValueItem("Destination:", swapDestSelect, &swap_patchNumber, 0, 127, 129, nprnToDisplayDest), // 129 = 1 with wrap around
    makeNamingItem(namebuf, namebuf, 20, namingFinished, namingAborted),                              // TODO: max Name width based on font
    makeActionItem("SWAP", swapPatchWithDestination),
};

MenuItem saveGlobalFxMenuItems[] = {
    makeActionItem("Exit w/o saving", goUpOne),
    makeActionItem("Save", saveGlobalFxSD_wrap),
};

MenuItem patchFxMenuItems[] = {
    makeActionItem("Back", goUpOne),
    makeValueItem("FXSource:", fxSourcToSynth, &fxSourcePatch, 0, 1, 1, nprnToDisplayPatchGlobal),
    {"SaveGlobalFx", MenuItemType::SubMenu, {.submenu = nullptr}},
    //{"PasteFx", MenuItemType::SubMenu, {.submenu = nullptr}},
    makeValueItem("DelayLevel:", patchToEffectsDelayLevel, getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYLEVEL), 0, 127, 1, nprnToDisplay03d),
    makeValueItem("DelayTime:", patchToEffectsDelayTimeL, getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYTIME), 0, 127, 1, nprnToDisplay04d10x),
    makeValueItem("DelayPong:", patchToEffectsDelayPong, getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYSPARE), 0, 127, -1, nprnToDisplay03dFlip),
    makeValueItem("DelayFB:", patchToEffectsDelayFeedback, getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYFEEDBACK), 0, 127, 1, nprnToDisplay03d),
    makeValueItem("DelayDamp:", patchToEffectsDelayDamp, getFxValue(EFFECTGROUPDELAY, CCEFFECTSDELAYDAMP), 0, 127, 1, nprnToDisplay03d),
    makeValueItem("RvbLevel:", patchToEffectsReverbLevel, getFxValue(EFFECTGROUPREVERB, CCEFFECTSREVERBLEVEL), 0, 127, 1, nprnToDisplay03d),
    makeValueItem("RvbTime:", patchToEffectsReverbTime, getFxValue(EFFECTGROUPREVERB, CCEFFECTSREVERBTIME), 0, 50, 1, nprnToDisplay04d100x),
    makeValueItem("RvbDensity:", patchToEffectsReverbDenseEarly, getFxValue(EFFECTGROUPREVERB, CCEFFECTSREVERBDENSEEARLY), 0, 127, 1, nprnToDisplay03d),
    makeValueItem("RvbDamp:", patchToEffectsReverbDamp, getFxValue(EFFECTGROUPREVERB, CCEFFECTSREVERBDAMP), 54, 74, 1, nprnToDisplay03dm),
    makeValueItem("ChrsOn:", patchToChorusOn, getFxValue(EFFECTGROUPCOMMON1, CCCHORUSON), 0, 2, 1, nprnToDisplayChorus),
    makeValueItem("ChrsDry:", patchToEffectsChorusDryLevel, getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSDRYLEVEL), 0, 127, 1, nprnToDisplay03d),
    makeValueItem("ChrsLfoF:", patchToEffectsChorusLfoFreq, getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSLFOFREQ), 0, 127, 1, nprnToDisplay2p1fHzDiv10),
    makeValueItem("ChrsFB:", patchToEffectsChorusFeedback, getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSFEEDBACK), 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("ChrsDly1:", patchToEffectsChorusDelay1, getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY1), 0, 127, 1, nprnToDisplay03dmS),
    makeValueItem("ChrsMod1:", patchToEffectsChorusMod1, getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD1), 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("ChrsWet1:", patchToEffectsChorusWet1, getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET1), 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("ChrsDly2:", patchToEffectsChorusDelay2, getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY2), 0, 127, 1, nprnToDisplay03dmS),
    makeValueItem("ChrsMod2:", patchToEffectsChorusMod2, getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD2), 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("ChrsWet2:", patchToEffectsChorusWet2, getFxValue(EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET2), 0, 127, 1, nprnToDisplay03dm),
};

MenuItem patchOsc1MenuItems[] = {
    makeActionItem("Back", goUpOne),
    makeValueItem("OctOsc1:", patchToOctOsc1, &current_patch.nrpn_msb_osc1[CCOCTOSC1], 62, 66, 1, nprnToDisplay03dm),
    makeValueItem("SemiOsc1:", patchToSemiOsc1, &current_patch.nrpn_msb_osc1[CCSEMIOSC1], 52, 72, 1, nprnToDisplay03dm),
    makeValueItem("CentOsc1:", patchToFineOsc1, &current_patch.nrpn_msb_osc1[CCFINEOSC1], 14, 114, 1, nprnToDisplay03dm),
    makeValueItem("BeatOsc1:", patchToBeatOsc1, &current_patch.nrpn_msb_osc1[CCBEATOSC1], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("SawOsc1:", patchToSawOsc1, &current_patch.nrpn_msb_osc1[CCSAWOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("TriOsc1:", patchToTriOsc1, &current_patch.nrpn_msb_osc1[CCTRIOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("PulseOsc1:", patchToPulseOsc1, &current_patch.nrpn_msb_osc1[CCPULSEOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("PWOsc1:", patchToPwOsc1, &current_patch.nrpn_msb_osc1[CCPWOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("PwmFOsc1:", patchToPwmFreqOsc1, &current_patch.nrpn_msb_osc1[CCPWMFREQOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("PwmDOsc1:", patchToPwmDepthOsc1, &current_patch.nrpn_msb_osc1[CCPWMDEPTHOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpTOsc1:", patchToSweepTimeOsc1, &current_patch.nrpn_msb_osc1[CCSWEEPTIMEOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpDOsc1:", patchToSweepDepthOsc1, &current_patch.nrpn_msb_osc1[CCSWEEPDEPTHOSC1], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("BAtnOsc1:", patchToBreathAttainOsc1, &current_patch.nrpn_msb_osc1[CCBREATHATTAINOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("BDptOsc1:", patchToBreathDepthOsc1, &current_patch.nrpn_msb_osc1[CCBREATHDEPTHOSC1], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("BThrOsc1:", patchToBreathThreshOsc1, &current_patch.nrpn_msb_osc1[CCBREATHTHRESHOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("BCrvOsc1:", patchToBreathOscCurveLines1, &current_patch.nrpn_msb_osc1[CCBREATHCURVEOSC1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LvlOsc1:", patchToLevelOsc1, &current_patch.nrpn_msb_osc1[CCLEVELOSC1], 0, 127, 1, nprnToDisplay03d),
};

MenuItem patchOsc2MenuItems[] = {
    makeActionItem("Back", goUpOne),
    makeValueItem("OctOsc2:", patchToOctOsc2, &current_patch.nrpn_msb_osc2[CCOCTOSC2], 62, 66, 1, nprnToDisplay03dm),
    makeValueItem("SemiOsc2:", patchToSemiOsc2, &current_patch.nrpn_msb_osc2[CCSEMIOSC2], 52, 72, 1, nprnToDisplay03dm),
    makeValueItem("CentOsc2:", patchToFineOsc2, &current_patch.nrpn_msb_osc2[CCFINEOSC2], 14, 114, 1, nprnToDisplay03dm),
    makeValueItem("BeatOsc2:", patchToBeatOsc2, &current_patch.nrpn_msb_osc2[CCBEATOSC2], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("SawOsc2:", patchToSawOsc2, &current_patch.nrpn_msb_osc2[CCSAWOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("TriOsc2:", patchToTriOsc2, &current_patch.nrpn_msb_osc2[CCTRIOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("PulseOsc2:", patchToPulseOsc2, &current_patch.nrpn_msb_osc2[CCPULSEOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("PWOsc2:", patchToPwOsc2, &current_patch.nrpn_msb_osc2[CCPWOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("PwmFOsc2:", patchToPwmFreqOsc2, &current_patch.nrpn_msb_osc2[CCPWMFREQOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("PwmDOsc2:", patchToPwmDepthOsc2, &current_patch.nrpn_msb_osc2[CCPWMDEPTHOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpTOsc2:", patchToSweepTimeOsc2, &current_patch.nrpn_msb_osc2[CCSWEEPTIMEOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpDOsc2:", patchToSweepDepthOsc2, &current_patch.nrpn_msb_osc2[CCSWEEPDEPTHOSC2], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("BAtnOsc2:", patchToBreathAttainOsc2, &current_patch.nrpn_msb_osc2[CCBREATHATTAINOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("BDptOsc2:", patchToBreathDepthOsc2, &current_patch.nrpn_msb_osc2[CCBREATHDEPTHOSC2], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("BThrOsc2:", patchToBreathThreshOsc2, &current_patch.nrpn_msb_osc2[CCBREATHTHRESHOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("BCrvOsc2:", patchToBreathOscCurveLines2, &current_patch.nrpn_msb_osc2[CCBREATHCURVEOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LvlOsc2:", patchToLevelOsc2, &current_patch.nrpn_msb_osc2[CCLEVELOSC2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("XFadeOsc2:", patchToXFade, &current_patch.nrpn_msb_common1[CCXFADE], 0, 1, 1, nprnToDisplayOnOff),
};

MenuItem patchOscFilt1MenuItems[]{
    makeActionItem("Back", goUpOne),
    makeValueItem("LnkOscFlt:", patchToLinkOscFilters, &current_patch.nrpn_msb_common1[CCLINKOSCFILTERS], 1, 2, 1, nprnToDisplayOnOff),
    makeValueItem("ModeOFlt1:", patchToModeOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCMODEOSCFILTER1], 0, 4, 1, nprnToDisplayFilterMode),
    makeValueItem("FreqOFlt1:", patchToFreqOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCFREQOSCFILTER1], 36, 124, 1, nprnToDisplayFilterFreqHz),
    makeValueItem("Q OscFlt1:", patchToQFactorOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCQFACTOROSCFILTER1], 5, 127, 1, nprnToDisplay2p1fHzDiv10),
    makeValueItem("KeyFlwOF1:", patchToKeyFollowOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCKEYFOLLOWOSCFILTER1], 52, 88, 1, nprnToDisplay03dm),
    makeValueItem("BrModOF1:", patchToBreathModOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCBREATHMODOSCFILTER1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("BrCrvOF1:", patchToBreathOscFiltCurveLines1, &current_patch.nrpn_msb_osc_filt1[CCBREATHCURVEOSCFILTER1], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("LfoF OF1:", patchToLfoFreqOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCLFOFREQOSCFILTER1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LfoD OF1:", patchToLfoDepthOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCLFODEPTHOSCFILTER1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LfoBrOF1:", patchToLfoBreathOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCLFOBREATHOSCFILTER1], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("LfoThOF1:", patchToLfoThreshOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCLFOTHRESHOSCFILTER1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpT OF1:", patchToSweepTimeOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCSWEEPTIMEOSCFILTER1], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpD OF1:", patchToSweepDepthOscFilter1, &current_patch.nrpn_msb_osc_filt1[CCSWEEPDEPTHOSCFILTER1], 0, 127, 1, nprnToDisplay03dm),
};
MenuItem patchOscFilt2MenuItems[]{
    makeActionItem("Back", goUpOne),
    makeValueItem("ModeOFlt2:", patchToModeOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCMODEOSCFILTER2], 0, 4, 1, nprnToDisplayFilterMode),
    makeValueItem("FreqOFlt2:", patchToFreqOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCFREQOSCFILTER2], 36, 124, 1, nprnToDisplayFilterFreqHz),
    makeValueItem("Q OscFlt2:", patchToQFactorOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCQFACTOROSCFILTER2], 5, 127, 1, nprnToDisplay2p1fHzDiv10),
    makeValueItem("KeyFlwOF2:", patchToKeyFollowOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCKEYFOLLOWOSCFILTER2], 52, 88, 1, nprnToDisplay03dm),
    makeValueItem("BrModOF2:", patchToBreathModOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCBREATHMODOSCFILTER2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("BrCrvOF2:", patchToBreathOscFiltCurveLines2, &current_patch.nrpn_msb_osc_filt2[CCBREATHCURVEOSCFILTER2], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("LfoF OF2:", patchToLfoFreqOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCLFOFREQOSCFILTER2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LfoD OF2:", patchToLfoDepthOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCLFODEPTHOSCFILTER2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LfoBrOF2:", patchToLfoBreathOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCLFOBREATHOSCFILTER2], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("LfoThOF2:", patchToLfoThreshOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCLFOTHRESHOSCFILTER2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpT OF2:", patchToSweepTimeOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCSWEEPTIMEOSCFILTER2], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpD OF2:", patchToSweepDepthOscFilter2, &current_patch.nrpn_msb_osc_filt2[CCSWEEPDEPTHOSCFILTER2], 0, 127, 1, nprnToDisplay03dm),
};
MenuItem patchNzFilt3MenuItems[]{
    makeActionItem("Back", goUpOne),
    makeValueItem("LinkNzFlt:", patchToLinkNoiseFilters, &current_patch.nrpn_msb_common1[CCLINKNOISEFILTERS], 1, 2, 1, nprnToDisplayOnOff),
    makeValueItem("ModeNzFl1:", patchToModeNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCMODENOISEFILTER3], 0, 4, 1, nprnToDisplayFilterMode),
    makeValueItem("FreqNzFl1:", patchToFreqNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCFREQNOISEFILTER3], 36, 124, 1, nprnToDisplayFilterFreqHz),
    makeValueItem("Q NzFilt1:", patchToQFactorNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCQFACTORNOISEFILTER3], 5, 127, 1, nprnToDisplay2p1fHzDiv10),
    makeValueItem("KeyFlwNF1:", patchToKeyFollowNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCKEYFOLLOWNOISEFILTER3], 52, 88, 1, nprnToDisplay03dm),
    makeValueItem("BrModNzF1:", patchToBreathModNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCBREATHMODNOISEFILTER3], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("BrCrvNzF1:", patchToBreathNoiseFiltCurveLines3, &current_patch.nrpn_msb_noise_filt3[CCBREATHCURVENOISEFILTER3], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("LfoF NzF1:", patchToLfoFreqNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCLFOFREQNOISEFILTER3], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LfoD NzF1:", patchToLfoDepthNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCLFODEPTHNOISEFILTER3], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LfoBrNzF1:", patchToLfoBreathNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCLFOBREATHNOISEFILTER3], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("LfoThNzF1:", patchToLfoThreshNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCLFOTHRESHNOISEFILTER3], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpT NzF1:", patchToSweepTimeNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCSWEEPTIMENOISEFILTER3], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpD NzF1:", patchToSweepDepthNoiseFilter3, &current_patch.nrpn_msb_noise_filt3[CCSWEEPDEPTHNOISEFILTER3], 0, 127, 1, nprnToDisplay03dm),
};
MenuItem patchNzFilt4MenuItems[]{
    makeActionItem("Back", goUpOne),
    makeValueItem("ModeNzFl2:", patchToModeNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCMODENOISEFILTER4], 0, 4, 1, nprnToDisplayFilterMode),
    makeValueItem("FreqNzFl2:", patchToFreqNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCFREQNOISEFILTER4], 36, 124, 1, nprnToDisplayFilterFreqHz),
    makeValueItem("Q NzFilt2:", patchToQFactorNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCQFACTORNOISEFILTER4], 5, 127, 1, nprnToDisplay2p1fHzDiv10),
    makeValueItem("KeyFlwNF2:", patchToKeyFollowNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCKEYFOLLOWNOISEFILTER4], 52, 88, 1, nprnToDisplay03dm),
    makeValueItem("BrModNzF2:", patchToBreathModNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCBREATHMODNOISEFILTER4], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("BrCrvNzF2:", patchToBreathNoiseFiltCurveLines4, &current_patch.nrpn_msb_noise_filt4[CCBREATHCURVENOISEFILTER4], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("LfoF NzF2:", patchToLfoFreqNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCLFOFREQNOISEFILTER4], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LfoD NzF2:", patchToLfoDepthNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCLFODEPTHNOISEFILTER4], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("LfoBrNzF2:", patchToLfoBreathNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCLFOBREATHNOISEFILTER4], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("LfoThNzF2:", patchToLfoThreshNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCLFOTHRESHNOISEFILTER4], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpT NzF2:", patchToSweepTimeNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCSWEEPTIMENOISEFILTER4], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("SwpD NzF2:", patchToSweepDepthNoiseFilter4, &current_patch.nrpn_msb_noise_filt4[CCSWEEPDEPTHNOISEFILTER4], 0, 127, 1, nprnToDisplay03dm),
};
MenuItem patchNoiseMenuItems[]{
    makeActionItem("Back", goUpOne),
    makeValueItem("NoiseTime:", patchToNoiseTime, &current_patch.nrpn_msb_noise[CCNOISETIME], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("NoiseBrth:", patchToNoiseBreathCurveLines, &current_patch.nrpn_msb_noise[CCNOISEBREATHCURVE], 0, 127, 1, nprnToDisplay03dm),
    makeValueItem("NoiseLvl:", patchToNoiseLevel, &current_patch.nrpn_msb_noise[CCNOISELEVEL], 0, 127, 1, nprnToDisplay03d),
};
// MenuItem patchFormantMenuItems[]{
//     makeActionItem("Back", goUpOne),
// };
//  MenuItem patchAmpMenuItems[]{
//      makeActionItem("Back", goUpOne),
//  };
MenuItem patchCommonMenuItems[]{
    makeActionItem("Back", goUpOne),
    makeValueItem("Key Trig:", patchToKeyTriggerSingle, &current_patch.nrpn_msb_common1[CCKEYTRIGGER], 0, 1, 1, nprnToDisplayKeyTrigger),
    // makeValueItem("OctBtnLvl:", patchToOctaveButtonLevel, &current_patch.nrpn_msb_common2[CCOCTBUTTONLEVEL], 0, 127, 1, nprnToDisplay03d),
    makeValueItem("BendRange:", patchToBendRange, &current_patch.nrpn_msb_common1[CCBENDRANGE], 0, 12, 1, nprnToDisplay03d),
    makeValueItem("BendStep:", patchToBendStep, &current_patch.nrpn_msb_common1[CCBENDSTEP], 0, 1, 1, nprnToDisplayOnOff),
};
MenuItem systemMenuItems[]{
    makeActionItem("Back", goUpOne),
    makeValueItem("Aux In:", auxInAdjustFun, &mix_linein, 0, 100, 1, nprnToDisplay03d),
    makeValueItem("Octave:", octaveAdjustFun, &Octave, 62, 66, 1, nprnToDisplay03dm),
    makeValueItem("Semi:", semiAdjustFun, &Transpose, 52, 76, 1, nprnToDisplay03dm),
    makeValueItem("Cents:", centsAdjustFun, &FineTuneCents, 14, 114, 1, nprnToDisplay03dm),
    makeValueItem("BreathCC:", breathccAdjustFun, &breath_cc, 1, 11, 1, nprnToDisplay03d),
};
MenuItem calibrationMenuItems[]{
    makeActionItem("Back", goUpOne),
    makeValueItem("CompAttack:", compAttackAdjustFun, &comp_params[CCCOMPATTACK], 1, 127, 1, nprnToDisplay2p1fHzDiv10), // 0.1ms to 12.7ms
    makeValueItem("CompRelease:", compReleaseAdjustFun, &comp_params[CCCOMPRELEASE], 1, 127, 1, nprnToDisplay03d),      // 1ms to 127ms
    makeValueItem("CompRatio:", compRatioAdjustFun, &comp_params[CCCOMPRATIO], 1, 127, 1, nprnToDisplay2p1fHzDiv10),    // 0.1 to 12.7
    makeValueItem("CompThresh:", compThreshAdjustFun, &comp_params[CCCOMPTHRESHOLD], 24, 64, 1, nprnToDisplay03dm),     // -40 to 0 dB
    makeValueItem("CompKnee:", compKneeAdjustFun, &comp_params[CCCOMPKNEE], 0, 40, 1, nprnToDisplay03d),                // 0 to 40 dB
    makeValueItem("CompMUGain:", compMUGainAdjustFun, &comp_params[CCCOMPMAKEUPGAIN], 0, 40, 1, nprnToDisplay03d),      // 0 to 40 db
    makeValueItem("NNBModCal:", nnBmodAdjustFun, &NNBModCal, 60, 100, 1, nprnToDisplay03d),                             // NN filter uses full OctaveControlFilter<N>
};

void setupMenus(U8G2 &u8g2)
{
    // clear name buffer
    for (int i = 0; i <= 20; i++)
        namebuf[i] = '\0';

    // allocate menus
    topMenuPtr = new Menu("Current Patch:", topMenuItems, sizeof(topMenuItems) / sizeof(MenuItem));
    // patchSelectMenuPtr = new Menu("Pch Select Menu", patchSelectMenuItems, sizeof(patchSelectMenuItems) / sizeof(MenuItem));
    patchPasteMenuPtr = new Menu("Pch Paste Menu", patchPasteMenuItems, sizeof(patchPasteMenuItems) / sizeof(MenuItem), setDestination);
    patchSwapMenuPtr = new Menu("Pch Swap Menu", patchSwapMenuItems, sizeof(patchSwapMenuItems) / sizeof(MenuItem));
    saveGlobalFxMenuPtr = new Menu("Save Glbl Fx Menu", saveGlobalFxMenuItems, sizeof(saveGlobalFxMenuItems) / sizeof(MenuItem)); //
    patchFxMenuPtr = new Menu("Fx Menu", patchFxMenuItems, sizeof(patchFxMenuItems) / sizeof(MenuItem));
    patchOsc1MenuPtr = new Menu("Osc1 Menu", patchOsc1MenuItems, sizeof(patchOsc1MenuItems) / sizeof(MenuItem));
    patchOsc2MenuPtr = new Menu("Osc2 Menu", patchOsc2MenuItems, sizeof(patchOsc2MenuItems) / sizeof(MenuItem));
    patchOscFilt1MenuPtr = new Menu("Osc Filter 1 Menu", patchOscFilt1MenuItems, sizeof(patchOscFilt1MenuItems) / sizeof(MenuItem));
    patchOscFilt2MenuPtr = new Menu("Osc Filter 2 Menu", patchOscFilt2MenuItems, sizeof(patchOscFilt2MenuItems) / sizeof(MenuItem));
    patchNzFilt3MenuPtr = new Menu("Noise Filter 1 Menu", patchNzFilt3MenuItems, sizeof(patchNzFilt3MenuItems) / sizeof(MenuItem));
    patchNzFilt4MenuPtr = new Menu("Noise Filter 2 Menu", patchNzFilt4MenuItems, sizeof(patchNzFilt4MenuItems) / sizeof(MenuItem));
    patchNoiseMenuPtr = new Menu("Noise Menu", patchNoiseMenuItems, sizeof(patchNoiseMenuItems) / sizeof(MenuItem));
    // patchFormantMenuPtr = new Menu("Formant Menu", patchFormantMenuItems, sizeof(patchFormantMenuItems) / sizeof(MenuItem));
    // patchAmpMenuPtr = new Menu("Amp Menu", patchAmpMenuItems, sizeof(patchAmpMenuItems) / sizeof(MenuItem));
    patchCommonMenuPtr = new Menu("Common Menu", patchCommonMenuItems, sizeof(patchCommonMenuItems) / sizeof(MenuItem));
    systemMenuPtr = new Menu("System Menu", systemMenuItems, sizeof(systemMenuItems) / sizeof(MenuItem));
    calibrationMenuPtr = new Menu("Calibration Menu", calibrationMenuItems, sizeof(calibrationMenuItems) / sizeof(MenuItem));

    // assign submenu pointers now that menus are allocated
    // topMenuItems[0].u.submenu = patchSelectMenuPtr;
    topMenuItems[2].u.submenu = patchPasteMenuPtr;
    topMenuItems[3].u.submenu = patchSwapMenuPtr;
    topMenuItems[4].u.submenu = patchFxMenuPtr;
    topMenuItems[5].u.submenu = patchOsc1MenuPtr;
    topMenuItems[6].u.submenu = patchOsc2MenuPtr;
    topMenuItems[7].u.submenu = patchOscFilt1MenuPtr;
    topMenuItems[8].u.submenu = patchOscFilt2MenuPtr;
    topMenuItems[9].u.submenu = patchNzFilt3MenuPtr;
    topMenuItems[10].u.submenu = patchNzFilt4MenuPtr;
    topMenuItems[11].u.submenu = patchNoiseMenuPtr;
    // topMenuItems[12].u.submenu = patchFormantMenuPtr;
    //  topMenuItems[13].u.submenu = patchAmpMenuPtr; // Amp is now a value item
    topMenuItems[14].u.submenu = patchCommonMenuPtr;
    topMenuItems[15].u.submenu = systemMenuPtr;
    topMenuItems[16].u.submenu = calibrationMenuPtr;
    patchFxMenuItems[2].u.submenu = saveGlobalFxMenuPtr;

    // create menu system instance
    ms = new MenuSystem(u8g2, topMenuPtr);
}
