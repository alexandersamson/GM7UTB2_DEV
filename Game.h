#ifndef Game_h
#define Game_h


#include <stdint.h>
#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include "GameTimer.h"
#include "AudioProcessing.h"
#include "Interface.h"
#include "GameModule.h"
#include "Gui.h"
#include <Chrono.h>




class Game {

  private:

    Interface *interface;
    GameTimer *mainTimer;
    AudioProcessing *audioProcessor;
    Constants *constants;
    Globals *globals;
    GameModule ** modules;
    Chrono chronoTimer10Fps;
    Chrono chronoTimer2Fps;

    //MODULE INFO PACKAGE (From the Constants file)


    uint32_t state;
    char lastUserInput = '\0';
    uint16_t modulesCount;
    uint16_t activeModule;

    //Key stuff
    struct ModuleGuiKeyStates {
      bool backKeyIsCaptured;
      bool backKeyIsShown;
      bool confirmKeyIsShown;
    };
    ModuleGuiKeyStates moduleGuiKeyStates = {
      false,
      false,
      false,
    };


    const char * customBackKeyString;
    const char * customConfirmKeyString;
    

  public:
    Game(Interface *interfaceObj, GameTimer *gameTimerObj, AudioProcessing *audioProcessorObj, Constants *constantsObj, Globals *globalsObj, GameModule ** moduleObj, uint16_t modulesCnt);
    void reset();
    uint32_t start(); //returns state
    void attachAndResetGameModules();
    void setState(uint32_t newState);
    uint32_t getState();
    uint16_t getMemBeepsChannel();
    bool isInStartedMode();
    bool isInValidationMode();
    bool getIsUserInputEnabled();
    void parseUserInput();
    void disarm();
    void detonate();
    void loop();
    void runModules();
    void runModulesGfx();
    void fetchModuleGuiKeysStates();
    void fetchModuleGuiKeysStrings();
    void handleInterfaceRequests();
   
};


 #endif