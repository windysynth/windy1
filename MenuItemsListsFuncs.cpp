// MenuItemsListsFuncs.cpp
//
#include <algorithm>
#include "OledMenu.h"
#include "globals.h"
#include "MenuItemsListsFuncs.h"
#include "patches.h"
#include "helpers.h"
#include "helpers.hpp"

extern AudioEffectEnvelope      env_squelch;


/*****************************
 ******************************
 * Setup the menu as an array of MenuItem
 * Create a MenuList and an instance of your 
 * menu class
 ******************************
 *****************************/

MenuItem PROGMEM topMenu[16] = {
    { "VolAdj  " , gotoVolAdjMenu   }
  , { "PatchSel" , gotoPatchSelectMenu   }
//  , { "PatchRst" , gotoPatchResetMenu   }
//  , { "PatchCpy" , gotoPatchCopyMenu   }
  , { "PatchPst" , gotoPatchPasteMenu   }
  , { "PatchSwp" , gotoPatchSwapMenu   }
  , { "Fx      " , gotoPatchFxMenu   }
  , { "Osc1    " , gotoPatchOsc1Menu   }
  , { "Osc2    " , gotoPatchOsc2Menu   }
  , { "OscFilt1" , gotoPatchOscFilter1Menu   }
  , { "OscFilt2" , gotoPatchOscFilter2Menu   }
  , { "Nz Filt1" , gotoPatchNoiseFilter3Menu   }
  , { "Nz Filt2" , gotoPatchNoiseFilter4Menu   }
  , { "Noise   " , gotoPatchNoiseMenu   }
  , { "Formant " , gotoPatchFormantMenu   }
  , { "Amp     " , gotoPatchAmpMenu   }
  , { "P Common" , gotoPatchCommonMenu   }
  , { "System  " , gotoSystemAdjMenu   }
};

MenuItem PROGMEM namingMenu[1] = {
     {"Edit Name:", namingFun}
};
bool namingFun(){
    //TODO: allow name_length to grow up to 20
    char str_withcursorbuf[32] = {0};
    //int name_length = strlen(myMenu.str_namingbuf)-1; 
    int name_length = 20; 
    if(myMenu.updateLeafValue){
        switch (myMenu.namingState)
         {
             case ALPHANUM:
                 // TODO: code to character of string at cursor
                myMenu.alphaNumIdx += myMenu.updateLeafValue;
                myMenu.alphaNumIdx = wrap( (int)myMenu.alphaNumIdx, 0, (int)alphaNumString.length()-1);
                myMenu.str_namingbuf[myMenu.namingPos] = alphaNumString.charAt(myMenu.alphaNumIdx);
                sprintf(myMenu.str_oledbuf, "LngPrs2wr\n%s",myMenu.str_namingbuf);
                return true;
             case POSITION:
                myMenu.namingPos += myMenu.updateLeafValue; 
                myMenu.namingPos = wrap(myMenu.namingPos, 0, name_length-1 );  // 20 characters max
                memcpy((byte*)str_withcursorbuf, (byte*)myMenu.str_namingbuf, sizeof(myMenu.str_namingbuf));
                str_withcursorbuf[myMenu.namingPos] = ']';
                sprintf(myMenu.str_oledbuf, "LngPrs2wr\n%s",str_withcursorbuf);
                return true;
             default:
                return true;
        }
    }
    if (myMenu.knobButtonSelType == LONG_PRESS){
        Serial8.println(F("writing patch with new Name"));
        trim_ws(myMenu.str_namingbuf);
        copyCurrentPatchToCopyBuffer();
        memcpy((byte*)copy_buffer_patch.patch_string, (byte*)myMenu.str_namingbuf,
              sizeof(myMenu.str_namingbuf));
        //saveCoppiedPatchSD does 
        // copyPatchBuffToPatchBuff(&current_patch, &copy_buffer_patch);
        saveCoppiedPatchSD(paste_patchNumber); 
        loadPatchSD(paste_patchNumber);
        return goUpOneMenu();
    }
    switch (myMenu.namingState)
    {
        case ALPHANUM:
            if (myMenu.knobButtonSelType == SINGLE_CLICK){
                extend_with_spaces(myMenu.str_namingbuf, 20); // 20 char max: TODO based on font size
                myMenu.namingPos++ ; 
                myMenu.namingPos = wrap(myMenu.namingPos, 0, name_length-1);  // 20 characters max
                sprintf(myMenu.str_oledbuf, "LngPrs2wr\n%s",myMenu.str_namingbuf);
                return true;
            }
            if (myMenu.knobButtonSelType == DOUBLE_CLICK){
                extend_with_spaces(myMenu.str_namingbuf, 20);
                myMenu.namingState = POSITION;
                //myMenu.namingPos-- ; 
                //myMenu.namingPos = wrap(myMenu.namingPos, 0, name_length-1);  // 20 characters max
                memcpy((byte*)str_withcursorbuf, (byte*)myMenu.str_namingbuf, sizeof(myMenu.str_namingbuf));
                str_withcursorbuf[myMenu.namingPos] = ']';
                sprintf(myMenu.str_oledbuf, "LngPrs2wr\n%s",str_withcursorbuf);
            }
            return true; 
        case POSITION:
            if (myMenu.knobButtonSelType == SINGLE_CLICK){
                myMenu.namingState = ALPHANUM;
                sprintf(myMenu.str_oledbuf, "LngPrs2wr\n%s",myMenu.str_namingbuf);
                return true;
            }
            if (myMenu.knobButtonSelType == DOUBLE_CLICK){
               sprintf(myMenu.str_namingbuf, "Noname              ");
            }
            return goUpOneMenu(); 
        default:
            return true;
    }
}

MenuItem PROGMEM escapeMenu[1] = { 
    { "Escaping..", goUpOneMenu    }
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
    sprintf(myMenu.str_oledbuf,"  %ld   ", vol);
    Serial8.println(F("volAdjustFun: updated vol "));
    return true;
 }
  //display.clearDisplay(); // erase display
  //display.display(); // refresh display
  Serial8.println(F("volAdjustFun: goto TopMenu"));
  //myMenu.setCurrentMenu(&listTopMenu);
  gotoTopMenu();
  return true;
}
MenuItem PROGMEM fxSourceMenu[1] = { 
    { "FxSource:\n ", fxSourceFun    }
};
bool fxSourceFun() {
 if(myMenu.updateLeafValue){
     int Temp = fxSourcePatch;
     Temp += myMenu.updateLeafValue;
     fxSourcePatch = std::clamp(Temp,0, 1);
     // re-apply synth variables to update effects 
     patchToSynthVariables(&current_patch); // fxSourcePatch is checked in here
     sprintf(myMenu.str_oledbuf,"  %s  ", fxSourcePatch ? "Patch " : "Global" );
     return true;
 }
 Serial8.println(F("fxSourceFun: goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM patchSelectMenu[1] = {
    { "Patch: " , patchSelectFun   }
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
    //ps.setCharAt( ps.indexOf(' '), '\n'); // TODO: spaces till end of line then \n
    //sprintf(myMenu.str_oledbuf, "%03d\n%s", current_patchNumber+1, ps.c_str() );
    char pn[3] ={0};
    sprintf(pn, "%03d", current_patchNumber+1 );
    display.drawStr(7*display.getMaxCharWidth(),display.getMaxCharHeight(),pn);
    sprintf(myMenu.str_oledbuf, " %s", ps.c_str() );
    Serial8.println(current_patch.patch_string);

//    if (updateEpromFlag)
//    {
//        eepromCurrentMillis = millis();
//        eepromPreviousMillis = eepromCurrentMillis; // reset timer every knobBot turn 
//    }
    return true;
 }
  //display.clearBuffer(); // erase display
  //display.clearDisplay(); // erase display
  //display.display(); // refresh display
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

MenuItem PROGMEM patchPasteMenu[1] = {
   { "Paste  " , patchPasteFun   }
};
bool patchPasteFun(){ 
  if(myMenu.updateLeafValue){
      char pn[3] ={0};
      paste_patchNumber += myMenu.updateLeafValue;
    if(paste_patchNumber > NUMBER_OF_PATCHES ){
        paste_patchNumber = 0; 
    }
    else if(paste_patchNumber < 0 ) {
        paste_patchNumber = NUMBER_OF_PATCHES; 
    } 
    if (paste_patchNumber == NUMBER_OF_PATCHES){
        sprintf(pn, "   ");
        display.drawStr(7*display.getMaxCharWidth(),display.getMaxCharHeight(),pn);
        sprintf(myMenu.str_oledbuf, "Exit w/o saving");
        //sprintf(myMenu.str_oledbuf, "Exit w/o\n saving");
    }
    else { 
        sprintf(pn, "%03d", paste_patchNumber+1 );
        display.drawStr(7*display.getMaxCharWidth(),display.getMaxCharHeight(),pn);
        String ps( loadedPatches[paste_patchNumber].patch_string );
        sprintf(myMenu.str_oledbuf, " %s", ps.c_str() );
        //ps.setCharAt( ps.indexOf(' '), '\n'); // TODO: spaces till end of line then \n
        //sprintf(myMenu.str_oledbuf, "%03d\n%s", paste_patchNumber+1, ps.c_str() );
        Serial8.println(loadedPatches[paste_patchNumber].patch_string);
        // load the patch 
        if (!patchLoaded[paste_patchNumber])
        {
            loadPatchSD(paste_patchNumber);
        }
    }
    return true;
  }
  // NUMBER_OF_PATCHES is Exit w/o writing
  if(paste_patchNumber == NUMBER_OF_PATCHES){
      //display.clearBuffer(); // erase display
      //display.clearDisplay(); // erase display
      Serial8.println(F("patchPasteFun: goUpOneMenu"));
      return goUpOneMenu(); 
  } 
  copyCurrentPatchToCopyBuffer();
  return gotoNamingMenu();
/*//---
  copyCurrentPatchToCopyBuffer();
  saveCoppiedPatchSD(paste_patchNumber); //does copyPatchBuffToPatchBuff(&current_patch, &copy_buffer_patch) inside;
  loadPatchSD(paste_patchNumber);
  return goUpOneMenu();
//--- */
}
MenuItem PROGMEM patchSwapMenu[1] = {
    { "Swap w/\n " , patchSwapFun   }
};
bool patchSwapFun(){
  if(myMenu.updateLeafValue){
      swap_patchNumber += myMenu.updateLeafValue;
    if(swap_patchNumber > NUMBER_OF_PATCHES ){
        swap_patchNumber = 0; 
    }
    else if(swap_patchNumber < 0 ) {
        swap_patchNumber = NUMBER_OF_PATCHES; 
    } 
    if (swap_patchNumber == NUMBER_OF_PATCHES){
        sprintf(myMenu.str_oledbuf, "Exit w/o\n saving");
    }
    else { 
        String ps( loadedPatches[swap_patchNumber].patch_string );
        ps.setCharAt( ps.indexOf(' '), '\n'); // TODO: spaces till end of line then \n
        sprintf(myMenu.str_oledbuf, "%03d\n%s", swap_patchNumber+1, ps.c_str() );
        Serial8.println(loadedPatches[swap_patchNumber].patch_string);
        // load the patch 
        if (!patchLoaded[swap_patchNumber])
        {
            loadPatchSD(swap_patchNumber);
        }
    }
    return true;
  }
  //display.clearBuffer(); // erase display
  //display.clearDisplay(); // erase display
  Serial8.println(F("patchPasteFun: goto TopMenu"));
  // NUMBER_OF_PATCHES is Exit w/o writing
  if(swap_patchNumber == NUMBER_OF_PATCHES){ return goUpOneMenu(); } 
  copyCurrentPatchToCopyBuffer();
  copyLoadedPatchToSwapBuffer(swap_patchNumber);
  saveCoppiedPatchSD(swap_patchNumber);
  saveSwappedPatchSD(current_patchNumber);
  loadPatchSD(swap_patchNumber);
  loadPatchSD(current_patchNumber);
  return goUpOneMenu();
}

MenuItem PROGMEM patchFxMenu[23] = {
    { "Back    " , goUpOneMenu   }
   ,{ "FxSource" , gotoFxSourceMenu   }
   ,{ "SaveGlbl" , gotoSaveFxGlobalMenu }
//   ,{ "FxCopy  " , gotoFxCopyMenu   }
   ,{ "FxPaste " , gotoFxPasteMenu   }
//   ,{ "FxSwap  " , gotoFxSwapMenu   }
   ,{ "DlyLevel" , gotoDelayLevelMenu   }
   ,{ "DlyTime " , gotoDelayTimeLMenu   }
   //,{ "DlyTimeR" , gotoDelayTimeRMenu   }
   ,{ "DlyPong " , gotoDelayPongMenu   }
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

/*
MenuItem PROGMEM fxCopyMenu[2] = {
    { "Back    " , goUpOneMenu   }
   ,{ "CopyFx  " , fxCopyFun   }
};
bool fxCopyFun(){ return goUpOneMenu(); }
*/

MenuItem PROGMEM saveFxGlobalMenu[1] = {
    { "SaveGlbl" , saveFxGlobalFun  }
};
bool saveFxGlobalFun(){
  static int bWrite = 0;
  if(myMenu.updateLeafValue){
    bWrite = getFxValue(&current_patch, EFFECTGROUPCOMMON1, CCCHORUSON);
    bWrite += myMenu.updateLeafValue;
    bWrite = std::clamp(bWrite, 0, 1);
    if (!bWrite){
        sprintf(myMenu.str_oledbuf, "Exit w/o\n saving");
    }
    else { 
        sprintf(myMenu.str_oledbuf, "Write\n Global Fx");
        Serial8.println(myMenu.str_oledbuf);
    }
    return true;
  }
  //display.clearBuffer(); // erase display
  //display.clearDisplay(); // erase display
  Serial8.println(F("fxPasteFun: goUpOneMenu"));
  if(!bWrite){ // exit without saving 
      bWrite = 0; // ensure next time in menu starts with exit
      return goUpOneMenu(); 
  } 
  
  bWrite = 0; // ensure next time in menu starts with exit
  saveGlobalFxSD(); // no need to reload after, gobal_buffer_fx already correct
  return goUpOneMenu();
}
MenuItem PROGMEM fxPasteMenu[1] = {
    { "PasteFx " , fxPasteFun   }
};
bool fxPasteFun(){ 
  if(myMenu.updateLeafValue){
      paste_FxNumber += myMenu.updateLeafValue;
    if(paste_FxNumber > NUMBER_OF_PATCHES ){
        paste_FxNumber = 0; 
    }
    else if(paste_FxNumber < 0 ) {
        paste_FxNumber = NUMBER_OF_PATCHES; 
    } 
    if (paste_FxNumber == NUMBER_OF_PATCHES){
        sprintf(myMenu.str_oledbuf, "Exit w/o\n saving");
    }
    else { 
        String ps( loadedPatches[paste_FxNumber].patch_string );
        ps.setCharAt( ps.indexOf(' '), '\n'); // TODO: spaces till end of line then \n
        sprintf(myMenu.str_oledbuf, "%03d\n%s", paste_FxNumber+1, ps.c_str() );
        Serial8.println(loadedPatches[paste_FxNumber].patch_string);
        // load the patch 
//        if (!patchLoaded[paste_FxNumber])
//        {
//            loadPatchSD(paste_FxNumber);
//        }
    }
    return true;
  }
  //display.clearBuffer(); // erase display
  //display.clearDisplay(); // erase display
  Serial8.println(F("fxPasteFun: goUpOneMenu"));
  // NUMBER_OF_PATCHES is Exit w/o writing
  if(paste_FxNumber == NUMBER_OF_PATCHES){ return goUpOneMenu(); } 
  Serial8.println("try to copy current fx");
  copyCurrentFxToCopyBufferFx();
  Serial8.println("copyCurrentFxToCopyBufferFx()");
  copyCopyBufferFxToPatch(paste_FxNumber);
  Serial8.println("copyCopyBufferFxToPatch()");
  savePatchSD(paste_FxNumber);
  Serial8.println("savePatchSD()");
  loadPatchSD(paste_FxNumber);
  Serial8.println("loadPatchSD()");
  return goUpOneMenu();
}
/*
MenuItem PROGMEM fxSwapMenu[2] = {
    { "Back " , goUpOneMenu   }
   ,{ "SwapFx " , fxSwapFun   }
};
bool fxSwapFun(){ return goUpOneMenu(); }
*/
MenuItem PROGMEM delayLevelMenu[1] = {
    { "Dly Lev:\n " , delayLevelFun   }
};
bool delayLevelFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYLEVEL);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp,0, 127), &current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYLEVEL);
     patchToEffectsDelayLevel(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYLEVEL));
     return true;
 }
 Serial8.println(F("delayLevelFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM delayTimeLMenu[1] = {
    { "DlyTime\n " , delayTimeLFun   }
};
bool delayTimeLFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYTIME);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp,0, 127), &current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYTIME);
     patchToEffectsDelayTimeL(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"  %04d    ", 
             10*getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYTIME));
     return true;
 }
 Serial8.println(F("delayLevelLFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM delayPongMenu[1] = {
    { "DlyPong\n " , delayPongFun   }
};
bool delayPongFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = 127 - getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYSPARE);
     Temp += myMenu.updateLeafValue;
     setFxValue(127 - std::clamp(Temp,0, 127), &current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYSPARE);
     patchToEffectsDelayPong(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
        127 - getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYSPARE));
     return true;
 }
 Serial8.println(F("delayPongFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM delayFeedbackMenu[1] = {
    { "Dly FB: \n " , delayFeedbackFun   }
};
bool delayFeedbackFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYFEEDBACK);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp,0, 127), &current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYFEEDBACK);
     patchToEffectsDelayFeedback(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYFEEDBACK));
     return true;
 }
 Serial8.println(F("delayFeedbackFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM delayDampMenu[1] = {
    { "DlyDamp:\n " , delayDampFun   }
};
bool delayDampFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYDAMP);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp,0, 127), &current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYDAMP);
     patchToEffectsDelayDamp(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYDAMP));
     return true;
 }
 Serial8.println(F("delayFeedbackFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM reverbLevelMenu[1] = {
    { "Rvb Lev:\n " , reverbLevelFun   }
};
bool reverbLevelFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBLEVEL);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp,0, 127), &current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBLEVEL);
     patchToEffectsReverbLevel(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBLEVEL));
     return true;
 }
 Serial8.println(F("reverbLevelFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM reverbTimeMenu[1] = {
    { "RvbTime:\n " , reverbTimeFun   }
};
bool reverbTimeFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBTIME);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp,10, 50), &current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBTIME);
     patchToEffectsReverbTime(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"  %04d    ", 
        100*getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBTIME)); //114,3,10,50,//1000 to 5000 ms
     return true;
 }
 Serial8.println(F("reverbTimeFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM reverbDensityMenu[1] = {
    { "RvbDens:\n " , reverbDensityFun   }
};
bool reverbDensityFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBDENSEEARLY);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp,0, 127), &current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBDENSEEARLY);
     patchToEffectsReverbDenseEarly(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
             getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBDENSEEARLY));
     return true;
 }
 Serial8.println(F("reverbDensityFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM reverbDampMenu[1] = {
    { "RvbDamp:\n " , reverbDampFun   }
};
bool reverbDampFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBDAMP);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp,54, 74), &current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBDAMP);
     patchToEffectsReverbDamp(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
             getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBDAMP)-64);
     return true;
 }
 Serial8.println(F("reverbDampFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

MenuItem PROGMEM chorusOnMenu[1] = {
    { "Chrs On:\n " , chorusOnFun   }
};
bool chorusOnFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCOMMON1, CCCHORUSON);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 1), &current_patch, EFFECTGROUPCOMMON1, CCCHORUSON);
     patchToChorusOn(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %s    ", 
        getFxValue(&current_patch, EFFECTGROUPCOMMON1, CCCHORUSON)? " On": "Off");
     return true;
 }
 Serial8.println(F("chorusOnFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM chorusDryMenu[1] = {
    { "ChrsDry:\n " , chorusDryFun   }
};
bool chorusDryFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDRYLEVEL);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 127), &current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDRYLEVEL);
     patchToEffectsChorusDryLevel(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
             getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDRYLEVEL));
     return true;
 }
 Serial8.println(F("chorusDryFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM chorusLfoFMenu[1] = {
    { "ChsLfoF:\n " , chorusLfoFFun   }
};
bool chorusLfoFFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSLFOFREQ);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 127), &current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSLFOFREQ);
     patchToEffectsChorusLfoFreq(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"  %2.1fHz  ", 
        (float)getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSLFOFREQ)/10.0);
     return true;
 }
 Serial8.println(F("chorusLfoFFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM chorusFeedbackMenu[1] = {
    { "Chrs FB:\n " , chorusFeedbackFun   }
};
bool chorusFeedbackFun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSFEEDBACK);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 127), &current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSFEEDBACK);
     patchToEffectsChorusFeedback(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSFEEDBACK)-64);
     return true;
 }
 Serial8.println(F("chorusFeedbackFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM chorusDelay1Menu[1] = {
    { "ChsDly1:\n " , chorusDelay1Fun   }
};
bool chorusDelay1Fun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY1);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 127), &current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY1);
     patchToEffectsChorusDelay1(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d mS ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY1));
     return true;
 }
 Serial8.println(F("chorusDelay1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM chorusMod1Menu[1] = {
    { "ChsMod1:\n " , chorusMod1Fun   }
};
bool chorusMod1Fun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD1);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 127), &current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD1);
     patchToEffectsChorusMod1(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD1)-64);
     return true;
 }
 Serial8.println(F("chorusMod1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM chorusWet1Menu[1] = {
    { "ChsWet1:\n " , chorusWet1Fun   }
};
bool chorusWet1Fun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET1);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 127), &current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET1);
     patchToEffectsChorusWet1(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET1)-64);
     return true;
 }
 Serial8.println(F("chorusWet1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM chorusDelay2Menu[1] = {
    { "ChsDly2:\n " , chorusDelay2Fun   }
};
bool chorusDelay2Fun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY2);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 127), &current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY2);
     patchToEffectsChorusDelay2(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d mS ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY2));
     return true;
 }
 Serial8.println(F("chorusDelay2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM chorusMod2Menu[1] = {
    { "ChsMod2:\n " , chorusMod2Fun   }
};
bool chorusMod2Fun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD2);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 127), &current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD2);
     patchToEffectsChorusMod2(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD2)-64);
     return true;
 }
 Serial8.println(F("chorusMod2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM chorusWet2Menu[1] = {
    { "ChsWet2:\n " , chorusWet2Fun   }
};
bool chorusWet2Fun(){ 
 if(myMenu.updateLeafValue){
     int Temp = getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET2);
     Temp += myMenu.updateLeafValue;
     setFxValue(std::clamp(Temp, 0, 127), &current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET2);
     patchToEffectsChorusWet2(&current_patch);
     //preUpdateSynthVariablesFlag = true;
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET2)-64);
     return true;
 }
 Serial8.println(F("chorusWet2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

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
bool octOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCOCTOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCOCTOSC1] = std::clamp(Temp,62, 66); // -2 to +2
     patchToOctOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCOCTOSC1]-64);
     return true;
 }
 Serial8.println(F("octOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM semiOsc1Menu[1] = {
    { "SemiOs1:\n " , semiOsc1Fun   }
};
bool semiOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCSEMIOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCSEMIOSC1] = std::clamp(Temp, 52, 76); // -12 to +12
     patchToSemiOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCSEMIOSC1]-64);
     return true;
 }
 Serial8.println(F("semiOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM centsOsc1Menu[1] = {
    { "CentOs1:\n " , centsOsc1Fun   }
};
bool centsOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCFINEOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCFINEOSC1] = std::clamp(Temp,14, 114);
     patchToFineOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCFINEOSC1]-64);
     return true;
 }
 Serial8.println(F("centsOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM beatOsc1Menu[1] = {
    { "BeatOs1:\n " , beatOsc1Fun   }
};
bool beatOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCBEATOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCBEATOSC1] = std::clamp(Temp,0, 127);
     patchToBeatOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBEATOSC1]-64);
     return true;
 }
 Serial8.println(F("centsOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sawOsc1Menu[1] = {
    { "SawOsc1:\n " , sawOsc1Fun   }
};
bool sawOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCSAWOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCSAWOSC1] = std::clamp(Temp,0, 127);
     patchToSawOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCSAWOSC1]);
     return true;
 }
 Serial8.println(F("sawOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM triOsc1Menu[1] = {
    { "TriOsc1:\n " , triOsc1Fun   }
};
bool triOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCTRIOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCTRIOSC1] = std::clamp(Temp,0, 127);
     patchToTriOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCTRIOSC1]);
     return true;
 }
 Serial8.println(F("triOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM pulesOsc1Menu[1] = {
    { "PlsOsc1:\n " , pulseOsc1Fun   }
};
bool pulseOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCPULSEOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCPULSEOSC1] = std::clamp(Temp,0, 127);
     patchToPulseOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCPULSEOSC1]);
     return true;
 }
 Serial8.println(F("pulseOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM PWOsc1Menu[1] = {
    { "PW Osc1:\n " , PWOsc1Fun   }
};
bool PWOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCPWOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCPWOSC1] = std::clamp(Temp,0, 127);
     patchToPwOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCPWOSC1]);
     return true;
 }
 Serial8.println(F("PWOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM pwmFreqOsc1Menu[1] = {
    { "PwmFOs1:\n " , pwmFreqOsc1Fun   }
};
bool pwmFreqOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCPWMFREQOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCPWMFREQOSC1] = std::clamp(Temp,0, 127);
     patchToPwmFreqOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCPWMFREQOSC1]);
     return true;
 }
 Serial8.println(F("pwmFreqOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM pwmDepthOsc1Menu[1] = {
    { "PwmDOs1:\n " , pwmDepthOsc1Fun   }
};
bool pwmDepthOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCPWMDEPTHOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCPWMDEPTHOSC1] = std::clamp(Temp,0, 127);
     patchToPwmDepthOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCPWMDEPTHOSC1]);
     return true;
 }
 Serial8.println(F("pwmFreqOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepTimeOsc1Menu[1] = {
    { "SwpTOs1:\n " , sweepTimeOsc1Fun   }
};
bool sweepTimeOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCSWEEPTIMEOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCSWEEPTIMEOSC1] = std::clamp(Temp,0, 127);
     patchToSweepTimeOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCSWEEPTIMEOSC1]);
     return true;
 }
 Serial8.println(F("sweepTimeOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepDepthOsc1Menu[1] = {
    { "SwpDOs1:\n " , sweepDepthOsc1Fun   }
};
bool sweepDepthOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCSWEEPDEPTHOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCSWEEPDEPTHOSC1] = std::clamp(Temp,0, 127);
     patchToSweepDepthOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCSWEEPDEPTHOSC1]-64);
     return true;
 }
 Serial8.println(F("sweepdepthOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathAttainOsc1Menu[1] = {
    { "BAtnOs1:\n " , breathAttainOsc1Fun   }
};
bool breathAttainOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCBREATHATTAINOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCBREATHATTAINOSC1] = std::clamp(Temp,0, 127);
     patchToBreathAttainOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBREATHATTAINOSC1]);
     return true;
 }
 Serial8.println(F("breathAttainOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathDepthOsc1Menu[1] = {
    { "BDptOs1:\n " , breathDepthOsc1Fun   }
};
bool breathDepthOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCBREATHDEPTHOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCBREATHDEPTHOSC1] = std::clamp(Temp,0, 127);
     patchToBreathDepthOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBREATHDEPTHOSC1]-64);
     return true;
 }
 Serial8.println(F("breathDepthOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathThresholdOsc1Menu[1] = {
    { "BthrOs1:\n " , breathThresholdOsc1Fun   }
};
bool breathThresholdOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCBREATHTHRESHOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCBREATHTHRESHOSC1] = std::clamp(Temp,0, 127);
     patchToBreathThreshOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBREATHTHRESHOSC1]);
     return true;
 }
 Serial8.println(F("breathAttainOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathCurveOsc1Menu[1] = {
    { "BCrvOs1:\n " , breathCurveOsc1Fun   }
};
bool breathCurveOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCBREATHCURVEOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCBREATHCURVEOSC1] = std::clamp(Temp,0, 127);
     patchToBreathOscCurveLines1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBREATHCURVEOSC1]);
     return true;
 }
 Serial8.println(F("breathAttainOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM levelOsc1Menu[1] = {
    { "LevOsc1:\n " , levelOsc1Fun   }
};
bool levelOsc1Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc1[CCLEVELOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc1[CCLEVELOSC1] = std::clamp(Temp,0, 127);
     patchToLevelOsc1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCLEVELOSC1]);
     return true;
 }
 Serial8.println(F("breathAttainOsc1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

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
bool octOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCOCTOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCOCTOSC2] = std::clamp(Temp,62, 66); // -2 to +2
     patchToOctOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCOCTOSC2]-64);
     return true;
 }
 Serial8.println(F("octOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM semiOsc2Menu[1] = {
    { "SemiOs2:\n " , semiOsc2Fun   }
};
bool semiOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCSEMIOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCSEMIOSC2] = std::clamp(Temp, 52, 76); // -12 to +12
     patchToSemiOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCSEMIOSC2]-64);
     return true;
 }
 Serial8.println(F("semiOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM centsOsc2Menu[1] = {
    { "CentOs2:\n " , centsOsc2Fun   }
};
bool centsOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCFINEOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCFINEOSC2] = std::clamp(Temp,14, 114);
     patchToFineOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCFINEOSC2]-64);
     return true;
 }
 Serial8.println(F("centsOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM beatOsc2Menu[1] = {
    { "BeatOs2:\n " , beatOsc2Fun   }
};
bool beatOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCBEATOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCBEATOSC2] = std::clamp(Temp,0, 127);
     patchToBeatOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBEATOSC2]-64);
     return true;
 }
 Serial8.println(F("centsOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sawOsc2Menu[1] = {
    { "SawOsc2:\n " , sawOsc2Fun   }
};
bool sawOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCSAWOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCSAWOSC2] = std::clamp(Temp,0, 127);
     patchToSawOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCSAWOSC2]);
     return true;
 }
 Serial8.println(F("sawOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM triOsc2Menu[1] = {
    { "TriOsc2:\n " , triOsc2Fun   }
};
bool triOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCTRIOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCTRIOSC2] = std::clamp(Temp,0, 127);
     patchToTriOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCTRIOSC2]);
     return true;
 }
 Serial8.println(F("triOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM pulesOsc2Menu[1] = {
    { "PlsOsc2:\n " , pulseOsc2Fun   }
};
bool pulseOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCPULSEOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCPULSEOSC2] = std::clamp(Temp,0, 127);
     patchToPulseOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCPULSEOSC2]);
     return true;
 }
 Serial8.println(F("pulseOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM PWOsc2Menu[1] = {
    { "PW Osc2:\n " , PWOsc2Fun   }
};
bool PWOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCPWOSC1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCPWOSC2] = std::clamp(Temp,0, 127);
     patchToPwOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCPWOSC2]);
     return true;
 }
 Serial8.println(F("PWOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM pwmFreqOsc2Menu[1] = {
    { "PwmFOs2:\n " , pwmFreqOsc2Fun   }
};
bool pwmFreqOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCPWMFREQOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCPWMFREQOSC2] = std::clamp(Temp,0, 127);
     patchToPwmFreqOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCPWMFREQOSC2]);
     return true;
 }
 Serial8.println(F("pwmFreqOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM pwmDepthOsc2Menu[1] = {
    { "PwmDOs2:\n " , pwmDepthOsc2Fun   }
};
bool pwmDepthOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCPWMDEPTHOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCPWMDEPTHOSC2] = std::clamp(Temp,0, 127);
     patchToPwmDepthOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCPWMDEPTHOSC2]);
     return true;
 }
 Serial8.println(F("pwmFreqOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepTimeOsc2Menu[1] = {
    { "SwpTOs2:\n " , sweepTimeOsc2Fun   }
};
bool sweepTimeOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCSWEEPTIMEOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCSWEEPTIMEOSC2] = std::clamp(Temp,0, 127);
     patchToSweepTimeOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCSWEEPTIMEOSC2]);
     return true;
 }
 Serial8.println(F("sweepTimeOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepDepthOsc2Menu[1] = {
    { "SwpDOs2:\n " , sweepDepthOsc2Fun   }
};
bool sweepDepthOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCSWEEPDEPTHOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCSWEEPDEPTHOSC2] = std::clamp(Temp,0, 127);
     patchToSweepDepthOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCSWEEPDEPTHOSC2]-64);
     return true;
 }
 Serial8.println(F("sweepdepthOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathAttainOsc2Menu[1] = {
    { "BAtnOs2:\n " , breathAttainOsc2Fun   }
};
bool breathAttainOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCBREATHATTAINOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCBREATHATTAINOSC2] = std::clamp(Temp,0, 127);
     patchToBreathAttainOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBREATHATTAINOSC2]);
     return true;
 }
 Serial8.println(F("breathAttainOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathDepthOsc2Menu[1] = {
    { "BDptOs2:\n " , breathDepthOsc2Fun   }
};
bool breathDepthOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCBREATHDEPTHOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCBREATHDEPTHOSC2] = std::clamp(Temp,0, 127);
     patchToBreathDepthOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBREATHDEPTHOSC2]-64);
     return true;
 }
 Serial8.println(F("breathDepthOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathThresholdOsc2Menu[1] = {
    { "BthrOs2:\n " , breathThresholdOsc2Fun   }
};
bool breathThresholdOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCBREATHTHRESHOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCBREATHTHRESHOSC2] = std::clamp(Temp,0, 127);
     patchToBreathThreshOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBREATHTHRESHOSC2]);
     return true;
 }
 Serial8.println(F("breathAttainOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathCurveOsc2Menu[1] = {
    { "BCrvOs2:\n " , breathCurveOsc2Fun   }
};
bool breathCurveOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCBREATHCURVEOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCBREATHCURVEOSC2] = std::clamp(Temp,0, 127);
     patchToBreathOscCurveLines2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBREATHCURVEOSC2]);
     return true;
 }
 Serial8.println(F("breathAttainOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM levelOsc2Menu[1] = {
    { "LevOsc2:\n " , levelOsc2Fun   }
};
bool levelOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc2[CCLEVELOSC2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc2[CCLEVELOSC2] = std::clamp(Temp,0, 127);
     patchToLevelOsc2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCLEVELOSC2]);
     return true;
 }
 Serial8.println(F("breathAttainOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM xFadeOsc2Menu[1] = {
    { "XFadOs2:\n " , xFadeOsc2Fun   }
};
bool xFadeOsc2Fun(){
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_common1[CCXFADE];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_common1[CCXFADE] = std::clamp(Temp,0, 1);
     patchToXFade(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %s    ", 
         current_patch.nrpn_msb_common1[CCXFADE]? " On": "Off");
     return true;
 }
 Serial8.println(F("xFadeOsc2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

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
bool linkOscFiltersFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_common1[CCLINKOSCFILTERS];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_common1[CCLINKOSCFILTERS] = std::clamp(Temp,1, 2);
     patchToLinkOscFilters(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     if(current_patch.nrpn_msb_common1[CCLINKOSCFILTERS]%2 == 0){
         ModeOscFilter2 = ModeOscFilter2_stored;
     }
     sprintf(myMenu.str_oledbuf," %s ", 
         current_patch.nrpn_msb_common1[CCLINKOSCFILTERS]%2 ? " Linkded": "UnLinked"); //1 Linked, 2 UnLinked
     return true;
 }
 Serial8.println(F("linkOscFiltersFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM modeOscFilter1Menu[1] = {
    { "ModeOF1:\n " , modeOscFilter1Fun   }
};
bool modeOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCMODEOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCMODEOSCFILTER1] = std::clamp(Temp,0, 4);
     patchToModeOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     switch (current_patch.nrpn_msb_osc_filt1[CCMODEOSCFILTER1]) {
         case LP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
         case HP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " HP"); break; 
         case BP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " BP"); break; 
         case NTC:
             sprintf(myMenu.str_oledbuf,"   %s    ", "NTC"); break; 
         case THRU:
             sprintf(myMenu.str_oledbuf,"   %s    ", "OFF"); break; 
         default:
             sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
     }
     return true;
 }
 Serial8.println(F("modeOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM freqOscFilter1Menu[1] = {
    { "FreqOF1:\n " , freqOscFilter1Fun   }
};
bool freqOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCFREQOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCFREQOSCFILTER1] = std::clamp(Temp,36, 124); // c2 to e9
     patchToFreqOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     float FreqFilterNN = (float)current_patch.nrpn_msb_osc_filt1[CCFREQOSCFILTER1];
     float FreqFilter = 440.0*pow(2, (FreqFilterNN-69.0)/12 );
     sprintf(myMenu.str_oledbuf,"%5.1fHz", FreqFilter);
     return true;
 }
 Serial8.println(F("freqOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM qOscFilter1Menu[1] = {
    { "Q OscF1:\n " , qOscFilter1Fun   }
};
bool qOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCQFACTOROSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCQFACTOROSCFILTER1] = std::clamp(Temp,5, 127); // 5 = 0.5
     patchToQFactorOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     float QFactor = (float)current_patch.nrpn_msb_osc_filt1[CCQFACTOROSCFILTER1]/10.0f;
     sprintf(myMenu.str_oledbuf,"   %2.1f   ", QFactor);
     return true;
 }
 Serial8.println(F("qOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM keyFollowOscFilter1Menu[1] = {
    { "KeyFOF1:\n " , keyFollowOscFilter1Fun   }
};
bool keyFollowOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCKEYFOLLOWOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCKEYFOLLOWOSCFILTER1] = std::clamp(Temp,52, 88); // 52 to 88 
     patchToKeyFollowOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_osc_filt1[CCKEYFOLLOWOSCFILTER1] - 64);
     return true;
 }
 Serial8.println(F("keyFollowOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathModOscFilter1Menu[1] = {
    { "BrMdOF1:\n " , breathModOscFilter1Fun   }
};
bool breathModOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCBREATHMODOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCBREATHMODOSCFILTER1] = std::clamp(Temp,0, 127); 
     patchToBreathModOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_osc_filt1[CCBREATHMODOSCFILTER1]);
     return true;
 }
 Serial8.println(F("breathModOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathCurveOscFilter1Menu[1] = {
    { "BrCvOF1:\n " , breathCurveOscFilter1Fun   }
};
bool breathCurveOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCBREATHCURVEOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCBREATHCURVEOSCFILTER1] = std::clamp(Temp,0, 127); 
     patchToBreathOscFiltCurveLines1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_osc_filt1[CCBREATHCURVEOSCFILTER1]-64);
     return true;
 }
 Serial8.println(F("breathCurveOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoFreqOscFilter1Menu[1] = {
    { "LfoFOF1:\n " , lfoFreqOscFilter1Fun   }
};
bool lfoFreqOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCLFOFREQOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCLFOFREQOSCFILTER1] = std::clamp(Temp,0, 127); 
     patchToLfoFreqOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_osc_filt1[CCLFOFREQOSCFILTER1]);
     return true;
 }
 Serial8.println(F("lfoFreqOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoDepthOscFilter1Menu[1] = {
    { "LfoDOF1:\n " , lfoDepthOscFilter1Fun   }
};
bool lfoDepthOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCLFODEPTHOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCLFODEPTHOSCFILTER1] = std::clamp(Temp,0, 127); 
     patchToLfoDepthOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt1[CCLFODEPTHOSCFILTER1]);
     return true;
 }
 Serial8.println(F("lfoDepthOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoBreathOscFilter1Menu[1] = {
    { "LfoBOF1:\n " , lfoBreathOscFilter1Fun   }
};
bool lfoBreathOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCLFOBREATHOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCLFOBREATHOSCFILTER1] = std::clamp(Temp,0, 127); 
     patchToLfoBreathOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt1[CCLFOBREATHOSCFILTER1]-64);
     return true;
 }
 Serial8.println(F("lfoBreathOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoThreshOscFilter1Menu[1] = {
    { "LfoTOF1:\n " , lfoThreshOscFilter1Fun   }
};
bool lfoThreshOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCLFOTHRESHOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCLFOTHRESHOSCFILTER1] = std::clamp(Temp,0, 127); 
     patchToLfoThreshOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt1[CCLFOTHRESHOSCFILTER1]);
     return true;
 }
 Serial8.println(F("lfoThreshOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepTimeOscFilter1Menu[1] = {
    { "SwpTOF1:\n " , sweepTimeOscFilter1Fun   }
};
bool sweepTimeOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCSWEEPTIMEOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCSWEEPTIMEOSCFILTER1] = std::clamp(Temp,0, 127); 
     patchToSweepTimeOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %02d    ", current_patch.nrpn_msb_osc_filt1[CCSWEEPTIMEOSCFILTER1]);
     return true;
 }
 Serial8.println(F("sweepTimeOscFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepDepthOscFilter1Menu[1] = {
    { "SwpDOF1:\n " , sweepDepthOscFilter1Fun   }
};
bool sweepDepthOscFilter1Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt1[CCSWEEPDEPTHOSCFILTER1];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt1[CCSWEEPDEPTHOSCFILTER1] = std::clamp(Temp,0, 127); 
     patchToSweepDepthOscFilter1(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt1[CCSWEEPDEPTHOSCFILTER1]-64);
     return true;
 }
 Serial8.println(F("sweepDepthNoiseFilter1Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

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
bool modeOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCMODEOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCMODEOSCFILTER2] = std::clamp(Temp,0, 4);
     patchToModeOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     switch (current_patch.nrpn_msb_osc_filt2[CCMODEOSCFILTER2]) {
         case LP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
         case HP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " HP"); break; 
         case BP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " BP"); break; 
         case NTC:
             sprintf(myMenu.str_oledbuf,"   %s    ", "NTC"); break; 
         case THRU:
             sprintf(myMenu.str_oledbuf,"   %s    ", "OFF"); break; 
         default:
             sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
     }
     return true;
 }
 Serial8.println(F("modeOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM freqOscFilter2Menu[1] = {
    { "FreqOF2:\n " , freqOscFilter2Fun   }
};
bool freqOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCFREQOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCFREQOSCFILTER2] = std::clamp(Temp,36, 124); // c2 to e9
     patchToFreqOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     float FreqFilterNN = (float)current_patch.nrpn_msb_osc_filt2[CCFREQOSCFILTER2];
     float FreqFilter = 440.0*pow(2, (FreqFilterNN-69.0)/12 );
     sprintf(myMenu.str_oledbuf,"%5.1fHz", FreqFilter);
     return true;
 }
 Serial8.println(F("freqOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM qOscFilter2Menu[1] = {
    { "Q OscF2:\n " , qOscFilter2Fun   }
};
bool qOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCQFACTOROSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCQFACTOROSCFILTER2] = std::clamp(Temp,5, 127); // 5 = 0.5
     patchToQFactorOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     float QFactor = (float)current_patch.nrpn_msb_osc_filt2[CCQFACTOROSCFILTER2]/10.0f;
     sprintf(myMenu.str_oledbuf,"   %2.1f   ", QFactor);
     return true;
 }
 Serial8.println(F("qOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM keyFollowOscFilter2Menu[1] = {
    { "KeyFOF2:\n " , keyFollowOscFilter2Fun   }
};
bool keyFollowOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCKEYFOLLOWOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCKEYFOLLOWOSCFILTER2] = std::clamp(Temp,52, 88); // 52 to 88 
     patchToKeyFollowOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_osc_filt2[CCKEYFOLLOWOSCFILTER2] - 64);
     return true;
 }
 Serial8.println(F("keyFollowOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathModOscFilter2Menu[1] = {
    { "BrMdOF2:\n " , breathModOscFilter2Fun   }
};
bool breathModOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCBREATHMODOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCBREATHMODOSCFILTER2] = std::clamp(Temp,0, 127); 
     patchToBreathModOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_osc_filt2[CCBREATHMODOSCFILTER2]);
     return true;
 }
 Serial8.println(F("breathModOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathCurveOscFilter2Menu[1] = {
    { "BrCvOF2:\n " , breathCurveOscFilter2Fun   }
};
bool breathCurveOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCBREATHCURVEOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCBREATHCURVEOSCFILTER2] = std::clamp(Temp,0, 127); 
     patchToBreathOscFiltCurveLines2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_osc_filt2[CCBREATHCURVEOSCFILTER2]-64);
     return true;
 }
 Serial8.println(F("breathCurveOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoFreqOscFilter2Menu[1] = {
    { "LfoFOF2:\n " , lfoFreqOscFilter2Fun   }
};
bool lfoFreqOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCLFOFREQOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCLFOFREQOSCFILTER2] = std::clamp(Temp,0, 127); 
     patchToLfoFreqOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_osc_filt2[CCLFOFREQOSCFILTER2]);
     return true;
 }
 Serial8.println(F("lfoFreqOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoDepthOscFilter2Menu[1] = {
    { "LfoDOF2:\n " , lfoDepthOscFilter2Fun   }
};
bool lfoDepthOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCLFODEPTHOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCLFODEPTHOSCFILTER2] = std::clamp(Temp,0, 127); 
     patchToLfoDepthOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_osc_filt2[CCLFODEPTHOSCFILTER2]);
     return true;
 }
 Serial8.println(F("lfoDepthOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoBreathOscFilter2Menu[1] = {
    { "LfoBOF2:\n " , lfoBreathOscFilter2Fun   }
};
bool lfoBreathOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCLFOBREATHOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCLFOBREATHOSCFILTER2] = std::clamp(Temp,0, 127); 
     patchToLfoBreathOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt2[CCLFOBREATHOSCFILTER2]-64);
     return true;
 }
 Serial8.println(F("lfoBreathOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoThreshOscFilter2Menu[1] = {
    { "LfoTOF2:\n " , lfoThreshOscFilter2Fun   }
};
bool lfoThreshOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCLFOTHRESHOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCLFOTHRESHOSCFILTER2] = std::clamp(Temp,0, 127); 
     patchToLfoThreshOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt2[CCLFOTHRESHOSCFILTER2]);
     return true;
 }
 Serial8.println(F("lfoThreshOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepTimeOscFilter2Menu[1] = {
    { "SwpTOF2:\n " , sweepTimeOscFilter2Fun   }
};
bool sweepTimeOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCSWEEPTIMEOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCSWEEPTIMEOSCFILTER2] = std::clamp(Temp,0, 127); 
     patchToSweepTimeOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %02d    ", current_patch.nrpn_msb_osc_filt2[CCSWEEPTIMEOSCFILTER2]);
     return true;
 }
 Serial8.println(F("sweepTimeOscFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepDepthOscFilter2Menu[1] = {
    { "SwpDOF2:\n " , sweepDepthOscFilter2Fun   }
};
bool sweepDepthOscFilter2Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_osc_filt2[CCSWEEPDEPTHOSCFILTER2];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_osc_filt2[CCSWEEPDEPTHOSCFILTER2] = std::clamp(Temp,0, 127); 
     patchToSweepDepthOscFilter2(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt2[CCSWEEPDEPTHOSCFILTER2]-64);
     return true;
 }
 Serial8.println(F("sweepDepthNoiseFilter2Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

MenuItem PROGMEM patchNoiseFilter3Menu[14] = { 
    { "Back    " , goUpOneMenu   }
   ,{ "Link NFs" , gotoLinkNoiseFiltersMenu   }
   ,{ "Mode NF1" , gotoModeNoiseFilter3Menu   }
   ,{ "Freq NF1" , gotoFreqNoiseFilter3Menu   }
   ,{ "Q NF1   " , gotoQNoiseFilter3Menu   }
   ,{ "KeyFlNF1" , gotoKeyFollowNoiseFilter3Menu   }
   ,{ "BrModNF1" , gotoBreathModNoiseFilter3Menu   }
   ,{ "BrCrvNF1" , gotoBreathCurveNoiseFilter3Menu   }
   ,{ "LfoF NF1" , gotoLfoFreqNoiseFilter3Menu   }
   ,{ "LfoD NF1" , gotoLfoDepthNoiseFilter3Menu   }
   ,{ "LfoBrNF1" , gotoLfoBreathNoiseFilter3Menu   }
   ,{ "LfoThNF1" , gotoLfoThresholdNoiseFilter3Menu   }
   ,{ "SwpT NF1" , gotoSweepTimeNoiseFilter3Menu   }
   ,{ "SwpD NF1" , gotoSweepDepthNoiseFilter3Menu   }
};

MenuItem PROGMEM linkNoiseFiltersMenu[1] = {
    { "LinkNFs:\n " , linkNoiseFiltersFun   }
};
bool linkNoiseFiltersFun() { 
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_common1[CCLINKNOISEFILTERS];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_common1[CCLINKNOISEFILTERS] = std::clamp(Temp,1, 2);
     patchToLinkNoiseFilters(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     if(current_patch.nrpn_msb_common1[CCLINKNOISEFILTERS]%2 == 0){
         ModeNoiseFilter4 = ModeNoiseFilter4_stored;
     }
     sprintf(myMenu.str_oledbuf," %s ", 
         current_patch.nrpn_msb_common1[CCLINKNOISEFILTERS]%2 ? " Linkded": "UnLinked"); //1 Linked, 2 UnLinked
     return true;
 }
 Serial8.println(F("linkNoiseFiltersFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM modeNoiseFilter3Menu[1] = {
    { "ModeNF1:\n " , modeNoiseFilter3Fun   }
};
bool modeNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCMODENOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCMODENOISEFILTER3] = std::clamp(Temp,0, 4);
     patchToModeNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     switch (current_patch.nrpn_msb_noise_filt3[CCMODENOISEFILTER3]) {
         case LP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
         case HP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " HP"); break; 
         case BP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " BP"); break; 
         case NTC:
             sprintf(myMenu.str_oledbuf,"   %s    ", "NTC"); break; 
         case THRU:
             sprintf(myMenu.str_oledbuf,"   %s    ", "OFF"); break; 
         default:
             sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
     }
     return true;
 }
 Serial8.println(F("modeNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM freqNoiseFilter3Menu[1] = {
    { "FreqNF1:\n " , freqNoiseFilter3Fun   }
};
bool freqNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCFREQNOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCFREQNOISEFILTER3] = std::clamp(Temp,36, 124); // c2 to e9
     patchToFreqNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     float FreqFilterNN = (float)current_patch.nrpn_msb_noise_filt3[CCFREQNOISEFILTER3];
     float FreqFilter = 440.0*pow(2, (FreqFilterNN-69.0)/12 );
     sprintf(myMenu.str_oledbuf,"%5.1fHz", FreqFilter);
     return true;
 }
 Serial8.println(F("freqNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM qNoiseFilter3Menu[1] = {
    { "Q NoiF1:\n " , qNoiseFilter3Fun   }
};
bool qNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCQFACTORNOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCQFACTORNOISEFILTER3] = std::clamp(Temp,5, 127); // 5 = 0.5
     patchToQFactorNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     float QFactor = (float)current_patch.nrpn_msb_noise_filt3[CCQFACTORNOISEFILTER3]/10.0f;
     sprintf(myMenu.str_oledbuf,"   %3.1f   ", QFactor);
     return true;
 }
 Serial8.println(F("qNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM keyFollowNoiseFilter3Menu[1] = {
    { "KeyFNF1:\n " , keyFollowNoiseFilter3Fun   }
};
bool keyFollowNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCKEYFOLLOWNOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCKEYFOLLOWNOISEFILTER3] = std::clamp(Temp,52, 88); // 52 to 88 
     patchToKeyFollowNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt3[CCKEYFOLLOWNOISEFILTER3] - 64);
     return true;
 }
 Serial8.println(F("keyFollowNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathModNoiseFilter3Menu[1] = {
    { "BrMdNF1:\n " , breathModNoiseFilter3Fun   }
};
bool breathModNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCBREATHMODNOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCBREATHMODNOISEFILTER3] = std::clamp(Temp,0, 127); 
     patchToBreathModNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt3[CCBREATHMODNOISEFILTER3]);
     return true;
 }
 Serial8.println(F("breathModNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathCurveNoiseFilter3Menu[1] = {
    { "BrCvNF1:\n " , breathCurveNoiseFilter3Fun   }
};
bool breathCurveNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCBREATHCURVENOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCBREATHCURVENOISEFILTER3] = std::clamp(Temp,0, 127); 
     patchToBreathNoiseFiltCurveLines3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt3[CCBREATHCURVENOISEFILTER3]-64);
     return true;
 }
 Serial8.println(F("breathCurveNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoFreqNoiseFilter3Menu[1] = {
    { "LfoFNF1:\n " , lfoFreqNoiseFilter3Fun   }
};
bool lfoFreqNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCLFOFREQNOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCLFOFREQNOISEFILTER3] = std::clamp(Temp,0, 127); 
     patchToLfoFreqNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt3[CCLFOFREQNOISEFILTER3]);
     return true;
 }
 Serial8.println(F("lfoFreqNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoDepthNoiseFilter3Menu[1] = {
    { "LfoDNF1:\n " , lfoDepthNoiseFilter3Fun   }
};
bool lfoDepthNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCLFODEPTHNOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCLFODEPTHNOISEFILTER3] = std::clamp(Temp,0, 127); 
     patchToLfoDepthNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt3[CCLFODEPTHNOISEFILTER3]);
     return true;
 }
 Serial8.println(F("lfoDepthNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoBreathNoiseFilter3Menu[1] = {
    { "LfoBNF1:\n " , lfoBreathNoiseFilter3Fun   }
};
bool lfoBreathNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCLFOBREATHNOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCLFOBREATHNOISEFILTER3] = std::clamp(Temp,0, 127); 
     patchToLfoBreathNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCLFOBREATHNOISEFILTER3]-64);
     return true;
 }
 Serial8.println(F("lfoBreathNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoThreshNoiseFilter3Menu[1] = {
    { "LfoTNF1:\n " , lfoThreshNoiseFilter3Fun   }
};
bool lfoThreshNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCLFOTHRESHNOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCLFOTHRESHNOISEFILTER3] = std::clamp(Temp,0, 127); 
     patchToLfoThreshNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCLFOTHRESHNOISEFILTER3]);
     return true;
 }
 Serial8.println(F("lfoThreshNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepTimeNoiseFilter3Menu[1] = {
    { "SwpTNF1:\n " , sweepTimeNoiseFilter3Fun   }
};
bool sweepTimeNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCSWEEPTIMENOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCSWEEPTIMENOISEFILTER3] = std::clamp(Temp,0, 127); 
     patchToSweepTimeNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCSWEEPTIMENOISEFILTER3]);
     return true;
 }
 Serial8.println(F("sweepTimeNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepDepthNoiseFilter3Menu[1] = {
    { "SwpDNF1:\n " , sweepDepthNoiseFilter3Fun   }
};
bool sweepDepthNoiseFilter3Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt3[CCSWEEPDEPTHNOISEFILTER3];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt3[CCSWEEPDEPTHNOISEFILTER3] = std::clamp(Temp,0, 127); 
     patchToSweepDepthNoiseFilter3(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCSWEEPDEPTHNOISEFILTER3]-64);
     return true;
 }
 Serial8.println(F("sweepDepthNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

MenuItem PROGMEM patchNoiseFilter4Menu[13] = {
    { "Back    " , goUpOneMenu   }
   ,{ "Mode NF2" , gotoModeNoiseFilter4Menu   }
   ,{ "Freq NF2" , gotoFreqNoiseFilter4Menu   }
   ,{ "Q NF2   " , gotoQNoiseFilter4Menu   }
   ,{ "KeyFlNF2" , gotoKeyFollowNoiseFilter4Menu   }
   ,{ "BrModNF2" , gotoBreathModNoiseFilter4Menu   }
   ,{ "BrCrvNF2" , gotoBreathCurveNoiseFilter4Menu   }
   ,{ "LfoF NF2" , gotoLfoFreqNoiseFilter4Menu   }
   ,{ "LfoD NF2" , gotoLfoDepthNoiseFilter4Menu   }
   ,{ "LfoBrNF2" , gotoLfoBreathNoiseFilter4Menu   }
   ,{ "LfoThNF2" , gotoLfoThresholdNoiseFilter4Menu   }
   ,{ "SwpT NF2" , gotoSweepTimeNoiseFilter4Menu   }
   ,{ "SwpD NF2" , gotoSweepDepthNoiseFilter4Menu   }
};

MenuItem PROGMEM modeNoiseFilter4Menu[1] = {
    { "ModeNF2:\n " , modeNoiseFilter4Fun   }
};
bool modeNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCMODENOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCMODENOISEFILTER4] = std::clamp(Temp,0, 4);
     patchToModeNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     switch (current_patch.nrpn_msb_noise_filt4[CCMODENOISEFILTER4]) {
         case LP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
         case HP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " HP"); break; 
         case BP:
             sprintf(myMenu.str_oledbuf,"   %s    ", " BP"); break; 
         case NTC:
             sprintf(myMenu.str_oledbuf,"   %s    ", "NTC"); break; 
         case THRU:
             sprintf(myMenu.str_oledbuf,"   %s    ", "OFF"); break; 
         default:
             sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
     }
     return true;
 }
 Serial8.println(F("modeNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM freqNoiseFilter4Menu[1] = {
    { "FreqNF2:\n " , freqNoiseFilter4Fun   }
};
bool freqNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCFREQNOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCFREQNOISEFILTER4] = std::clamp(Temp,36, 124); // c2 to e9
     patchToFreqNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     float FreqFilterNN = (float)current_patch.nrpn_msb_noise_filt4[CCFREQNOISEFILTER4];
     float FreqFilter = 440.0*pow(2, (FreqFilterNN-69.0)/12 );
     sprintf(myMenu.str_oledbuf,"%5.1fHz", FreqFilter);
     return true;
 }
 Serial8.println(F("freqNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM qNoiseFilter4Menu[1] = {
    { "Q NoiF2:\n " , qNoiseFilter4Fun   }
};
bool qNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCQFACTORNOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCQFACTORNOISEFILTER4] = std::clamp(Temp,5, 127); // 5 = 0.5
     patchToFreqNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     float QFactor = (float)current_patch.nrpn_msb_noise_filt4[CCQFACTORNOISEFILTER4]/10.0f;
     sprintf(myMenu.str_oledbuf,"   %4.1f   ", QFactor);
     return true;
 }
 Serial8.println(F("qNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM keyFollowNoiseFilter4Menu[1] = {
    { "KeyFNF2:\n " , keyFollowNoiseFilter4Fun   }
};
bool keyFollowNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCKEYFOLLOWNOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCKEYFOLLOWNOISEFILTER4] = std::clamp(Temp,52, 88); // 52 to 88 
     patchToKeyFollowNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt4[CCKEYFOLLOWNOISEFILTER4] - 64);
     return true;
 }
 Serial8.println(F("keyFollowNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathModNoiseFilter4Menu[1] = {
    { "BrMdNF2:\n " , breathModNoiseFilter4Fun   }
};
bool breathModNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCBREATHMODNOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCBREATHMODNOISEFILTER4] = std::clamp(Temp,0, 127); 
     patchToBreathModNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt4[CCBREATHMODNOISEFILTER4]);
     return true;
 }
 Serial8.println(F("breathModNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM breathCurveNoiseFilter4Menu[1] = {
    { "BrCvNF2:\n " , breathCurveNoiseFilter4Fun   }
};
bool breathCurveNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCBREATHCURVENOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCBREATHCURVENOISEFILTER4] = std::clamp(Temp,0, 127); 
     patchToBreathNoiseFiltCurveLines4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt4[CCBREATHCURVENOISEFILTER4]-64);
     return true;
 }
 Serial8.println(F("breathCurveNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoFreqNoiseFilter4Menu[1] = {
    { "LfoFNF2:\n " , lfoFreqNoiseFilter4Fun   }
};
bool lfoFreqNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCLFOFREQNOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCLFOFREQNOISEFILTER4] = std::clamp(Temp,0, 127); 
     patchToLfoFreqNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt4[CCLFOFREQNOISEFILTER4]);
     return true;
 }
 Serial8.println(F("lfoFreqNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoDepthNoiseFilter4Menu[1] = {
    { "LfoDNF2:\n " , lfoDepthNoiseFilter4Fun   }
};
bool lfoDepthNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCLFODEPTHNOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCLFODEPTHNOISEFILTER4] = std::clamp(Temp,0, 127); 
     patchToLfoDepthNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"%03d ", current_patch.nrpn_msb_noise_filt4[CCLFODEPTHNOISEFILTER4]);
     return true;
 }
 Serial8.println(F("lfoDepthNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoBreathNoiseFilter4Menu[1] = {
    { "LfoBNF2:\n " , lfoBreathNoiseFilter4Fun   }
};
bool lfoBreathNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCLFOBREATHNOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCLFOBREATHNOISEFILTER4] = std::clamp(Temp,0, 127); 
     patchToLfoBreathNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt4[CCLFOBREATHNOISEFILTER4]-64);
     return true;
 }
 Serial8.println(F("lfoBreathNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM lfoThreshNoiseFilter4Menu[1] = {
    { "LfoTNF2:\n " , lfoThreshNoiseFilter4Fun   }
};
bool lfoThreshNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCLFOTHRESHNOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCLFOTHRESHNOISEFILTER4] = std::clamp(Temp,0, 127); 
     patchToLfoThreshNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt4[CCLFOTHRESHNOISEFILTER4]);
     return true;
 }
 Serial8.println(F("lfoThreshNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepTimeNoiseFilter4Menu[1] = {
    { "SwpTNF2:\n " , sweepTimeNoiseFilter4Fun   }
};
bool sweepTimeNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCSWEEPTIMENOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCSWEEPTIMENOISEFILTER4] = std::clamp(Temp,0, 127); 
     patchToSweepTimeNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt4[CCSWEEPTIMENOISEFILTER4]);
     return true;
 }
 Serial8.println(F("sweepTimeNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM sweepDepthNoiseFilter4Menu[1] = {
    { "SwpDNF2:\n " , sweepDepthNoiseFilter4Fun   }
};
bool sweepDepthNoiseFilter4Fun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise_filt4[CCSWEEPDEPTHNOISEFILTER4];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise_filt4[CCSWEEPDEPTHNOISEFILTER4] = std::clamp(Temp,0, 127); 
     patchToSweepDepthNoiseFilter4(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt4[CCSWEEPDEPTHNOISEFILTER4]-64);
     return true;
 }
 Serial8.println(F("sweepDepthNoiseFilter4Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

MenuItem PROGMEM patchNoiseMenu[4] = {
    { "Back    " , goUpOneMenu   }
   ,{ "Nz Time " , gotoNoiseTimeMenu   }
   ,{ "NzBreath" , gotoNoiseBreathMenu   }
   ,{ "Nz Level" , gotoNoiseLevelMenu   }
};

MenuItem PROGMEM noiseTimeMenu[1] = {
    { "Nz Time:\n " , noiseTimeFun   }
};
bool noiseTimeFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise[CCNOISETIME];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise[CCNOISETIME] = std::clamp(Temp,0, 127); 
     patchToNoiseTime(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise[CCNOISETIME]);
     return true;
 }
 Serial8.println(F("noiseTimeFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM noiseBreathMenu[1] = {
    { "NzBreth:\n " , noiseBreathFun   }
};
bool noiseBreathFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise[CCNOISEBREATHCURVE];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise[CCNOISEBREATHCURVE] = std::clamp(Temp,0, 127); 
     patchToNoiseBreathCurveLines(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise[CCNOISEBREATHCURVE]-64);
     return true;
 }
 Serial8.println(F("noiseTimeFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM noiseLevelMenu[1] = {
    { "NzLevel:\n " , noiseLevelFun   }
};
bool noiseLevelFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_noise[CCNOISELEVEL];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_noise[CCNOISELEVEL] = std::clamp(Temp,0, 127); 
     patchToNoiseLevel(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise[CCNOISELEVEL]);
     return true;
 }
 Serial8.println(F("noiseLevelFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

MenuItem PROGMEM patchFormantMenu[1] = {
    { "Formant:\n ", formantSelectFun    }
};
bool formantSelectFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_common1[CCFORMANT];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_common1[CCFORMANT] = std::clamp(Temp,0, 2);
     patchToFormant(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     switch (current_patch.nrpn_msb_common1[CCFORMANT]) {
         case 0:
             sprintf(myMenu.str_oledbuf,"  %s", "OFF     "); break; 
         case 1:
             sprintf(myMenu.str_oledbuf,"  %s", "WoodWind"); break; 
         case 2:
             sprintf(myMenu.str_oledbuf,"  %s", "String  "); break; 
         default:
             sprintf(myMenu.str_oledbuf,"  %s", "OFF     "); break; 
      }
     return true;
 }
 Serial8.println(F("modeNoiseFilter3Fun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

MenuItem PROGMEM patchAmpMenu[1] = {
    { "Amp:\n ", patchAmpAdjustFun    }
};
bool patchAmpAdjustFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_common2[CCAMPLEVEL];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_common2[CCAMPLEVEL] = std::clamp(Temp,0, 127); 
     patchToAmpLevel(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_common2[CCAMPLEVEL]);
     return true;
 }
 Serial8.println(F("patchAmpAdjustFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

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
bool keyTriggerFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_common1[CCKEYTRIGGER];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_common1[CCKEYTRIGGER] = std::clamp(Temp,0, 1);
     patchToKeyTriggerSingle(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"  %s  ", 
             current_patch.nrpn_msb_common1[CCKEYTRIGGER]? "Single": " Multi");
     return true;
 }
 Serial8.println(F("keyTriggerFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM octButtonLevelMenu[1] = {
    { "OctBtnL:\n ", octButtonLevelFun    }
};
bool octButtonLevelFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_common2[CCOCTBUTTONLEVEL];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_common2[CCOCTBUTTONLEVEL] = std::clamp(Temp,0, 127); 
     patchToOctButtonLevel(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_common2[CCOCTBUTTONLEVEL]);
     return true;
 }
 Serial8.println(F("octButtonLevelFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM bendRangeMenu[1] = {
    { "BendRng:\n ", bendRangeFun    }
};
bool bendRangeFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_common1[CCBENDRANGE];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_common1[CCBENDRANGE] = std::clamp(Temp,0, 127); 
     patchToBendRange(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_common1[CCBENDRANGE]);
     return true;
 }
 Serial8.println(F("bendRangeFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}
MenuItem PROGMEM bendStepMenu[1] = {
    { "BendStp:\n ", bendStepFun    }
};
bool bendStepFun() {
 if(myMenu.updateLeafValue){
     int Temp = current_patch.nrpn_msb_common1[CCBENDSTEP];
     Temp += myMenu.updateLeafValue;
     current_patch.nrpn_msb_common1[CCBENDSTEP] = std::clamp(Temp,0, 1);
     patchToBendStep(&current_patch);
     //preUpdateSynthVariablesFlag = true; // to use squelch
     updateSynthVariablesFlag = true;
     sprintf(myMenu.str_oledbuf,"   %s    ", 
             current_patch.nrpn_msb_common1[CCBENDSTEP]? " On": "Off");
     return true;
 }
 Serial8.println(F("bendStepFun calls goUpOneMenu"));
 return goUpOneMenu(); 
}

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
    sprintf(myMenu.str_oledbuf,"  %ld   ", mix_linein);
    Serial8.println(F("mixLineIn: updated mix_linein "));    
    return true; 
 }
//  display.clearDisplay(); // erase display
//  display.display(); // refresh display
  Serial8.println(F("auxInFun: goUpOneMenu"));
  return goUpOneMenu(); 
}
MenuItem PROGMEM octaveMenu[1] = {
    { "Octave: \n ", octaveFun    }
};
bool octaveFun() { 
  if(myMenu.updateLeafValue){
    Octave += myMenu.updateLeafValue;
    Octave = std::clamp((int)Octave,-2, 2);
    Octavef = ((float)Octave*12.0);
    sprintf(myMenu.str_oledbuf,"  %ld   ", Octave);
    Serial8.println(F("octaveFun: update Octave "));    
    return true; 
 }
//  display.clearDisplay(); // erase display
//  display.display(); // refresh display
  Serial8.println(F("octaveFun: goUpOneMenu"));
  return goUpOneMenu(); 
}
MenuItem PROGMEM semiMenu[1] = {
    { "Semiton:\n ", semiFun    }
};
bool semiFun() { 
  if(myMenu.updateLeafValue){
    Transpose += myMenu.updateLeafValue;
    Transpose = std::clamp((int)Transpose,-12, 12);
    Transposef = ((float)Transpose);
    sprintf(myMenu.str_oledbuf,"  %ld   ", Transpose);
    Serial8.println(F("semiFun: update Transpose "));    
    return true; 
  }
//  display.clearDisplay(); // erase display
//  display.display(); // refresh display
  Serial8.println(F("semiFun: goUpOneMenu"));
  return goUpOneMenu(); 
}
MenuItem PROGMEM centsMenu[1] = {
    { "Cents:  \n ", centsFun    }
};
bool centsFun() { 
  if(myMenu.updateLeafValue){
    FineTuneCents += myMenu.updateLeafValue;
    FineTuneCents = std::clamp((int)FineTuneCents,-100, 100);
    FineTuneCentsf =  ((float)FineTuneCents)/100.0f;
    sprintf(myMenu.str_oledbuf,"  %ld   ", FineTuneCents);
    Serial8.println(F("centsFun: update FineTuneCents "));    
    return true; 
  }
//  display.clearDisplay(); // erase display
//  display.display(); // refresh display
  Serial8.println(F("centsFun: goUpOneMenu"));
  return goUpOneMenu(); 
}
MenuItem PROGMEM breathCCMenu[1] = {
    { "BrethCC:\n ", breathCCFun    }
};
bool breathCCFun() { 
  if(myMenu.updateLeafValue){
    // always skip cc 06 (used with RPNs/NRPNs)
    breath_cc += myMenu.updateLeafValue>0 ? 
            (breath_cc == 5 ? 2 : 1) 
            : myMenu.updateLeafValue<0 ? 
            (breath_cc == 7 ? -2 : -1)
            : 0;
    breath_cc = std::clamp((int)breath_cc,1, 11);
    sprintf(myMenu.str_oledbuf,"  %ld   ", breath_cc);
    Serial8.println(F("breathCCFun: update breath_cc "));    
    return true; 
  } 
//  display.clearDisplay(); // erase display
//  display.display(); // refresh display
  Serial8.println(F("breathCCFun: goUpOneMenu"));    
  return goUpOneMenu(); 
}

MenuList listTopMenu(topMenu, 16);
MenuList listEscapeMenu(escapeMenu, 1);
MenuList listVolAdjustMenu(volAdjustMenu, 1);
MenuList listFxSourceMenu(fxSourceMenu, 1);
MenuList listPatchSelectMenu(patchSelectMenu, 1);
MenuList listPatchResetMenu(patchResetMenu, 2);
MenuList listPatchPasteMenu(patchPasteMenu, 1);
MenuList listNamingMenu(namingMenu, 1);
MenuList listPatchSwapMenu(patchSwapMenu, 1);
MenuList listPatchFxMenu(patchFxMenu, 23);
MenuList listPatchOsc1Menu(patchOsc1Menu, 18);
MenuList listPatchOsc2Menu(patchOsc2Menu, 19);
MenuList listPatchOscFilter1Menu(patchOscFilter1Menu, 14);
MenuList listPatchOscFilter2Menu(patchOscFilter2Menu, 13);
MenuList listPatchNoiseFilter3Menu(patchNoiseFilter3Menu, 14);
MenuList listPatchNoiseFilter4Menu(patchNoiseFilter4Menu, 13);
MenuList listPatchNoiseMenu(patchNoiseMenu, 4);
MenuList listPatchFormantMenu(patchFormantMenu, 1);
MenuList listPatchAmpMenu(patchAmpMenu, 1);
MenuList listPatchCommonMenu(patchCommonMenu, 5);

MenuList listSystemAdjMenu(systemAdjMenu, 6);

//MenuList listFxCopyMenu(fxCopyMenu,2);
MenuList listSaveFxGlobalMenu(saveFxGlobalMenu,1);
MenuList listFxPasteMenu(fxPasteMenu,1);
//MenuList listFxSwapMenu(fxSwapMenu,2);
MenuList listDelayLevelMenu(delayLevelMenu,1);
MenuList listDelayTimeLMenu(delayTimeLMenu,1);
MenuList listDelayPongMenu(delayPongMenu,1);
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
MenuList listModeNoiseFilter3Menu(modeNoiseFilter3Menu, 1);
MenuList listFreqNoiseFilter3Menu(freqNoiseFilter3Menu, 1);
MenuList listQNoiseFilter3Menu(qNoiseFilter3Menu, 1);
MenuList listKeyFollowNoiseFilter3Menu(keyFollowNoiseFilter3Menu, 1);
MenuList listBreathModNoiseFilter3Menu(breathModNoiseFilter3Menu, 1);
MenuList listBreathCurveNoiseFilter3Menu(breathCurveNoiseFilter3Menu, 1);
MenuList listLfoFreqNoiseFilter3Menu(lfoFreqNoiseFilter3Menu, 1);
MenuList listLfoDepthNoiseFilter3Menu(lfoDepthNoiseFilter3Menu, 1);
MenuList listLfoBreathNoiseFilter3Menu(lfoBreathNoiseFilter3Menu, 1);
MenuList listLfoThresholdNoiseFilter3Menu(lfoThreshNoiseFilter3Menu, 1);
MenuList listSweepTimeNoiseFilter3Menu(sweepTimeNoiseFilter3Menu, 1);
MenuList listSweepDepthNoiseFilter3Menu(sweepDepthNoiseFilter3Menu, 1);

MenuList listModeNoiseFilter4Menu(modeNoiseFilter4Menu, 1);
MenuList listFreqNoiseFilter4Menu(freqNoiseFilter4Menu, 1);
MenuList listQNoiseFilter4Menu(qNoiseFilter4Menu, 1);
MenuList listKeyFollowNoiseFilter4Menu(keyFollowNoiseFilter4Menu, 1);
MenuList listBreathModNoiseFilter4Menu(breathModNoiseFilter4Menu, 1);
MenuList listBreathCurveNoiseFilter4Menu(breathCurveNoiseFilter4Menu, 1);
MenuList listLfoFreqNoiseFilter4Menu(lfoFreqNoiseFilter4Menu, 1);
MenuList listLfoDepthNoiseFilter4Menu(lfoDepthNoiseFilter4Menu, 1);
MenuList listLfoBreathNoiseFilter4Menu(lfoBreathNoiseFilter4Menu, 1);
MenuList listLfoThresholdNoiseFilter4Menu(lfoThreshNoiseFilter4Menu, 1);
MenuList listSweepTimeNoiseFilter4Menu(sweepTimeNoiseFilter4Menu, 1);
MenuList listSweepDepthNoiseFilter4Menu(sweepDepthNoiseFilter4Menu, 1);

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
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  display.clearBuffer(); // erase display
  //display.clearDisplay(); // erase display
  //display.display(); // refresh display
  myMenu.setCurrentMenu(&listVolAdjustMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %ld   ", vol);
  display.drawStr(0,2*display.getMaxCharHeight(),myMenu.str_oledbuf); //2nd line
  //display.println(myMenu.str_oledbuf);
  return true;
}
bool gotoPatchSelectMenu() {
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  display.clearBuffer(); // erase display
  //display.clearDisplay(); // erase display
  //display.display(); // refresh display
  myMenu.setCurrentMenu(&listPatchSelectMenu);
  myMenu.knobAcceleration = 1;
  String ps( current_patch.patch_string );
  //ps.setCharAt( ps.indexOf(' '), '\n'); 
  //sprintf(myMenu.str_oledbuf, "%03d\n%s", current_patchNumber+1, ps.c_str() );
  //sprintf(myMenu.str_oledbuf, "%03d %s", current_patchNumber+1, ps.c_str() );
    //sprintf(myMenu.str_oledbuf, "%03d\n%s", current_patchNumber+1, ps.c_str() );
  char pn[3] ={0};
  sprintf(pn, "%03d", current_patchNumber+1 );
  display.drawStr(7*display.getMaxCharWidth(),display.getMaxCharHeight(),pn);
  //display.sendBuffer(); //draw number before "Patch: " 
  sprintf(myMenu.str_oledbuf, " %s", ps.c_str() );
  myMenu.drawStrNl(0,2*display.getMaxCharHeight(),myMenu.str_oledbuf); //2nd line
  Serial8.println(myMenu.str_oledbuf);
  return true;
}
bool gotoPatchResetMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchResetMenu);
  return true;
}
/*
bool gotoPatchCopyMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchCopyMenu);
  return true;
}
*/
bool gotoNamingMenu(){
  //display.clearDisplay(); // erase display
  display.clearBuffer(); // erase display
  //myMenu.previousMenuStack.push(myMenu.currentMenu);
  //myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listNamingMenu);
  myMenu.knobAcceleration = 1;
  memcpy((byte*)myMenu.str_namingbuf,(byte*)copy_buffer_patch.patch_string, 
          sizeof(myMenu.str_namingbuf));
  sprintf(myMenu.str_oledbuf, "LngPrs2wr\n%s",myMenu.str_namingbuf);
  myMenu.namingPos = 0; 
  myMenu.alphaNumIdx = alphaNumString.indexOf(myMenu.str_namingbuf[0]); 
  myMenu.namingState = ALPHANUM;
  return true;
}
bool gotoPatchPasteMenu(){
  //display.clearDisplay(); // erase display
  display.clearBuffer(); // erase display
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchPasteMenu);
  myMenu.knobAcceleration = 4;
    char pn[3] ={0};
    if (paste_patchNumber == NUMBER_OF_PATCHES){
        sprintf(pn, "   ");
        display.drawStr(7*display.getMaxCharWidth(),display.getMaxCharHeight(),pn);
        sprintf(myMenu.str_oledbuf, "Exit w/o saving");
        //sprintf(myMenu.str_oledbuf, "Exit w/o\n saving");
        myMenu.drawStrNl(0,2*display.getMaxCharHeight(),myMenu.str_oledbuf); //2nd line
    }
    else { 
        sprintf(pn, "%03d", paste_patchNumber+1 );
        display.drawStr(7*display.getMaxCharWidth(),display.getMaxCharHeight(),pn);

        String ps( loadedPatches[paste_patchNumber].patch_string );
        sprintf(myMenu.str_oledbuf, " %s", ps.c_str() );
        myMenu.drawStrNl(0,2*display.getMaxCharHeight(),myMenu.str_oledbuf); //2nd line
        //ps.setCharAt( ps.indexOf(' '), '\n'); // TODO: spaces till end of line then \n
        //sprintf(myMenu.str_oledbuf, "%03d\n%s", paste_patchNumber+1, ps.c_str() );
        Serial8.println(loadedPatches[paste_patchNumber].patch_string);
        // load the patch 
        if (!patchLoaded[paste_patchNumber])
        {
            loadPatchSD(paste_patchNumber);
        }
    }
  return true;
}
bool gotoPatchSwapMenu(){
  //display.clearDisplay(); // erase display
  display.clearBuffer(); // erase display
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchSwapMenu);
  myMenu.knobAcceleration = 4;
    if (paste_patchNumber == NUMBER_OF_PATCHES){
        sprintf(myMenu.str_oledbuf, "Exit w/o\n saving");
    }
    else { 
        String ps( loadedPatches[paste_patchNumber].patch_string );
        ps.setCharAt( ps.indexOf(' '), '\n'); // TODO: spaces till end of line then \n
        sprintf(myMenu.str_oledbuf, "%03d\n%s", paste_patchNumber+1, ps.c_str() );
        Serial8.println(loadedPatches[paste_patchNumber].patch_string);
        // load the patch 
        if (!patchLoaded[paste_patchNumber])
        {
            loadPatchSD(paste_patchNumber);
        }
    }
  return true;
}
bool gotoPatchFxMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchFxMenu);
  myMenu.knobAcceleration = 1;
    return true;
}
bool gotoPatchOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchOsc1Menu);
  myMenu.knobAcceleration = 1;
    return true;
}
bool gotoPatchOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchOsc2Menu);
  myMenu.knobAcceleration = 1;
    return true;
}
bool gotoPatchOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchOscFilter1Menu);
  myMenu.knobAcceleration = 1;
    return true;
}
bool gotoPatchOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchOscFilter2Menu);
  myMenu.knobAcceleration = 1;
    return true;
}
bool gotoPatchNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchNoiseFilter3Menu);
  myMenu.knobAcceleration = 1;
    return true;
}
bool gotoPatchNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchNoiseFilter4Menu);
  myMenu.knobAcceleration = 1;
    return true;
}
bool gotoPatchNoiseMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchNoiseMenu);
  myMenu.knobAcceleration = 1;
    return true;
}
bool gotoPatchFormantMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchFormantMenu);
  myMenu.knobAcceleration = 1;
  switch (current_patch.nrpn_msb_common1[CCFORMANT]) {
     case 0:
         sprintf(myMenu.str_oledbuf,"  %s", "OFF     "); break; 
     case 1:
         sprintf(myMenu.str_oledbuf,"  %s", "WoodWind"); break; 
     case 2:
         sprintf(myMenu.str_oledbuf,"  %s", "String  "); break; 
     default:
         sprintf(myMenu.str_oledbuf,"  %s", "OFF     "); break; 
   }
  //display.clearDisplay();
  display.clearBuffer(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.drawStr(0,14,myMenu.str_oledbuf);
    return true;
}
bool gotoPatchAmpMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchAmpMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_common2[CCAMPLEVEL]);
  //display.clearDisplay();
  display.clearBuffer(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.drawStr(0,14,myMenu.str_oledbuf);
    return true;
}
bool gotoPatchCommonMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPatchCommonMenu);
  myMenu.knobAcceleration = 1;
    return true;
}

bool gotoSystemAdjMenu() {
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSystemAdjMenu);
  myMenu.knobAcceleration = 1;
  return true;
}

bool goUpOneMenu() {
  //display.clearDisplay(); // erase display
  display.clearBuffer(); // erase display
  //display.display(); // refresh display
  display.sendBuffer();
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
  //display.clearDisplay(); // erase display
  display.clearBuffer(); // erase display
  //display.display(); // refresh display
  display.sendBuffer();
  myMenu.setCurrentMenu(&listTopMenu);
  myMenu.knobAcceleration = 1;
  return true;
}


// patchFxMenu functions
bool gotoFxSourceMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFxSourceMenu);
  myMenu.knobAcceleration = 1;
    sprintf(myMenu.str_oledbuf,"  %s  ", 
          fxSourcePatch ? "Patch ":"Global");
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
/*
bool gotoFxCopyMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFxCopyMenu);
  return true;
}
*/
bool gotoSaveFxGlobalMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSaveFxGlobalMenu);
  myMenu.knobAcceleration = 1;
    sprintf(myMenu.str_oledbuf, "Exit w/o\n saving");
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoFxPasteMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFxPasteMenu);
  myMenu.knobAcceleration = 4;
  if (paste_patchNumber == NUMBER_OF_PATCHES){
      sprintf(myMenu.str_oledbuf, "Exit w/o\n saving");
  }
  else { 
    String ps( loadedPatches[paste_patchNumber].patch_string );
    ps.setCharAt( ps.indexOf(' '), '\n'); // TODO: spaces till end of line then \n
    sprintf(myMenu.str_oledbuf, "%03d\n%s", paste_patchNumber+1, ps.c_str() );
    Serial8.println(loadedPatches[paste_patchNumber].patch_string);
    // load the patch 
//    if (!patchLoaded[paste_patchNumber])
//    {
//        loadPatchSD(paste_patchNumber);
//    }
  }
  return true;
}
/*
bool gotoFxSwapMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFxSwapMenu);
  return true;
}
*/
bool gotoDelayLevelMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayLevelMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYLEVEL));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoDelayTimeLMenu(){
  //display.display(); // refresh display
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayTimeLMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %04d    ", 
        10*getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYTIME));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoDelayPongMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayPongMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        127 - getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYSPARE));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoDelayFeedbackMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayFeedbackMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYFEEDBACK));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoDelayDampMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listDelayDampMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPDELAY, CCEFFECTSDELAYDAMP));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoReverbLevelMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listReverbLevelMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBLEVEL));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoReverbTimeMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listReverbTimeMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %04d    ", 
        100*getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBTIME)); //114,3,10,50,//1000 to 5000 ms
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoReverbDensityMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listReverbDensityMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBDENSEEARLY));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoReverbDampMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listReverbDampMenu);
  myMenu.knobAcceleration = 1;
  // -10 to +10
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPREVERB, CCEFFECTSREVERBDAMP)-64);
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
// gogoReverbSparemenu()
bool gotoChorusOnMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusOnMenu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %s    ", 
        getFxValue(&current_patch, EFFECTGROUPCOMMON1, CCCHORUSON)? " On": "Off");
  //display.clearDisplay(); // erase //display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoChorusDryMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusDryMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDRYLEVEL));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoChorusLfoFMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusLfoFMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %2.1fHz  ", 
        (float)getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSLFOFREQ)/10.0);
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoChorusFeedbackMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusFeedbackMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSFEEDBACK)-64);
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoChorusDelay1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusDelay1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d mS ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY1));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoChorusMod1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusMod1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD1)-64);
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoChorusWet1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusWet1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET1)-64);
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoChorusDelay2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusDelay2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d mS ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSDELAY2));
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoChorusMod2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusMod2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSMOD2)-64);
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoChorusWet2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listChorusWet2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", 
        getFxValue(&current_patch, EFFECTGROUPCHORUS, CCEFFECTSCHORUSWET2)-64);
  //display.clearDisplay();
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}

// patchOsc1Menu functions
bool gotoOctOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listOscOsc1Menu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCOCTOSC1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSemiOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSemiOsc1Menu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCSEMIOSC1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoCentsOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listCentsOsc1Menu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCFINEOSC1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBeatOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBeatOsc1Menu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBEATOSC1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSawOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSawOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCSAWOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoTriOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listTriOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCTRIOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoPulseOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPulseOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCPULSEOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoPWOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPWOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCPWOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoPwmFreqOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPwmFreqOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCPWMFREQOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoPwmDepthOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPwmDepthOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCPWMDEPTHOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepTimeOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCSWEEPTIMEOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepDepthOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCSWEEPDEPTHOSC1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathAttainOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathAttainOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBREATHATTAINOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathDepthOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathDepthOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBREATHDEPTHOSC1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathThresholdOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathThresholdOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBREATHTHRESHOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathCurveOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCBREATHCURVEOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLevelOsc1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLevelOsc1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc1[CCLEVELOSC1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}

// patchOsc2Menu functions
//

bool gotoOctOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listOscOsc2Menu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCOCTOSC2]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSemiOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSemiOsc2Menu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCSEMIOSC2]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoCentsOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listCentsOsc2Menu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCFINEOSC2]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBeatOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBeatOsc2Menu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBEATOSC2]-64);
 // display.clearDisplay(); // erase display
 // display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSawOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSawOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCSAWOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoTriOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listTriOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCTRIOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoPulseOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPulseOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCPULSEOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoPWOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPWOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCPWOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoPwmFreqOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPwmFreqOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCPWMFREQOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoPwmDepthOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listPwmDepthOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCPWMDEPTHOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepTimeOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCSWEEPTIMEOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepDepthOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCSWEEPDEPTHOSC2]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathAttainOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathAttainOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBREATHATTAINOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathDepthOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathDepthOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBREATHDEPTHOSC2]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathThresholdOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathThresholdOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBREATHTHRESHOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathCurveOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCBREATHCURVEOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLevelOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLevelOsc2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc2[CCLEVELOSC2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoXFadeOsc2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listXFadeOsc2Menu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %s    ", 
          current_patch.nrpn_msb_common1[CCXFADE]? " On": "Off");
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}


// patchOscFilter1Menu Functions 
bool gotoLinkOscFiltersMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLinkOscFiltersMenu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf," %s ", 
    current_patch.nrpn_msb_common1[CCLINKOSCFILTERS]%2 ? " Linkded": "UnLinked"); //1 Linked, 2 UnLinked
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoModeOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listModeOscFilter1Menu);
  myMenu.knobAcceleration = 1;
   switch (current_patch.nrpn_msb_osc_filt1[CCMODEOSCFILTER1]) {
     case LP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
     case HP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " HP"); break; 
     case BP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " BP"); break; 
     case NTC:
         sprintf(myMenu.str_oledbuf,"   %s    ", "NTC"); break; 
     case THRU:
         sprintf(myMenu.str_oledbuf,"   %s    ", "OFF"); break; 
     default:
         sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
   }
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoFreqOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFreqOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  float FreqFilterNN = (float)current_patch.nrpn_msb_osc_filt1[CCFREQOSCFILTER1];
  float FreqFilter = 440.0*pow(2, (FreqFilterNN-69.0)/12 );
  sprintf(myMenu.str_oledbuf,"%5.1fHz", FreqFilter);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoQOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listQOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  float QFactor = (float)current_patch.nrpn_msb_osc_filt1[CCQFACTOROSCFILTER1]/10.0;
  sprintf(myMenu.str_oledbuf,"   %2.1f   ", QFactor);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoKeyFollowOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyFollowOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt1[CCKEYFOLLOWOSCFILTER1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathModOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathModOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt1[CCBREATHMODOSCFILTER1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathCurveOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt1[CCBREATHCURVEOSCFILTER1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoFreqOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoFreqOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt1[CCLFOFREQOSCFILTER1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoDepthOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoDepthOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt1[CCLFODEPTHOSCFILTER1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoBreathOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoBreathOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt1[CCLFOBREATHOSCFILTER1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoThresholdOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoThresholdOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt1[CCLFOTHRESHOSCFILTER1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepTimeOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt1[CCSWEEPTIMEOSCFILTER1]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepDepthOscFilter1Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthOscFilter1Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt1[CCSWEEPDEPTHOSCFILTER1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}

// patchOscFilter2Menu Functions
bool gotoModeOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listModeOscFilter2Menu);
  myMenu.knobAcceleration = 1;
   switch (current_patch.nrpn_msb_osc_filt2[CCMODEOSCFILTER2]) {
     case LP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
     case HP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " HP"); break; 
     case BP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " BP"); break; 
     case NTC:
         sprintf(myMenu.str_oledbuf,"   %s    ", "NTC"); break; 
     case THRU:
         sprintf(myMenu.str_oledbuf,"   %s    ", "OFF"); break; 
     default:
         sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
   }
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoFreqOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFreqOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  float FreqFilterNN = (float)current_patch.nrpn_msb_osc_filt2[CCFREQOSCFILTER2];
  float FreqFilter = 440.0*pow(2, (FreqFilterNN-69.0)/12 );
  sprintf(myMenu.str_oledbuf,"%5.1fHz", FreqFilter);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoQOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listQOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  float QFactor = (float)current_patch.nrpn_msb_osc_filt2[CCQFACTOROSCFILTER2]/10.0;
  sprintf(myMenu.str_oledbuf,"   %2.1f   ", QFactor);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoKeyFollowOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyFollowOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt2[CCKEYFOLLOWOSCFILTER2]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathModOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathModOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt2[CCBREATHMODOSCFILTER2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);  
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathCurveOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt2[CCBREATHCURVEOSCFILTER1]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoFreqOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoFreqOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_osc_filt2[CCLFOFREQOSCFILTER2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoDepthOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoDepthOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt2[CCLFODEPTHOSCFILTER2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoBreathOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoBreathOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt2[CCLFOBREATHOSCFILTER2]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoThresholdOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoThresholdOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt2[CCLFOTHRESHOSCFILTER2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepTimeOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt2[CCSWEEPTIMEOSCFILTER2]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepDepthOscFilter2Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthOscFilter2Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_osc_filt2[CCSWEEPDEPTHOSCFILTER2]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}


// patchNoiseFilter3Menu Functions 
bool gotoLinkNoiseFiltersMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLinkNoiseFiltersMenu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf," %s ", 
    current_patch.nrpn_msb_common1[CCLINKNOISEFILTERS]%2 ? " Linkded": "UnLinked"); //1 Linked, 2 UnLinked
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoModeNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listModeNoiseFilter3Menu);
  myMenu.knobAcceleration = 1;
  switch (current_patch.nrpn_msb_noise_filt3[CCMODENOISEFILTER3]) {
     case LP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
     case HP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " HP"); break; 
     case BP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " BP"); break; 
     case NTC:
         sprintf(myMenu.str_oledbuf,"   %s    ", "NTC"); break; 
     case THRU:
         sprintf(myMenu.str_oledbuf,"   %s    ", "OFF"); break; 
     default:
         sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
   }
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoFreqNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFreqNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  float FreqFilterNN = (float)current_patch.nrpn_msb_noise_filt3[CCFREQNOISEFILTER3];
  float FreqFilter = 440.0*pow(2, (FreqFilterNN-69.0)/12 );
  sprintf(myMenu.str_oledbuf,"%5.1fHz", FreqFilter);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoQNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listQNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  float QFactor = (float)current_patch.nrpn_msb_noise_filt3[CCQFACTORNOISEFILTER3]/10.0;
  sprintf(myMenu.str_oledbuf,"   %2.1f   ", QFactor);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoKeyFollowNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyFollowNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCKEYFOLLOWNOISEFILTER3]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathModNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathModNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCBREATHMODNOISEFILTER3]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);    
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathCurveNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCBREATHCURVENOISEFILTER3]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoFreqNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoFreqNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCLFOFREQNOISEFILTER3]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoDepthNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoDepthNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_noise_filt3[CCLFODEPTHNOISEFILTER3]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoBreathNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoBreathNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCLFOBREATHNOISEFILTER3]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoThresholdNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoThresholdNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCLFOTHRESHNOISEFILTER3]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepTimeNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCSWEEPTIMENOISEFILTER3]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepDepthNoiseFilter3Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthNoiseFilter3Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt3[CCSWEEPDEPTHNOISEFILTER3]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}

// patchNoiseFilter4Menu Functions
bool gotoModeNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listModeNoiseFilter4Menu);
  myMenu.knobAcceleration = 1;
    switch (current_patch.nrpn_msb_noise_filt4[CCMODENOISEFILTER4]) {
     case LP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
     case HP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " HP"); break; 
     case BP:
         sprintf(myMenu.str_oledbuf,"   %s    ", " BP"); break; 
     case NTC:
         sprintf(myMenu.str_oledbuf,"   %s    ", "NTC"); break; 
     case THRU:
         sprintf(myMenu.str_oledbuf,"   %s    ", "OFF"); break; 
     default:
         sprintf(myMenu.str_oledbuf,"   %s    ", " LP"); break; 
   }
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoFreqNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listFreqNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  float FreqFilterNN = (float)current_patch.nrpn_msb_noise_filt4[CCFREQNOISEFILTER4];
  float FreqFilter = 440.0*pow(2, (FreqFilterNN-69.0)/12 );
  sprintf(myMenu.str_oledbuf,"%5.1fHz", FreqFilter);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoQNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listQNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  float QFactor = (float)current_patch.nrpn_msb_noise_filt4[CCQFACTORNOISEFILTER4]/10.0;
  sprintf(myMenu.str_oledbuf,"   %2.1f   ", QFactor);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoKeyFollowNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyFollowNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt4[CCKEYFOLLOWNOISEFILTER4]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathModNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathModNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt4[CCBREATHMODNOISEFILTER4]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);    
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBreathCurveNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBreathCurveNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt4[CCBREATHCURVENOISEFILTER4]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoFreqNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoFreqNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise_filt4[CCLFOFREQNOISEFILTER4]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoDepthNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoDepthNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_noise_filt4[CCLFODEPTHNOISEFILTER4]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoBreathNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoBreathNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_noise_filt4[CCLFOBREATHNOISEFILTER4]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoLfoThresholdNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listLfoThresholdNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_noise_filt4[CCLFOTHRESHNOISEFILTER4]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepTimeNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepTimeNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_noise_filt4[CCSWEEPTIMENOISEFILTER4]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoSweepDepthNoiseFilter4Menu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listSweepDepthNoiseFilter4Menu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_noise_filt4[CCSWEEPDEPTHNOISEFILTER4]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}

// patchNoiseMenu Functions
bool gotoNoiseTimeMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listNoiseTimeMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise[CCNOISETIME]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoNoiseBreathMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listNoiseBreathMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise[CCNOISEBREATHCURVE]-64);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoNoiseLevelMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listNoiseLevelMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"   %03d    ", current_patch.nrpn_msb_noise[CCNOISELEVEL]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}

bool gotoKeyTriggerMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listKeyTriggerMenu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"  %s  ", 
      current_patch.nrpn_msb_common1[CCKEYTRIGGER]? "Single": "  Mult");
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoOctButtonLevelMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listOctButtonLevelMenu);
  myMenu.knobAcceleration = 4;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_common2[CCOCTBUTTONLEVEL]);
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBendRangeMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBendRangeMenu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"  %03d    ", current_patch.nrpn_msb_common1[CCBENDRANGE]); // 0 12 semitones
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}
bool gotoBendStepMenu(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  myMenu.setCurrentMenu(&listBendStepMenu);
  myMenu.knobAcceleration = 1;
  sprintf(myMenu.str_oledbuf,"   %s    ", 
      current_patch.nrpn_msb_common1[CCBENDSTEP]? " On": "Off");
  //display.clearDisplay(); // erase display
  //display.println(myMenu.str_oledbuf);
  display.clearBuffer(); // erase display
  display.drawStr(0,14,myMenu.str_oledbuf); //TODO: is 14 the correct place?
  return true;
}



void helpSysAdjGoto(){
  myMenu.previousMenuStack.push(myMenu.currentMenu);
  myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  display.clearBuffer(); // erase display
  display.sendBuffer(); // refresh display

}

// systemAdjMenu Menu functions
bool gotoAuxInMenu() {
  helpSysAdjGoto();
  //myMenu.previousMenuStack.push(myMenu.currentMenu);
  //myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  //display.clearDisplay(); // erase display
  //display.display(); // refresh display
  myMenu.setCurrentMenu(&listAuxInMenu);
  sprintf(myMenu.str_oledbuf,"  %ld   ", mix_linein);
  display.println(myMenu.str_oledbuf);
  return true;
}

bool gotoOctaveMenu() {
  helpSysAdjGoto();
  //myMenu.previousMenuStack.push(myMenu.currentMenu);
  //myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  //display.clearDisplay(); // erase display
  //display.display(); // refresh display
  myMenu.setCurrentMenu(&listOctaveMenu);
  sprintf(myMenu.str_oledbuf,"  %ld   ", Octave);
  display.println(myMenu.str_oledbuf);
  return true;
}

bool gotoSemiMenu() {
  helpSysAdjGoto();
  //myMenu.previousMenuStack.push(myMenu.currentMenu);
  //myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  //display.clearDisplay(); // erase display
  //display.display(); // refresh display
  myMenu.setCurrentMenu(&listSemiMenu);
  sprintf(myMenu.str_oledbuf,"  %ld   ", Transpose);
  display.println(myMenu.str_oledbuf);
  return true;
}

bool gotoCentsMenu() {
  helpSysAdjGoto();
  //myMenu.previousMenuStack.push(myMenu.currentMenu);
  //myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  //display.clearDisplay(); // erase display
  //display.display(); // refresh display
  myMenu.setCurrentMenu(&listCentsMenu);
  sprintf(myMenu.str_oledbuf,"  %ld   ", FineTuneCents);
  display.println(myMenu.str_oledbuf);
  return true;
}

bool gotoBreathCCMenu() {
  helpSysAdjGoto();
  //myMenu.previousMenuStack.push(myMenu.currentMenu);
  //myMenu.previousItemIndexStack.push(myMenu.currentItemIndex);
  //display.clearDisplay(); // erase display
  //display.display(); // refresh display
  myMenu.setCurrentMenu(&listBreathCCMenu);
  //TODO: handle Channel Aftertouch
  sprintf(myMenu.str_oledbuf,"  %ld   ", breath_cc);
  display.println(myMenu.str_oledbuf);
  return true;
}



