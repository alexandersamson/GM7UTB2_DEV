#ifndef GMDisarmCode_h
#define GMDisarmCode_h

#include <Arduino.h>
#include "GameModule.h"
#include <ILI9341_T4.h>
#include <tgx.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include "Constants.h"
#include "Globals.h"
#include "Sprites.h"
#include "Interface.h"
#include "GameTimer.h"

using namespace tgx;


class GMDisarmCode : public GameModule {




private:
  Image<RGB565> *tgxImage;
  Sprites *sprites;
  Interface *interface;
  GameTimer *internalTimer;
  GameTimer *validationTimer;
  Constants *constants;
  Globals *globals;

  const char * const moduleName[COUNT_LANGUAGES] = {"Keypad module", "Keypadmodule"};
  const char * const moduleDescription[COUNT_LANGUAGES] = {"Solve this module by entering in the correct code(s) at the keypad terminal.", "Los deze module op door het ingeven van de juiste code(s) op het keypadterminal."};

  #define GAME_VALIDATION_DISARM_CODE 1
  #define GAME_VALIDATION_BONUS_CODE 2
  #define GAME_VALIDATION_PUNISH_CODE 4
  #define GAME_VALIDATION_HINT_CODE 8
  #define GAME_VALIDATION_SYSTEM_CODE 16
  #define GAME_VALIDATION_INVALID_CODE 32
  #define GAME_VALIDATION_ALREADY_USED 64


  char gameStatusString[9][MODULE_CONFIG_STRING_MAX_LENGTH] = {"PREPARED", "COUNTING DOWN", "STOPPED", "PAUSED", "DEFUSED", "EXPIRED", "SLOW COUNTDOWN", "FAST COUNTDOWN", "UNPREPARED"};


  
  

  uint16_t keypadLegendBlinkKeysArrayNoEntries[12] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    0, 1, 0
  };
  uint16_t keypadLegendBlinkKeysArraySomeEntries[12] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1
  };
  uint16_t keypadLegendBlinkKeysArrayBufferFull[12] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    1, 0, 1
  };
  uint16_t keypadLegendBlinkKeysArrayDisabled[12] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0
  };
  uint16_t keypadLegendBlinkColorArray[12] = {
    0b1111111111111111, 0b1111111111111111, 0b1111111111111111,
    0b1111111111111111, 0b1111111111111111, 0b1111111111111111,
    0b1111111111111111, 0b1111111111111111, 0b1111111111111111,
    0b1111111111111111, 0b1111111111111111, 0b1111111111111111
  };
  uint8_t keypadLegendButtonsRows = 4;
  uint8_t keypadLegendButtonsCols = 3;
  struct Vector2 {
    uint16_t x;
    uint16_t y;
  };
  Vector2 keypadLegendPos = { 11, 28 };
  Vector2 keypadLegendFirstKeyOffset = { 3, 4 };
  Vector2 keypadLegendBtnSize = { 11, 9 };
  Vector2 keypadLegendBtnSpacing = { 2, 2 };


//MODULE INFO TEXTS
  const char * const infoTextDefault[COUNT_LANGUAGES] = {"Unknown state", "Onbekende status"};
  const char * const infoTextEnterCode[COUNT_LANGUAGES] = {"Enter command code", "Geef opdrachtcode"};
  const char * const infoTextValidating[COUNT_LANGUAGES] = {"Validating...", "Valideren..."};
  const char * const infoTextDeactivated[COUNT_LANGUAGES] = {"Module inactive", "Module inactief"};
  const char * const infoTextSolved[COUNT_LANGUAGES] = {"Module solved", "Module opgelost"};

  const char * disarmModuleInfoText;

  
  //Module specific methods
  uint16_t _validateAndLogCodeEntryPerCategory(char * buffer, char * codeSet[], char * codeLog[], uint16_t maxCodes, uint16_t returnValue);
  uint16_t _validateCodeEntry();
  void _abortValidation();
  char * _getEnteredStringBuffer();
  void _clearEnteredStringBuffer();
  uint16_t _getEnteredStringBufferPosition();
  bool _isFullEnteredStringBuffer();
  uint16_t _getMaxEnteredStringBufferSize();
  void _drawDefuseCodeBlock();

public:
  GMDisarmCode(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Interface *interfaceObj, GameTimer *internalTimerObj, GameTimer *validationTimerObj, Constants *constantsObj, Globals *globalsObj);
  void reset();
  void loadModuleConfig();
  void activate();
  void deactivate();
  void enable();
  void disable();
  void postGameProcess();
  void switchAwayToAnotherModule();
  const char * getName();
  void setName(char * newName = NULL);
  const char * getDescription();
  void setDescription(char * newDescription = NULL);
  bool isVisible();
  void makeVisible();
  Position getPosition();
  void setPosition(Position pos);
  void setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  uint32_t getState();
  uint32_t getProgress();
  void setProgress(uint32_t prg); 
  void setState(uint32_t st);
  uint32_t getValidationTimerCurrentMillisLeft();
  uint32_t getValidationTimerSetMillis();
  void inputKeyboard(char key);
  void inputInteger(uint32_t num);
  void setGameModifierOutput(uint16_t newValue);
  bool isAvailableGameModifierOutput();
  uint16_t consumeGameModifierOutput();
  void updateGuiKeyStates();
  void onStateChange();
  void loop(uint32_t gameState);
  void loopGfx();



};

#endif