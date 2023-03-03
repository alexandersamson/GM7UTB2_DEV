#include "Interface.h"

// #define INTERFACE_STATUS_BOOT 1
// #define INTERFACE_STATUS_LOG_IN 2
// #define INTERFACE_STATUS_LOGGED_IN_USER 4
// #define INTERFACE_STATUS_LOGGED_IN_ADMIN 8
// #define INTERFACE_STATUS_LOGGED_IN_DEV 16
// #define INTERFACE_STATUS_LOGGED_OUT 32
// #define INTERFACE_STATUS_INGAME 64
// #define INTERFACE_STATUS_USER_MENU 128
// #define INTERFACE_STATUS_ADMIN_MENU 256
// #define INTERFACE_STATUS_DEV_MENU 512
// #define INTERFACE_STATUS_LOCKED_OUT 1024

Interface::Interface(FileSystem *fileSystemObj, Gm7Can *gm7CanObj, Image<RGB565> *tgxImageObj, Sprites *spritesObj, AudioProcessing *audioProcessingObj, Constants *constantsObj, Globals *globalsObj, ILI9341_T4::ILI9341Driver *tftObj, uint16_t *frameBufferObj) 
: fileSystem(fileSystemObj), gm7Can(gm7CanObj), tgxImage(tgxImageObj), sprites(spritesObj), audioProcessor(audioProcessingObj), constants(constantsObj), globals(globalsObj), tft(tftObj), frameBuffer(frameBufferObj){
  // Image<RGB565> *tgxImage;
  // Sprites *sprites;
  // AudioProcessing *audioProcessor;

  // //GENERAL
  // uint32_t lastReadMillis;
  // uint32_t deltaMillis;

  // //KEYS
  // bool receivedNewKeyInput;

  // //Keypad
  // bool    isEnabledKeypad;
  // uint32_t debounceTimerSet;
  // uint32_t holdButtonTimer;
  // uint32_t longpressTimerSet;
  // char buttonPressChar;
  // char buttonLongPressChar;
  // uint8_t lastPressed[2];
  // bool buttonPress;
  // bool buttonPressed;
  // bool buttonPressReleased;
  // bool buttonLongPressReleased;
  // bool buttonPressAvailable;
  // bool buttonLongPressAvailable;


}

void Interface::calculateProgressBootValueMax (){
  progressBootValueMax = 0;
  if(BOOT_ENABLE_DISPLAY) { progressBootValueMax += BOOT_PROGRESS_VALUE_DISPLAY; }
  if(BOOT_ENABLE_AUDIO) { progressBootValueMax += BOOT_PROGRESS_VALUE_AUDIO; }
  if(BOOT_ENABLE_NEOPIXEL_10) { progressBootValueMax += BOOT_PROGRESS_VALUE_NEOPIXEL_10; }
  if(BOOT_ENABLE_KEYPAD) { progressBootValueMax += BOOT_PROGRESS_VALUE_KEYPAD; }
}


//BOOT SECTION
void Interface::boot(){
  calculateProgressBootValueMax();
  for (int i = 0; i < 320 * 240; i++) frameBuffer[i] = constants->colorGuiMenuBackground[constants->getColorScheme()];
  status = INTERFACE_STATUS_BOOT;
  lastReadMillis = millis();
  tgxImage->drawText("Booting...", iVec2 {2,14}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  delay(20); 
  tft->update(frameBuffer);

  tgxImage->drawText("Loading config files...", iVec2 {2,32}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  delay(20);
  tft->update(frameBuffer);
  fileSystem->begin();
  audioProcessor->blockInterrupts();
  while(fileSystem->isDoingTask() == true){
    delay(1);
  }
  tgxImage->drawText("DONE!", iVec2 {240,32}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  delay(20);
  tft->update(frameBuffer);

  tgxImage->drawText("Loading Audio processor...", iVec2 {2,50}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  audioProcessor->allowInterupts();
  delay(20);
  tft->update(frameBuffer);
  audioProcessor->begin();
  tgxImage->drawText("DONE!", iVec2 {240,50}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  audioProcessor->playSound("startup.wav", AUDIO_CHANNEL_NOTIFICATIONS, false, false);
  delay(20);
  tft->update(frameBuffer);


  tgxImage->drawText("Loading I/O Peripherals...", iVec2 {2,68}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  delay(20);
  tft->update(frameBuffer);
  if(BOOT_ENABLE_KEYPAD){ initKeypad(); }
  tgxImage->drawText("DONE!", iVec2 {240,68}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  delay(20);  
  tft->update(frameBuffer);

  tgxImage->drawText("Starting CAN controller...", iVec2 {2,86}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  delay(20);
  tft->update(frameBuffer);
  gm7Can->begin(constants->DEVICE_UID_64, constants->DEVICE_TYPE_ID, constants->DEVICE_MODEL, constants->DEVICE_SHORT_NAME, constants->DEVICE_VENDOR);
  tgxImage->drawText("DONE!", iVec2 {240,86}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  delay(20);  
  tft->update(frameBuffer);

  tgxImage->drawText("Logging in...", iVec2 {2,104}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  delay(20);  
  tft->update(frameBuffer);
  _statusChangeLogInUser(); //TODO: Make proper login functionality
  tgxImage->drawText("DONE!", iVec2 {240,104}, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
  delay(20);
  tft->update(frameBuffer);
  delay(400);
  audioProcessor->playBackgroundMusic(globals->config.backgroundMusicMenuFile); //Finally start the background music
}


//BOOT - INIT KEYPAD
bool Interface::initKeypad(uint8_t r1Pin, uint8_t r2Pin, uint8_t r3Pin, uint8_t r4Pin, uint8_t c1Pin, uint8_t c2Pin, uint8_t c3Pin){
  debounceTimerSet = 30; //millis debounce timer
  holdButtonTimer = 0;
  longpressTimerSet = 500;
  buttonPressChar = '\0';
  buttonPressed = false;
  buttonPressReleased = true;
  buttonLongPressReleased = true;
  buttonPressAvailable = false;
  buttonLongPressAvailable = false;
  lastPressed[0] = 0;
  lastPressed[1] = 0;
  pinMode(37, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(39, INPUT_PULLDOWN);
  pinMode(41, INPUT_PULLDOWN);
  pinMode(40, INPUT_PULLDOWN);
  pinMode(38, INPUT_PULLDOWN);
  delay(5);
  progressBootValueCurrent += BOOT_PROGRESS_VALUE_KEYPAD;
  setIsEnabledKeypad(true);
  return true;
}

//GENERAL METHODS
void Interface::updateDeltaMillis(){
  uint32_t curMillis = millis();
  deltaMillis = curMillis - lastReadMillis;
  lastReadMillis = curMillis;
}

uint32_t Interface::getDeltaMillis(){
  return deltaMillis;
}


//INPUT CONSUMER SELECTORS
void Interface::setSelectedInputConsumer(uint16_t newSelection){
  selectedInputConsumer = newSelection;
}

uint16_t Interface::getSelectedInputConsumer(){
  return selectedInputConsumer;
}

bool Interface::getSelectedInputConsumerIsGame(){
  return (selectedInputConsumer == INPUT_CONSUMER_GAME);
}

bool Interface::getSelectedInputConsumerIsGui(){
  return (selectedInputConsumer == INPUT_CONSUMER_GUI);
}


//DATA PACKAGE IO
bool Interface::inputCommonDataPackage(Globals::CommonInternalDataPackage receivedDataPackage){
  if(receivedDataPackage.command == constants->COMMAND_CHANGE_GAME_STATUS_REQUEST){
    if(requestStartGame() == false){
      return false;
    }
    setSelectedInputConsumer(INPUT_CONSUMER_GAME);
    return true;
  }
  return false;
}


//KEYS BUFFER
void Interface::addKeyToKeysReceivedBuffer(char key){
  if(key == 'R'){
    audioProcessor->blockInterrupts();
    delay(5);
    fileSystem->saveConfigFileToSd();
    delay(5);
    audioProcessor->allowInterupts(); 
    Serial.println("Save complete");  
    return; 
  }
  if(key == 'T'){
    globals->addMessage(0, 0, "Chickens");
  }
  for (int i = KEYS_RECEIVED_BUFFER_SIZE-2; i >= 0 ; i--){
      keysReceivedBuffer[i+1] = keysReceivedBuffer[i];
  }
  keysReceivedBuffer[0] = key;
}

char * Interface::getKeysReceivedBuffer(){
  return (char *) keysReceivedBuffer;
}

char Interface::getLastReceivedKey(){
  return (char) keysReceivedBuffer[0];
}

char Interface::consumeLastReceivedKey(){
  if(receivedNewKeyInput){
    receivedNewKeyInput = false;
    return (char) keysReceivedBuffer[0];
  }
  return '\0';
}

void Interface::setReceivedNewKeyInput(bool val = true){
  receivedNewKeyInput = val;
}

bool Interface::getReceivedNewKeyInput(){
  receivedNewKeyInput;
}

bool Interface::isNumeric(char key){
  if ((key > 47) && (key < 58)){
    return true;
  }
  return false;
}


//KEYPAD

void Interface::resetKeypadPress(){
    buttonPressReleased = true;
    buttonLongPressReleased = true;
    buttonPressAvailable = false;
    buttonLongPressAvailable = false;
    holdButtonTimer = 0;
}


void Interface::readKeypadPress(){
//(uint8_t r1Pin = 39, uint8_t r2Pin = 41, uint8_t r3Pin = 40, uint8_t r4Pin = 38, uint8_t c1Pin = 37, uint8_t c2Pin = 35, uint8_t c3Pin = 36);
  uint8_t cols[3] = { 37, 35, 36 };
  uint8_t rows[4] = { 39, 41, 40, 38 };
  uint8_t pressed[2] = {0, 0};
  buttonPressed = false;
  for(int i=0; i<3; i++){
    for(int j=0; j<3; j++){
      if(j != i){
        digitalWriteFast(cols[j], LOW);
      }
    }
    digitalWriteFast(cols[i], HIGH);
    delayMicroseconds(10);
    //Serial.print(digitalRead(cols[0]));
    //Serial.print(digitalRead(cols[1]));
    //Serial.println(digitalRead(cols[2]));
    for(int k=0; k<4; k++){
      if(digitalRead(rows[k]) == HIGH){
        pressed[0] = k; //row
        pressed[1] = i; //col
        buttonPressed = true;
        //Serial.println(keypadMatrix[pressed[0]][pressed[1]]);
        break;
      }
    }
  }
  if(buttonPressed == false){
    resetKeypadPress();
    return;
  }
  if((lastPressed[0] != pressed[0] || lastPressed[1] != pressed[1])){
    resetKeypadPress();
    lastPressed[0] = pressed[0];
    lastPressed[1] = pressed[1];
  }
  if((holdButtonTimer >= debounceTimerSet) && ( buttonPressReleased == true)) {
    buttonPressReleased = false;
    buttonPressAvailable = true;
    buttonPressChar = keypadMatrix[pressed[0]][pressed[1]];
  } else if((holdButtonTimer >= longpressTimerSet)  && ( buttonLongPressReleased == true)) {
    buttonLongPressReleased = false;
    buttonLongPressAvailable = true;
    buttonPressChar = keypadMatrixShift[pressed[0]][pressed[1]];
  } else if(buttonPressReleased == true || buttonLongPressReleased == true){
    holdButtonTimer += deltaMillis;
  } 
}


bool Interface::getIsEnabledKeypad(){
  return isEnabledKeypad;
}

void Interface::setIsEnabledKeypad(bool isEnabled){
  isEnabledKeypad = isEnabled;
}

void Interface::readKeypad(){
  readKeypadPress();
  if(buttonPressAvailable || buttonLongPressAvailable){
    if(buttonPressAvailable){
      buttonPressAvailable = false;      
    }
    if(buttonLongPressAvailable){
      buttonLongPressAvailable = false;      
    }
    addKeyToKeysReceivedBuffer(buttonPressChar);
    setReceivedNewKeyInput();
  }
}

void Interface::processSerialRxChar(char chr){
  if(chr == 13){chr = constants->keypadHash;}
  if(chr == 8){chr = constants->keypadStar;}
  if(chr == 87){chr = constants->keypadUp;}
  if(chr == 65){chr = constants->keypadLeft;}
  if(chr == 83){chr = constants->keypadDown;}
  if(chr == 68){chr = constants->keypadRight;}
    addKeyToKeysReceivedBuffer(chr);
    setReceivedNewKeyInput();  
}


//KEY STATE CHANGES
void Interface::pushGameGuiKeysStates(bool stateCaptured, bool showConfirm, bool showBack){
  guiKeyStates.backKeyIsCaptured = stateCaptured;
  guiKeyStates.confirmKeyIsShown = showConfirm;
  guiKeyStates.backKeyIsShown = showBack;
}

void Interface::pushGameGuiKeysStrings(const char * confirmChar, const char * backChar){
  customConfirmKeyStringPtr = confirmChar;
  customBackKeyStringPtr = backChar;
}

bool Interface::getKeyStateBackIsCaptured(){
  return guiKeyStates.backKeyIsCaptured;
}
bool Interface::getKeyStateBackIsShown(){
  return guiKeyStates.backKeyIsShown;
}
bool Interface::getKeyStateConfirmIsShown(){
  return guiKeyStates.confirmKeyIsShown;
}
const char * Interface::getKeyStateBackString(){
  return customBackKeyStringPtr;
}
const char * Interface::getKeyStateConfirmString(){
  return customConfirmKeyStringPtr;
}

uint16_t Interface::getActiveGameModule(){
  return activeGameModule;
}

void Interface::setActiveGameModule(uint16_t newActiveModule){
  activeGameModule = newActiveModule;
}


///////////////////
// CAN SECTION
//////////////////

//START parseCanRxBufferStorage()
void Interface::parseCanRxBufferStorage(){
  if(gm7Can->canBufferStorageCountAvailable[CAN_BUFFER_STORAGE_INDEX_RX] == 0){
    return; //nothing to do
  }
  uint8_t pos = 0;
  while(gm7Can->canBufferStorageCountAvailable[CAN_BUFFER_STORAGE_INDEX_RX]){
    Gm7Can::CanBuffer canBuffer = gm7Can->readFromCanBufferStorage(CAN_BUFFER_STORAGE_INDEX_RX);
    Serial.println(canBuffer.pmid, DEC);  
    Serial.println(canBuffer.uid, DEC);  
    if(canBuffer.pmid == gm7Can->canProtocol.MODULE_STATUS_AND_PROGRESS){
      globals->addMessage(0,0,"MODULE STATUS CHANGED", false, 2000);  
    }
    if((canBuffer.pmid > gm7Can->canProtocol.DEVICE_SECTION_START) && (canBuffer.pmid < gm7Can->canProtocol.DEVICE_SECTION_END)){// Generic device data received
      processReceivedDeviceData(canBuffer.uid, canBuffer.pmid, canBuffer.payload);
    }
    if(((canBuffer.pmid > gm7Can->canProtocol.DEVICE_TYPE_SECTION_START) && (canBuffer.pmid < gm7Can->canProtocol.DEVICE_TYPE_SECTION_END)) || (canBuffer.pmid == gm7Can->canProtocol.DEVICE_REGISTRATION_REQUEST)){ //Registrationrequest received
      processDeviceRegistrationRequest(canBuffer.uid, canBuffer.pmid, canBuffer.payload);
    }
    if((canBuffer.pmid > gm7Can->canProtocol.MODULE_SECTION_START) && (canBuffer.pmid < gm7Can->canProtocol.MODULE_SECTION_END)){ //Specific module data received
      processReceivedCanModuleData(canBuffer.uid, canBuffer.pmid, canBuffer.payload);
    }
  }

} //END parseCanRxBufferStorage()

//Sends the game main timer over can to the network. It sends the current time and the set time, both in uint32 milliseconds formats.
void Interface::sendMainTimerOverCan(uint32_t timerCurrent, uint32_t timerSet){
  gm7Can->sendMainTimer(timerCurrent, timerSet);
}

//Sends the game validation timer over can to the network. It sends the current time and the set time, both in uint32 milliseconds formats.
void Interface::sendValidationTimerOverCan(uint32_t timerCurrent, uint32_t timerSet){
  gm7Can->sendValidationTimer(timerCurrent, timerSet);
}



void Interface::processDeviceRegistrationRequest(uint16_t uid, uint16_t pmid, char * payload){
  
  if(((pmid >= gm7Can->canProtocol.DEVICE_TYPE_SECTION_END) || (pmid <= gm7Can->canProtocol.DEVICE_TYPE_SECTION_START)) && (pmid != gm7Can->canProtocol.DEVICE_REGISTRATION_REQUEST)){
    return;
  }
  if(pmid == gm7Can->canProtocol.DEVICE_TYPE_MODULE_GAME_MODULE){ //When it's a game module
    for(int i = 0; i < EXTERNAL_MODULES_COUNT_MAX; i++){
      if(globals->externalGameModuleData[i].isRegistered == true && globals->externalGameModuleData[i].canUid == uid){
        //It's already registered and found, just turn it online.
        //TODO: IMPLEMENT THIS
        Serial.print("Existing registration. RegistrationRequest from: "); Serial.println(uid);
        return;
      }
    }
    //2. When no existing module has been found, register a new one if there's a place available in the modules buffer (#define in constants >> MODULES_COUNT).
    for(int i = 0; i < EXTERNAL_MODULES_COUNT_MAX; i++){
      if(globals->externalGameModuleData[i].isRegistered == false){
        //There's an unassigned external module location available in the buffer. We can assign it to this module.
        globals->externalGameModuleData[i].isRegistered = true; //register it
        globals->externalGameModuleData[i].canUid = uid;
        if((pmid > gm7Can->canProtocol.DEVICE_TYPE_SECTION_START) && (pmid < gm7Can->canProtocol.DEVICE_TYPE_SECTION_END)){
          //If the PMID is within the range of the preset DEVICE TYPES (Gm7CanProtocol.h), we can assume its device type ID is just the PMID
          globals->externalGameModuleData[i].deviceTypeId = pmid;
        } else {
          //In any other case we need to extract the Device Type Id from the payload
          globals->externalGameModuleData[i].deviceTypeId = gm7Can->canProtocol.extractUint16FromBuffer(payload, Gm7CanProtocol::DEFAULT_CAN_MESSAGE_LENGTH_MAX, 0);
        }
        Serial.print("New registration. RegistrationRequest from: "); Serial.println(uid);
        return;
      }
    }
  }

} //END of processDeviceRegistrationRequest()


void Interface::processReceivedDeviceData(uint16_t uid, uint16_t pmid, char * payload){
  for(int i = 0; i < EXTERNAL_MODULES_COUNT_MAX; i++){
    if((globals->externalGameModuleData[i].canUid == uid) && (globals->externalGameModuleData[i].isRegistered == true)){
      if(pmid == gm7Can->canProtocol.DEVICE_SERIAL){ globals->externalGameModuleData[i].uid64 = gm7Can->canProtocol.extractUint64FromBuffer(payload, Gm7CanProtocol::DEFAULT_CAN_MESSAGE_LENGTH_MAX, 0);} //UID64 (Serial)
      if(pmid == gm7Can->canProtocol.DEVICE_TYPE_ID){ globals->externalGameModuleData[i].deviceTypeId = gm7Can->canProtocol.extractDeviceTypeIdFromBuffer(payload, Gm7CanProtocol::DEFAULT_CAN_MESSAGE_LENGTH_MAX);} //Device Type Id (uint16)
      if(pmid == gm7Can->canProtocol.DEVICE_MODEL){ strlcpy(globals->externalGameModuleData[i].deviceModel, payload, Gm7CanProtocol::DEFAULT_CAN_MESSAGE_LENGTH_MAX);}//Model
      if(pmid == gm7Can->canProtocol.DEVICE_VENDOR){ strlcpy(globals->externalGameModuleData[i].deviceVendor, payload, Gm7CanProtocol::DEFAULT_CAN_MESSAGE_LENGTH_MAX);}//Vendor
      if(pmid == gm7Can->canProtocol.DEVICE_SHORT_NAME){ strlcpy(globals->externalGameModuleData[i].deviceShortName, payload, Gm7CanProtocol::DEFAULT_CAN_MESSAGE_LENGTH_MAX);}//Short name
      // Serial.print("UIDCAN: "); Serial.println(globals->externalGameModuleData[i].canUid);
      // Serial.print("UID_64: "); Serial.println((uint32_t)globals->externalGameModuleData[i].uid64);  
      // Serial.print("DEVTID: "); Serial.println(globals->externalGameModuleData[i].deviceTypeId);  
      // Serial.print("MODEL : "); Serial.println(globals->externalGameModuleData[i].deviceModel);  
      // Serial.print("VENDOR: "); Serial.println(globals->externalGameModuleData[i].deviceVendor);  
      // Serial.print("S.NAME: "); Serial.println(globals->externalGameModuleData[i].deviceShortName);        
    }
  }

}


void Interface::processReceivedCanModuleData(uint16_t uid, uint16_t pmid, char * payload){
  for(int i = 0; i < EXTERNAL_MODULES_COUNT_MAX; i++){
    if((globals->externalGameModuleData[i].canUid == uid) && (globals->externalGameModuleData[i].isRegistered == true)){
      if(pmid == gm7Can->canProtocol.MODULE_STATUS_AND_PROGRESS){
        Gm7CanProtocol::StatusAndProgress statusAndProgress = gm7Can->canProtocol.decodeModuleStatusAndProgress(payload, Gm7CanProtocol::DEFAULT_CAN_MESSAGE_LENGTH_MAX);
        globals->externalGameModuleData[i].status = statusAndProgress.status;
        globals->externalGameModuleData[i].progress = statusAndProgress.progress;
        globals->externalGameModuleData[i].progressMax = statusAndProgress.progressMax;
        Serial.print("STATUS : "); Serial.println(globals->externalGameModuleData[i].status);  
        Serial.print("PROGRS: ");  Serial.println(globals->externalGameModuleData[i].progress);  
        Serial.print("PROGMX: ");  Serial.println(globals->externalGameModuleData[i].progressMax);   
      }
    }
  }
}




//////////////////
// AUDIO SECTION
//////////////////

//AUDIO METHODS
void Interface::playBeep(){
  audioProcessor->playSound(constants->audioMemBeep, audioProcessor->getMemBeepsChannel(), 1, 0);
}

void Interface::playBlip(){
  audioProcessor->playSound(constants->audioMemBlip, audioProcessor->getMemBeepsChannel(), 1, 0);
}

void Interface::playPing(){
  audioProcessor->playSound(constants->audioMemPing, audioProcessor->getMemBeepsChannel(), 1, 0);
}

void Interface::playPong(){
  audioProcessor->playSound(constants->audioMemPong, audioProcessor->getMemBeepsChannel(), 1, 0);
}

void Interface::playTick(){
  audioProcessor->playSound(constants->audioMemTick, audioProcessor->getMemBeepsChannel(), 1, 0);
}

void Interface::playTickSoft(){
  audioProcessor->playSound(constants->audioMemTickSoft, audioProcessor->getMemBeepsChannel(), 1, 0);
}

void Interface::playConfirm(){
  audioProcessor->playSound(constants->audioMemConfirm, audioProcessor->getMemBeepsChannel(), 1, 0);
}

void Interface::playError(){
  audioProcessor->playSound(constants->audioMemError, audioProcessor->getMemBeepsChannel(), 1, 0);
}

void Interface::playBlocked(){
  audioProcessor->playSound(constants->audioMemBlocked, audioProcessor->getMemBeepsChannel(), 1, 0);
}

void Interface::playCancel(){
  audioProcessor->playSound(constants->audioMemCancel, audioProcessor->getMemBeepsChannel(), 1, 0);
}


void Interface::playNotificationValidationBonus(){
  audioProcessor->playSound(globals->config.notificationSoundGameBonus, audioProcessor->getNotificationsChannel(), 1, 0);
}

void Interface::playNotificationValidationPunish(){
  audioProcessor->playSound(globals->config.notificationSoundGamePunish, audioProcessor->getNotificationsChannel(), 1, 0);
}

void Interface::playNotificationValidationFailed(){
  audioProcessor->playSound(globals->config.notificationSoundValidationFailed, audioProcessor->getNotificationsChannel(), 1, 0);
}

void Interface::playNotificationValidationAlreadyUsed(){
  audioProcessor->playSound(globals->config.notificationSoundValidationAlready, audioProcessor->getNotificationsChannel(), 1, 0);
}

void Interface::playNotificationValidationSuccess(){
  audioProcessor->playSound(globals->config.notificationSoundValidationSuccess, audioProcessor->getNotificationsChannel(), 1, 0);
}

void Interface::playNotificationModuleSolved(){
  audioProcessor->playSound(globals->config.notificationSoundModuleSolved, audioProcessor->getNotificationsChannel(), 1, 0);
}


//TIMER BEEPS. Normal, Elevated and Final
void Interface::playTimerTickNormal(){
  audioProcessor->playSound(constants->audioMemTickSoft, audioProcessor->getMemBeepsChannel(), 1, 0);
}
void Interface::playTimerTickElevated(){
  audioProcessor->playSound(constants->audioMemBlip, audioProcessor->getMemBeepsChannel(), 1, 0);
}
void Interface::playTimerTickFinal(){
  audioProcessor->playSound(constants->audioMemBeep, audioProcessor->getMemBeepsChannel(), 1, 0);
}




void Interface::lowerBackgroundMusic(){
  audioProcessor->lowerGainBackgroundMusic();
}
void Interface::normalizeBackgroundMusic(){
  audioProcessor->resetGainBackgroundMusic();
}
void Interface::playNotificationDecoding(){
  audioProcessor->playSound(globals->config.notificationSoundValidationStarting, audioProcessor->getNotificationsChannel(), 1, 0);
}
void Interface::stopNotificationDecoding(){
  audioProcessor->stopIfThisFileIsPlaying(globals->config.notificationSoundValidationStarting, audioProcessor->getNotificationsChannel());
}

void Interface::playNotificationWrongCode(){
  playError();
  audioProcessor->playSound(globals->config.notificationSoundValidationFailed, audioProcessor->getNotificationsChannel(), 1, 0);
}

void Interface::playIntesifiedBackgroundMusic(){
  audioProcessor->interruptBackgroundMusic(globals->config.backgroundMusicGameFileHighTension);
}

void Interface::stopIntesifiedBackgroundMusicAndPlayNormalBackgroundMusic(){
  audioProcessor->resumeFromInterruptedBackgroundMusic();
}

bool Interface::getIsPlayingIntensifiedBackgroundMusic(){
  return audioProcessor->getIsPlayingIntensifiedBackgroundMusic();
}

void Interface::blockAudioInterrupts(){
    audioProcessor->blockInterrupts();
}

void Interface::allowAudioInterrupts(){
    audioProcessor->allowInterupts();
}


void Interface::makeGameStateRequest(uint32_t requestedState){
  requestedGameState = requestedState;
  newGameStateRequest = true;
}

bool Interface::getIsNewGameStateRequestAvailable(){
  return newGameStateRequest;
}

uint32_t Interface::consumeGameStateRequest(){
  if(newGameStateRequest == false){
    return 0;
  }
  newGameStateRequest = false;
  return requestedGameState;  
}


bool Interface::requestStartGame(){
  if(requestStatusChangeToIngame()){
    makeGameStateRequest(GAME_STATE_STARTED);
    audioProcessor->playBackgroundMusic(globals->config.backgroundMusicGameFile);
    return true;
  }
  return false;
}



void Interface::_statusChangeLogOut(){
  _statusChangeExitAllMenus();
  _statusChangeExitAllGames();
  uint32_t bitMask = (uint32_t)~(INTERFACE_STATUS_LOGGED_IN_USER + INTERFACE_STATUS_LOGGED_IN_ADMIN + INTERFACE_STATUS_LOGGED_IN_DEV);
  status = (status & bitMask);
  bitMask = (uint32_t)(INTERFACE_STATUS_LOG_IN);
  status = (status & bitMask);
}

void Interface::_statusChangeLogInUser(){
  uint32_t bitMask = (uint32_t)(INTERFACE_STATUS_LOGGED_IN_USER);
  status = (status | bitMask);
  globals->addMessage(0, 0, constants->labelLoggedInAsUser[globals->getLang()], true, 1000);
}

void Interface::_statusChangeLogInAdmin(){
  uint32_t bitMask = (uint32_t)(INTERFACE_STATUS_LOGGED_IN_ADMIN);
  status = (status | bitMask);
  globals->addMessage(0, 0, constants->labelLoggedInAsAdmin[globals->getLang()], true, 1000);
}

void Interface::_statusChangeLogInDev(){
  uint32_t bitMask = (uint32_t)(INTERFACE_STATUS_LOGGED_IN_DEV);
  status = (status | bitMask);
  globals->addMessage(0, 0, constants->labelLoggedInAsDev[globals->getLang()], true, 1000);
}

void Interface::_statusChangeExitAllMenus(){
  uint32_t bitMask = (uint32_t)~(INTERFACE_STATUS_LOG_IN + INTERFACE_STATUS_USER_MENU + INTERFACE_STATUS_ADMIN_MENU + INTERFACE_STATUS_DEV_MENU);
  status = (status & bitMask);
}

void Interface::_statusChangeExitAllGames(){
  uint32_t bitMask = (uint32_t)~(INTERFACE_STATUS_INGAME);
  status = (status & bitMask);
}

void Interface::_statusChangeEnterUserMenu(){
  _statusChangeExitAllMenus();
  _statusChangeExitAllGames();
  uint32_t bitMask = (uint32_t)(INTERFACE_STATUS_USER_MENU);
  status = (status | bitMask);
}

void Interface::_statusChangeEnterAdminMenu(){
  _statusChangeExitAllMenus();
  _statusChangeExitAllGames();
  uint32_t bitMask = (uint32_t)(INTERFACE_STATUS_ADMIN_MENU);
  status = (status | bitMask);
}

void Interface::_statusChangeEnterDevMenu(){
  _statusChangeExitAllMenus();
  _statusChangeExitAllGames();
  uint32_t bitMask = (uint32_t)(INTERFACE_STATUS_DEV_MENU);
  status = (status | bitMask);
}

void Interface::_statusChangeEnterGame(){
  _statusChangeExitAllMenus();
  _statusChangeExitAllGames();
  uint32_t bitMask = (uint32_t)(INTERFACE_STATUS_INGAME);
  status = (status | bitMask);
  newGameStateRequest = true;
  uint16_t requestedGameState;
}


//INTERFACE IO STATUS CHANGE REQUESTS
bool Interface::requestStatusChangeToIngame(){
  if(hasAccessAsUser()){
    _statusChangeEnterGame();
    return true;
  }
  return false;
}

bool Interface::requestStatusChangeToUserMenu(){
  if(hasAccessAsUser()){
    _statusChangeEnterUserMenu();
    return true;
  }
  return false;
}

bool Interface::getStatusIsIngame(){
  return ((status & INTERFACE_STATUS_INGAME) > 0);
}

bool Interface::getStatusIsInUserMenu(){
  return ((status & INTERFACE_STATUS_USER_MENU) > 0);
}

bool Interface::getStatusIsInAdminMenu(){
  return ((status & INTERFACE_STATUS_ADMIN_MENU) > 0);
}

bool Interface::getStatusIsInDevMenu(){
  return ((status & INTERFACE_STATUS_DEV_MENU) > 0);
}

//Get access levels
bool Interface::hasAccessAsUser(){
  return ((status & (INTERFACE_STATUS_LOGGED_IN_USER + INTERFACE_STATUS_LOGGED_IN_ADMIN + INTERFACE_STATUS_LOGGED_IN_DEV)) > 0);
}

bool Interface::hasAccessAsAdmin(){
  return ((status & (INTERFACE_STATUS_LOGGED_IN_ADMIN + INTERFACE_STATUS_LOGGED_IN_DEV)) > 0);
}

bool Interface::hasAccessAsDev(){
  return ((status & INTERFACE_STATUS_LOGGED_IN_DEV) > 0);
}

uint16_t Interface::getCurrentAccessLevel(){
  uint16_t level = 0;
  if(hasAccessAsUser()){ level += 1; }
  if(hasAccessAsAdmin()){ level += 2; }
  if(hasAccessAsDev()){ level += 4; }
  return level;
}



//LOOP INTERFACE
void Interface::loop(){
  updateDeltaMillis();
  gm7Can->updateDeviceGameStatusAndProgress(globals->status, globals->progress, globals->progressMax);
  if(getIsEnabledKeypad()){
      readKeypad();
      parseCanRxBufferStorage();
  }
}
