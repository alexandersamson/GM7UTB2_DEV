#ifndef Prompt_h
#define Prompt_h

#include <Arduino.h>
#include <ILI9341_T4.h>
#include <tgx.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include "Constants.h"
#include "Globals.h"
#include "Sprites.h"
#include "Interface.h"
#include "GuiButtons.h"

using namespace tgx;

class Prompt{

  private:
  
  protected:
    Image<RGB565> *tgxImage;
    Sprites *sprites;
    Constants *constants;
    Globals *globals;
    Interface *interface;
    GuiButtons *guiButtons;

    struct Position
    { 
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height; 
    };
    Position position = {10,10,300,220}; // default prompt sizes
    uint32_t id = 0;
    char * name = new char[SIZE_ARRAY_NAME];
    char * description = new char[SIZE_ARRAY_DESCRIPTION];
    bool visible = false;
    bool enabled = true;
    uint32_t integerValue = 0;
    char * stringValue = new char[64];

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


  public:
    Prompt(){}
    virtual   void initCommonDataPackage() = 0;
    virtual   void reset() = 0;
    virtual   void enable() = 0;
    virtual   void disable() = 0;
    virtual   uint32_t getId() = 0;
    virtual   void setId(uint32_t newId) = 0;
    virtual   char * getName() = 0;
    virtual   void setName(char * newName) = 0;
    virtual   char * getDescription() = 0;
    virtual   bool isVisible() = 0;
    virtual   void show() = 0;
    virtual   void hide() = 0;
    virtual   Position getPosition() = 0;
    virtual   void setPosition(Position pos) = 0;
    virtual   void setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
    virtual   void inputChar(char key) = 0;
    virtual   void inputInteger(uint32_t) = 0;
    virtual   void drawButtons() = 0;
    virtual   void loop() = 0;
    virtual   void loopGfx() = 0;

    //Own methods
    bool sendDataPackageToInterfaceAndClosePrompt(Interface *interface, Globals::CommonInternalDataPackage dataPackageToSend);
    bool sendDataPackageToInterface(Interface *interface, Globals::CommonInternalDataPackage dataPackageToSend);
    void requestToClose();
    bool isRequestingToClose();
    void close();
};


#endif