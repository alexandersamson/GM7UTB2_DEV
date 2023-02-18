#include <stdint.h>
#include "FileSystem.h"

FileSystem::FileSystem(Constants *constantsObj, Globals *globalsObj): constants(constantsObj), globals(globalsObj){
  
}


FileSystem::ReturnStates FileSystem::begin(){
  Serial.begin(115200);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    delay(500);
    if (!(SD.begin(SDCARD_CS_PIN))) {
      delay(500);
      if (!(SD.begin(SDCARD_CS_PIN))) {
        while (1) {
        Serial.println("FileSystem::ReturnStates FileSystem::begin() >>> Unable to access the SD card after 3 tries. Please reboot");
        delay(1000);
        }        
      }
    }
  }
  readConfigFileFromSd();
  delay(10);
}



FileSystem::ReturnStates FileSystem::readConfigFileFromSd(){
  startTask();
  File file = SD.open(constants->CONFIG_FILE_NAME);
  StaticJsonDocument<JSON_DOCUMENT_CONFIG_FILE_SIZE> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, file);
  if (error){
    //Serial.println(F("Failed to read file, using default configuration"));
    return FileSystem::RETURN_ERROR_READ_SD_FILE;
  }
//GENERAL SETTINGS
  globals->config.languageId = jsonDocument["languageId"] | 0;

//GAME AND TIMER SETTINGS
  globals->config.gameTimerSet = jsonDocument["gameTimerSet"] | 3600000;
  //Serial.println(globals->config.gameTimerSet);
  
//BACKGROUND MUSIC FILES
  strlcpy(globals->config.backgroundMusicMenuFile, jsonDocument["backgroundMusicMenuFile"] | "backgroundMusicMenu.wav", sizeof(globals->config.backgroundMusicMenuFile));                     
  strlcpy(globals->config.backgroundMusicGameFile, jsonDocument["backgroundMusicGameFile"] | "backgroundMusicGame.wav", sizeof(globals->config.backgroundMusicGameFile));                     
  strlcpy(globals->config.backgroundMusicGameFileHighTension, jsonDocument["backgroundMusicGameFileHighTension"] | "backgroundMusicGameHighTension.wav", sizeof(globals->config.backgroundMusicGameFileHighTension));          
  strlcpy(globals->config.backgroundMusicWinFile, jsonDocument["backgroundMusicWinFile"] | "backgroundMusicWin.wav", sizeof(globals->config.backgroundMusicWinFile));    
  strlcpy(globals->config.backgroundMusicLossFile, jsonDocument["backgroundMusicLossFile"] | "backgroundMusicLoss.wav", sizeof(globals->config.backgroundMusicLossFile));    
  strlcpy(globals->config.backgroundMusicDrawFile, jsonDocument["backgroundMusicDrawFile"] | "backgroundMusicDraw.wav", sizeof(globals->config.backgroundMusicDrawFile));   
  strlcpy(globals->config.backgroundMusicPausedFile, jsonDocument["backgroundMusicPausedFile"] | "backgroundMusicPaused.wav", sizeof(globals->config.backgroundMusicPausedFile));   

//SOUND NOTIFICATIONS
  strlcpy(globals->config.notificationSoundGameDetonate, jsonDocument["notificationSoundGameDetonate"] | "notificationGameDisarmingDetonate.wav", sizeof(globals->config.notificationSoundGameDetonate));  
  strlcpy(globals->config.notificationSoundGameDisarmingSuccess, jsonDocument["notificationSoundGameDisarmingSuccess"] | "notificationGameDisarmingSuccess.wav", sizeof(globals->config.notificationSoundGameDisarmingSuccess));  
  strlcpy(globals->config.notificationSoundGameLost, jsonDocument["notificationSoundGameLost"] | "notificationGameLost.wav", sizeof(globals->config.notificationSoundGameLost));     
  strlcpy(globals->config.notificationSoundGameWon, jsonDocument["notificationSoundGameWon"] | "notificationGameWon.wav", sizeof(globals->config.notificationSoundGameWon));     
  strlcpy(globals->config.notificationSoundGameBonus, jsonDocument["notificationSoundGameBonus"] | "notificationGameBonus.wav", sizeof(globals->config.notificationSoundGameBonus));  
  strlcpy(globals->config.notificationSoundGamePunish, jsonDocument["notificationSoundGamePunish"] | "notificationGamePunish.wav", sizeof(globals->config.notificationSoundGamePunish));
  strlcpy(globals->config.notificationSoundModuleDisabled, jsonDocument["notificationSoundModuleDisabled"] | "notificationModuleDisabled.wav", sizeof(globals->config.notificationSoundModuleDisabled));     
  strlcpy(globals->config.notificationSoundModuleEnabled, jsonDocument["notificationSoundModuleEnabled"] | "notificationModuleEnabled.wav", sizeof(globals->config.notificationSoundModuleEnabled));     
  strlcpy(globals->config.notificationSoundModuleFailed, jsonDocument["notificationSoundModuleFailed"] | "notificationModuleFailed.wav", sizeof(globals->config.notificationSoundModuleFailed));     
  strlcpy(globals->config.notificationSoundModulePaused, jsonDocument["notificationSoundModulePaused"] | "notificationModulePaused.wav", sizeof(globals->config.notificationSoundModulePaused));     
  strlcpy(globals->config.notificationSoundModuleResumed, jsonDocument["notificationSoundModuleResumed"] | "notificationModuleResumed.wav", sizeof(globals->config.notificationSoundModuleResumed));     
  strlcpy(globals->config.notificationSoundModuleSolved, jsonDocument["notificationSoundModuleSolved"] | "notificationModuleSolved.wav", sizeof(globals->config.notificationSoundModuleSolved));     
  strlcpy(globals->config.notificationSoundTimer1MinuteRemaining, jsonDocument["notificationSoundTimer1MinuteRemaining"] | "notificationTimer1MinuteRemaining.wav", sizeof(globals->config.notificationSoundTimer1MinuteRemaining));     
  strlcpy(globals->config.notificationSoundTimer5MinutesRemaining, jsonDocument["notificationSoundTimer5MinutesRemaining"] | "notificationTimer5MinutesRemaining.wav", sizeof(globals->config.notificationSoundTimer5MinutesRemaining));     
  strlcpy(globals->config.notificationSoundTimer10MinutesRemaining, jsonDocument["notificationSoundTimer10MinutesRemaining"] | "notificationTimer10MinutesRemaining.wav", sizeof(globals->config.notificationSoundTimer10MinutesRemaining));     
  strlcpy(globals->config.notificationSoundTimer10SecondsRemaining, jsonDocument["notificationSoundTimer10SecondsRemaining"] | "notificationTimer10SecondsRemaining.wav", sizeof(globals->config.notificationSoundTimer10SecondsRemaining));     
  strlcpy(globals->config.notificationSoundTimer15MinutesRemaining, jsonDocument["notificationSoundTimer15MinutesRemaining"] | "notificationTimer15MinutesRemaining.wav", sizeof(globals->config.notificationSoundTimer15MinutesRemaining));     
  strlcpy(globals->config.notificationSoundTimer30MinutesRemaining, jsonDocument["notificationSoundTimer30MinutesRemaining"] | "notificationTimer30MinutesRemaining.wav", sizeof(globals->config.notificationSoundTimer30MinutesRemaining));     
  strlcpy(globals->config.notificationSoundTimer30SecondsRemaining, jsonDocument["notificationSoundTimer30SecondsRemaining"] | "notificationTimer30SecondsRemaining.wav", sizeof(globals->config.notificationSoundTimer30SecondsRemaining));     
  strlcpy(globals->config.notificationSoundTimer45MinutesRemaining, jsonDocument["notificationSoundTimer45MinutesRemaining"] | "notificationTimer45MinutesRemaining.wav", sizeof(globals->config.notificationSoundTimer45MinutesRemaining));     
  strlcpy(globals->config.notificationSoundTimer60MinutesRemaining, jsonDocument["notificationSoundTimer60MinutesRemaining"] | "notificationTimer60MinutesRemaining.wav", sizeof(globals->config.notificationSoundTimer60MinutesRemaining));     
  strlcpy(globals->config.notificationSoundTimer90MinutesRemaining, jsonDocument["notificationSoundTimer90MinutesRemaining"] | "notificationTimer90MinutesRemaining.wav", sizeof(globals->config.notificationSoundTimer90MinutesRemaining));     
  strlcpy(globals->config.notificationSoundTimer120MinutesRemaining, jsonDocument["notificationSoundTimer120MinutesRemaining"] | "notificationTimer120MinutesRemaining.wav", sizeof(globals->config.notificationSoundTimer120MinutesRemaining));     
  strlcpy(globals->config.notificationSoundTimer180MinutesRemaining, jsonDocument["notificationSoundTimer180MinutesRemaining"] | "notificationTimer180MinutesRemaining.wav", sizeof(globals->config.notificationSoundTimer180MinutesRemaining));     
  strlcpy(globals->config.notificationSoundTimerNormalSpeed, jsonDocument["notificationSoundTimerNormalSpeed"] | "notificationTimerNormalSpeed.wav", sizeof(globals->config.notificationSoundTimerNormalSpeed));     
  strlcpy(globals->config.notificationSoundTimerPaused, jsonDocument["notificationSoundTimerPaused"] | "notificationTimerPaused.wav", sizeof(globals->config.notificationSoundTimerPaused));     
  strlcpy(globals->config.notificationSoundTimerResumed, jsonDocument["notificationSoundTimerResumed"] | "notificationTimerResumed.wav", sizeof(globals->config.notificationSoundTimerResumed));     
  strlcpy(globals->config.notificationSoundTimerStarted, jsonDocument["notificationSoundTimerStarted"] | "notificationTimerStarted.wav", sizeof(globals->config.notificationSoundTimerStarted));     
  strlcpy(globals->config.notificationSoundUnknownCommand, jsonDocument["notificationSoundUnknownCommand"] | "notificationUnknownCommand.wav", sizeof(globals->config.notificationSoundUnknownCommand));     
  strlcpy(globals->config.notificationSoundValidationStarting, jsonDocument["notificationSoundValidationStarting"] | "notificationValidationStarting.wav", sizeof(globals->config.notificationSoundValidationStarting));     
  strlcpy(globals->config.notificationSoundValidationAlready, jsonDocument["notificationSoundValidationAlready"] | "notificationValidationAlready.wav", sizeof(globals->config.notificationSoundValidationAlready));     
  strlcpy(globals->config.notificationSoundValidationFailed, jsonDocument["notificationSoundValidationFailed"] | "notificationValidationFailed.wav", sizeof(globals->config.notificationSoundValidationFailed));           
  strlcpy(globals->config.notificationSoundValidationSuccess, jsonDocument["notificationSoundValidationSuccess"] | "notificationValidationSuccess.wav", sizeof(globals->config.notificationSoundValidationSuccess));  

  JsonArray modules = jsonDocument["moduleConfigs"].as<JsonArray>();
  uint16_t countModules = min(modules.size(), MODULES_COUNT);

  //MODULE CONFIGS
  for(int idxModule = 0; idxModule < countModules; idxModule ++ ){
    uint32_t idValue = ((uint32_t) modules[idxModule]["_id"]);
    if(idValue != idxModule){
      Serial.println("FileSystem::readConfigFileFromSd() >>> Module configs >>> Mismatch found in Module ID / Index. Please fix and reload.");
      Serial.print("FileSystem::readConfigFileFromSd() >>> Module configs >>> Expected ID: ");
      Serial.println(idxModule);
      Serial.print("FileSystem::readConfigFileFromSd() >>> Module configs >>> Got instead: ");

      Serial.println(idValue);
      globals->addMessage(ID_SYS_ERROR_CONFIG_LOAD_FAIL_MODULES, SYS_ERROR, "Failed to load modules.\nPlease check config.");
      globals->haltSystem(); //Freeze most of the system, because anything happen from this point on will only lead to chaos, confusion and probably undefined behaviour if continued. ^Alexander
    }
    globals->moduleConfigs[idxModule].progress = (modules[idxModule]["progress"]) ? (uint32_t) modules[idxModule]["progress"] : 0;
    globals->moduleConfigs[idxModule].progressMin = (modules[idxModule]["progressMin"]) ? (uint32_t) modules[idxModule]["progressMin"] : 0;
    globals->moduleConfigs[idxModule].progressMax = (modules[idxModule]["progressMax"]) ? (uint32_t) modules[idxModule]["progressMax"] : 1;
    globals->moduleConfigs[idxModule].internalTimerSet = (modules[idxModule]["internalTimerSet"]) ? (uint32_t) modules[idxModule]["internalTimerSet"] : 0;
    globals->moduleConfigs[idxModule].validationTimerSet = (modules[idxModule]["validationTimerSet"]) ? (uint32_t) modules[idxModule]["validationTimerSet"] : 1000;
    
    //DISARM STRINGS
    JsonArray disarmStrings = modules[idxModule]["disarmStrings"].as<JsonArray>();
    uint16_t countDisarmStrings = min(disarmStrings.size(), MODULE_CONFIG_MAX_COMMAND_STRINGS);
    for(int idxCmdString = 0; idxCmdString < countDisarmStrings; idxCmdString++){
      const char * disarm = disarmStrings[idxCmdString];
      strlcpy(globals->moduleConfigs[idxModule].disarmStrings[idxCmdString],
      disarmStrings[idxCmdString] | constants->emptyString,
      sizeof(globals->moduleConfigs[idxModule].disarmStrings[idxCmdString]));  
      //Serial.println(disarm);
    }
    //BONUS STRINGS
    JsonArray bonusStrings = modules[idxModule]["bonusStrings"].as<JsonArray>();
    uint16_t countBonusStrings = min(bonusStrings.size(), MODULE_CONFIG_MAX_COMMAND_STRINGS);
    for(int idxCmdString = 0; idxCmdString < countBonusStrings; idxCmdString++){
      const char * bonus = bonusStrings[idxCmdString];
      strlcpy(globals->moduleConfigs[idxModule].bonusStrings[idxCmdString],
      bonusStrings[idxCmdString] | "\0",
      sizeof(globals->moduleConfigs[idxModule].bonusStrings[idxCmdString]));  
      //Serial.println(bonus);
    }
    //PUNISH STRINGS
    JsonArray punishStrings = modules[idxModule]["punishStrings"].as<JsonArray>();
    uint16_t countPunishStrings = min(punishStrings.size(), MODULE_CONFIG_MAX_COMMAND_STRINGS);
    for(int idxCmdString = 0; idxCmdString < countPunishStrings; idxCmdString++){
      const char * punish = punishStrings[idxCmdString];
      strlcpy(globals->moduleConfigs[idxModule].punishStrings[idxCmdString],
      punishStrings[idxCmdString] | "\0",
      sizeof(globals->moduleConfigs[idxModule].punishStrings[idxCmdString]));  
      //Serial.println(punish);
    }
    //GPIO STRINGS
    JsonArray gpioStrings = modules[idxModule]["gpioStrings"].as<JsonArray>();    
    uint16_t countGpioStrings = min(gpioStrings.size(), MODULE_CONFIG_MAX_COMMAND_STRINGS);
    for(int idxCmdString = 0; idxCmdString < countGpioStrings; idxCmdString++){
      const char * gpio = gpioStrings[idxCmdString];
      strlcpy(globals->moduleConfigs[idxModule].gpioStrings[idxCmdString],
      gpioStrings[idxCmdString] | "\0",
      sizeof(globals->moduleConfigs[idxModule].gpioStrings[idxCmdString]));  
      //Serial.println(gpio);
    }
  } //END MODULE CONFIGS (FOR LOOP)
  

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
  //dumpConfigToSerial();
  endTask();
  return FileSystem::RETURN_SUCCESS;
}



FileSystem::ReturnStates FileSystem::saveConfigFileToSd(){
  startTask();
  StaticJsonDocument<JSON_DOCUMENT_CONFIG_FILE_SIZE> jsonDocument;

  //DEVICE
  jsonDocument["_deviceModel"] = constants->DEVICE_MODEL;
  jsonDocument["_deviceSerialNumber"] = constants->DEVICE_SERIAL_NUMBER;
  jsonDocument["_deviceSoftwareVersion"] = constants->DEVICE_SOFTWARE_VERSION;
  jsonDocument["_deviceProductionDate"] = constants->DEVICE_PRODUCTION_DATE;
  jsonDocument["_deviceOwner"] = constants->DEVICE_OWNER;
  //GENERAL
  jsonDocument["languageId"] = globals->config.languageId;
  //GAME
  jsonDocument["gameTimerSet"] = globals->config.gameTimerSet;
  //AUDIO
  jsonDocument["backgroundMusicMenuFile"] = globals->config.backgroundMusicMenuFile;
  jsonDocument["backgroundMusicGameFile"] = globals->config.backgroundMusicGameFile;
  jsonDocument["backgroundMusicGameFileHighTension"] = globals->config.backgroundMusicGameFileHighTension;
  jsonDocument["backgroundMusicWinFile"] = globals->config.backgroundMusicWinFile;
  jsonDocument["backgroundMusicLossFile"] = globals->config.backgroundMusicLossFile;
  jsonDocument["backgroundMusicDrawFile"] = globals->config.backgroundMusicDrawFile;
  jsonDocument["backgroundMusicPausedFile"] = globals->config.backgroundMusicPausedFile;
  jsonDocument["notificationSoundGameDetonate"] = globals->config.notificationSoundGameDetonate;
  jsonDocument["notificationSoundGameDisarmingSuccess"] = globals->config.notificationSoundGameDisarmingSuccess;
  jsonDocument["notificationSoundGameLost"] = globals->config.notificationSoundGameLost;
  jsonDocument["notificationSoundGameWon"] = globals->config.notificationSoundGameWon;
  jsonDocument["notificationSoundGameBonus"] = globals->config.notificationSoundGameBonus;
  jsonDocument["notificationSoundGamePunish"] = globals->config.notificationSoundGamePunish;
  jsonDocument["notificationSoundModuleDisabled"] = globals->config.notificationSoundModuleDisabled;
  jsonDocument["notificationSoundModuleEnabled"] = globals->config.notificationSoundModuleEnabled;
  jsonDocument["notificationSoundModuleFailed"] = globals->config.notificationSoundModuleFailed;
  jsonDocument["notificationSoundModulePaused"] = globals->config.notificationSoundModulePaused;
  jsonDocument["notificationSoundModuleResumed"] = globals->config.notificationSoundModuleResumed;
  jsonDocument["notificationSoundModuleSolved"] = globals->config.notificationSoundModuleSolved;
  jsonDocument["notificationSoundTimer1MinuteRemaining"] = globals->config.notificationSoundTimer1MinuteRemaining;
  jsonDocument["notificationSoundTimer5MinutesRemaining"] = globals->config.notificationSoundTimer5MinutesRemaining;
  jsonDocument["notificationSoundTimer10MinutesRemaining"] = globals->config.notificationSoundTimer10MinutesRemaining;
  jsonDocument["notificationSoundTimer10SecondsRemaining"] = globals->config.notificationSoundTimer10SecondsRemaining;
  jsonDocument["notificationSoundTimer15MinutesRemaining"] = globals->config.notificationSoundTimer15MinutesRemaining;
  jsonDocument["notificationSoundTimer30MinutesRemaining"] = globals->config.notificationSoundTimer30MinutesRemaining;
  jsonDocument["notificationSoundTimer30SecondsRemaining"] = globals->config.notificationSoundTimer30SecondsRemaining;
  jsonDocument["notificationSoundTimer45MinutesRemaining"] = globals->config.notificationSoundTimer45MinutesRemaining;
  jsonDocument["notificationSoundTimer60MinutesRemaining"] = globals->config.notificationSoundTimer60MinutesRemaining;
  jsonDocument["notificationSoundTimer90MinutesRemaining"] = globals->config.notificationSoundTimer90MinutesRemaining;
  jsonDocument["notificationSoundTimer120MinutesRemaining"] = globals->config.notificationSoundTimer120MinutesRemaining;
  jsonDocument["notificationSoundTimer180MinutesRemaining"] = globals->config.notificationSoundTimer180MinutesRemaining;
  jsonDocument["notificationSoundTimerNormalSpeed"] = globals->config.notificationSoundTimerNormalSpeed;
  jsonDocument["notificationSoundTimerPaused"] = globals->config.notificationSoundTimerPaused;
  jsonDocument["notificationSoundTimerResumed"] = globals->config.notificationSoundTimerResumed;
  jsonDocument["notificationSoundTimerStarted"] = globals->config.notificationSoundTimerStarted;
  jsonDocument["notificationSoundUnknownCommand"] = globals->config.notificationSoundUnknownCommand;
  jsonDocument["notificationSoundValidationStarting"] = globals->config.notificationSoundValidationStarting;
  jsonDocument["notificationSoundValidationAlready"] = globals->config.notificationSoundValidationAlready;
  jsonDocument["notificationSoundValidationFailed"] = globals->config.notificationSoundValidationFailed;
  jsonDocument["notificationSoundValidationSuccess"] = globals->config.notificationSoundValidationSuccess;
  //MODULES
  JsonArray data = jsonDocument.createNestedArray("moduleConfigs");
  JsonObject moduleDataObjects[MODULES_COUNT];
  for(int modulesCount = 0; modulesCount < MODULES_COUNT; modulesCount++){
      moduleDataObjects[modulesCount] = data.createNestedObject();
      moduleDataObjects[modulesCount]["_id"] = globals->moduleConfigs[modulesCount].id;
      moduleDataObjects[modulesCount]["_name"] = globals->moduleConfigs[modulesCount].name;
      moduleDataObjects[modulesCount]["_description"] = globals->moduleConfigs[modulesCount].description;
      moduleDataObjects[modulesCount]["progressMax"] = globals->moduleConfigs[modulesCount].progressMax;
      moduleDataObjects[modulesCount]["triesMax"] = globals->moduleConfigs[modulesCount].triesMax;
      moduleDataObjects[modulesCount]["triesResetAtMax"] = globals->moduleConfigs[modulesCount].triesResetAtMax;
      moduleDataObjects[modulesCount]["punishAfterMaxTries"] = globals->moduleConfigs[modulesCount].punishAfterMaxTries;
      moduleDataObjects[modulesCount]["resetAfterSolved"] = globals->moduleConfigs[modulesCount].resetAfterSolved;
      moduleDataObjects[modulesCount]["bonusAfterSolved"] = globals->moduleConfigs[modulesCount].bonusAfterSolved;
      moduleDataObjects[modulesCount]["validationTimerSet"] = globals->moduleConfigs[modulesCount].validationTimerSet;
      moduleDataObjects[modulesCount]["initialDigitalOutputStatesHigh"] = globals->moduleConfigs[modulesCount].initialDigitalOutputStatesHigh;
      moduleDataObjects[modulesCount]["initialDigitalOutputStatesLow"] = globals->moduleConfigs[modulesCount].initialDigitalOutputStatesLow;
      //Set the module arrays
      JsonArray disarmStrings = moduleDataObjects[modulesCount].createNestedArray("disarmStrings");
      JsonArray bonusStrings = moduleDataObjects[modulesCount].createNestedArray("bonusStrings");
      JsonArray punishStrings = moduleDataObjects[modulesCount].createNestedArray("punishStrings");
      JsonArray gpioStrings = moduleDataObjects[modulesCount].createNestedArray("gpioStrings");
      JsonArray digitalOutputSetToHighByGpioString = moduleDataObjects[modulesCount].createNestedArray("digitalOutputSetToHighByGpioString");
      JsonArray digitalOutputSetToLowByGpioString = moduleDataObjects[modulesCount].createNestedArray("digitalOutputSetToLowByGpioString");
      JsonArray digitalOutputDurationTimers = moduleDataObjects[modulesCount].createNestedArray("digitalOutputDurationTimers");
      JsonArray digitalOutputGpioStringIsReusable = moduleDataObjects[modulesCount].createNestedArray("digitalOutputGpioStringIsReusable");
      for(int i = 0; i < MODULE_CONFIG_MAX_COMMAND_STRINGS; i ++){
        disarmStrings.add(globals->moduleConfigs[modulesCount].disarmStrings[i]);
        bonusStrings.add(globals->moduleConfigs[modulesCount].bonusStrings[i]);
        punishStrings.add(globals->moduleConfigs[modulesCount].punishStrings[i]);
        gpioStrings.add(globals->moduleConfigs[modulesCount].gpioStrings[i]);
        digitalOutputSetToHighByGpioString.add(globals->moduleConfigs[modulesCount].digitalOutputSetToHighByGpioString[i]);
        digitalOutputSetToLowByGpioString.add(globals->moduleConfigs[modulesCount].digitalOutputSetToLowByGpioString[i]);
        digitalOutputDurationTimers.add(globals->moduleConfigs[modulesCount].digitalOutputDurationTimers[i]);
        digitalOutputGpioStringIsReusable.add(globals->moduleConfigs[modulesCount].digitalOutputGpioStringIsReusable[i]);
      }
  }
  //Open the config file on the SD and store the JSON Object there

  File file = SD.open(constants->CONFIG_TEMP_FILE_NAME, FILE_WRITE);
  if (!file) {
    //Serial.println(F("Failed to create file"));
    return FileSystem::RETURN_ERROR_OPEN_SD_FILE;
  }
  serializeJson(jsonDocument, Serial);
  if (serializeJson(jsonDocument, file) == 0) {
    //Serial.println(F("Failed to write to file"));
    return FileSystem::RETURN_ERROR_WRITE_SD_FILE;
  }
  file.close();
  delay(1);
  if(SD.exists(constants->CONFIG_BACKUP_FILE_NAME)){
    SD.remove(constants->CONFIG_BACKUP_FILE_NAME);
  }
  if(SD.exists(constants->CONFIG_FILE_NAME)){
    SD.rename(constants->CONFIG_FILE_NAME,constants->CONFIG_BACKUP_FILE_NAME);
  }
  if(SD.exists(constants->CONFIG_TEMP_FILE_NAME)){ 
    SD.rename(constants->CONFIG_TEMP_FILE_NAME,constants->CONFIG_FILE_NAME);
  }
  //Serial.println(F("Config saved succesfully"));
  endTask();
  return FileSystem::RETURN_SUCCESS;
}




FileSystem::ReturnStates FileSystem::dumpConfigToSerial() {
  startTask();
  // Open file for reading
  File file = SD.open(constants->CONFIG_FILE_NAME);
  if (!file){
    //Serial.println(F("Failed to read file, using default configuration"));
    return FileSystem::RETURN_ERROR_READ_SD_FILE;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  //Serial.println();

  // Close the file
  file.close();
  endTask();
  return FileSystem::RETURN_SUCCESS;
}


bool FileSystem::isDoingTask(){
  return doingTask;
}

void FileSystem::startTask(){
  doingTask = true;
}

void FileSystem::endTask(){
  doingTask = false;
}



