#include <stdint.h>
#ifndef Globals_h
#define Globals_h
#include <Arduino.h>
#include "Constants.h"

class Globals {

private:
Constants * constants;

public:
  Globals::Globals(Constants * constantsObj): constants(constantsObj){ };

  //STATUSSES
  uint32_t status = 0;
  uint16_t progress = 0;
  uint16_t progressMax = 0;


  struct ModuleConfig { //About 1120 bytes per object
    uint16_t id = 0;
    bool hasAssignedId = false;
    char name[SIZE_ARRAY_NAME] = "Module";
    char description[SIZE_ARRAY_DESCRIPTION] = "";
    bool isExternalCanModule = false;
    uint16_t canUid = 0x0000;
    bool isOnlineOverCan = false;
    bool autoSelectOnStateChange = false;
    uint32_t version = 1;
    bool enabled = 1;
    uint16_t progress = 0;
    uint16_t progressMin = 0;
    uint16_t progressMax = 1;
    uint16_t triesCurrent = 0;
    uint16_t triesMax = 1;
    uint16_t triesTotal = 0;
    bool triesResetAtMax = true;
    bool punishAfterMaxTries = false;
    bool resetAfterSolved = false;
    bool bonusAfterSolved = false;
    uint32_t mainTimerCurrent = 0;
    uint32_t mainTimerSet = 0;
    uint32_t internalTimerCurrent = 0;
    uint32_t internalTimerSet = 0;
    uint32_t validationTimerCurrent = 0;
    uint32_t validationTimerSet = 2000;
    char disarmStrings[MODULE_CONFIG_MAX_COMMAND_STRINGS][MODULE_CONFIG_STRING_MAX_LENGTH] =  {"","","","","","","","","",""};
    char bonusStrings[MODULE_CONFIG_MAX_COMMAND_STRINGS][MODULE_CONFIG_STRING_MAX_LENGTH] =   {"","","","","","","","","",""};
    char punishStrings[MODULE_CONFIG_MAX_COMMAND_STRINGS][MODULE_CONFIG_STRING_MAX_LENGTH] =  {"","","","","","","","","",""};
    char gpioStrings[MODULE_CONFIG_MAX_COMMAND_STRINGS][MODULE_CONFIG_STRING_MAX_LENGTH] =    {"","","","","","","","","",""};
    uint32_t digitalOutputSetToHighByGpioString[MODULE_CONFIG_MAX_COMMAND_STRINGS] =          {0,0,0,0,0,0,0,0,0,0}; //Bitmask 32 of which outputs to drive low when a certain GPIO string is received
    uint32_t digitalOutputSetToLowByGpioString[MODULE_CONFIG_MAX_COMMAND_STRINGS] =           {0,0,0,0,0,0,0,0,0,0}; //Bitmask 32 of which outputs to drive high when a certain GPIO string is received
    uint32_t digitalOutputDurationTimers[MODULE_CONFIG_MAX_COMMAND_STRINGS] =                 {0,0,0,0,0,0,0,0,0,0}; //How long does a digital output stays in the new state before turning to the default
    bool digitalOutputGpioStringIsReusable[MODULE_CONFIG_MAX_COMMAND_STRINGS] =               {0,0,0,0,0,0,0,0,0,0}; //Are GPIO-strings reusable? 
    uint32_t initialDigitalOutputStatesHigh = 0;
    uint32_t initialDigitalOutputStatesLow = 0;
  };
  
  ModuleConfig moduleConfigs[MODULES_COUNT];


  struct Config {
    uint32_t languageId;
    uint32_t gameTimerSet;
    char backgroundMusicMenuFile[SIZE_ARRAY_FILENAME];
    char backgroundMusicGameFile[SIZE_ARRAY_FILENAME];
    char backgroundMusicGameFileHighTension[SIZE_ARRAY_FILENAME];
    char backgroundMusicWinFile[SIZE_ARRAY_FILENAME];
    char backgroundMusicLossFile[SIZE_ARRAY_FILENAME];
    char backgroundMusicDrawFile[SIZE_ARRAY_FILENAME];
    char backgroundMusicPausedFile[SIZE_ARRAY_FILENAME];
    char notificationSoundGameDetonate[SIZE_ARRAY_FILENAME];
    char notificationSoundGameDisarmingSuccess[SIZE_ARRAY_FILENAME];
    char notificationSoundGameLost[SIZE_ARRAY_FILENAME];
    char notificationSoundGameWon[SIZE_ARRAY_FILENAME];
    char notificationSoundGamePunish[SIZE_ARRAY_FILENAME];
    char notificationSoundGameBonus[SIZE_ARRAY_FILENAME];
    char notificationSoundModuleDisabled[SIZE_ARRAY_FILENAME];
    char notificationSoundModuleEnabled[SIZE_ARRAY_FILENAME];
    char notificationSoundModuleFailed[SIZE_ARRAY_FILENAME];
    char notificationSoundModulePaused[SIZE_ARRAY_FILENAME];
    char notificationSoundModuleResumed[SIZE_ARRAY_FILENAME];
    char notificationSoundModuleSolved[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer1MinuteRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer5MinutesRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer10MinutesRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer10SecondsRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer15MinutesRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer30MinutesRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer30SecondsRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer45MinutesRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer60MinutesRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer90MinutesRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer120MinutesRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimer180MinutesRemaining[SIZE_ARRAY_FILENAME];
    char notificationSoundTimerNormalSpeed[SIZE_ARRAY_FILENAME];
    char notificationSoundTimerPaused[SIZE_ARRAY_FILENAME];
    char notificationSoundTimerResumed[SIZE_ARRAY_FILENAME];
    char notificationSoundTimerStarted[SIZE_ARRAY_FILENAME];
    char notificationSoundUnknownCommand[SIZE_ARRAY_FILENAME];
    char notificationSoundValidationStarting[SIZE_ARRAY_FILENAME];
    char notificationSoundValidationAlready[SIZE_ARRAY_FILENAME];
    char notificationSoundValidationFailed[SIZE_ARRAY_FILENAME];
    char notificationSoundValidationSuccess[SIZE_ARRAY_FILENAME];
  };

  Config config;

  //32 bytes of CHAR and 4 uint32_t's can fit in this package
  struct CommonInternalDataPackage {
    uint32_t id;
    uint32_t iteration;
    uint32_t sourceId;
    uint32_t destinationId;
    uint32_t command;
    char dataStr[32];
    uint32_t dataInt[4];
  };


  //This indexer is used to assign ID's t new (external) modules. In that way config files can be attached to these modules.
  uint16_t modulesAssignedIdIndexer = 0;

  //SYSTEM CONTROLS
  bool systemIsOnline = true;
  void haltSystem(){
    systemIsOnline = false;
  };

  void resumeSystem(){
    systemIsOnline = true;
  };

  bool getSystemIsOnline(){
    return systemIsOnline;
  };

  //DEBUG HANDLERS
  #define MAX_MESSAGES_COUNT 6
  #define SYS_ERROR 0
  #define SYS_WARNING 1
  #define SYS_NOTE 2
  #define SYS_INFO 3
  #define SYS_SUCCESS 4
  #define SYS_FAILED 5
  #define ID_SYS_ERROR_CONFIG_LOAD_FAIL_GENERAL 0
  #define ID_SYS_ERROR_CONFIG_LOAD_FAIL_MODULES 1
  struct NotificationMessage {
    bool isNew = false;
    bool writeToLog = false;
    char const * message = "";
    uint32_t id = 0;
    uint32_t type = 0;
    uint32_t durationSet = 0;    
    uint32_t durationLeft = 0;
  };  
  NotificationMessage notificationMessages[MAX_MESSAGES_COUNT];  //Maximum of 5 consecutive messages

  void addMessage(uint32_t id, uint32_t type, const char * message, bool writeToLog = false, uint32_t duration = 10000){
    for(int i = MAX_MESSAGES_COUNT-1; i >= 0; i--){ //Write at end of log
      if(notificationMessages[i].isNew == false && notificationMessages[i].durationLeft < 1){
        notificationMessages[i] = {
          true,
          writeToLog,
          message,
          id,
          type,
          duration,
          duration
        };
        return;
      }      
    }
    Serial.println("Globals::addMessage() >>> ERROR: Message buffer is full");
  };

  char charBuffer[MAX_MESSAGES_COUNT][21];
  uint8_t charBufferCounter = 0;
  void addMessage(uint32_t id, uint32_t type, uint64_t message, bool writeToLog = false, uint32_t duration = 10000){
    sprintf(charBuffer[charBufferCounter], "%llu", message);
    addMessage(id, type, charBuffer[charBufferCounter], writeToLog, duration);
    charBufferCounter++;
    if(charBufferCounter >= MAX_MESSAGES_COUNT){
      charBufferCounter = 0;
    }
  };

  uint32_t getLang() {return config.languageId; };


  //Returns true if any bit in the flags matches with any bit in the provided requiredFlag parameter.
  //If the bool requiresMatchAll is set to true, ALL the bits in requiredFlag needs to be found in the flags parameter before it returns true
  bool bitwiseHasFlag(uint32_t flags, uint32_t requiredFlag, bool requiresMatchAll = false){
    return (requiresMatchAll == true) ? ((flags & requiredFlag) == requiredFlag) : ((flags & requiredFlag) > 0);
  };

  //Returns [true] or [false] based on bitwise operations on a uint32 parameter 'flags' and provided 'maskRequiresAtLeastOne' 'maskRequiresAll' and a 'maskForbidden'. 
  //If not AT LEAST one of the bits in the provided maskRequiresAtLeastOne mask is found, it will return [false];
  //If not ALL of the required bits in the required mask are found, it will return [false];
  //If ANY of the forbidden bits in the forbidden mask is found, it will return [false].
  //It will return [true] if none of above conditions were met. ^Alexander
  bool bitwiseFlagsCheck(uint32_t flags, uint32_t maskRequiresAtLeastOne = 0, uint32_t maskRequiresAll = 0, uint32_t maskForbidden = 0){
    if(((flags & maskForbidden) > 0) || ((flags & maskRequiresAll) != maskRequiresAll) || (((flags & maskRequiresAtLeastOne) == 0) && (maskRequiresAtLeastOne > 0))){
      return false;
    }
    return true;
  };

};




#endif