#include <MenuClass.h>

boolean fun1();
boolean fun2();
boolean fun3();
boolean fun4();
boolean gotoMenuOne();
boolean gotoMenuTwo();

const int upButtonPin = 4;
const int downButtonPin = 5;
const int selectButtonPin = 6;


/*****************************
 ******************************
 * Define the new derived menu class
 * implement the functions to update 
 * the menu, make a selection, and
 * display the menu
 ******************************
 *****************************/

class SimpleSerialMenu: 
public MenuClass {

public:
  int updateSelection();
  boolean selectionMade();
  void displayMenu();

};

int SimpleSerialMenu::updateSelection() {
  // This is a simplified example
  // Edge detection and debouncing are exercises left to the user
  if(digitalRead(upButtonPin) == LOW){
    return 1;
  }
  if(digitalRead(downButtonPin) == LOW){
    return -1;
  }
  return 0;
}

boolean SimpleSerialMenu::selectionMade() {
  // This is a simplified example
  // Edge detection and debouncing are exercises left to the user
  return !digitalRead(selectButtonPin);  
}

void SimpleSerialMenu::displayMenu() {

  //  Blink WIthout Delay to keep prints from blasting out too fast. 
  static unsigned long preMil = millis();
  unsigned long curMil = millis();
  if(curMil - preMil >= 500) {  // print every half second
    preMil = curMil;

    char outBuf[17];

    for (int i = 0; i < currentMenu->getSize(); i++) {

      if (i == currentItemIndex) {
        Serial.print(F("---> "));
      }
      else {
        Serial.print(F("     "));
      }
      // use getText method to pull text out into a buffer you can print
      getText(outBuf, i);
      Serial.println(outBuf);
    }
    Serial.print(F("currentItemIndex =  "));
    Serial.println(currentItemIndex);
    for (int i = 0; i < 3; i++) {  // put some spaces before next print
      Serial.println();
    }
  }
}



/*****************************
 ******************************
 * Setup the menu as an array of MenuItem
 * Create a MenuList and an instance of your 
 * menu class
 ******************************
 *****************************/

MenuItem PROGMEM firstList[3] = { 
  { 
    "Item_one", fun1    }
  , { 
    "Item_two", fun2    }
  , { 
    "Menu_2", gotoMenuTwo    }
};

MenuItem PROGMEM secondList[3] = {
  {
    "Back" , gotoMenuOne   }
  ,{
    "Item_three" , fun3   }
  ,{
    "Item_four" , fun4   }
};

MenuList list1(firstList, 3);
MenuList list2(secondList, 3);

SimpleSerialMenu myMenu;



void setup(){
  pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);

  Serial.begin(19200);
  myMenu.setCurrentMenu(&list1);

  Serial.println(F("Running Menu Test:"));
}

void loop(){
  myMenu.doMenu();

  //  Whatever other code you have.  
  //  Try to keep it non-blocking while the 
  //  menu is displayed  
  delay(250);  // because my buttons aren't debounced. 
}



/*****************************
 ******************************
 * Define the functions you want your menu to call
 * They can be blocking or non-blocking
 * They should take no arguments and return a boolean
 * true if the function is finished and doesn't want to run again
 * false if the function is not done and wants to be called again
 ******************************
 *****************************/


boolean fun1() {
  // blocking functions that simply return true will be run once and will complete
  //  before the code goes to the next loop
  Serial.println(F("FUN 1:  running right now and printing this long line"));
  delay(3000);
  return true;
}

boolean fun2() {
  Serial.println(F("FUN 2:  running right now and printing this long line"));
  delay(3000);
  return true;
}

boolean fun3() {
  Serial.println(F("FUN 3:  locked here until button push"));
  delay(100);
  if(digitalRead(selectButtonPin) == LOW){
    return true;  // signal finished if button pressed
  }
  else {
    return false;  // no button press, keep running
  }
}

boolean fun4() {
  // remember you have to have at least one case return true or you lock things up
  Serial.println(F("FUN 4:  Locked here forever"));
  delay(3000);
  return false;
}

boolean gotoMenuOne() {
  myMenu.setCurrentMenu(&list1);
  return true;
}

boolean gotoMenuTwo() {
  myMenu.setCurrentMenu(&list2);
  return true;
}


