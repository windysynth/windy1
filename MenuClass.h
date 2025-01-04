// Simple Menu Class
//
// By Delta_G
//
// https://forum.arduino.cc/index.php?topic=353045.0
//
#ifndef MENUCLASS_H_
#define MENUCLASS_H_

#define USING_PROGMEM

#include "Arduino.h"

typedef boolean (*Item_Function)();

const typedef struct MenuItem_t {
 char text[16];
 Item_Function func;
} MenuItem;

class MenuList {
private:
 MenuItem *menuItems;
 uint8_t listSize;
public:
 MenuItem* getItem(int aIndex);
 MenuList(MenuItem* aList, uint8_t aSize) :
 menuItems(aList), listSize(aSize) {
 }
 uint8_t getSize();

};

class MenuClass {
private:

protected:
 boolean cancelFlag;
 boolean runningFunction;
public:
 MenuList *currentMenu;
 int currentItemIndex;

 MenuClass();
 MenuClass(MenuList* aList);

 void doMenu();
 void setCurrentMenu(MenuList*);
 boolean runFunction();
 void getText(char*, int);
 virtual void displayMenu() = 0;
 virtual boolean checkForCancel();
 virtual int updateSelection() = 0;
 virtual boolean selectionMade() = 0;

};

#endif /* MENUCLASS_H_ */

