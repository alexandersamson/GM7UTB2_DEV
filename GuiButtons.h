#ifndef GuiButtons_h
#define GuiButtons_h

#include <Arduino.h>
#include <ILI9341_T4.h>
#include <tgx.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include "Constants.h"
#include "Globals.h"
#include "Sprites.h"

class GuiButtons{

  #define ICON_BUTTON_SIZE_SMALL 0
  #define ICON_BUTTON_SIZE_MEDIUM 1
  #define ICON_BUTTON_SIZE_BIG 2

  #define BUTTON_HEIGHT_SMALL 14
  #define BUTTON_HEIGHT_MEDIUM 18
  #define BUTTON_HEIGHT_BIG 25

  private:
    Image<RGB565> *tgxImage;
    Sprites *sprites;
    Constants *constants;
    Globals *globals;

  public:
  GuiButtons(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Constants *constantsObj, Globals *globalsObj);
  uint16_t drawKeypadButtonWithLabel(uint16_t x, uint16_t y, char buttonChar, const char * buttonLabel, uint16_t size, bool isEnabled);
  void drawKeypadButtonIcon(char character, uint16_t x, uint16_t y, uint16_t size = ICON_BUTTON_SIZE_SMALL, bool showAsEnabled = true);


};

#endif
