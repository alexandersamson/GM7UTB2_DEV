#include "wiring.h"
#include <stdint.h>
#include "GameModule.h"


void GameModule::assignInternalId(uint16_t newInternalId){
  hasAssignedInternalId = true;
  internalId = newInternalId;
}

bool GameModule::getHasAssignedInternalId(){
  return hasAssignedInternalId;
}

uint16_t GameModule::getAssignedInternalId(){
  return globals->moduleConfigs[internalId].id;
}

uint16_t GameModule::getStatusColor(){
  if ((state & GAME_MODULE_STATE_INACTIVE) > 0) return statusColors.inactive;
  if ((state & GAME_MODULE_STATE_INFORMATIVE) > 0) return statusColors.informative;
  if ((state & GAME_MODULE_STATE_SOLVED) > 0) return statusColors.solved;
  if ((state & GAME_MODULE_STATE_FAILED) > 0) return statusColors.failed;
  if ((state & GAME_MODULE_STATE_ACTIVE) > 0) return statusColors.active;
  return statusColors.inactive;
}

uint16_t GameModule::getProgressBarWidth(uint16_t endTitleHeaderX){
  uint16_t startX = endTitleHeaderX + 3;
  uint16_t maxWidth = position.width - startX - 24;
  uint16_t prg = map(progress, progressMin, progressMax, 1, maxWidth);
  return prg;
}


void GameModule::setProgressInternal(uint32_t prg){
  if(prg > progressMax){
    progress = progressMax;
  }else if(prg < progressMin){
    progress = progressMin;
  } else{
    progress = prg;
  }
}

void GameModule::drawModuleFrame(Image<RGB565> *tgxImage){
    //Draw box and title
    uint16_t titleBoxX = tgxImage->measureText(getName(), iVec2{ getPosition().posX+2, getPosition().posY+13 },font_tgx_Arial_9, false).lx();
    tgxImage->fillRect(position.posX, position.posY, titleBoxX+6, 16, colorScheme.frame, 1.0f);
    //tgxImage->fillRect(getPosition().posX, getPosition().posY, getPosition().width, 16, colorScheme.frame, 1.0f);
    tgxImage->drawRect(getPosition().posX, getPosition().posY+16, getPosition().width, getPosition().height, colorScheme.frame, 1.0f);
    tgxImage->drawText(getName(), iVec2{ getPosition().posX+2, getPosition().posY+13 }, colorScheme.textInverted, font_tgx_Arial_9, true);

    //Draw the progress bar header and bar
    uint16_t progressBarStart = titleBoxX+7;
    uint16_t progressBarEnd = position.width-21;
    uint16_t progressBarWidth = getProgressBarWidth(titleBoxX);
    tgxImage->drawFastHLine(iVec2{ titleBoxX, getPosition().posY }, position.width-titleBoxX, colorScheme.frame);
    tgxImage->drawFastVLine(iVec2{ position.width-1, getPosition().posY }, 16, colorScheme.frame);
    uint16_t barColor;
    if(getStateIsSolved() == true){barColor = colorScheme.progressBarComplete;}
    else if(getStateIsFailed() == true){barColor = colorScheme.progressBarAlert;}
    else {barColor = colorScheme.progressBar;}
    tgxImage->fillRect(progressBarStart, position.posY+2, progressBarWidth, 13, barColor, 1.0f);
    tgxImage->drawFastVLine(iVec2{ position.width-17, getPosition().posY }, 16, colorScheme.frame);
    tgxImage->drawFastVLine(iVec2{ position.width-18, getPosition().posY }, 16, colorScheme.frame);
    tgxImage->drawFastVLine(iVec2{ position.width-19, getPosition().posY }, 16, colorScheme.frame);

    //Draw the progress text in the bar
    char combinedString[24]; //2x uint32 0-4294967295 (20) + ' / ' (3) + '/0' (1) = 24 max size needed
    sprintf(combinedString, "%u / %u", progress, progressMax);

    uint16_t barStringLength = tgxImage->measureText(combinedString, iVec2{ 0, 0 },font_tgx_Arial_8, false).lx();
    if(progressBarWidth < (barStringLength + 6)){
      tgxImage->drawText(combinedString, iVec2{ progressBarStart+progressBarWidth+3, getPosition().posY+12 }, colorScheme.text, font_tgx_Arial_8, false);
    } else {
      tgxImage->drawText(combinedString, iVec2{ progressBarStart+progressBarWidth-barStringLength-3, getPosition().posY+12 }, colorScheme.textInverted, font_tgx_Arial_8, false);
    }

    //Draw status light
    tgxImage->fillRect(position.width-15, position.posY+2, 13, 13, GameModule::getStatusColor(), 1.0f);
}





bool GameModule::hasConsumedSolvedTrigger(){
 return consumedSolvedTrigger;
}

void GameModule::consumeSolvedTrigger(){
  consumedSolvedTrigger = true;
}


bool GameModule::getStateIsSolved(){
  return globals->bitwiseHasFlag(state, GAME_MODULE_STATE_SOLVED);
}

bool GameModule::getStateIsFailed(){
  return globals->bitwiseHasFlag(state, GAME_MODULE_STATE_FAILED);
}

bool GameModule::getStateIsActive(){
  return globals->bitwiseHasFlag(state, GAME_MODULE_STATE_ACTIVE);
}

bool GameModule::getStateIsConsideredActivated(){
 uint32_t maskRequiredAtLeastOne = GAME_MODULE_STATE_ACTIVE + GAME_MODULE_STATE_VALIDATING;
 return globals->bitwiseHasFlag(state, maskRequiredAtLeastOne);
}

bool GameModule::getStateIsActivatedAndNotValidating(){
  uint32_t maskRequiredAtLeastOne = 0;
  uint32_t maskRequiredAll = GAME_MODULE_STATE_ACTIVE;
  uint32_t maskForbidden = GAME_MODULE_STATE_VALIDATING + GAME_MODULE_STATE_FAILED + GAME_MODULE_STATE_INACTIVE + GAME_MODULE_STATE_SOLVED;
  return globals->bitwiseFlagsCheck(state, maskRequiredAtLeastOne, maskRequiredAll, maskForbidden);
}

bool GameModule::getStateIsInactive(){
  return globals->bitwiseHasFlag(state, GAME_MODULE_STATE_INACTIVE);
}

bool GameModule::getStateIsValidating(){
  return globals->bitwiseHasFlag(state, GAME_MODULE_STATE_VALIDATING);
}

bool GameModule::getStateIsAltered(){
  return globals->bitwiseHasFlag(state, GAME_MODULE_STATE_ALTERED);
}

bool GameModule::getStateIsSolvable(){
  uint32_t maskRequiredAtLeastOne = 0;
  uint32_t maskRequiredAll = GAME_MODULE_STATE_SOLVABLE;
  uint32_t maskForbidden = GAME_MODULE_STATE_INFORMATIVE + GAME_MODULE_STATE_DISABLED + GAME_MODULE_STATE_FAILED + GAME_MODULE_STATE_INACTIVE + GAME_MODULE_STATE_SOLVED;
  return globals->bitwiseFlagsCheck(state, maskRequiredAtLeastOne, maskRequiredAll, maskForbidden);
}

bool GameModule::getStateIsConsideredContributingToGameplay(){
  uint32_t maskRequiredAtLeastOne = 0;
  uint32_t maskRequiredAll = 0;
  uint32_t maskForbidden =  GAME_MODULE_STATE_DISABLED + GAME_MODULE_STATE_INACTIVE;
  return globals->bitwiseFlagsCheck(state, maskRequiredAtLeastOne, maskRequiredAll, maskForbidden);
}

bool GameModule::getStateIsConsideredLoopable(){
  uint32_t maskRequiredAtLeastOne = GAME_MODULE_STATE_ACTIVE + GAME_MODULE_STATE_VALIDATING + GAME_MODULE_STATE_PAUSED + GAME_MODULE_STATE_SOLVED + GAME_MODULE_STATE_FAILED + GAME_MODULE_STATE_INFORMATIVE;
  uint32_t maskRequiredAll = 0;
  uint32_t maskForbidden = GAME_MODULE_STATE_DISABLED;
  return globals->bitwiseFlagsCheck(state, maskRequiredAtLeastOne, maskRequiredAll, maskForbidden);
}

bool GameModule::getStateIsConsideredToStayUnchangedPostGame(){
  uint32_t maskRequiredAtLeastOne = GAME_MODULE_STATE_SOLVED + GAME_MODULE_STATE_FAILED + GAME_MODULE_STATE_INFORMATIVE + GAME_MODULE_STATE_DISABLED;
  return globals->bitwiseHasFlag(state, maskRequiredAtLeastOne); 
}


void GameModule::setStateAddFlagAltered(){
  state |= GAME_MODULE_STATE_ALTERED;
}

void GameModule::setStateRemoveFlagAltered(){
  state &= ~GAME_MODULE_STATE_ALTERED;
}

void GameModule::setStateAddFlagValidating(){
  state |= GAME_MODULE_STATE_VALIDATING;
}

void GameModule::setStateRemoveFlagValidating(){
  state &= ~GAME_MODULE_STATE_VALIDATING;
}

void GameModule::setStateAddFlagSolved(){
  state |= GAME_MODULE_STATE_SOLVED;
}

void GameModule::setStateRemoveFlagSolved(){
  state &= ~GAME_MODULE_STATE_SOLVED;
}

void GameModule::setStateAddFlagFailed(){
  state |= GAME_MODULE_STATE_FAILED;
}

void GameModule::setStateRemoveFlagFailed(){
  state &= ~GAME_MODULE_STATE_FAILED;
}

void GameModule::setStateAddFlagInactive(){
  state |= GAME_MODULE_STATE_INACTIVE;
}

void GameModule::setStateRemoveFlagInactive(){
  state &= ~GAME_MODULE_STATE_INACTIVE;
}

void GameModule::setStateAddFlagSolvable(){
  state |= GAME_MODULE_STATE_SOLVABLE;
}

void GameModule::setStateRemoveFlagSolvable(){
  state &= ~GAME_MODULE_STATE_SOLVABLE;
}


//For some modules the 'back key' ('*' in most cases) will be also needed for the functioning of the module itself. In that case the module needs to capture this key for it's own functioning and release it afterwards.
//When the derived module does not release the Back Key it will be impossible to return to the in-game menu. For single-module games, this can be sought after, because it won't add extra menu layers for the player to get lost in.
//Please do not forget to set the setCustomBackKeyString to a custom value in case the back key needs a different value than the GUI offers ('open menu' of some sorts) ^Alexander
void GameModule::captureBackKey(){
  backKeyIsCaptured = true;
}

void GameModule::releaseBackKey(){
  backKeyIsCaptured = false;
}

bool GameModule::isCapturedBackKey(){
  return backKeyIsCaptured;
}

void GameModule::setCustomBackKeyString(const char * newBackString){
  customBackKeyString = newBackString;
}

const char * GameModule::getCustomBackKeyString(){
  return customBackKeyString;
}

void GameModule::setCustomConfirmKeyString(const char * newConfirmString){
  customConfirmKeyString = newConfirmString;
}

const char * GameModule::getCustomConfirmKeyString(){
  return customConfirmKeyString;
}

void GameModule::showBackKey(){
  backKeyIsShown = true;
}

void GameModule::hideBackKey(){
  backKeyIsShown = false;
}

bool GameModule::getBackKeyIsShown(){
  return backKeyIsShown;
}

void GameModule::showConfirmKey(){
  confirmKeyIsShown = true;
}

void GameModule::hideConfirmKey(){
  confirmKeyIsShown = false;
}

bool GameModule::getConfirmKeyIsShown(){
  return confirmKeyIsShown;
}

bool GameModule::getAndUpdateStateHasChanged(){
  if (state != lastKnownState){
    lastKnownState = state;
    onStateChange(); //Call the pure virtual hook method in the derived class
    return true;
  }
  return false;
}


bool GameModule::getIsExternalCanModule(){
  return isExternalCanModule;
}

bool GameModule::getIsOnlineOverCan(){
  return isOnlineOverCan;
}

void GameModule::turnOnlineOverCan(){
  isOnlineOverCan = true;
}

void GameModule::turnOfflineOverCan(){
  isOnlineOverCan = false;
}

