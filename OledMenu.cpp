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
  //int fontV = display.getAscent()-display.getDescent();
  int fontV = display.getMaxCharHeight();
  //int fontH = display.getMaxCharWidth();
  if (escape()) {
    Serial8.println("escape()");
    //setCurrentMenu(&listEscapeMenu);
    if(currentMenu != &listTopMenu){ 
        goUpOneMenu(); 
        getText(str_serial8buf, currentItemIndex);
        Serial8.println(str_serial8buf);
    } else {
        currentItemIndex = 0; // got top of top
    }
    displayMenu();
  }
    Serial8.print("doMenu, menuSize = ");
    Serial8.println(currentMenu->getSize());
    Serial8.print("OledMenu::runningFunction = ");
    Serial8.println(runningFunction);
  if(currentMenu->getSize() != 1) { 
      Serial8.println("MenuClass::doMenu");
      MenuClass::doMenu(); return; 
  }
  if (runningFunction) {
      Serial8.println("runningFunction");
   runningFunction = !runFunction();
    return; // skip displaying the menu if we're going to run item function again.
  }
  updateLeafValue = updateSelection(); // leaf funcs update values
  if(updateLeafValue){   // knobBot moved, update value and fill str_oledbuf
    Serial8.println("updateLeafValue true");
    //if(!runFunction()) { display.print("updateLeafValue ERROR!"); return;} 
    runningFunction = !runFunction();
    Serial8.println(str_oledbuf);
    //display.drawStr(0,2*fontV,"             "); // TODO: draw black box instead
    //display.drawStr(0,3*fontV,"             ");
    //display.drawStr(0,4*fontV,"             ");
    //display.drawStr(2*fontH,2*fontV,str_oledbuf);
    drawStrNl(0,2*fontV,str_oledbuf);
    display.sendBuffer();
    /*
    display.setCursor(0,18);
    display.println("          ");
    display.println("          ");
    display.println("          ");
    display.setCursor(0,18);
    display.println(str_oledbuf);
    display.display();
    */
    return; 
  } 
  knobButtonSelType = selectionMade();
  if (NONE != knobButtonSelType) { // selection made, run item function once now
    Serial8.println("selectionMade()");
    if (!runFunction()) { // If item function returns false then it isn't done and we need to keep running it.
      runningFunction = true;
      Serial8.println("runningFunction true");
      return; // skip displaying the menu if we're going to run item function again.
    }
    Serial8.println("call displayMenu()");
    displayMenu();
  }
}

void OledMenu::resetButtonsAndKnobs(){
  static int knobCount = 0;
  static int buttonCount = 0;
  while(knobBot.read()){
      knobBot.write(0); // reset knobBot position to zero ignore new 
      knobCount++; 
  }
  newKnob = 0;
  // button pin capacitance must be keeping knobBotButton low at power up
  // This waits here till the knob is high
  knobBotButton.update();
  if (knobBotButton.read()){
      sprintf(str_serial8buf, "knobCount: %d, buttonCount: %d ", knobCount, buttonCount);
  }
  else{
      do{
        buttonCount += 1;  
        delay(50);
        knobBotButton.update();
      }while( !knobBotButton.rose());
  }
  _knobButtonSelType = NONE;
  sprintf(str_serial8buf, "knobCount: %d, buttonCount: %d ", knobCount, buttonCount);
  Serial8.println(str_serial8buf);
}

bool OledMenu::checkButtons(){
    //bool topButtonChanged = topButton.update();
    topButton.update();
    bool topButtonState = topButton.read(); ;// false means pressed
    botButton.update();
    bool botButtonState = botButton.read(); ;// false means pressed
    if (topButton.rose()){
        if (!longTopButtonPressPending){
            longTopButtonPress = false; 
            shortTopButtonPress = true; 
        }
        else
        {
            longTopButtonPress = true; 
            shortTopButtonPress = false; 
        }
        longTopButtonPressPending = false; 
        return true;
    }
    else if ( topButtonState == false && topButton.currentDuration() > longTopButtonPressTime ) {
        longTopButtonPressPending = true; 
        return true;

    }
    return false;
}

bool OledMenu::checkButtonsAndKnobs(){
    //bool knobButtonChanged = knobBotButton.update();
    knobBotButton.update();
    bool knobButtonState = knobBotButton.read(); ;// false means pressed
    int32_t newKnob_temp = 0;
    uint32_t currentTime = millis();
    if (knobBotButton.rose()){
        if(knobBotButton.previousDuration() > knobButtonLongPressInterval){
            knobButtonPossibleSingleClick = false;
            _knobButtonSelType = LONG_PRESS;
            lastClickTime = 0;
        } else if(currentTime - lastClickTime < knobButtonDoubleClickInterval){
            knobButtonPossibleSingleClick = false;
            _knobButtonSelType = DOUBLE_CLICK;
            lastClickTime = 0;
        } else  {
            _knobButtonSelType = NONE;
            knobButtonPossibleSingleClick = true;
            lastClickTime = currentTime;
        }
        knobBot.write(0); // reset knobBot position to zero ignore new 
        return true;
    }
    else if (knobButtonPossibleSingleClick && 
            ( currentTime - lastClickTime > knobButtonSingleClickInterval) ){
        knobButtonPossibleSingleClick = false;
        _knobButtonSelType = SINGLE_CLICK;
        lastClickTime = 0;
        knobBot.write(0); // reset knobBot position to zero ignore new 
        return true;
    }
    if (!knobButtonState) { 
        knobBot.write(0); // ignore knobBot event if knobBotButton is currently pressed
        return false;
    }
    if (newKnob != 0) { return false; } // don't read again if not done with last time
    newKnob_temp = knobBot.read()/4; //don't need 4X counting mode, so integer div by 4
    newKnob = newKnob_temp > 0 ? 1 : newKnob_temp < 0 ? -1 : 0; 
    if (newKnob != 0){
        knobBot.write(0);
        uint32_t tNOW = millis();
        if(tNOW - lastTimeKnobMoved > knobSlowTime){
            lastTimeKnobMoved = tNOW;
            return true;
        }
        newKnob *= knobAcceleration;
        lastTimeKnobMoved = tNOW;
        return true;
    } else { return false; }
}

int OledMenu::updateSelection() {
    if (updateEpromFlag){
      eepromCurrentMillis = millis();
      eepromPreviousMillis = eepromCurrentMillis; // reset timer every knobBot turn 
    }
    int32_t newKnob_temp = newKnob;
    newKnob = 0; 
    return newKnob_temp; 
}

bool OledMenu::escape() {
    if (updateEpromFlag){
        eepromCurrentMillis = millis();
        eepromPreviousMillis = eepromCurrentMillis; // reset timer every button press 
    }
    bool shortTopButtonPress_temp = shortTopButtonPress;
    shortTopButtonPress = false;
    return shortTopButtonPress_temp;
}

selection_t OledMenu::selectionMade() {
    if (updateEpromFlag){
        eepromCurrentMillis = millis();
        eepromPreviousMillis = eepromCurrentMillis; // reset timer every button press
    }
    selection_t knobButtonSelType_temp = _knobButtonSelType;
    _knobButtonSelType = NONE;
    return knobButtonSelType_temp;
}

void OledMenu::displayMenu() {
    char outBuf[10];
    //display.setCursor(0,0);
    int fontV = display.getMaxCharHeight();
    int fontH = display.getMaxCharWidth();
    int itemsPerScreen = display.getDisplayHeight()/fontV;
    int menuSize = currentMenu->getSize();
    int topScreenIndex = (currentItemIndex/itemsPerScreen)*itemsPerScreen; //int div for truncate
    int botScreenIndex = topScreenIndex + itemsPerScreen; 
    int currentScreenIndex = 0;

    if (menuSize == 1) {
        getText(outBuf, 0); // text from MenuItem (e.g., "Patch:")
        /*
        display.println(outBuf);
        display.setCursor(0,18);
        display.println("          ");
        display.println("          ");
        display.println("          ");
        display.setCursor(0,18);
        display.println(str_oledbuf);
        display.display(); 
        */
        display.drawStr(0,fontV,outBuf);
        //display.drawStr(0,fontV*2,"          ");
        //display.drawStr(0,fontV*3,"          ");
        //display.drawStr(0,fontV*4,"          ");
        //display.drawStr(0,fontV*2,str_oledbuf);
        display.sendBuffer();
        Serial8.println(outBuf);
        Serial8.print(F("display Menu, menuSize = "));
        Serial8.println(menuSize);
        return;
    }

    for (int i = topScreenIndex; i < botScreenIndex; i++) {
      if (i == currentItemIndex) {
        Serial8.print("->");
        //display.print("->");
        display.drawStr(0,fontV*(1+currentScreenIndex),"->");
      }
      else {
        Serial8.print("  ");
        //display.print("  ");
        display.drawStr(0,fontV*(1+currentScreenIndex),"  ");
      }
      // use getText method to pull text out into a buffer you can print
      if (i < menuSize){
          getText(outBuf, i);
          //display.println(outBuf);
          display.drawStr(2*fontH,fontV*(1+currentScreenIndex),outBuf);
      } else { 
          //display.println("        ");
          display.drawStr(2*fontH,fontV*(1+currentScreenIndex),"        ");
      }
      currentScreenIndex++;
      Serial8.println(outBuf);
      //display.display(); 
    }
      display.sendBuffer(); 

    Serial8.print(F("currentItemIndex =  "));
    Serial8.println(currentItemIndex);
    for (int i = 0; i < 3; i++) {  // put some spaces before next print
      Serial8.println();
    }
}

void OledMenu::drawStrNl(int x, int y, const char* str){
    int bufIdx = 0;
    int fontV = display.getMaxCharHeight();
    //int fontH = display.getMaxCharWidth();
    int dispH = display.getDisplayWidth();
    //int fontD = display.getDescent();
    int fontA = display.getAscent();

    //str_buf using global so we allocate it statically
    for (int i = 0; str[i] != '\0'; i++){
        if ( str[i] == '\n') {
            str_buf[bufIdx] = '\0'; 
            display.setDrawColor(0);
            display.drawBox(0,y-fontA,dispH,fontV);
            display.setDrawColor(1);
            display.drawStr(x, y, str_buf);
            y += fontV; // Move to next line
            bufIdx = 0;         // reset buffer index
        } else {
            str_buf[bufIdx++] = str[i]; 
        }
    }
    // Print any remaining text after the last \n or if no \n was present
    str_buf[bufIdx] = '\0'; // Null-terminatet t
    display.setDrawColor(0);
    display.drawBox(0,y-fontA,dispH,fontV);
    display.setDrawColor(1);
    display.drawStr(x, y, str_buf);
}


