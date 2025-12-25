// MenuSystem.h
#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include "patches.h"

// small nested menu system supporting values, actions, submenus and naming menus
#define MENUSYSTEM_STACK_DEPTH 8

class Menu;

using ActionFunc = void (*)();
using EnterMenuFunc = void (*)();
using NamingFinishFunc = void (*)(const char *name);
using NamingAbortFunc = void (*)();

enum class MenuItemType
{
  Action,
  SubMenu,
  Value,
  Naming
};
enum class SynthValueType
{
  Uint8,
  Float,
  Bool,
  FilterMode // corresponds to filterMode_t
};

using PatchToSynthFunc = void (*)();
using ValueToDispFunc = void (*)(char *dispValBuf, uint8_t *val);

struct ValueDescriptor
{
  uint8_t *valPtr;               // pointer is always to uint8_t storage (caller may cast their variable's address)
  PatchToSynthFunc patchToSynth; // function to call to update synth from valPtr
  // SynthValueType type;           // type of value
  double min;
  double max;
  double step;
  ValueToDispFunc valueToDisp; // if non-null, call to convert value to display string
};

struct NamingDescriptor
{
  char *buf;      // buffer including null terminator/capacity
  uint8_t maxlen; // maximum name length (characters)
  NamingFinishFunc finish;
  NamingAbortFunc abort;
};

struct MenuItem
{
  const char *title;
  MenuItemType type;
  union
  {
    ActionFunc action;
    Menu *submenu;
    ValueDescriptor value;
    NamingDescriptor naming;
  } u;
};

class Menu
{
public:
  Menu(const char *title, MenuItem *items, uint8_t count, EnterMenuFunc enterFunc = nullptr);
  const char *title() const { return _title; }
  MenuItem *items() { return _items; }
  uint8_t count() const { return _count; }
  EnterMenuFunc getEnterFunc() const { return _enterFunc; }

private:
  const char *_title;
  MenuItem *_items;
  uint8_t _count;
  EnterMenuFunc _enterFunc;
};

class MenuSystem
{
public:
  MenuSystem(U8G2 &u8g2, Menu *rootMenu);
  void begin();
  void pollEncoders(int16_t botDelta, int16_t topDelta, bool botButtonPressed, bool topButtonPressed);
  void draw();

  // allow external callers (actions) to exit to parent menu
  void exitToParent();

  // helpers for creating descriptors
  static ValueDescriptor makeValue(uint8_t *valPtr, PatchToSynthFunc p, double min, double max, double step, ValueToDispFunc vtd = nullptr);
  static NamingDescriptor makeNaming(char *buf, uint8_t maxlen, NamingFinishFunc f, NamingAbortFunc a);

private:
  U8G2 &_u8g2;
  Menu *_root;
  Menu *_current;
  Menu *_parentStack[MENUSYSTEM_STACK_DEPTH];
  uint8_t _parentDepth;

  uint8_t _selIndex;                              // index into current menu
  uint8_t _selIndexStack[MENUSYSTEM_STACK_DEPTH]; // index into current menu
  uint8_t _pageStart;                             // first visible index
  uint8_t _pageStartStack[MENUSYSTEM_STACK_DEPTH];
  bool _skipPatchToSynth;
  // static const uint8_t LINES = 5; // lines per screen (font dependent)
  uint8_t _itemsPerScreen; // lines per screen (font dependent)

  bool _inNaming;
  NamingDescriptor _namingDesc;
  uint8_t _nameEditIndex;

  void moveSelection(int delta);
  void pageUp();
  void pageDown();
  void enterItem();
  void adjustValueAtSelection(int delta);
  void adjustNamingChar(int delta);
  void renderMenu();
  void renderNaming();
  String valueToString(const ValueDescriptor &vd);
};
