#ifndef Gui_h
#define Gui_h

#include <Arduino.h>
#include <ILI9341_T4.h>
#include <tgx.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include "Constants.h"
#include "Sprites.h"
#include "Interface.h"
#include "Globals.h"
#include "GuiButtons.h"
#include "NotificationBadge.h"
#include "Prompt.h"

using namespace tgx;

class Gui{

    //SEE Constants.h for values below:
    // #define GUI_GAME_MENU 0
    // #define GUI_GAME_SUB_MENU 1
    // #define GUI_MAIN_MENU 2
    // #define GUI_MAIN_SUB_MENU 3
    // #define GUI_PROMPT 4
    // #define GUI_ALERT_BADGE 5
    // #define GUI_LOGIN 6

  private:
    Image<RGB565> *tgxImage;
    Sprites *sprites;
    Interface *interface;
    Constants *constants;
    Globals *globals;
    GuiButtons *guiButtons;
    NotificationBadge *notificationBadge;
    Prompt ** prompts;
    Constants::MenuEntry *c_menuEntries;


    uint16_t promptsCount;
    uint16_t activePrompt = 0;

    //USER INPUT
    char lastUserInput;

    //BUTTON LABELS BOTTOM BAR
    bool showBottomButtons = true;
    bool showButtonBottomLeft = false;
    bool showButtonBottomMiddle = false;
    bool showButtonBottomRight = false;




    //SELECTORS
    uint16_t activeMenu = GUI_MENU_SELECTED_NONE;
        bool showMenu[GUI_COUNT_MENUS_TOTAL] =          {false, false, false, false, false, false, false};
    uint16_t selectedCurrent[GUI_COUNT_MENUS_TOTAL] =   {GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE};
    uint16_t selectedCurrentId[GUI_COUNT_MENUS_TOTAL] = {GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE};
    uint16_t cursorCurrent[GUI_COUNT_MENUS_TOTAL] =     {GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE};
    uint16_t cursorCurrentId[GUI_COUNT_MENUS_TOTAL] =   {GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE};
    uint16_t cursorMin[GUI_COUNT_MENUS_TOTAL] =         {0, 0, 0, 0, 0, 0, 0};
    uint16_t cursorMax[GUI_COUNT_MENUS_TOTAL] =         {GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE, GUI_MENU_SELECTED_NONE};


    uint16_t menuEntryIdMatrix[GUI_COUNT_MENUS_TOTAL][GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT];

    

    //ANIMATION
    #define ANIMATE_OPEN false
    #define ANIMATE_CLOSE true
    #define ANIMATION_VARIANT_LEFT_TO_RIGHT_OPEN 1
    #define ANIMATION_VARIANT_LEFT_TO_RIGHT_CLOSE 2
    #define ANIMATION_VARIANT_RIGHT_TO_LEFT_OPEN 3
    #define ANIMATION_VARIANT_RIGHT_TO_LEFT_CLOSE 4
    #define ANIMATION_VARIANT_CENTER_OPEN 5
    #define ANIMATION_VARIANT_CENTER_CLOSE 6
    #define ANIMATION_VARIANT_TOP_TO_BOTTOM_OPEN 7
    #define ANIMATION_VARIANT_TOP_TO_BOTTOM_CLOSE 8
    struct FrameBox {
      uint16_t x;
      uint16_t y;
      uint16_t w;
      uint16_t h;
    };

    struct AnimationData {
      uint16_t animationFramesDelay = 0;
      uint16_t animationFramesDelaySet = 0;
      uint16_t animationVariantSet = 0;
      uint16_t animationVariantCurrent = 0;
      bool animationOngoing = false;
      uint32_t animationLastMillis = 0;
      uint16_t animationMaxFrames = 0;
      uint16_t animationCurrentFrame = 0;
    };

    AnimationData animations[GUI_COUNT_MENUS_TOTAL] = {
      {
        0,  0,  ANIMATION_VARIANT_LEFT_TO_RIGHT_OPEN,  ANIMATION_VARIANT_LEFT_TO_RIGHT_OPEN, false, 0, 12, 0 //GAME MENU
      },
      {
        0,  0,  ANIMATION_VARIANT_RIGHT_TO_LEFT_OPEN,  ANIMATION_VARIANT_RIGHT_TO_LEFT_OPEN, false, 0, 12, 0 //GAME SUBMENU
      },
      {
        0,  0,  ANIMATION_VARIANT_LEFT_TO_RIGHT_OPEN,  ANIMATION_VARIANT_LEFT_TO_RIGHT_OPEN, false, 0, 12, 0 //MAIN MENU
      },
      {
        0,  0,  ANIMATION_VARIANT_RIGHT_TO_LEFT_OPEN,  ANIMATION_VARIANT_RIGHT_TO_LEFT_OPEN, false, 0, 12, 0 //MAIN SUBMENU
      },
      {
        0,  0,  ANIMATION_VARIANT_CENTER_OPEN, ANIMATION_VARIANT_CENTER_OPEN, false, 0, 14, 0 //PROMPT
      },
      {
        0,  0,  ANIMATION_VARIANT_CENTER_OPEN,  ANIMATION_VARIANT_CENTER_OPEN, false, 0, 12, 0 //ALERT BADGE
      },
      {
        0,  0,  ANIMATION_VARIANT_CENTER_OPEN,  ANIMATION_VARIANT_CENTER_OPEN, false, 0, 14, 0 //LOGIN SCREEN
      }
    };

    //DIMENSIONS / POSITIONS

              
    #define GUI_ORIGIN_X 0
    #define GUI_ORIGIN_Y 0
    #define OFFSET_TEXT_X 4
    #define OFFSET_TEXT_Y 14
    #define GUI_PADDING 2
    #define DISPLAY_W 320
    #define DISPLAY_H 240
    #define GAME_TIMER_H 64
    #define STATUS_BAR_H 13
    #define BUTTON_BAR_H 18
    #define MENU_HEADER_H 16
    #define MENU_ITEM_LINE_H 16
    #define PROMPT_PADDING 6
    #define BADGE_PADDING 12    
                                  //X            //Y                                             //W         //H
    FrameBox boxStatusBar      =  {GUI_ORIGIN_X, GUI_ORIGIN_Y,                                  DISPLAY_W,  STATUS_BAR_H };
    FrameBox boxGameTimer      =  {GUI_ORIGIN_X, GUI_ORIGIN_Y + STATUS_BAR_H,                   DISPLAY_W,  GAME_TIMER_H };
    FrameBox boxButtonBar      =  {GUI_ORIGIN_X, DISPLAY_H - BUTTON_BAR_H - 1,                  DISPLAY_W,  STATUS_BAR_H };
    FrameBox boxMenu           =  {GUI_ORIGIN_X, GUI_ORIGIN_Y + STATUS_BAR_H,                   DISPLAY_W,  DISPLAY_H-STATUS_BAR_H - BUTTON_BAR_H };
    FrameBox boxMenuHeader     =  {GUI_ORIGIN_X, GUI_ORIGIN_Y + STATUS_BAR_H,                   DISPLAY_W,  MENU_HEADER_H };
    FrameBox boxMenuContent    =  {GUI_ORIGIN_X, GUI_ORIGIN_Y + STATUS_BAR_H + MENU_HEADER_H,   DISPLAY_W,  DISPLAY_H-STATUS_BAR_H - BUTTON_BAR_H - MENU_HEADER_H };

    uint16_t padding = GUI_PADDING;
    uint16_t menuSplitX = 108;
    uint16_t menuSplitXPadding = 1;
    uint16_t gameTimerHeight = GAME_TIMER_H;


    //HELPER METHODS
    void _drawProgressBarSimpleHorizontal(Gui::FrameBox box, uint32_t dataMin, uint32_t dataMax, uint32_t dataCurrent);
    Gui::FrameBox _getCalculatedHeaderTextPos(Gui::FrameBox box);
    Gui::FrameBox _getCalculatedContentTextPos(Gui::FrameBox box);
    

  public:
    Gui(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Interface *interfaceObj, Constants *constantsObj, Globals *globalsObj, GuiButtons *guiButtonsObj, NotificationBadge *notificationBadgeObj, Prompt ** promptsObj, uint16_t promptsCnt);

    void init();
    void resetToWelcomeScreen();



    void drawMenu(uint16_t menu);
    void drawAnimatedMenuFrame(uint16_t animIdx, FrameBox box, uint16_t frameColor, uint16_t backgroundColor, float opacity = 1.0f);
    void drawBottomButtons();
    uint16_t drawKeypadButtonWithLabel(uint16_t x, uint16_t y, char buttonChar, char * buttonLabel, uint16_t size, bool isEnabled);
    void drawMenuHeader(FrameBox box, uint16_t menu);
    void drawMenuContent(FrameBox box, uint16_t menu);
    void drawTextLineAtYPosIncrement(Gui::FrameBox box, char* text, uint16_t pos, bool isHeader = false, bool isSelected = false, bool isActive = true);
    const char * const getBackButtonLabel();
    const char * const getConfirmButtonLabel();
    const char * const getUpButtonLabel();
    const char * const getDownButtonLabel();
    
    //INPUT PARSING
    void parseUserInput();
    void handleBackButtonPressed();
    void handleHashButtonPressed();
    void handleNumericButtonPressed(char inputChar);
    bool performUserAction(uint16_t actionId);



    //ANIMATIONS


    void runAnimationForMenu(uint16_t menu, bool isForClosing);
    FrameBox animationBoxGetFrame(uint16_t menu, FrameBox startBox, FrameBox endBox);

    //DIMENSIONAL STUFF
    void setMenuSplitX(uint16_t split);
    uint16_t getMenuSplitX();
    uint16_t getMenuSplitXPadding();
    uint16_t getGameTimerObjectH();
    FrameBox getMenuDimensions(uint16_t menu);
    FrameBox getPromptDimensions();
    FrameBox getFittingMenuBox();


    //MENU LOGICS
    void openMenu(uint16_t menu);
    void closeMenu(uint16_t menu = GUI_MENU_SELECTED_NONE);
    void closeActiveMenu();
    void resetAndCloseMenu(uint16_t menu);
    void openBadge();
    void closeBadge();
    void openPrompt(uint16_t menu, uint16_t actionTarget);
    bool navigateToMenuItem(uint16_t menu, uint16_t index = 0, bool searchUp = false, bool isCheckOnly = false);
    void navigateNextMenuItem(uint16_t menu);
    void navigatePreviousMenuItem(uint16_t menu);
    bool canNavigateNextMenuItem(uint16_t menu);
    bool canNavigatePreviousMenuItem(uint16_t menu);
    bool selectMenuItem(uint16_t menu, uint16_t pos);
    bool isAnyMenuStillOpen();

    //LOOPS & FRAMEBUFFER DRAWING
    void updateMenuEntries(uint16_t menu);
    void updateSubMenuContents(uint16_t selectedParentEntryId);
    void loopAnimations();
    void drawGui();
    void loop();
    void loopGfx();
    
};

#endif
