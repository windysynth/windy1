// Simple Menu Class
//
// By Delta_G
//
// https://forum.arduino.cc/index.php?topic=353045.0
//

#include "MenuClass.h"

MenuItem* MenuList::getItem(int aIndex) {
 // To make modulo work with negatives the way we want
 while (aIndex < 0) {
 aIndex += listSize;
 }
 return &(menuItems[aIndex % listSize]);
}

uint8_t MenuList::getSize() {
 return this->listSize;
}

MenuClass::MenuClass() {
 currentMenu = 0;
 currentItemIndex = 0;
 cancelFlag = false;
 runningFunction = false;
}

MenuClass::MenuClass(MenuList* aList) {
 currentMenu = aList;
 currentItemIndex = 0;
 cancelFlag = false;
 runningFunction = false;

}

boolean MenuClass::runFunction() {
 Item_Function theFunc;

#ifdef USING_PROGMEM
 theFunc =
 (Item_Function) pgm_read_word(&(currentMenu->getItem(currentItemIndex)->func));
#else
 theFunc = currentMenu->getItem(currentItemIndex)->func;

#endif
 return theFunc();

}

void MenuClass::doMenu() {
  cancelFlag = checkForCancel();
  Serial8.print(F("MenuClass::runningFuction = "));
  Serial8.println(runningFunction); 
  if (runningFunction) {
    Serial8.println("MenuClass::runFunction again");
   runningFunction = !runFunction();
    return; // skip displaying the menu if we're going to run item function again.
  }
    //menuHandler();
    cancelFlag = false;
    int selUp = updateSelection();
    if (selUp) {
      currentItemIndex += selUp;
      //  Avoid negative numbers with the modulo
      //  to roll the menu over properly
      while (currentItemIndex < 0) {
        currentItemIndex += currentMenu->getSize();
      }
      currentItemIndex %= currentMenu->getSize();
        Serial8.println("displayMenu() selUp");
      displayMenu();
      return; // only check for selection if menu not also updating
    } 
    if (NONE != selectionMade()) { // selection made, run item function once now
      if (!runFunction()) { // If item function returns false then it isn't done and we need to keep running it.
        runningFunction = true;
        Serial8.println("MenuClass::runningFuntion true");
        return; // skip displaying the menu if we're going to run item function again.
        }
        Serial8.println("displayMenu() selectionMade");
        displayMenu();
      }
}

void MenuClass::getText(char* aBuf, int aIndex) {
#ifdef USING_PROGMEM
 strcpy_P(aBuf, (currentMenu->getItem(aIndex)->text));
#else
 strcpy(aBuf, (currentMenu->getItem(aIndex)->text));
#endif
}

boolean MenuClass::checkForCancel() {
 return false;
}

void MenuClass::setCurrentMenu(MenuList* aMenu) {
 currentMenu = aMenu;
 currentItemIndex = 0;
}


