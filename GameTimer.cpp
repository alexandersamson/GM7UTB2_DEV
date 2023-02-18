
#include <stdint.h>
#include "GameTimer.h"

#define FLAG_PREPARED_TIMER 1
#define FLAG_STARTED_TIMER 2
#define FLAG_STOPPED_TIMER 4
#define FLAG_PAUSED_TIMER 8
#define FLAG_DEFUSED_TIMER 16
#define FLAG_EXPIRED_TIMER 32
#define FLAG_BONUS_TIMER 64
#define FLAG_PUNISH_TIMER 128
#define FLAG_UNPREPARED_TIMER 256


#define TIMER_TICKING (FLAG_STARTED_TIMER + FLAG_BONUS_TIMER + FLAG_PUNISH_TIMER)
#define TIMER_HALTED 61
#define TIMER_PAUSED 8
#define TIMER_SPED_UP 128
#define TIMER_SLOWED_DOWN 64
#define TIMER_ACCUMULATE_TOTAL 202
#define TIMER_NOT_LOOPING (FLAG_PREPARED_TIMER + FLAG_STOPPED_TIMER + FLAG_DEFUSED_TIMER + FLAG_EXPIRED_TIMER + FLAG_UNPREPARED_TIMER)
#define TIMER_STATUS_MODIFIABLE (FLAG_STARTED_TIMER + FLAG_BONUS_TIMER + FLAG_PUNISH_TIMER)
#define TIMER_STATUS_FRESH_STARTABLE (FLAG_PREPARED_TIMER + FLAG_UNPREPARED_TIMER)

#define TIMER_DEFAULT_SETTING_ONE_HOUR 3600000
#define TIMER_DEFAULT_SETTING_BASE_SPEED 100
#define TIMER_DEFAULT_SETTING_BONUS_DURATION 10000
#define TIMER_DEFAULT_SETTING_BONUS_SPEED 10        //slow down
#define TIMER_DEFAULT_SETTING_PUNISH_DURATION 10000
#define TIMER_DEFAULT_SETTING_PUNISH_SPEED 1000     // speed up


// class GameTimer {
//   private:
//     unsigned long gameTimeLeftSetMain; //At what time is this timer set to expire. Resetting timer will use this value.
//     unsigned long gameTimeLeftCurrentMain; //How much millis there are left on this timer.
//     unsigned long gameTimeStartedAtMain; //At which millis() time did this timer start?
//     unsigned long gameTimeElapsedMain; //How much time has been elapsed since this timer started? This timer will stop at pauses.
//     unsigned long gameTimeElapsedTotalMain; //How much time has been elapsed since this timer started? This counter will count raw play time from start till stop, without calculating bonusses, pauses and punishments.
//     uint16_t millisLeftThisSecondMain; //How many millis are left in the current second counting down?
//     uint16_t tickSpeedMain; //How fast does this timer tick? 100 = normal speed. <100 is slower, >100 is faster
//     bool secondPassedMain;
//     bool hour2MarkPassedMain;
//     bool hour1MarkPassedMain;
//     bool mins30PassedMain;
//     bool mins10PassedMain;
//     bool mins5PassedMain;
//     bool mins1PassedMain;
//     bool secs10PassedMain;
//     uint16_t statusMain;
//   public:
//     loopTimer(uint16_t timer);
// };

char timerSubTitle[9][16];

GameTimer::GameTimer(Constants *constantsObj, Globals *globalsObj): constants(constantsObj), globals(globalsObj){
    // strcpy(timerSubTitle[0], (char *) "PREPARED");
    // strcpy(timerSubTitle[1], (char *) "COUNTING DOWN");
    // strcpy(timerSubTitle[2], (char *) "STOPPED");
    // strcpy(timerSubTitle[3], (char *) "PAUSED");
    // strcpy(timerSubTitle[4], (char *) "DEFUSED");
    // strcpy(timerSubTitle[5], (char *) "EXPIRED");
    // strcpy(timerSubTitle[6], (char *) "SLOW COUNTDOWN");
    // strcpy(timerSubTitle[7], (char *) "FAST COUNTDOWN");
    // strcpy(timerSubTitle[8], (char *) "UNPREPARED");
}

void GameTimer::reset(unsigned long expiresInMillis = 0){
  if(expiresInMillis > 0){
    gameTimeLeftSet = expiresInMillis;
  } else if(gameTimeLeftSet < 1){
    gameTimeLeftSet = (globals->config.gameTimerSet > 0) ? globals->config.gameTimerSet : TIMER_DEFAULT_SETTING_ONE_HOUR;
  }
  gameTimeLeftCurrent = gameTimeLeftSet;
  gameTimeStartedAt = 0;
  gameTimePausedTotal = 0;
  gameTimeElapsed = 0;
  gameTimeElapsedTotal = 0;
  lastMillisValue = millis();
  millisLeftThisSecond = 0;
  tickSpeedModifierBase = TIMER_DEFAULT_SETTING_BASE_SPEED;
  tickSpeedModifierCurrent = tickSpeedModifierBase;
  tickSpeedModifierDurationLeft = 0;
  secondPassed = false;
  hour2MarkPassed = false;
  hour1MarkPassed = false;
  mins30Passed = false;
  mins10Passed = false;
  mins5Passed = false;
  mins1Passed = false;
  secs10Passed = false;
  timerStatus = FLAG_UNPREPARED_TIMER;
  showTimerObject = false;
}

void GameTimer::show(){
  showTimerObject = true;
}

void GameTimer::hide(){
  showTimerObject = false;
}

//A prepared timer will be displayable
void GameTimer::prepare(){
  timerStatus = FLAG_PREPARED_TIMER;
}

bool GameTimer::start(){
  if(gameTimeLeftCurrent > 0){
    if((timerStatus & TIMER_STATUS_FRESH_STARTABLE) > 0){
        unsigned long curMillis = millis();
        gameTimeStartedAt = curMillis;
        lastMillisValue = curMillis;
    }
    timerStatus = FLAG_STARTED_TIMER;
    loop();
    return true;
  }
  return false;
}


void GameTimer::expire(){
  timerStatus = FLAG_EXPIRED_TIMER;
  gameTimeLeftCurrent = 0;
  millisLeftThisSecond = 0;
}

void GameTimer::setTickSpeedModifiers(uint16_t value, unsigned long duration){
  tickSpeedModifierCurrent = value;
  tickSpeedModifierDurationLeft = duration;
  if((timerStatus & TIMER_STATUS_MODIFIABLE) > 0){
    if(tickSpeedModifierBase > value){
      timerStatus = FLAG_BONUS_TIMER;      
    } else if (tickSpeedModifierBase < value){
      timerStatus = FLAG_PUNISH_TIMER; 
    }
  }
}


void GameTimer::stopTickSpeedModifiers(){
  tickSpeedModifierCurrent = tickSpeedModifierBase;
  tickSpeedModifierDurationLeft = 0;
  if((timerStatus & TIMER_STATUS_MODIFIABLE) > 0){
    timerStatus = FLAG_STARTED_TIMER;   
  }
}


bool GameTimer::getTickSpeedModifierIsActive(){
  return (tickSpeedModifierCurrent != tickSpeedModifierBase) ? true : false;
}

bool GameTimer::getTickSpeedModifierIsFaster(){
  return (tickSpeedModifierCurrent > tickSpeedModifierBase) ? true : false;
}

bool GameTimer::getTickSpeedModifierIsSlower(){
  return (tickSpeedModifierCurrent < tickSpeedModifierBase) ? true : false;
}

uint32_t GameTimer::getDurationLeftForSpeedModifier(){
  return tickSpeedModifierDurationLeft;
}


void GameTimer::loop(){
  if((timerStatus & TIMER_NOT_LOOPING) > 0){
    return;
  }
  //First get the delta millis for this loop run and update the last value with current millis value.
  unsigned long curMillis = millis();
  unsigned long deltaMillis = curMillis - lastMillisValue;
  lastMillisValue = curMillis;
  
  if((timerStatus & TIMER_PAUSED) > 0){
    gameTimePausedTotal += deltaMillis; //Add time to the paused time
  }

  if((timerStatus & TIMER_ACCUMULATE_TOTAL) > 0){
    gameTimeElapsedTotal += deltaMillis; //Add up to the total game time
    gameTimeElapsed += (deltaMillis - gameTimePausedTotal); //Add up to the total game time, expluding pauses
  }

  //When the timer actually is counting down run this statement
  if((timerStatus & TIMER_TICKING) > 0){
    //Check the bonus/punishment duration timer
    if(tickSpeedModifierDurationLeft > 0){
      if(((tickSpeedModifierDurationLeft - deltaMillis) < 1)
      || ((tickSpeedModifierDurationLeft - deltaMillis) > tickSpeedModifierDurationLeft)){
        tickSpeedModifierDurationLeft = 0;
        stopTickSpeedModifiers();
      } else {
        tickSpeedModifierDurationLeft -= deltaMillis;
      }
    }
    //Check the game timer
    if(((gameTimeLeftCurrent - ((deltaMillis * tickSpeedModifierCurrent) / TIMER_DEFAULT_SETTING_BASE_SPEED)) < 1) //When reaching 0
    || (gameTimeLeftCurrent - ((deltaMillis * tickSpeedModifierCurrent) / TIMER_DEFAULT_SETTING_BASE_SPEED)) > gameTimeLeftCurrent){ // When overflowing (since unsigned long can't go lower than 0)
      expire();
    } else {
      gameTimeLeftCurrent -= (deltaMillis * tickSpeedModifierCurrent) / TIMER_DEFAULT_SETTING_BASE_SPEED;
      if(millisLeftThisSecond < (gameTimeLeftCurrent % 1000)){
        secondPassed = true;
      }
      millisLeftThisSecond = gameTimeLeftCurrent % 1000;
    }
  }
}

bool GameTimer::isShown(){
  return showTimerObject;
}

//Is the timer active? inlcuding paused state.
bool GameTimer::isActive(){
  return ((timerStatus & TIMER_NOT_LOOPING) == 0);
}

//Is the timer actually ticking?
bool GameTimer::isTicking(){
  return ((timerStatus & TIMER_TICKING) > 0);
}

//Is the timer actually expired?
bool GameTimer::isExpired(){
  return ((timerStatus & FLAG_EXPIRED_TIMER) > 0);
}

uint16_t GameTimer::getTimerStatus(){
  return timerStatus;
}


unsigned long GameTimer::getGameTimeLeftCurrent(){
  return gameTimeLeftCurrent;
}


unsigned long GameTimer::getGameTimeLeftSet(){
  return gameTimeLeftSet;  
}


uint16_t GameTimer::getMillisLeftThisSecond(){
  return millisLeftThisSecond;  
}


//consume the secondPassed variable, that will be set every 1 second (or equivalant while bonus/punishing)
bool GameTimer::consumeSecondHasPassed(){
  if(secondPassed == true)
  {
    secondPassed = false; //consume the value
    return true;
  }
  return false;
}


char* GameTimer::getTimerString(bool showExpired = false) {
  if(showTimerObject == false){
    return (char *) constants->emptyString;
  }
  unsigned long decis = gameTimeLeftCurrent / 100;
  unsigned long seconds = gameTimeLeftCurrent / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  decis %= 10;
  seconds %= 60;
  minutes %= 60;
  hours %= 100;
  if((timerStatus == FLAG_UNPREPARED_TIMER)){
    return (char *) constants->timerInactive[globals->getLang()];
  } else if((timerStatus == FLAG_EXPIRED_TIMER) && (showExpired == true)){
    return (char *) constants->timerExpired[globals->getLang()];
  } else{
    sprintf(timerString, "%02lu %02lu %02lu %01lu", hours, minutes, seconds, decis);
    return timerString;
  }
  return (char *) constants->emptyString;

}

void GameTimer::disarm(){
  stopTickSpeedModifiers();
  timerStatus = FLAG_DEFUSED_TIMER;
}

void GameTimer::detonate(){
  stopTickSpeedModifiers();
  timerStatus = FLAG_EXPIRED_TIMER;
}

#define SUBTITLE_PREPARED_TIMER 0
#define SUBTITLE_STARTED_TIMER 1
#define SUBTITLE_STOPPED_TIMER 2
#define SUBTITLE_PAUSED_TIMER 3
#define SUBTITLE_DEFUSED_TIMER 4
#define SUBTITLE_EXPIRED_TIMER 5
#define SUBTITLE_BONUS_TIMER 6
#define SUBTITLE_PUNISH_TIMER 7
#define SUBTITLE_UNPREPARED_TIMER 8
const char* GameTimer::getTimerSubString(){
  if(showTimerObject == false){
    return constants->emptyString;
  }
    //Serial.println("getTimerSubString");
    //Serial.println(timerSubTitle[0]);
  uint16_t lang = globals->getLang();
  if(timerStatus == FLAG_PREPARED_TIMER){return constants->timerSubtitles[lang][SUBTITLE_PREPARED_TIMER];}
  if(timerStatus == FLAG_STARTED_TIMER){return constants->timerSubtitles[lang][SUBTITLE_STARTED_TIMER];}
  if(timerStatus == FLAG_STOPPED_TIMER){return constants->timerSubtitles[lang][SUBTITLE_STOPPED_TIMER];}
  if(timerStatus == FLAG_PAUSED_TIMER){return constants->timerSubtitles[lang][SUBTITLE_PAUSED_TIMER];}
  if(timerStatus == FLAG_DEFUSED_TIMER){return constants->timerSubtitles[lang][SUBTITLE_DEFUSED_TIMER];}
  if(timerStatus == FLAG_EXPIRED_TIMER){return constants->timerSubtitles[lang][SUBTITLE_EXPIRED_TIMER];}
  if(timerStatus == FLAG_BONUS_TIMER){return constants->timerSubtitles[lang][SUBTITLE_BONUS_TIMER];}
  if(timerStatus == FLAG_PUNISH_TIMER){return constants->timerSubtitles[lang][SUBTITLE_PUNISH_TIMER];}
  if(timerStatus == FLAG_UNPREPARED_TIMER){return constants->timerSubtitles[lang][SUBTITLE_UNPREPARED_TIMER];}
  return constants->emptyString;
}







