#include "Game.h"


Game::Game(Interface *interfaceObj, GameTimer *gameTimerObj, AudioProcessing *audioProcessingObj, Constants *constantsObj, Globals *globalsObj, GameModule **moduleObj, uint16_t modulesCnt) 
    : interface(interfaceObj), 
      mainTimer(gameTimerObj), 
      audioProcessor(audioProcessingObj), 
      constants(constantsObj),
      globals(globalsObj),
      modules(moduleObj)
{
  modulesCount = modulesCnt;
}


void Game::reset(){
  lastUserInput = '\0';
  setState(GAME_STATE_UNPREPARED);
  attachAndResetGameModules();
}

uint32_t Game::start(){
  mainTimer->reset(globals->config.gameTimerSet);
  mainTimer->show();
  mainTimer->prepare();
  if(mainTimer->start() == true){
    setState(GAME_STATE_STARTED);
    modules[activeModule]->activate();
    fetchModuleGuiKeysStrings();
    fetchModuleGuiKeysStates();
  } else {
    setState(GAME_STATE_ERROR);
  }
  return state;
}


//TODO: This whole method is riddled with criss-cross calls and assignments. Needs to be cleaned.
void Game::attachAndResetGameModules(){
  Serial.print("Resetting Game...");
  activeModule = 0;
  globals->modulesAssignedIdIndexer = 0;
  interface->setActiveGameModule(activeModule);
  for(int i = 0; i < modulesCount; i++){
    modules[i]->assignInternalId(i);
    globals->moduleConfigs[i].id = i;
    globals->moduleConfigs[i].hasAssignedId = true;
    modules[i]->reset();
  }
  for(int i = 0; i < EXTERNAL_MODULES_COUNT_MAX; i++){
    if(globals->externalGameModuleData[i].isRegistered && ((globals->externalGameModuleData[i].status & GAME_MODULE_STATE_DISABLED) == 0)){
      Serial.println("External Module Found (CAN bus).");
    }
  }
  Serial.println(" DONE!");
}



void Game::setState(uint32_t newState){
  state = newState;
  globals->status = newState;
}



uint32_t Game::getState(){
  return state;
}



bool Game::isInStartedMode(){
  return ((state & GAME_STATE_STARTED) > 0);
}


bool Game::isInValidationMode(){
  return ((state & GAME_STATE_VALIDATING) > 0);
}


bool Game::getIsUserInputEnabled(){
  if(!mainTimer->isTicking()){
    return false;
  }
  if(state == GAME_STATE_VALIDATING){
    return false;
  }
  return true;
}



void Game::parseUserInput(){
  if(interface->getSelectedInputConsumer() != INPUT_CONSUMER_GAME){
    return;
  }
  lastUserInput = interface->consumeLastReceivedKey();
  if(lastUserInput == '\0'){
    return;
  }
  char input = lastUserInput;
  lastUserInput = '\0';
  if(!getIsUserInputEnabled()){
    return;
  }
  modules[activeModule]->inputKeyboard(input);
}

void Game::disarm(){
    setState(GAME_STATE_ENDED_WIN);
    mainTimer->disarm();
    modules[activeModule]->postGameProcess();
    fetchModuleGuiKeysStates();
    fetchModuleGuiKeysStrings();
    audioProcessor->resumeFromInterruptedBackgroundMusic();
    audioProcessor->playSound(globals->config.notificationSoundGameDisarmingSuccess, AUDIO_CHANNEL_NOTIFICATIONS, 1);
    audioProcessor->playBackgroundMusic(globals->config.backgroundMusicWinFile, AUDIO_CHANNEL_BACKGROUND_MUSIC_1);
}


void Game::detonate(){
  setState(GAME_STATE_ENDED_LOSS);
  mainTimer->detonate();
  modules[activeModule]->postGameProcess();
  fetchModuleGuiKeysStates();
  fetchModuleGuiKeysStrings();
  audioProcessor->resumeFromInterruptedBackgroundMusic();
  audioProcessor->playSound(globals->config.notificationSoundGameDetonate, AUDIO_CHANNEL_NOTIFICATIONS, 1);
  audioProcessor->playBackgroundMusic(globals->config.backgroundMusicLossFile, AUDIO_CHANNEL_BACKGROUND_MUSIC_1);
}



void Game::runModules(){
  if((state & (GAME_STATE_ENDED_WIN+GAME_STATE_ENDED_LOSS+GAME_STATE_ENDED_DRAW)) > 0){
    return; //Game is done for, so no need to calculate module stuff any more
  }
  bool disarmed = true; //First set it to true, it will be set to false if any solvable module is still unsolved.
  for(int i=0; i<modulesCount; i++){
    if(i >= MODULES_COUNT){
      break;
    }
    if ((modules[i]->getStateIsSolved() == false) && (modules[i]->getStateIsSolvable() == true)){
      disarmed = false;
    }
  }
  for(int i = 0; i < EXTERNAL_MODULES_COUNT_MAX; i++){
    if(globals->externalGameModuleData[i].isRegistered && ((globals->externalGameModuleData[i].status & GAME_MODULE_STATE_DISABLED) == 0) && ((globals->externalGameModuleData[i].status & GAME_MODULE_STATE_SOLVABLE ) > 0)){
      // Serial.println(globals->externalGameModuleData[i].status);
      if((globals->externalGameModuleData[i].status & GAME_MODULE_STATE_SOLVED ) == 0){
        disarmed = false;
      }
    }
  }
  if(disarmed == true && ((state & GAME_STATE_ENDED_WIN) == 0)){
    disarm();
    return;
  }

  if(modules[activeModule]->getStateIsSolvable() == false){
    return;
  }
  if((disarmed == false) && (modules[activeModule]->hasConsumedSolvedTrigger() == false) && (modules[activeModule]->getStateIsSolved())){
    modules[activeModule]->consumeSolvedTrigger();
    interface->playNotificationModuleSolved();
  }
  if(modules[activeModule]->getStateIsConsideredLoopable()){
    modules[activeModule]->loop(state);
  }
  fetchModuleGuiKeysStates();
  fetchModuleGuiKeysStrings();
  if(modules[activeModule]->isAvailableGameModifierOutput()){
    uint16_t moduleOutput = modules[activeModule]->consumeGameModifierOutput();
    uint32_t speedModifier = 100;
    uint32_t modifierDuration = 1000;
    if(moduleOutput == GAME_MODULE_OUTPUT_SOLVED){
      //Do something
    } else if(moduleOutput == GAME_MODULE_OUTPUT_REWARD_STREAM){           //Streaming rewards/punishments will have a very short duration and are supposed to be triggered by a ongoing state change. Like a button or momentary switch that is held in place. ^Alexander
      speedModifier = 20;
      modifierDuration = 50;
    } else if(moduleOutput == GAME_MODULE_OUTPUT_PUNISH_STREAM){
      speedModifier = 1000;
      modifierDuration = 50;
    } else if(moduleOutput == GAME_MODULE_OUTPUT_PUNISH_SMALL){
      speedModifier = 1000;
      modifierDuration = 1000;
    } else if(moduleOutput == GAME_MODULE_OUTPUT_PUNISH_MEDIUM){
      speedModifier = 1000;
      modifierDuration = 5000;
    } else if(moduleOutput == GAME_MODULE_OUTPUT_PUNISH_BIG){
      speedModifier = 1000;
      modifierDuration = 30000;
    } else if(moduleOutput == GAME_MODULE_OUTPUT_REWARD_SMALL){
      speedModifier = 10;
      modifierDuration = 5000;
    } else if(moduleOutput == GAME_MODULE_OUTPUT_REWARD_MEDIUM){
      speedModifier = 10;
      modifierDuration = 30000;
    } else if(moduleOutput == GAME_MODULE_OUTPUT_REWARD_BIG){
      speedModifier = 10;
      modifierDuration = 180000;
    }
    uint32_t duration;
    if((speedModifier < 100) && (mainTimer->getTickSpeedModifierIsSlower() == true)){
      duration = max(mainTimer->getDurationLeftForSpeedModifier(), modifierDuration);
    } else if ((speedModifier > 100) && (mainTimer->getTickSpeedModifierIsFaster() == true)){
      duration = max(mainTimer->getDurationLeftForSpeedModifier(), modifierDuration);
    } else {
      duration = modifierDuration;
    }
    mainTimer->setTickSpeedModifiers(speedModifier, duration);
  }
}


void Game::fetchModuleGuiKeysStates(){
  moduleGuiKeyStates.backKeyIsCaptured = modules[activeModule]->isCapturedBackKey();
  moduleGuiKeyStates.confirmKeyIsShown = modules[activeModule]->getConfirmKeyIsShown();
  moduleGuiKeyStates.backKeyIsShown = modules[activeModule]->getBackKeyIsShown();
  interface->pushGameGuiKeysStates(moduleGuiKeyStates.backKeyIsCaptured, moduleGuiKeyStates.confirmKeyIsShown, moduleGuiKeyStates.backKeyIsShown);
}


void Game::fetchModuleGuiKeysStrings(){
  customBackKeyString = modules[activeModule]->getCustomBackKeyString();
  customConfirmKeyString = modules[activeModule]->getCustomConfirmKeyString();
  interface->pushGameGuiKeysStrings(customConfirmKeyString, customBackKeyString);
}



void Game::handleInterfaceRequests(){
  if(interface->getIsNewGameStateRequestAvailable() == false){
    return;
  }
  uint32_t requestedState = interface->consumeGameStateRequest();
  if(requestedState == GAME_STATE_STARTED){
    reset();
    start();
    interface->setSelectedInputConsumer(INPUT_CONSUMER_GAME);
  }
}



void Game::loop(){
  handleInterfaceRequests();
  if((state & GAME_LOOPABLE) > 0){
    parseUserInput();
    runModules();

    //Check for intensified background music
    if((mainTimer->getTickSpeedModifierIsFaster() == true) && (interface->getIsPlayingIntensifiedBackgroundMusic() == false) && (mainTimer->getDurationLeftForSpeedModifier() > 1000)){
      interface->playIntesifiedBackgroundMusic();
    } else if((mainTimer->getTickSpeedModifierIsFaster() == false) && (interface->getIsPlayingIntensifiedBackgroundMusic() == true)){
      interface->stopIntesifiedBackgroundMusicAndPlayNormalBackgroundMusic();
    }

    //Detonation check
    if(mainTimer->isExpired() && ((state & GAME_STATE_ENDED_LOSS) == 0)){
      detonate();
      return;
    }


    //TICKING CLOCK SOUND
    if ((mainTimer->consumeSecondHasPassed() == true) && (audioProcessor->channelIsPlaying(AUDIO_CHANNEL_MEM_BEEPS_1) == false)) {  //These beeps are lower priority than other notifications, so skip one if some other beep is playing.
      if (mainTimer->getGameTimeLeftCurrent() <= 10000) {
        interface->playTimerTickFinal();
      } else if (mainTimer->getGameTimeLeftCurrent() <= 60000) {
        interface->playTimerTickElevated();
      } else {
        interface->playTimerTickNormal();
      }
    }
  }


  //This will send the main timer and validation timer data over the can bus. When the timer is doing nothing, it will update once every 500 millis, otherwise every 77 millis.
  if (chronoTimer13Fps.hasPassed(77)){
    chronoTimer13Fps.restart();
    if(mainTimer->isTicking() || chronoTimer2Fps.hasPassed(500)){
      chronoTimer2Fps.restart();
      interface->sendMainTimerOverCan(mainTimer->getGameTimeLeftCurrent(), mainTimer->getGameTimeLeftSet());
    }
    if(modules[activeModule]->getStateIsValidating()){
      interface->sendValidationTimerOverCan(modules[activeModule]->getValidationTimerCurrentMillisLeft(), modules[activeModule]->getValidationTimerSetMillis());
    }
  }

}




//Method runModulesGfx is called by the 60fps timer in main sketch
void Game::runModulesGfx(){
  if((state & GAME_LOOPABLE) > 0){
    modules[activeModule]->loopGfx();
  }
}

