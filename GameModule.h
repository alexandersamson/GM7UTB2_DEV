#include <stdint.h>
#ifndef GameModule_h
#define GameModule_h

#include <Arduino.h>
#include <ILI9341_T4.h>
#include <tgx.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include "Constants.h"
#include "Globals.h"
#include "Sprites.h"
#include "Interface.h"




using namespace tgx;

class GameModule{

protected:


  struct Position
  { 
      uint16_t posX;
      uint16_t posY;
      uint16_t width;
      uint16_t height; 
  };
  Position position = {0,100,320,80}; // default module size
  struct ColorScheme
  { 
      uint16_t text;
      uint16_t textInverted;
      uint16_t textAlert;
      uint16_t frame;
      uint16_t frameAlert;
      uint16_t body; 
      uint16_t background; 
      uint16_t backgroundAlert;  
      uint16_t progressBar;  
      uint16_t progressBarAlert;
      uint16_t progressBarComplete;
  };
  ColorScheme colorScheme = {
    0b1111111111111111,
    0b0000000000000000,
    0b1111100000000000,
    0b0000000111101111,
    0b1111100011100111,  //frame alert
    0b0000000011100111, 
    0b0000000001100001, 
    0b0001100000100000, 
    0b0011111111111111, //progress bar
    0b0011111111111111, //progress Alert
    0b0011111111111111  //progress green
  }; 
    struct StatusColors
  { 
      uint16_t inactive;
      uint16_t active;
      uint16_t solved;
      uint16_t failed;
      uint16_t informative;
  };
  StatusColors statusColors = {
    0x6B6D,
    0xFB80,
    0x07E0,
    0xF8A2,
    0b0000000111101111 
  }; 
  uint16_t internalId;
  bool hasAssignedInternalId;
  const char * name;
  const char * description;
  uint16_t canUid = 0;
  bool isExternalCanModule = false;
  bool isOnlineOverCan = false;
  uint32_t state = GAME_MODULE_STATE_INACTIVE;
  uint32_t progress = 0;
  uint32_t progressMin = 0;
  uint32_t progressMax = 1;
  uint32_t progressMaxState = GAME_MODULE_STATE_SOLVED;
  uint16_t triesCurrent = 0;
  uint16_t triesMax = 0;
  uint16_t triesTotal = 0;
  uint16_t triesFlags = 0b0000000000000000;
  uint32_t triesMaxState = GAME_MODULE_STATE_FAILED;
  uint32_t triesResetAtMax = true; //are we able to do this module over and over after the max tries are used up?
  uint32_t internalTimerSet = 0;
  uint32_t validationTimerSet = 1000;
  uint16_t gameModifierOutput = GAME_MODULE_OUTPUT_NORMAL;
  bool consumedSolvedTrigger = false;
  bool enabled = true;
  bool visible = false;
  uint32_t lastKnownState = 0;
  bool stateHasChanged = false;

  //Key stuff
  bool backKeyIsCaptured = false;
  bool backKeyIsShown = false;
  bool confirmKeyIsShown = false;
  char * customBackKeyString  = new char[SIZE_ARRAY_BUTTON_LABEL_SMALL];
  char * customConfirmKeyString  = new char[SIZE_ARRAY_BUTTON_LABEL_SMALL];

  //Import stuff
  Image<RGB565> *tgxImage;
  Sprites *sprites;
  Interface *interface;
  Constants *constants;
  Globals *globals;

  //STRING DATA
  char * disarmStringsSet[MODULE_CONFIG_MAX_COMMAND_STRINGS];
  char * disarmStringsUsed[MODULE_CONFIG_MAX_COMMAND_STRINGS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  char * bonusStringsSet[MODULE_CONFIG_MAX_COMMAND_STRINGS];
  char * bonusStringsUsed[MODULE_CONFIG_MAX_COMMAND_STRINGS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  char * punishStringsSet[MODULE_CONFIG_MAX_COMMAND_STRINGS];
  char * punishStringsUsed[MODULE_CONFIG_MAX_COMMAND_STRINGS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  char * gpioStringsSet[MODULE_CONFIG_MAX_COMMAND_STRINGS];
  char * gpioStringsUsed[MODULE_CONFIG_MAX_COMMAND_STRINGS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  char enteredStringBuffer[MODULE_CONFIG_STRING_MAX_LENGTH] = "\0";
public:

  //GameModule(Image<tgx::RGB565>* imageObj, Sprites* spritesObj, Interface* interfaceObj): tgxImage(imageObj), sprites(spritesObj), interface(interfaceObj){};
  GameModule(){}
  virtual   void reset() = 0;
  virtual   void loadModuleConfig() = 0;
  virtual   void activate() = 0;
  virtual   void deactivate() = 0;
  virtual   void enable() = 0;
  virtual   void disable() = 0;
  virtual   void postGameProcess() = 0;
  virtual   void switchAwayToAnotherModule() = 0;
  virtual   const char * getName() = 0;
  virtual   void setName(char * newName) = 0;
  virtual   const char * getDescription() = 0;
  virtual   void setDescription(char * newDescription) = 0;
  virtual   bool isVisible() = 0;
  virtual   void makeVisible() = 0;
  virtual   Position getPosition() = 0;
  virtual   void setPosition(Position pos) = 0;
  virtual   void setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
  virtual   uint32_t getState() = 0;
  virtual   void setState(uint32_t st) = 0;
  virtual   uint32_t getProgress() = 0;
  virtual   void setProgress(uint32_t progr) = 0; 
  virtual   uint32_t getValidationTimerCurrentMillisLeft() = 0;
  virtual   uint32_t getValidationTimerSetMillis() = 0;
  virtual   void inputKeyboard(char key) = 0;
  virtual   void inputInteger(uint32_t) = 0;
  virtual   void setGameModifierOutput(uint16_t newValue) =0;
  virtual   bool isAvailableGameModifierOutput() = 0;
  virtual   uint16_t consumeGameModifierOutput() = 0;
  virtual   void updateGuiKeyStates() = 0;
  virtual   void onStateChange() = 0;
  virtual   void loop(uint32_t gameState) = 0;
  virtual   void loopGfx() = 0;

  //Base methods
  void assignInternalId(uint16_t newInternalId);
  bool getHasAssignedInternalId();
  uint16_t getAssignedInternalId();
  void setProgressInternal(uint32_t prg);
  uint16_t getStatusColor();
  uint16_t getProgressBarWidth(uint16_t endTitleHeaderX);
  void drawModuleFrame(Image<RGB565> *tgxImage);
  bool hasConsumedSolvedTrigger();
  void consumeSolvedTrigger();

  //GET STATES
  bool getStateIsSolved();
  bool getStateIsFailed();
  bool getStateIsInactive();
  bool getStateIsValidating();
  bool getStateIsAltered();
  bool getStateIsActive();
  bool getStateIsSolvable();
  bool getStateIsConsideredActivated();
  bool getStateIsConsideredLoopable();
  bool getStateIsConsideredContributingToGameplay();
  bool getStateIsActivatedAndNotValidating();
  bool getStateIsConsideredToStayUnchangedPostGame();

  //SET STATES
  void setStateAddFlagAltered();
  void setStateRemoveFlagAltered();
  void setStateAddFlagValidating();
  void setStateRemoveFlagValidating();
  void setStateAddFlagSolved();
  void setStateRemoveFlagSolved();
  void setStateAddFlagFailed();
  void setStateRemoveFlagFailed();
  void setStateAddFlagInactive();
  void setStateRemoveFlagInactive();
  void setStateAddFlagSolvable();
  void setStateRemoveFlagSolvable();

  void captureBackKey();
  void releaseBackKey();
  bool isCapturedBackKey();
  void setCustomBackKeyString(const char * newBackString);
  const char * getCustomBackKeyString();
  void setCustomConfirmKeyString(const char * newConfirmString);
  const char * getCustomConfirmKeyString();
  void showBackKey();
  void hideBackKey();
  void showConfirmKey();
  void hideConfirmKey();
  bool getConfirmKeyIsShown();
  bool getBackKeyIsShown();
  bool getAndUpdateStateHasChanged();
  bool getIsExternalCanModule();
  bool getIsOnlineOverCan();
  void turnOnlineOverCan();
  void turnOfflineOverCan();
  };

#endif