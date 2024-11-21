#include "OledMenu.h"
#include "globals.h"
#include "MenuItemsListsFuncs.h"

/*****************************
 ******************************
 * the new derived menu class
 * implementations of
 * the functions to update 
 * the menu, make a selection, and
 * display the menu.
 * Also, reset buttons and knobs,
 * check the buttons and knobs for
 * events.
 ******************************
 *****************************/

void OledMenu::doMenu(){
    Serial8.print(F("doMenu, menuSize = "));
    Serial8.println(currentMenu->getSize());
  if(currentMenu->getSize() != 1) { 
      Serial8.println("MenuClass::doMenu");
      MenuClass::doMenu(); return; }
  if (runningFunction) {
      Serial8.println("runningFunction");
   runningFunction = !runFunction();
    return; // skip displaying the menu if we're going to run item function again.
  }
  updateLeafValue = updateSelection(); // leaf funcs update values
  if(updateLeafValue){   // knob moved, update value and fill str_oledbuf
    Serial8.println("updateLeafValue true");
    if(!runFunction()) { display.print("updateLeafValue ERROR!"); return;} 
    Serial8.println(str_oledbuf);
    display.setCursor(0,18);
    display.println(str_oledbuf);
    display.display();
    return; 
  } 
  if (selectionMade()) { // selection made, run item function once now
    Serial8.println("selectionMade()");
    if (!runFunction()) { // If item function returns false then it isn't done and we need to keep running it.
      runningFunction = true;
      return; // skip displaying the menu if we're going to run item function again.
    }
  Serial8.println("call displayMenu()");
  displayMenu();
  }
}

void OledMenu::resetButtonsAndKnobs(){
  static int knobCount = 0;
  static int buttonCount = 0;
  while(knob.read()){
      knob.write(0); // reset knob position to zero ignore new 
      knobCount++; 
  }
  newKnob = 0;
  // button pin capacitance must be keeping knobButton low at power up
  // This waits here till the knob is high
  while(!knobButton.update()){
    buttonCount = 1;  
    if(knobButton.rose()) { 
        buttonCount = 2; // shouldn't reach here 
        break; 
    }
  }
  longKnobButtonPressPending = false;
  longKnobButtonPress = false; 
  shortKnobButtonPress = false; 
  sprintf(str_serial8buf, "knobCount: %d, buttonCount: %d ", knobCount, buttonCount);
  Serial8.println(str_serial8buf);
}

bool OledMenu::checkButtonsAndKnobs(){
    bool knobButtonChanged = knobButton.update();
    bool knobButtonState = knobButton.read(); ;// false means pressed
    int32_t newKnob_temp = 0;
    if (knobButton.rose()){
        if (!longKnobButtonPressPending){
            longKnobButtonPress = false; 
            shortKnobButtonPress = true; 
        }
        else
        {
            longKnobButtonPress = true; 
            shortKnobButtonPress = false; 
        }
        longKnobButtonPressPending = false; 
        knob.write(0); // reset knob position to zero ignore new 
        return true;
    }
    else if ( knobButtonState == false && knobButton.currentDuration() > longKnobButtonPressTime ) {
        longKnobButtonPressPending = true; 
    }
    if (!knobButtonState) { 
        knob.write(0); // ignore knob event if buttonButton is currently pressed
        return false;
    }
    newKnob_temp = knob.read()/4; //don't need 4X counting mode, so integer div by 4
    newKnob = newKnob_temp > 0 ? 1 : newKnob_temp < 0 ? -1 : 0; 
    if (newKnob){
        knob.write(0);
        return true;
    } else { return false; }
}

int OledMenu::updateSelection() {
  int32_t newKnob_temp = newKnob;
  newKnob = 0; 
  return newKnob_temp; 
}

boolean OledMenu::selectionMade() {
    bool shortKnobButtonPress_temp = shortKnobButtonPress;
    shortKnobButtonPress = false;
    return shortKnobButtonPress_temp;
}

void OledMenu::displayMenu() {
    char outBuf[10];
    display.setCursor(0,0);
    int itemsPerScreen = 4;
    int menuSize = currentMenu->getSize();
    int topScreenIndex = (currentItemIndex/itemsPerScreen)*itemsPerScreen; //int div for truncate
    int botScreenIndex = topScreenIndex + itemsPerScreen; 

    if (menuSize == 1) {
        display.print("  ");
        getText(outBuf, 0);
        display.println(outBuf);
        display.println("          ");
        display.println("          ");
        display.println("          ");
        display.setCursor(0,18);
        display.println(str_oledbuf);
        display.display(); 
        Serial8.println(outBuf);
        Serial8.print(F("display Mennu, menuSize = "));
        Serial8.println(menuSize);
        return;
    }
    for (int i = topScreenIndex; i < botScreenIndex; i++) {
      if (i == currentItemIndex) {
        Serial8.print("->");
        display.print("->");
      }
      else {
        Serial8.print("  ");
        display.print("  ");
      }
      // use getText method to pull text out into a buffer you can print
      if (i < menuSize){
          getText(outBuf, i);
          display.println(outBuf);
      } else { 
          display.println("        ");
      }
      Serial8.println(outBuf);
      display.display(); 
    }

    Serial8.print(F("currentItemIndex =  "));
    Serial8.println(currentItemIndex);
    for (int i = 0; i < 3; i++) {  // put some spaces before next print
      Serial8.println();
    }
}



