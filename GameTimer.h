#ifndef GameTimer_h
#define GameTimer_h


#include <stdint.h>
#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"



class GameTimer {
  
  private:
    Constants *constants;
    Globals *globals;
    unsigned long gameTimeLeftSet; //At what time is this timer set to expire. Resetting timer will use this value.
    unsigned long gameTimeLeftCurrent; //How much millis there are left on this timer.
    unsigned long gameTimeStartedAt; //At which millis() time did this timer start?
    unsigned long gameTimePausedTotal; //How much time has been elapsed since this timer started? This timer will stop at pauses.
    unsigned long gameTimeElapsed; //How much time has been elapsed since this timer started? This timer will stop at pauses.
    unsigned long gameTimeElapsedTotal; //How much time has been elapsed since this timer started? This counter will count raw play time from start till stop, without calculating bonusses, pauses and punishments.
    unsigned long lastMillisValue;//What was last millis value?
    uint16_t millisLeftThisSecond; //How many millis are left in the current second counting down?
    uint16_t tickSpeedModifierBase; //How fast does this timer tick as a setting? 100 = normal speed. <100 is slower, >100 is faster
    uint16_t tickSpeedModifierCurrent; //How fast does this timer tick currently? 100 = normal speed. <100 is slower, >100 is faster
    unsigned long tickSpeedModifierDurationLeft; //When this is positive there will be modifier applied to the tick speed modifier. 
    unsigned long bonusTickSpeedModifierDuration;
    uint16_t bonusTickSpeedModifierValue;
    unsigned long punishTickSpeedModifierDuration;
    uint16_t punishTickSpeedModifierValue;
    bool secondPassed;
    bool hour2MarkPassed;
    bool hour1MarkPassed;
    bool mins30Passed;
    bool mins10Passed;
    bool mins5Passed;
    bool mins1Passed;
    bool secs10Passed;
    bool showTimerObject;
    uint16_t timerStatus;
    char timerString[16];

  public:
    GameTimer(Constants *constantsObj, Globals *globalsObj);
    void reset(unsigned long expiresInMillis = 0);
    void show();
    void hide();
    void prepare();
    bool start();
    void loop();
    void expire();
    void setTickSpeedModifiers(uint16_t value, unsigned long duration);
    void stopTickSpeedModifiers();
    bool getTickSpeedModifierIsActive();
    bool getTickSpeedModifierIsFaster();
    bool getTickSpeedModifierIsSlower();
    uint32_t getDurationLeftForSpeedModifier();
    bool isShown();
    bool isActive();
    bool isTicking();
    bool isExpired();
    uint16_t getTimerStatus();
    uint16_t getMillisLeftThisSecond();
    bool consumeSecondHasPassed();
    unsigned long getGameTimeLeftCurrent();
    unsigned long getGameTimeLeftSet();
    char* getTimerString(bool showExpired = false);
    const char* getTimerSubString();
    void disarm();
    void detonate();
};


 #endif