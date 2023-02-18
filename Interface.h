#ifndef Interface_h
#define Interface_h


#include <stdint.h>
#include <Arduino.h>
#include "FileSystem.h"
#include "Gm7Can.h"
#include <ILI9341_T4.h>
#include <tgx.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include "Constants.h"
#include "Globals.h"
#include "Sprites.h"
#include "AudioProcessing.h"


#define BOOT_ENABLE_DISPLAY true
#define BOOT_ENABLE_AUDIO true
#define BOOT_ENABLE_NEOPIXEL_10 true
#define BOOT_ENABLE_KEYPAD true

#define BOOT_PROGRESS_VALUE_DISPLAY 50
#define BOOT_PROGRESS_VALUE_AUDIO 50
#define BOOT_PROGRESS_VALUE_NEOPIXEL_10 50
#define BOOT_PROGRESS_VALUE_KEYPAD 50

#define INPUT_CONSUMER_GAME 1
#define INPUT_CONSUMER_GUI 2

#define INTERFACE_STATUS_BOOT 1
#define INTERFACE_STATUS_LOG_IN 2
#define INTERFACE_STATUS_LOGGED_IN_USER 4
#define INTERFACE_STATUS_LOGGED_IN_ADMIN 8
#define INTERFACE_STATUS_LOGGED_IN_DEV 16
#define INTERFACE_STATUS_INGAME 32
#define INTERFACE_STATUS_USER_MENU 64
#define INTERFACE_STATUS_ADMIN_MENU 128
#define INTERFACE_STATUS_DEV_MENU 256
#define INTERFACE_STATUS_LOCKED_OUT 512

#define KEYPAD_VAL_1 '1'
#define KEYPAD_VAL_2 '2'
#define KEYPAD_VAL_3 '3'
#define KEYPAD_VAL_4 '4'
#define KEYPAD_VAL_5 '5'
#define KEYPAD_VAL_6 '6'
#define KEYPAD_VAL_7 '7'
#define KEYPAD_VAL_8 '8'
#define KEYPAD_VAL_9 '9'
#define KEYPAD_VAL_0 '0'
#define KEYPAD_VAL_HASH '#'
#define KEYPAD_VAL_STAR '<'

#define KEYPAD_SHIFT_1 'A'
#define KEYPAD_SHIFT_2 'B'
#define KEYPAD_SHIFT_3 'C'
#define KEYPAD_SHIFT_4 'D'
#define KEYPAD_SHIFT_5 'E'
#define KEYPAD_SHIFT_6 'F'
#define KEYPAD_SHIFT_7 'G'
#define KEYPAD_SHIFT_8 'H'
#define KEYPAD_SHIFT_9 'I'
#define KEYPAD_SHIFT_0 'J'
#define KEYPAD_SHIFT_HASH '@'
#define KEYPAD_SHIFT_STAR '*'

#define KEYS_RECEIVED_BUFFER_SIZE 32

using namespace tgx;

class Interface {
  //KEYS BUFFER
  char keysReceivedBuffer[KEYS_RECEIVED_BUFFER_SIZE + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  //The last termination is mandatory!
  char keypadMatrix[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
  char keypadMatrixShift[4][3] = {
    {'a', 'b', 'c'},
    {'d', 'e', 'f'},
    {'g', 'h', 'i'},
    {'<', '^', '>'}};
  private:
  FileSystem *fileSystem;
  Gm7Can *gm7Can;
  Image<RGB565> *tgxImage;
  Sprites *sprites;
  AudioProcessing *audioProcessor;
  Constants *constants;
  Globals *globals;
  ILI9341_T4::ILI9341Driver *tft;
  uint16_t *frameBuffer;

  //GUI KEYS STATE
    struct GuiKeyStates {
      bool backKeyIsCaptured;
      bool backKeyIsShown;
      bool confirmKeyIsShown;
    };
   GuiKeyStates guiKeyStates = {
      false,
      false,
      false,
    };

  const char * customBackKeyStringPtr;
  const char * customConfirmKeyStringPtr;

  //GENERAL
  uint32_t status;
  uint32_t lastReadMillis;
  uint32_t deltaMillis;

  //BOOT
  uint16_t progressBootValueCurrent;
  uint16_t progressBootValueMax;

  //INPUT CONSUMER SELECTION
  uint16_t selectedInputConsumer = INPUT_CONSUMER_GAME; //TODO: make this automatic ^Alexander
  
  //KEYS
  bool receivedNewKeyInput;
  
  //KEYPAD
  bool    isEnabledKeypad;
  uint32_t debounceTimerSet;
  uint32_t holdButtonTimer;
  uint32_t longpressTimerSet;
  uint8_t lastPressed[2] = { 0, 0 };
  char buttonPressChar;
  char buttonLongPressChar;
  bool buttonPress;
  bool buttonPressed;
  bool buttonPressReleased;
  bool buttonLongPressReleased;
  bool buttonPressAvailable;
  bool buttonLongPressAvailable;

  //REQUESTED GAMESTATES
  bool newGameStateRequest = false;
  uint32_t requestedGameState;

  //STATUSSES AND USER ACCESS
  void _statusChangeLogOut();
  void _statusChangeLogInUser();
  void _statusChangeLogInAdmin();
  void _statusChangeLogInDev();
  void _statusChangeExitAllMenus();
  void _statusChangeExitAllGames();
  void _statusChangeEnterUserMenu();
  void _statusChangeEnterAdminMenu();
  void _statusChangeEnterDevMenu();
  void _statusChangeEnterGame();

  //GAME MODULES
  uint16_t activeGameModule;


  public:
  //CTOR
  Interface(FileSystem *fileSystemObj, Gm7Can *gm7CanObj, Image<RGB565> *tgxImageObj, Sprites *spritesObj, AudioProcessing *audioProcessorObj, Constants *constants, Globals *globalsObj, ILI9341_T4::ILI9341Driver *tftObj, uint16_t *frameBufferObj);
  //GENERAL
  void updateDeltaMillis();
  uint32_t getDeltaMillis();
  //BOOT
  void calculateProgressBootValueMax ();
  void boot();
  //KEYS BUFFER
  void addKeyToKeysReceivedBuffer(char key);
  char * getKeysReceivedBuffer();
  char getLastReceivedKey();
  char consumeLastReceivedKey(); //Resets the new char var
  void setReceivedNewKeyInput(bool val = true);
  bool getReceivedNewKeyInput();
  void processSerialRxChar(char chr);
  bool isNumeric(char key);
  //KEYPAD
  bool initKeypad(uint8_t r1Pin = 39, uint8_t r2Pin = 41, uint8_t r3Pin = 40, uint8_t r4Pin = 38, uint8_t c1Pin = 37, uint8_t c2Pin = 35, uint8_t c3Pin = 36);
  void resetKeypadPress();
  void readKeypadPress();
  bool getIsEnabledKeypad();
  void setIsEnabledKeypad(bool isEnabled);
  void readKeypad();

  //CAN TRANSCEIVER
  void parseCanRxBufferStorage();
  void sendMainTimerOverCan(uint32_t timerCurrent, uint32_t timerSet);
  void processDeviceRegistrationRequest(uint16_t uid, uint16_t pmid, char * payload);
  void processReceivedDeviceData(uint16_t uid, uint16_t pmid, char * payload);
  void processReceivedCanModuleData(uint16_t uid, uint16_t pmid, char * payload);

  //LOOP INTERFACE
  void loop();
  //AUDIO FEEDBACK
  void playBeep();
  void playBlip();
  void playPing();
  void playPong();
  void playTick();
  void playTickSoft();
  void playConfirm();
  void playError();
  void playBlocked();
  void playCancel();
  void playTimerTickNormal();
  void playTimerTickElevated();
  void playTimerTickFinal();
  void playNotificationValidationBonus();
  void playNotificationValidationPunish();
  void playNotificationValidationFailed();
  void playNotificationValidationAlreadyUsed();
  void playNotificationValidationSuccess();
  void playNotificationModuleSolved();
  void lowerBackgroundMusic();
  void normalizeBackgroundMusic();
  void playIntesifiedBackgroundMusic();
  void stopIntesifiedBackgroundMusicAndPlayNormalBackgroundMusic();
  bool getIsPlayingIntensifiedBackgroundMusic();
  void playNotificationDecoding();
  void stopNotificationDecoding();
  void playNotificationWrongCode();
  void blockAudioInterrupts();
  void allowAudioInterrupts();

  //GAME MECHANICS/PASSTHROUGHS
  void makeGameStateRequest(uint32_t requestedState);
  bool getIsNewGameStateRequestAvailable();
  uint32_t consumeGameStateRequest();
  bool requestStartGame();

  //UNSORTED
  void pushGameGuiKeysStates(bool stateCaptured, bool showConfirm, bool showBack);
  void pushGameGuiKeysStrings(const char * confirmChar, const char * backChar);
  bool getKeyStateBackIsCaptured();
  bool getKeyStateBackIsShown();
  bool getKeyStateConfirmIsShown();
  const char * getKeyStateBackString();
  const char * getKeyStateConfirmString();
  uint16_t getActiveGameModule();
  void setActiveGameModule(uint16_t newActiveModule);

  // IO ROUTING
  void setSelectedInputConsumer(uint16_t newSelection);
  uint16_t getSelectedInputConsumer();
  bool getSelectedInputConsumerIsGame();
  bool getSelectedInputConsumerIsGui();

  //INTERNAL DATA PACKAGE HANDLING
  bool inputCommonDataPackage(Globals::CommonInternalDataPackage receivedDataPackage);

  //INTERFACE IO STATUS CHANGE REQUESTS
  bool requestStatusChangeToIngame();
  bool requestStatusChangeToUserMenu();
  bool getStatusIsIngame();
  bool getStatusIsInUserMenu();
  bool getStatusIsInAdminMenu();
  bool getStatusIsInDevMenu();
  //Get access levels
  bool hasAccessAsUser();
  bool hasAccessAsAdmin();
  bool hasAccessAsDev();
  uint16_t getCurrentAccessLevel();

};

#endif