// menuSetup.h
/****************************************************************
  menuSetup.h - Menu definitions for Windy1 synthesizer project
  --------------------------------------------------------------
  This file defines the menu structure and items for the Windy1
  synthesizer project using the MenuSystem library.

  Created by Your Name, Date.
  --------------------------------------------------------------
  License: MIT License
****************************************************************/
#ifndef menusetup_h_
#define menusetup_h_

#include "MenuSystem.h"
#include <Audio.h>

// forward declarations for actions
void updateUI();
void goUpOne();
void pastePatchToDestination();
void swapPatchWithDestination();
void saveGlobalFxSD_wrap();
void namingFinished(const char *s);
void namingAborted();
void genSplashScreen();
void UITimeout(void);
void resetUITimeout(void);
void sgtl_power_down();
void EEPROM_update(int addr, int val);
void pinMode_INPUT_PULLUP_100K(uint8_t pin);
void pinMode_INPUT_PULLUP_47K(uint8_t pin);
void drawStrNl(int x, int y, const char *str);
void changeFilterMode(void);
void processMIDI(bool midi_from_host_flag);
void noteOffFun(void);

// Audio System externs
extern AudioSynthWaveformDc dc_breathThreshOsc1; // xy=1291.333251953125,70

// Define menu items and menus (arrays). Submenu pointer will be set in setup to avoid static init order issues.
extern MenuItem topMenuItems[];
// extern MenuItem patchSelectMenuItems[];
extern MenuItem patchPasteMenuItems[];
extern MenuItem patchSwapMenuItems[];
extern MenuItem patchFxMenuItems[];
extern MenuItem saveGlobalFxMenuItems[];
extern MenuItem patchOsc1MenuItems[];
extern MenuItem patchOsc2MenuItems[];
extern MenuItem patchOscFilt1MenuItems[];
extern MenuItem patchOscFilt2MenuItems[];
extern MenuItem patchNzFilt3MenuItems[];
extern MenuItem patchNzFilt4MenuItems[];
extern MenuItem patchNoiseMenuItems[];
extern MenuItem patchFormantMenuItems[];
// extern MenuItem patchAmpMenuItems[];
extern MenuItem patchCommonMenuItems[];
extern MenuItem systemMenuItems[];

extern MenuSystem *ms;

extern Menu *topMenuPtr;
// extern Menu *patchSelectMenuPtr;
extern Menu *patchPasteMenuPtr;
extern Menu *patchSwapMenuPtr;
extern Menu *patchFxMenuPtr;
extern Menu *saveGlobalFxMenuPtr;
extern Menu *patchOsc1MenuPtr;
extern Menu *patchOsc2MenuPtr;
extern Menu *patchOscFilt1MenuPtr;
extern Menu *patchOscFilt2MenuPtr;
extern Menu *patchNzFilt3MenuPtr;
extern Menu *patchNzFilt4MenuPtr;
extern Menu *patchNoiseMenuPtr;
extern Menu *patchFormantMenuPtr;
// extern Menu *patchAmpMenuPtr;
extern Menu *patchCommonMenuPtr;
extern Menu *systemMenuPtr;

extern char namebuf[20 + 1]; // naming buffer

void setupMenus(U8G2 &u8g2);

#endif // menusetup_h_
