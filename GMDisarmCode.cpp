#include <stdint.h>
#include "GMDisarmCode.h"


// char gameStatusString[9][MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char disarmStringsSet[GAME_MAX_DISARM_CODES][MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char disarmStringsUsed[GAME_MAX_DISARM_CODES][MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char enteredStringBuffer[MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char bonusStringsSet[GAME_MAX_BONUS_CODES][MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char bonusStringsUsed[GAME_MAX_BONUS_CODES][MODULE_CONFIG_STRING_MAX_LENGTH + 1];


GMDisarmCode::GMDisarmCode(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Interface *interfaceObj, GameTimer *internalTimerObj, GameTimer *validationTimerObj, Constants *constantsObj, Globals *globalsObj)
  : tgxImage(tgxImageObj), sprites(spritesObj), interface(interfaceObj), internalTimer(internalTimerObj), validationTimer(validationTimerObj), constants(constantsObj), globals(globalsObj) {
  setPosition({ 0, 100, 320, 80 });
  GameModule::setCustomConfirmKeyString(constants->labelConfirm[globals->getLang()]);
  GameModule::setCustomBackKeyString(constants->labelBackspace[globals->getLang()]);
}



void GMDisarmCode::reset() {
  if(GameModule::getHasAssignedInternalId() == false){
    Serial.print("GMDisarmCode::reset() >> Trying to set a module name without any assigned internal ID. Please make sure an internal ID has been assigned to this module.");
      return;
  }
  loadModuleConfig();
  setState(GAME_MODULE_STATE_INACTIVE);
  GameModule::setStateAddFlagSolvable(); //This is a solvable module
  consumedSolvedTrigger = false;
  makeVisible();
  setProgress(0);
  internalTimer->reset(internalTimerSet);
  validationTimer->reset(validationTimerSet);
}



void GMDisarmCode::loadModuleConfig(){
  if(GameModule::getHasAssignedInternalId() == false){
    Serial.print("GMDisarmCode::loadModuleConfig() >> Trying to set a module name without any assigned internal ID. Please make sure an internal ID has been assigned to this module.");
      return;
  }
  setName();
  setDescription();
  progress = globals->moduleConfigs[internalId].progress;
  progressMin = globals->moduleConfigs[internalId].progressMin;
  progressMax = globals->moduleConfigs[internalId].progressMax;
  internalTimerSet = globals->moduleConfigs[internalId].internalTimerSet; 
  validationTimerSet = globals->moduleConfigs[internalId].validationTimerSet; 

  for (int i = 0; i< MODULE_CONFIG_MAX_COMMAND_STRINGS; i++){
     disarmStringsSet[i] = globals->moduleConfigs[internalId].disarmStrings[i];
     bonusStringsSet[i] = globals->moduleConfigs[internalId].bonusStrings[i];
     punishStringsSet[i] = globals->moduleConfigs[internalId].punishStrings[i];
     gpioStringsSet[i] = globals->moduleConfigs[internalId].gpioStrings[i];
  }
}


void GMDisarmCode::activate() {
  setState(GAME_MODULE_STATE_ACTIVE);
  GameModule::setStateAddFlagSolvable(); //This is a solvable module
}

void GMDisarmCode::deactivate() {
  setState(GAME_MODULE_STATE_INACTIVE);
  GameModule::releaseBackKey();
}

void GMDisarmCode::enable() {
  enabled = true;
}

//Disabled modules are deactivated, but also do not count towards win/loss conditions
void GMDisarmCode::disable() {
  setState(GAME_MODULE_STATE_DISABLED);
  GameModule::releaseBackKey();
}

//WHEN A GAME ENDS
void GMDisarmCode::postGameProcess(){
  if(GameModule::getStateIsConsideredToStayUnchangedPostGame() == false){
    GameModule::setStateAddFlagInactive();
  }
  _abortValidation();
  updateGuiKeyStates();
}

//WHEN SWITCHING TO ANOTHER MODULE
void GMDisarmCode::switchAwayToAnotherModule(){
  _abortValidation();
  updateGuiKeyStates();
}


const char *GMDisarmCode::getName() {
  return name;
}

void GMDisarmCode::setName(char * newName) {
  if(GameModule::getHasAssignedInternalId() == false){
    Serial.print("GMDisarmCode::setName(char * newName) >> Trying to set a module name without any assigned internal ID. Please make sure an internal ID has been assigned to this module.");
      return;
  }
  name = (newName == NULL) ? moduleName[globals->getLang()] : newName;
  strlcpy(globals->moduleConfigs[internalId].name, name, sizeof(globals->moduleConfigs[internalId].name));   
}


const char *GMDisarmCode::getDescription() {
  return description;
}

void GMDisarmCode::setDescription(char * newDescription) {
  if(GameModule::getHasAssignedInternalId() == false){
    Serial.print("GMDisarmCode::setDescription(char * newDescription) >> Trying to set a module name without any assigned internal ID. Please make sure an internal ID has been assigned to this module.");
      return;
  }
  description = (newDescription == NULL) ? moduleDescription[globals->getLang()] : newDescription;
  strlcpy(globals->moduleConfigs[internalId].description, description, sizeof(globals->moduleConfigs[internalId].description));   
}

bool GMDisarmCode::isVisible() {
  return visible;
}

void GMDisarmCode::makeVisible() {
  visible = true;
}

GMDisarmCode::Position GMDisarmCode::getPosition() {
  return position;
}

void GMDisarmCode::setPosition(Position pos) {
  position.height = pos.height;
  position.width = pos.width;
  position.posX = pos.posX;
  position.posY = pos.posY;
}

void GMDisarmCode::setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  position.height = h;
  position.width = w;
  position.posX = x;
  position.posY = y;
}

uint32_t GMDisarmCode::getState() {
  return state;
}

void GMDisarmCode::setState(uint32_t st) {
  state = st;
}

uint32_t GMDisarmCode::getProgress() {
  return progress;
}

void GMDisarmCode::setProgress(uint32_t prg) {
  GameModule::setProgressInternal(prg);
}


void GMDisarmCode::inputKeyboard(char key) {
  if ((GameModule::getStateIsValidating() == true) && key == constants->keypadStar) {  //Reset an ongoing validation process
    _abortValidation();
    interface->playCancel();
  }
  if (GameModule::getStateIsActivatedAndNotValidating() == false) {  //When there's not a non-validating active module state
    return;
  }
  uint16_t pos = _getEnteredStringBufferPosition();
  //When a global deletion char is received
  if (key == '<') {
    enteredStringBuffer[0] = constants->emptyChar;  //Set the first byte to empty string
    interface->playBlip();
    return;
  }
  //When a deletion char is received
  if (key == constants->keypadStar) {
    if (pos > 0) {
      enteredStringBuffer[pos - 1] = constants->emptyChar;  //Delete the char
      interface->playTickSoft();
    }
    return;
  }
  //When there's at least 1 normal number in the buffer and '#' is pressed
  if (key == constants->keypadHash) {
    if (pos > 0) {
      //Start confirming entered code!
      interface->playConfirm();
      GameModule::setStateAddFlagValidating();
      if (validationTimerSet < 1) {
        return;
      }
      validationTimer->start();
      interface->playNotificationDecoding();
      interface->lowerBackgroundMusic();
    }
    return;
  }
  //When the \0 is at the end of the buffer (no more space in the buffer)
  if (pos >= (MODULE_CONFIG_STRING_MAX_LENGTH - 1)) {
    interface->playBlocked();
    return;
  }
  if (!interface->isNumeric(key)) {
    return;
  }
  interface->playPong();
  enteredStringBuffer[pos] = key;
  enteredStringBuffer[pos + 1] = constants->emptyChar;
}



void GMDisarmCode::inputInteger(uint32_t) {
}

void GMDisarmCode::setGameModifierOutput(uint16_t newValue) {
  gameModifierOutput = newValue;
}


bool GMDisarmCode::isAvailableGameModifierOutput() {
  if (gameModifierOutput != GAME_MODULE_OUTPUT_NORMAL) {
    return true;
  }
  return false;
}


uint16_t GMDisarmCode::consumeGameModifierOutput() {
  uint16_t gameMO = gameModifierOutput;
  gameModifierOutput = GAME_MODULE_OUTPUT_NORMAL;
  return gameMO;
}


uint16_t GMDisarmCode::_validateAndLogCodeEntryPerCategory(char *buffer, char *codeSet[], char *codeLog[], uint16_t maxCodes, uint16_t returnValue) {
  for (int i = 0; i < maxCodes; i++) {
    if (strcmp((char *)codeSet[i], (char *)buffer) == 0) {
      //Serial.print("Match found");
      for (int j = 0; j < maxCodes; j++) {
        if (codeLog[j] == NULL) {
          continue;
        }
        if (strcmp((char *)codeLog[j], (char *)buffer) == 0) {
          //Serial.print("Found code in log");
          return GAME_VALIDATION_ALREADY_USED;
        }
      }
      for (int k = 0; k < maxCodes; k++) {
        Serial.print("Finding free spot in log...");
        if (codeLog[k] == NULL) {
          codeLog[k] = codeSet[i];
          //Serial.print("Wrote code in log");
          return returnValue;
        }
      }
    }
  }
  return GAME_VALIDATION_INVALID_CODE;
}


uint16_t GMDisarmCode::_validateCodeEntry() {
  uint16_t validationResult;

  validationResult = _validateAndLogCodeEntryPerCategory(enteredStringBuffer, disarmStringsSet, disarmStringsUsed, (uint16_t)MODULE_CONFIG_MAX_COMMAND_STRINGS, (uint16_t)GAME_VALIDATION_DISARM_CODE);
  if (validationResult != GAME_VALIDATION_INVALID_CODE) { return validationResult; }

  validationResult = _validateAndLogCodeEntryPerCategory(enteredStringBuffer, bonusStringsSet, bonusStringsUsed, (uint16_t)MODULE_CONFIG_MAX_COMMAND_STRINGS, (uint16_t)GAME_VALIDATION_BONUS_CODE);
  if (validationResult != GAME_VALIDATION_INVALID_CODE) { return validationResult; }

  validationResult = _validateAndLogCodeEntryPerCategory(enteredStringBuffer, punishStringsSet, punishStringsUsed, (uint16_t)MODULE_CONFIG_MAX_COMMAND_STRINGS, (uint16_t)GAME_VALIDATION_PUNISH_CODE);
  if (validationResult != GAME_VALIDATION_INVALID_CODE) { return validationResult; }

  return GAME_VALIDATION_INVALID_CODE;
}


void GMDisarmCode::_abortValidation() {
  if(GameModule::getStateIsValidating()){
    GameModule::setStateRemoveFlagAltered();
    GameModule::setStateRemoveFlagValidating();
  }
  _clearEnteredStringBuffer();
  if (validationTimerSet < 1) {
    return;
  }
  validationTimer->reset(validationTimerSet);
  interface->stopNotificationDecoding();
  interface->normalizeBackgroundMusic();
}


uint16_t GMDisarmCode::_getEnteredStringBufferPosition() {
  char *ptrChar;
  uint16_t pos;
  ptrChar = strchr(enteredStringBuffer, constants->emptyChar);
  if (ptrChar == NULL) {  //can't find any null termination, so clear the whole buffer
    _clearEnteredStringBuffer();
    return 0;
  }
  return ptrChar - enteredStringBuffer;
}


char *GMDisarmCode::_getEnteredStringBuffer() {
  return enteredStringBuffer;
}


bool GMDisarmCode::_isFullEnteredStringBuffer() {
  return (_getEnteredStringBufferPosition() >= (MODULE_CONFIG_STRING_MAX_LENGTH - 1));
}


uint16_t GMDisarmCode::_getMaxEnteredStringBufferSize() {
  return (MODULE_CONFIG_STRING_MAX_LENGTH - 1);
}

void GMDisarmCode::_clearEnteredStringBuffer() {
  for(int i = 0; i < MODULE_CONFIG_STRING_MAX_LENGTH; i++){
    enteredStringBuffer[i] = constants->emptyChar;
  }
  
}




void GMDisarmCode::_drawDefuseCodeBlock() {
  tgxImage->blitMasked(sprites->getKeypadBig().sprite, sprites->getKeypadBig().alphaMaskColor, iVec2{ position.posX + keypadLegendPos.x, position.posY + keypadLegendPos.y }, 1.0f);
  if (GameModule::getStateIsActivatedAndNotValidating() == true) {
    disarmModuleInfoText = infoTextEnterCode[globals->getLang()];
  } else if (GameModule::getStateIsAltered()) {
    disarmModuleInfoText = infoTextValidating[globals->getLang()];
  } else if (GameModule::getStateIsInactive()) {
    disarmModuleInfoText = infoTextDeactivated[globals->getLang()];
  } else if (GameModule::getStateIsSolved()) {
    disarmModuleInfoText = infoTextSolved[globals->getLang()];
  } else {
    disarmModuleInfoText = infoTextDefault[globals->getLang()];
  }
  
  tgxImage->drawText((char *)disarmModuleInfoText, iVec2{ position.posX + 65, position.posY + 45 }, RGB565_White, font_tgx_Arial_16, true);
  uint16_t activeKeysArray[12];
  if ((GameModule::getStateIsValidating()) || (state == GameModule::getStateIsSolved())) {
    for (int i = 0; i < 12; i++) {
      activeKeysArray[i] = keypadLegendBlinkKeysArrayDisabled[i];
    }
  } else if (_getEnteredStringBufferPosition() == 0) {
    for (int i = 0; i < 12; i++) {
      activeKeysArray[i] = keypadLegendBlinkKeysArrayNoEntries[i];
    }
  } else if (_isFullEnteredStringBuffer()) {
    for (int i = 0; i < 12; i++) {
      activeKeysArray[i] = keypadLegendBlinkKeysArrayBufferFull[i];
    }
  } else {
    for (int i = 0; i < 12; i++) {
      activeKeysArray[i] = keypadLegendBlinkKeysArraySomeEntries[i];
    }
  }
  for (int row = 0; row < keypadLegendButtonsRows; row++) {
    for (int col = 0; col < keypadLegendButtonsCols; col++) {
      if (activeKeysArray[col + (keypadLegendButtonsCols * row)] > 0) {
        // tgxImage.drawRect(
        //   keypadLegendPos.x+keypadLegendFirstKeyOffset.x + (col * (keypadLegendBtnSize.x + keypadLegendBtnSpacing.x)),   //X
        //   keypadLegendPos.y+keypadLegendFirstKeyOffset.y + (row * (keypadLegendBtnSize.y + keypadLegendBtnSpacing.y)),   //Y
        //   keypadLegendBtnSize.x,                                                                                         //W
        //   keypadLegendBtnSize.y,                                                                                         //H
        //   keypadLegendBlinkColorArray[col + (keypadLegendButtonsCols * row)]                                             //COLOR
        // );
      } else {
        tgxImage->fillRect(
          position.posX + keypadLegendPos.x + keypadLegendFirstKeyOffset.x + (col * (keypadLegendBtnSize.x + keypadLegendBtnSpacing.x)),  //X
          position.posY + keypadLegendPos.y + keypadLegendFirstKeyOffset.y + (row * (keypadLegendBtnSize.y + keypadLegendBtnSpacing.y)),  //Y
          keypadLegendBtnSize.x,                                                                                                          //W
          keypadLegendBtnSize.y,                                                                                                          //H
          (uint16_t)0b0000000000000000,                                                                                                   //COLOR
          0.80f                                                                                                                           //OPACITY
        );
      }
    }
  }
  uint16_t codeEntryYOffset = 57;
  uint16_t codeEntryXOffset = 67;
  uint16_t progressPos = 0;
  if (GameModule::getStateIsValidating()){
    progressPos = map(validationTimer->getGameTimeLeftCurrent(), validationTimer->getGameTimeLeftSet(), 0, 0, _getMaxEnteredStringBufferSize());
  }
  for (int i = 0; i < _getMaxEnteredStringBufferSize(); i++) {
    uint16_t color = ((progressPos >= i) && (GameModule::getStateIsValidating())) ? 0b0000001111100000 : 0b0000000011000000;
    tgxImage->fillRect(position.posX + codeEntryXOffset + (16 * i), position.posY + codeEntryYOffset, 13, 20, color);
  }
  if (GameModule::getStateIsActivatedAndNotValidating() == true) {
    tgxImage->drawText(_getEnteredStringBuffer(), iVec2{ position.posX + (codeEntryXOffset - 1), position.posY + codeEntryYOffset + 20 }, RGB565_Lime, font_tgx_Arial_20, true);
    if ((millis() & 0b10000000) > 0) {  //flips about 8 times a second (1000/128) ^Alexander
      if (!_isFullEnteredStringBuffer()) {
        uint16_t pos = _getEnteredStringBufferPosition();
        tgxImage->fillRect(position.posX + codeEntryXOffset + (16 * pos), position.posY + codeEntryYOffset, 13, 20, (uint16_t)0b0000001111100000);
      }
    }
  }
}



//The basic GUI has two keys with labels visible on the bottom of the screen. This method can override some of their functionality
void GMDisarmCode::updateGuiKeyStates() {
  if ((GameModule::getStateIsAltered() == false) && (GameModule::getStateIsActivatedAndNotValidating() == true)) {
    //Serial.println("UNALTERED");
    GameModule::hideConfirmKey();
    GameModule::hideBackKey();
    GameModule::releaseBackKey();
    return;
  }
  if ((GameModule::getStateIsAltered() == true) && (GameModule::getStateIsActivatedAndNotValidating() == true)) {
    //Serial.println("ALTERED");
    GameModule::showConfirmKey();
    GameModule::showBackKey();
    GameModule::captureBackKey();
    GameModule::setCustomBackKeyString(constants->labelBackspace[globals->getLang()]);
    return;
  }
  if (GameModule::getStateIsValidating() == true) {
    //Serial.println("VALIDATING");
    GameModule::hideConfirmKey();
    GameModule::showBackKey();
    GameModule::captureBackKey();
    GameModule::setCustomBackKeyString(constants->labelCancel[globals->getLang()]);
    return;
  }
  if (GameModule::getStateIsSolved() == true) {
   // Serial.println("SOLVED");
    GameModule::hideConfirmKey();
    GameModule::hideBackKey();
    GameModule::releaseBackKey();
    return;
  }
  if (GameModule::getStateIsInactive() == true) {
    GameModule::hideConfirmKey();
    GameModule::hideBackKey();
    GameModule::releaseBackKey();
    return;
  }
}


//HOOKS
void GMDisarmCode::onStateChange() {
  updateGuiKeyStates();
}


//LOOPS
void GMDisarmCode::loop(uint32_t gameState) {

  //LOOP THE MODULE TIMERS
  if(internalTimerSet > 0){
    internalTimer->loop();
  }
  if(validationTimerSet > 0){
    validationTimer->loop();
  }
  
  if ((GameModule::getStateIsValidating() == true) && (validationTimer->isExpired() || (validationTimerSet < 1))) {
    uint16_t validationResult = _validateCodeEntry();
    if (validationResult == GAME_VALIDATION_INVALID_CODE) {
      setGameModifierOutput(GAME_MODULE_OUTPUT_PUNISH_MEDIUM);
      interface->playError();
      interface->playNotificationValidationFailed();
    } 
    if (validationResult == GAME_VALIDATION_ALREADY_USED) {
      setGameModifierOutput(GAME_MODULE_OUTPUT_PUNISH_SMALL);
      interface->playError();
      interface->playNotificationValidationAlreadyUsed();
    }
    if (validationResult == GAME_VALIDATION_DISARM_CODE) {
      setGameModifierOutput(GAME_MODULE_OUTPUT_NORMAL);
      setProgress(getProgress() + 1);
      interface->playConfirm();
      if(getProgress() < progressMax){
        interface->playNotificationValidationSuccess();
      } else {
        setGameModifierOutput(GAME_MODULE_OUTPUT_SOLVED);
      }
    }
    if (validationResult == GAME_VALIDATION_BONUS_CODE) {
      setGameModifierOutput(GAME_MODULE_OUTPUT_REWARD_MEDIUM);
      interface->playPong();
      interface->playNotificationValidationBonus();
    }
    if (validationResult == GAME_VALIDATION_PUNISH_CODE) {
      setGameModifierOutput(GAME_MODULE_OUTPUT_PUNISH_BIG);
      interface->playError();
      interface->playNotificationValidationPunish();
    }
    _abortValidation();
  }


  if (GameModule::getStateIsActivatedAndNotValidating()) {
    if ((GameModule::getStateIsAltered() == true) && (enteredStringBuffer[0] == constants->emptyChar)) {
      GameModule::setStateRemoveFlagAltered();
    } else if ((GameModule::getStateIsAltered() == false) && (enteredStringBuffer[0] != constants->emptyChar)) {
      GameModule::setStateAddFlagAltered();
    }
  }


  if ((GameModule::getStateIsSolved() == false) && (progress == progressMax)) {
    GameModule::setStateAddFlagSolved();
    if(validationTimerSet > 0){
      validationTimer->reset(validationTimerSet);
    }
    if(internalTimerSet > 0){
      internalTimer->reset(validationTimerSet);
    }
  }
  getAndUpdateStateHasChanged();
}


void GMDisarmCode::loopGfx() {
  if (isVisible()) {
    GameModule::drawModuleFrame(tgxImage);
    GMDisarmCode::_drawDefuseCodeBlock();
  }
}
