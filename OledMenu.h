#pragma once
#include <stack>
#include "MenuClass.h"
//#include "globals.h"


#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h> 
#include <Bounce2.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>

//------------------- ssd1306_128x32_i2c OLED --------------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     33 // Reset pin # (or -1 if sharing Arduino reset pin)
//#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);


/*****************************
 ******************************
 * Define the new derived menu class
 * implement the functions to update 
 * the menu, make a selection, and
 * display the menu
 ******************************
 *****************************/

class OledMenu: 
public MenuClass {

public:
  bool checkButtonsAndKnobs(); // 
  bool checkButtons(); // 
  void resetButtonsAndKnobs(); // 
  float knobButtonSingleClickInterval= 250;  // milliiseconds
  float knobButtonDoubleClickInterval= 250;  // milliiseconds
  float knobButtonLongPressInterval = 500;  // milliiseconds
  uint32_t lastClickTime = 0;
  selection_t knobButtonSelType = NONE;
  bool knobButtonPossibleSingleClick = false;
  float longTopButtonPressTime = 500;  // milliiseconds
  bool longTopButtonPressPending = false;
  bool longTopButtonPress = false; 
  bool shortTopButtonPress = false; 
  float longBotButtonPressTime = 500;  // milliiseconds
  bool longBotButtonPressPending = false;
  bool longBotButtonPress = false; 
  bool shortBotButtonPress = false; 
  bool reDoMenu = false;
  int32_t newKnob = 0;
  uint32_t lastTimeKnobMoved = 0;
  uint32_t knobSlowTime = 150; 
  int32_t knobAcceleration = 1; // set this in goto<name>Fun for <name> menu
  int updateSelection(); // checks up there is an update to selection
  int updateLeafValue = 0;
  namingstate_t namingState = ALPHANUM;
  bool escape(); // Checks if escape (top) button pressed 
  selection_t selectionMade(); // Checks if a selection has been made
  void doMenu(); // overiding MenuClass::doMenu() to handle leaf menus
  void displayMenu(); // 
  char str_oledbuf[64] = {0};
  char str_namingbuf[32] = {0};
  int namingPos = 0;
  int alphaNumIdx = 0;
  char str_serial8buf[64] = {0};
  MenuList *previousMenu; // to go back up from leaf menu
  int previousItemIndex = 0;
  std::stack<MenuList *> previousMenuStack; // to go back up from leaf menu
  std::stack<int> previousItemIndexStack;

private:
  selection_t _knobButtonSelType = NONE;
  
};





