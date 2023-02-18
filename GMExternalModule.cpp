#include <stdint.h>
#include "GMExternalModule.h"


// char gameStatusString[9][MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char disarmStringsSet[GAME_MAX_DISARM_CODES][MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char disarmStringsUsed[GAME_MAX_DISARM_CODES][MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char enteredStringBuffer[MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char bonusStringsSet[GAME_MAX_BONUS_CODES][MODULE_CONFIG_STRING_MAX_LENGTH + 1];
// char bonusStringsUsed[GAME_MAX_BONUS_CODES][MODULE_CONFIG_STRING_MAX_LENGTH + 1];


GMExternalModule::GMExternalModule(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Interface *interfaceObj, GameTimer *internalTimerObj, GameTimer *validationTimerObj, Constants *constantsObj, Globals *globalsObj)
  : tgxImage(tgxImageObj), sprites(spritesObj), interface(interfaceObj), internalTimer(internalTimerObj), validationTimer(validationTimerObj), constants(constantsObj), globals(globalsObj) {
  setPosition({ 0, 100, 320, 80 });
  GameModule::setCustomConfirmKeyString(constants->labelConfirm[globals->getLang()]);
  GameModule::setCustomBackKeyString(constants->labelBackspace[globals->getLang()]);
  isExternalCanModule = true;
}



void GMExternalModule::reset() {
  if(GameModule::getHasAssignedInternalId() == false){
    Serial.print("GMExternalModule::reset() >> Trying to reset a module without any assigned internal ID. Please make sure an internal ID has been assigned to this module.");
    return;
  }
  loadModuleConfig();
  setState(GAME_MODULE_STATE_INACTIVE);
  consumedSolvedTrigger = false;
  makeVisible();
  setProgress(0);
  internalTimer->reset(internalTimerSet);
  validationTimer->reset(validationTimerSet);
}

void GMExternalModule::loadModuleConfig(){
  if(GameModule::getHasAssignedInternalId() == false){
    Serial.print("GMExternalModule::loadModuleConfig() >> Trying to load a module config without any assigned internal ID. Please make sure an internal ID has been assigned to this module.");
    return;

  }
}


void GMExternalModule::activate() {
  setState(GAME_MODULE_STATE_ACTIVE);
}

void GMExternalModule::deactivate() {
  setState(GAME_MODULE_STATE_INACTIVE);
  GameModule::releaseBackKey();
}

void GMExternalModule::enable() {
  enabled = true;
}

//Disabled modules are deactivated, but also do not count towards win/loss conditions
void GMExternalModule::disable() {
  setState(GAME_MODULE_STATE_DISABLED);
  GameModule::releaseBackKey();
}

//WHEN A GAME ENDS
void GMExternalModule::postGameProcess(){
  updateGuiKeyStates();
}

//WHEN SWITCHING TO ANOTHER MODULE
void GMExternalModule::switchAwayToAnotherModule(){
  updateGuiKeyStates();
}


const char *GMExternalModule::getName() {
  return name;
}

void GMExternalModule::setName(char * newName) {
  if(GameModule::getHasAssignedInternalId() == false){
    Serial.print("GMExternalModule::setName(char * newName) >> Trying to set a module name without any assigned internal ID. Please make sure an internal ID has been assigned to this module.");
    return;
  }  
}


const char *GMExternalModule::getDescription() {
  return description;
}

void GMExternalModule::setDescription(char * newDescription) {
  if(GameModule::getHasAssignedInternalId() == false){
    Serial.print("GMExternalModule::setDescription(char * newDescription) >> Trying to set a module description without any assigned internal ID. Please make sure an internal ID has been assigned to this module.");
      return;
  }
}

bool GMExternalModule::isVisible() {
  return visible;
}

void GMExternalModule::makeVisible() {
  visible = true;
}

GMExternalModule::Position GMExternalModule::getPosition() {
  return position;
}

void GMExternalModule::setPosition(Position pos) {
  position.height = pos.height;
  position.width = pos.width;
  position.posX = pos.posX;
  position.posY = pos.posY;
}

void GMExternalModule::setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  position.height = h;
  position.width = w;
  position.posX = x;
  position.posY = y;
}

uint32_t GMExternalModule::getState() {
  return state;
}

void GMExternalModule::setState(uint32_t st) {
  state = st;
}

uint32_t GMExternalModule::getProgress() {
  return progress;
}

void GMExternalModule::setProgress(uint32_t prg) {
  GameModule::setProgressInternal(prg);
}


void GMExternalModule::inputKeyboard(char key) {
  //Not relevant
}



void GMExternalModule::inputInteger(uint32_t) {

}

void GMExternalModule::setGameModifierOutput(uint16_t newValue) {
  gameModifierOutput = newValue;
}


bool GMExternalModule::isAvailableGameModifierOutput() {
  if (gameModifierOutput != GAME_MODULE_OUTPUT_NORMAL) {
    return true;
  }
  return false;
}


uint16_t GMExternalModule::consumeGameModifierOutput() {
  uint16_t gameMO = gameModifierOutput;
  gameModifierOutput = GAME_MODULE_OUTPUT_NORMAL;
  return gameMO;
}




//The basic GUI has two keys with labels visible on the bottom of the screen. This method can override some of their functionality
void GMExternalModule::updateGuiKeyStates() {
 
}


//HOOKS
void GMExternalModule::onStateChange() {
  updateGuiKeyStates();
}


//LOOPS
void GMExternalModule::loop(uint32_t gameState) {

  //LOOP THE MODULE TIMERS
  if(internalTimerSet > 0){
    internalTimer->loop();
  }
  if(validationTimerSet > 0){
    validationTimer->loop();
  }


  if (GameModule::getStateIsActivatedAndNotValidating()) {
    if ((GameModule::getStateIsAltered()) && (enteredStringBuffer[0] == constants->emptyChar)) {
      GameModule::setStateRemoveFlagAltered();
    } else {
      GameModule::setStateAddFlagAltered();
    }
  }

  if ((state != GAME_MODULE_STATE_SOLVED) && (progress == progressMax)) {
    state = GAME_MODULE_STATE_SOLVED;
    if(validationTimerSet > 0){
      validationTimer->reset(validationTimerSet);
    }
    if(internalTimerSet > 0){
      internalTimer->reset(validationTimerSet);
    }
  }
  getAndUpdateStateHasChanged();
}


void GMExternalModule::loopGfx() {
  if (isVisible()) {
    GameModule::drawModuleFrame(tgxImage);
  }
}
