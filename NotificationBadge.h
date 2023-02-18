#ifndef NotificationBadge_h
#define NotificationBadge_h

#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include <tgx.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include "Sprites.h"
#include "GuiButtons.h"

class NotificationBadge{

  private:
    Constants * constants;
    Globals * globals;
    Image<RGB565> *tgxImage;
    Sprites *sprites;
    GuiButtons *guiButtons;

    struct Position
    { 
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height; 
    };
    Position position = {10,10,300,230}; // default badge size
    uint32_t id = 0;
    char * name;
    char * description;
    bool visible = false;
    bool enabled = true;
    uint32_t integerValue = 0;
    char * stringValue;

    //Vars to handling closing
    bool captureBackspace = false;
    bool requestingToClose = false;

    //Data handling
    uint32_t dataPacakgeIterator = 0;

      // struct CommonInternalDataPackage {
        // uint32_t id;
        // uint32_t iteration;
        // uint32_t sourceId;
        // uint32_t destinationId;
        // uint32_t command;
        // char dataStr[32];
        // uint32_t dataInt[4];
      //}
    Globals::CommonInternalDataPackage dataPackage = {
      0,
      0,
      0,
      0,
      0,
      "\0",
      {0,0,0,0}
    };

    #define MAX_COUNT_BADGE_TEXT_LINES 5
    struct BadgeTextLine{
      char * text;
      bool disabled = false;
      bool selected = false;
      bool highlighted = false;
      uint32_t durationLeft = 0;
      uint32_t durationSet = 0;
    };

    BadgeTextLine badgeTextLines[MAX_COUNT_BADGE_TEXT_LINES];

    uint32_t lastMillis;


  public:
    NotificationBadge(Constants * constantsObj, Globals * globalsObj, Image<RGB565> * tgxImageObj, Sprites * spritesObj, GuiButtons * guiButtonsObj);
    void init();
    void setPosition(NotificationBadge::Position pos);
    void setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    uint16_t getAvailableMessagesCount();
    uint16_t getContentHeight();
    void drawTextLines();
    void closeForced();
    void loop();
    void loopGfx();
    

};

#endif
