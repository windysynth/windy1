#include "MenuSystem.h"
#include "globals.h"

Menu::Menu(const char *title, MenuItem *items, uint8_t count, EnterMenuFunc enterFunc)
    : _title(title), _items(items), _count(count), _enterFunc(enterFunc) {}

MenuSystem::MenuSystem(U8G2 &u8g2, Menu *rootMenu)
    : _u8g2(u8g2), _root(rootMenu), _current(rootMenu), _parentDepth(0), _selIndex(0), _pageStart(0),_skipPatchToSynth(false), _inNaming(false), _nameEditIndex(0) {}

void MenuSystem::begin()
{
  // https://github.com/olikraus/u8g2/wiki/fntgrp -- list of fonts
  _u8g2.setFont(u8g2_font_6x10_mf);
  int fontV = _u8g2.getMaxCharHeight();
  // int fontH = _u8g2.getMaxCharWidth();
  _itemsPerScreen = _u8g2.getDisplayHeight() / fontV - 1; // leave one line for title
  // LINES = itemsPerScreen;
  // nothing for now
}

ValueDescriptor MenuSystem::makeValue(uint8_t *valPtr, PatchToSynthFunc p, double min, double max, double step, ValueToDispFunc vtd)
{
  ValueDescriptor v;
  v.valPtr = valPtr;
  v.patchToSynth = p;
  v.min = min;
  v.max = max;
  v.step = step;
  v.valueToDisp = vtd;
  return v;
}

NamingDescriptor MenuSystem::makeNaming(char *buf, uint8_t maxlen, NamingFinishFunc f, NamingAbortFunc a)
{
  NamingDescriptor n;
  n.buf = buf;
  n.maxlen = maxlen;
  n.finish = f;
  n.abort = a;
  return n;
}

void MenuSystem::pollEncoders(int16_t botDelta, int16_t topDelta, bool botButtonPressed, bool topButtonPressed)
{
  if (_inNaming)
  {
    // move highlighted char
    if (botDelta > 0)
    {
      _nameEditIndex = (_nameEditIndex + 1) % _namingDesc.maxlen;
    }
    else if (botDelta < 0)
    {
      if (_nameEditIndex == 0)
        _nameEditIndex = _namingDesc.maxlen - 1;
      else
        _nameEditIndex = (_nameEditIndex - 1);
    }
    if (topDelta != 0)
    {
      adjustNamingChar(topDelta > 0 ? 1 : -1);
    }
    if (botButtonPressed)
    {
      // finish naming: trim spaces
      // ensure buffer is null terminated (assume caller allocated at least maxlen+1)
      if (_namingDesc.buf)
      {
        _namingDesc.buf[_namingDesc.maxlen] = '\0';
        // copy into temp to allow safe trimming
        char tmp[64];
        strncpy(tmp, _namingDesc.buf, min((size_t)63, (size_t)_namingDesc.maxlen));
        tmp[63] = '\0';
        // trim leading
        char *s = tmp;
        while (*s == ' ')
          s++;
        // trim trailing
        int end = (int)strlen(s) - 1;
        while (end >= 0 && s[end] == ' ')
        {
          s[end] = '\0';
          end--;
        }
        if (_namingDesc.finish)
          _namingDesc.finish(s);
      }
      _inNaming = false;
    }
    if (topButtonPressed)
    {
      if (_namingDesc.abort)
        _namingDesc.abort();
      _inNaming = false;
    }
    return;
  } // if (_inNaming)

  if (botDelta != 0)
  {
    moveSelection(botDelta > 0 ? 1 : -1);
    _skipPatchToSynth = true;
  }
  if (topDelta != 0)
    adjustValueAtSelection(topDelta > 0 ? 1 : -1);

  if (botButtonPressed)
  {
    enterItem();
  }
  if (topButtonPressed)
  {
    exitToParent(); // checks for root inside
  }
}

void MenuSystem::moveSelection(int delta)
{

  int cnt = _current->count(); // count of items in current menu
  if (cnt == 0)
    return;
  int prev = (int)_selIndex;
  int newIndex = prev + delta;
  // wrap
  if (newIndex < 0)
    newIndex = cnt - 1;
  if (newIndex >= cnt)
    newIndex = 0;

  // _itemsPerScreen;
  // int menuSize = currentMenu->getSize();
  // int topScreenIndex = (newIndex / _itemsPerScreen) * _itemsPerScreen + 1; // int div for truncate
  // int botScreenIndex = topScreenIndex + _itemsPerScreen;
  // int currentScreenIndex = 0;

  // compute new selection with wrapping
  _selIndex = (uint8_t)newIndex;
  // paging behavior: show fixed groups/pages of size _itemsPerScreen.
  // The visible page should be the page that contains the selected item.
  // That is, pageStart = floor(selIndex / _itemsPerScreen) * _itemsPerScreen.
  // int maxStart = (cnt > _itemsPerScreen) ? (cnt - _itemsPerScreen) : 0;
  int newPageStart = (_selIndex / _itemsPerScreen) * _itemsPerScreen;
  // if (newPageStart > maxStart)
  //   newPageStart = maxStart;
  _pageStart = (uint8_t)newPageStart;
}

void MenuSystem::enterItem()
{
  MenuItem &it = _current->items()[_selIndex];
  if (it.type == MenuItemType::Action)
  {
    if (it.u.action)
      it.u.action();
    return;
  }
  if (it.type == MenuItemType::SubMenu)
  {
    // push parent
    if (_parentDepth < sizeof(_parentStack) / sizeof(_parentStack[0]))
    {
      _selIndexStack[_parentDepth] = _selIndex;
      _pageStartStack[_parentDepth] = _pageStart;
      _parentStack[_parentDepth++] = _current;
      _current = it.u.submenu;
      _selIndex = 0;
      _pageStart = 0;
      // call enterMenu callback if present
      if (_current->getEnterFunc())
        _current->getEnterFunc()();
    }
    return;
  }
  if (it.type == MenuItemType::Value)
  {
    // nothing special on enter
    return;
  }
  if (it.type == MenuItemType::Naming)
  {
    _inNaming = true;
    _namingDesc = it.u.naming;
    // pad with spaces up to maxlen and ensure null-terminated (only if buf present)
    uint8_t maxl = _namingDesc.maxlen;
    if (_namingDesc.buf)
    {
      for (uint8_t i = 0; i < maxl; i++)
        if (_namingDesc.buf[i] == 0)
          _namingDesc.buf[i] = ' ';
      _namingDesc.buf[maxl] = 0;
    }
    _nameEditIndex = 0;
    return;
  }
}

void MenuSystem::exitToParent()
{
  if (_parentDepth == 0)
    return;
  _current = _parentStack[--_parentDepth];
  _selIndex = _selIndexStack[_parentDepth];
  _pageStart = _pageStartStack[_parentDepth];
  if (_parentDepth == 0)
    _skipPatchToSynth = true; //to keep changes made in sub menus to get to menu to write changes 
}

void MenuSystem::adjustValueAtSelection(int delta)
{
  MenuItem &it = _current->items()[_selIndex];
  if (it.type != MenuItemType::Value)
    return;
  ValueDescriptor &vd = it.u.value;
  uint8_t *p8 = vd.valPtr; // vd.valPtr is always uint8_t*
  int step = 0;
  int v = 0;
  if (vd.step > 127)
  {
    step = (int(vd.step) % 128);
    v = (int)(*p8) + delta * (step);
    if (v < (int)vd.min)
      v = (int)vd.max - (step - 1); // wrap around
    if (v > (int)vd.max)
      v = (int)vd.min + (step - 1); // wrap around
  }
  else
  {
    v = (int)(*p8) + delta * (vd.step);
    if (v < (int)vd.min)
      v = (int)vd.min;
    if (v > (int)vd.max)
      v = (int)vd.max;
  }

  *p8 = (uint8_t)v;
}

void MenuSystem::adjustNamingChar(int delta)
{
  static const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~!@#$%^&*()_+-`'|/ ";
  char *buf = _namingDesc.buf;
  int pos = _nameEditIndex;
  // find current char index
  const char *cur = strchr(charset, buf[pos]);
  int idx = 0;
  if (cur)
    idx = (int)(cur - charset);
  int len = strlen(charset);
  idx = (idx + delta + len) % len;
  buf[pos] = charset[idx];
}

void MenuSystem::draw()
{
  _u8g2.clearBuffer();
  if (_inNaming)
    renderNaming();
  else
    renderMenu();
  _u8g2.sendBuffer();
}

void MenuSystem::renderMenu()
{
  //_u8g2.setFont(u8g2_font_6x10_mf); // set in begin()
  int fontV = _u8g2.getMaxCharHeight();
  int fontH = _u8g2.getMaxCharWidth();
  _u8g2.drawStr(0, fontV, _current->title());
  int y = fontV * 2;

  if (_root == _current) // show current patch number (+1) on top menu
  {
    MenuItem &itCurrentPatchNumber = _current->items()[0];
    char patchNumStr[5];
    snprintf(patchNumStr, sizeof(patchNumStr), "%03d", itCurrentPatchNumber.u.value.valPtr ? *(itCurrentPatchNumber.u.value.valPtr) + 1 : 0);
    _u8g2.drawStr(128 - fontH * 5 - 2, y - fontV, patchNumStr);
  }

  for (uint8_t i = 0; i < _itemsPerScreen; i++)
  {
    uint8_t idx = _pageStart + i;
    if (idx >= _current->count())
      break;
    MenuItem &it = _current->items()[idx];
    char title[32];
    snprintf(title, sizeof(title), "%s", it.title);
    if (idx == _selIndex)
    {
      // draw highlight
      _u8g2.setDrawColor(0);
    }
    else
    {
      // not highlighted
      _u8g2.setDrawColor(1);
    }
    _u8g2.drawStr(2, y, title);
    // right side if value
    if (it.type == MenuItemType::Value)
    {
      String vs = valueToString(it.u.value); // calls vd.patchToSynth() to update the synth from the raw "midi" 7bit storage
      int w = _u8g2.getStrWidth(vs.c_str());
      if (_root == _current && idx == 0)
        _u8g2.drawStr(0, y, vs.c_str()); // align left for current patch
      else
        _u8g2.drawStr(128 - w - 2, y, vs.c_str());
    }
    if (idx == _selIndex)
    {
      _u8g2.setDrawColor(1);
    }
    y += fontV;
  }
}

void MenuSystem::renderNaming()
{
  //_u8g2.setFont(u8g2_font_6x12_mf);
  int fontV = _u8g2.getMaxCharHeight();
  int fontH = _u8g2.getMaxCharWidth();

  _u8g2.drawStr(0, fontV, "Edit name:");
  _u8g2.drawStr(2, fontV * 2.5f, _namingDesc.buf);
  // highlight char
  int x = _u8g2.getStrWidth(String(_namingDesc.buf).substring(0, _nameEditIndex).c_str());
  _u8g2.drawFrame(x, fontV * 1.4f, fontH * 1.5f, fontV * 1.5f);
}

String MenuSystem::valueToString(const ValueDescriptor &vd)
{
  char buf[32];

  if (!_skipPatchToSynth) // because exitToParent to Top
  {
      // first patch to synth so we can display the new value (e.g. patch name)
      if (vd.patchToSynth)
      {
        vd.patchToSynth(); // updates the synth from the raw "midi" 7bit storage
      }
      else // no patchToSynth function
      {
        strcpy(buf, "er ToSyn");
        return String(buf);
      }
  }
  else
  {
      _skipPatchToSynth = false;
  }

  // call converter/applicator
  if (vd.valueToDisp != nullptr)
  {
    vd.valueToDisp(buf, vd.valPtr);
  }
  else
  {
    strcpy(buf, "er ToDisp");
    return String(buf);
  }

  return String(buf);
}
