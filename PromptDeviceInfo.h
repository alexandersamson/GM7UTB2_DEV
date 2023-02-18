#ifndef PromptDeviceInfo_h
#define PromptDeviceInfo_h

#include <Arduino.h>
#include "Prompt.h"
#include <ILI9341_T4.h>
#include <tgx.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include "Sprites.h"
#include "Constants.h"
#include "Globals.h"
#include "Interface.h"
#include "GuiButtons.h"



using namespace tgx;

class PromptDeviceInfo : public Prompt {

  protected:
    Image<RGB565> *tgxImage;
    Sprites *sprites;
    Constants *constants;
    Globals *globals;
    Interface *interface;
    GuiButtons *guiButtons;

  public:
    PromptDeviceInfo(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Constants *constantsObj, Globals *globalsObj, Interface *interfaceObj, GuiButtons *guiButtonsObj);
    void initCommonDataPackage();
    void reset();
    void enable();
    void disable();
    uint32_t getId();
    void setId(uint32_t newId);
    char * getName();
    void setName(char * newName);
    char * getDescription();
    bool isVisible();
    void show();
    void hide();
    Prompt::Position getPosition();
    void setPosition(Prompt::Position pos);
    void setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void inputChar(char key);
    void inputInteger(uint32_t);
    void drawButtons();
    void loop();
    void loopGfx();

};

  #endif