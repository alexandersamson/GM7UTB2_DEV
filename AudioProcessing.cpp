#include "core_pins.h"
#include "Audio.h"
#include "AudioProcessing.h"




#define TIMER_TICK 0
#define TIMER_TICK_FAST 1
#define TIMER_STARTED 2
#define TIMER_PAUSED 3
#define TIMER_STOPPED 4
#define TIMER_2_HOURS_LEFT 5
#define TIMER_1_HOUR_LEFT 6
#define TIMER_30_MINS_LEFT 7
#define TIMER_10_MINS_LEFT 8
#define TIMER_5_MINS_LEFT 9
#define TIMER_1_MIN_LEFT 10
#define TIMER_10_SECS_LEFT 11
#define TIMER_EXPIRED 12
#define TIMER_DEFUSED 13
#define TIMER_BONUS 14
#define TIMER_PUNISH 15
#define SYSTEM_CONFIRMED 16
#define SYSTEM_ERROR 17
#define SYSTEM_BEEP 18
#define SYSTEM_TICK 19
#define SYSTEM_PING 20
#define SYSTEM_PONG 21
#define SYSTEM_BLIP 22
#define SYSTEM_LOW_BATTERY 23


// GUItool: begin automatically generated code
AudioPlaySdWav           playNotificationsSdWav; //xy=261,393
AudioPlaySdWav           playBackgroundSdWav2;     //xy=300,169
AudioPlaySdWav           playBackgroundSdWav1;     //xy=301,129
AudioPlayMemory          playMem1;       //xy=301,303
AudioPlayMemory          playMem2; //xy=302,348
AudioMixer4              mixerFxL;         //xy=543,313
AudioMixer4              mixerFxR; //xy=544,395
AudioMixer4              mixerBackgroundR; //xy=550,188
AudioMixer4              mixerBackgroundL;         //xy=552,118
AudioMixer4              mixerMasterL;         //xy=766,239
AudioMixer4              mixerMasterR; //xy=766,323
AudioAnalyzePeak         measuredPeaks[2];  //xy=756,525
AudioOutputI2S2          i2sOut;         //xy=995,280
AudioConnection          patchCord1(playNotificationsSdWav, 0, mixerFxL, 2);
AudioConnection          patchCord2(playNotificationsSdWav, 1, mixerFxR, 2);
AudioConnection          patchCord3(playBackgroundSdWav2, 0, mixerBackgroundL, 1);
AudioConnection          patchCord4(playBackgroundSdWav2, 1, mixerBackgroundR, 1);
AudioConnection          patchCord5(playBackgroundSdWav1, 0, mixerBackgroundL, 0);
AudioConnection          patchCord6(playBackgroundSdWav1, 0, mixerBackgroundR, 0);
AudioConnection          patchCord7(playMem1, 0, mixerFxL, 0);
AudioConnection          patchCord8(playMem1, 0, mixerFxR, 0);
AudioConnection          patchCord9(playMem2, 0, mixerFxL, 1);
AudioConnection          patchCord10(playMem2, 0, mixerFxR, 1);
AudioConnection          patchCord11(mixerFxL, 0, mixerMasterL, 1);
AudioConnection          patchCord12(mixerFxR, 0, mixerMasterR, 1);
AudioConnection          patchCord13(mixerBackgroundR, 0, mixerMasterR, 0);
AudioConnection          patchCord14(mixerBackgroundL, 0, mixerMasterL, 0);
AudioConnection          patchCord15(mixerMasterL, measuredPeaks[0]);
AudioConnection          patchCord16(mixerMasterL, 0, i2sOut, 0);
AudioConnection          patchCord17(mixerMasterR, measuredPeaks[1]);
AudioConnection          patchCord18(mixerMasterR, 0, i2sOut, 1);
// GUItool: end automatically generated code






AudioProcessing::AudioProcessing(Constants *constantsObj, Globals *globalsObj) : constants(constantsObj), globals(globalsObj){
  SimpleInterfaceBeeps _beeps = SimpleInterfaceBeeps();
}



void AudioProcessing::begin() {
  AudioMemory(40);
  mixerMasterGains[0] = AUDIO_GAIN_MASTER;
  mixerMasterGains[1] = AUDIO_GAIN_MASTER;
  mixerBackgroundGains[0] = AUDIO_GAIN_BACKGROUND;
  mixerBackgroundGains[1] = AUDIO_GAIN_BACKGROUND;
  mixerFxGains[0] = AUDIO_GAIN_MEMORY;
  mixerFxGains[1] = AUDIO_GAIN_MEMORY;
  mixerFxGains[2] = AUDIO_GAIN_NOTIFICATIONS;

  mixerMasterL.gain(0, mixerMasterGains[0]);
  mixerMasterR.gain(0, mixerMasterGains[0]);
  mixerMasterL.gain(1, mixerMasterGains[1]);
  mixerMasterR.gain(1, mixerMasterGains[1]);

  mixerBackgroundL.gain(0, mixerBackgroundGains[0]);
  mixerBackgroundR.gain(0, mixerBackgroundGains[0]);
  mixerBackgroundL.gain(1, mixerBackgroundGains[1]);
  mixerBackgroundR.gain(1, mixerBackgroundGains[1]);

  mixerFxL.gain(0, mixerFxGains[0]);
  mixerFxR.gain(0, mixerFxGains[0]);
  mixerFxL.gain(1, mixerFxGains[1]);
  mixerFxR.gain(1, mixerFxGains[1]);
  mixerFxL.gain(2, mixerFxGains[2]);
  mixerFxR.gain(3, mixerFxGains[2]);

  //default turn on background music
  playBackground = true;

  //Serial.println("AudioProcessing>>>Audio files prepared.");
}

void AudioProcessing::blockInterrupts(){
  NVIC_DISABLE_IRQ(IRQ_SOFTWARE);
  allowRequests = false;
}

void AudioProcessing::allowInterupts(){
  NVIC_ENABLE_IRQ(IRQ_SOFTWARE);
  allowRequests = true;
}


//There are 5 channels:
//playMem1 (AUDIO_CHANNEL_MEM_BEEPS_1) = For small beeps and ticks, ticks and notification sounds played from flash memory (less than 1 second)
//playMem2 (AUDIO_CHANNEL_MEM_BEEPS_2) = Same as above. Second voice channel, for concurrent playback of mem sounds
//playNotificationsSdWav (notifications) = for short, but more substantial notifications and messaging (about 1 to 5 seconds)
//SD2 (background music) = for longer tracks, meant as background music or i (longer than 1 minute)
void AudioProcessing::playSound(const char *fileName, uint16_t channel, uint8_t killConcurrentChannel, uint8_t killAllChannels) {
  if(allowRequests == false){
    return;
  }
  if(channel >= AUDIO_TOTAL_CHANNELS_COUNT ){
    return;
  }
  if(fileName == NULL || fileName == constants->emptyString){
    return;
  }

  if(requestAlreadyExistsInRequestPackages(fileName, channel)){
    return;
  }

  for(int i = 0; i < MAX_COUNT_PLAY_REQUEST_PACKAGES; i++){
    if(playRequestPackages[i].isNewRequest != true){
      playRequestPackages[i].isNewRequest = true;
      playRequestPackages[i].channel = channel;
      playRequestPackages[i].killCurrentChannel = killConcurrentChannel;
      playRequestPackages[i].killAllChannels = killAllChannels;
      playRequestPackages[i].filePointer = fileName;
      return;
    }
  }  
}


bool AudioProcessing::requestAlreadyExistsInRequestPackages(const char *fileName, uint16_t channel){
  for(int i = 0; i < MAX_COUNT_PLAY_REQUEST_PACKAGES; i++){
    if(playRequestPackages[i].isNewRequest == true && playRequestPackages[i].filePointer == fileName && playRequestPackages[i].channel == channel){
        return true;
    }
  }
  return false;
}



float AudioProcessing::getPeak(uint16_t peak){
  if (measuredPeaks[peak].available()) {
    return measuredPeaks[peak].read();
  } else {
    return 0.0;
  }
}

void AudioProcessing::setGain(uint16_t audioChannel, float gain, bool isTemporarely){
  // Serial.println("GAIN RQ:");
  // Serial.print("Channel ");Serial.print(audioChannel);Serial.print(" with gain:  ");Serial.print(gain);Serial.print(" Is temp?:  ");Serial.println(isTemporarely);
  // Serial.print("BG1 Gain ");Serial.print(mixerBackgroundGains[0]);
  // Serial.print("  BG2 Gain ");Serial.println(mixerBackgroundGains[1]);

  if(audioChannel == AUDIO_CHANNEL_MEM_BEEPS_1){
    if(isTemporarely == false){
      mixerFxGains[AUDIO_MIXER_LOCATION_MEMORY_1] = gain;
    }
    mixerFxL.gain(AUDIO_MIXER_LOCATION_MEMORY_1, gain);
    mixerFxR.gain(AUDIO_MIXER_LOCATION_MEMORY_1, gain);
    return;
  }
  if(audioChannel == AUDIO_CHANNEL_MEM_BEEPS_2){
    if(isTemporarely == false){
      mixerFxGains[AUDIO_MIXER_LOCATION_MEMORY_2] = gain;
    }
    mixerFxL.gain(AUDIO_MIXER_LOCATION_MEMORY_2, gain);
    mixerFxR.gain(AUDIO_MIXER_LOCATION_MEMORY_2, gain);
    return;
  }
  if(audioChannel == AUDIO_CHANNEL_NOTIFICATIONS){
    if(isTemporarely == false){
      mixerFxGains[AUDIO_CHANNEL_NOTIFICATIONS] = gain;
    }
    mixerFxL.gain(AUDIO_MIXER_LOCATION_NOTIFICATIONS, gain);
    mixerFxR.gain(AUDIO_MIXER_LOCATION_NOTIFICATIONS, gain);
    return;
  }
  if(audioChannel == AUDIO_CHANNEL_BACKGROUND_MUSIC_1){
    if(isTemporarely == false){
      mixerBackgroundGains[AUDIO_CHANNEL_BACKGROUND_MUSIC_1];
    }
    mixerBackgroundL.gain(AUDIO_MIXER_LOCATION_BACKGROUND_1, gain);
    mixerBackgroundR.gain(AUDIO_MIXER_LOCATION_BACKGROUND_1, gain);
    return;
  }
  if(audioChannel == AUDIO_CHANNEL_BACKGROUND_MUSIC_2){
    if(isTemporarely == false){
      mixerBackgroundGains[AUDIO_CHANNEL_BACKGROUND_MUSIC_2];
    }
    mixerBackgroundL.gain(AUDIO_MIXER_LOCATION_BACKGROUND_2, gain);
    mixerBackgroundR.gain(AUDIO_MIXER_LOCATION_BACKGROUND_2, gain);
    return;
  }
}

void AudioProcessing::setTemporaryGain(uint16_t audioChannel, float gain){
  setGain(audioChannel, gain, true);
}


float AudioProcessing::getGain(uint16_t audioChannel){
  if(audioChannel == AUDIO_CHANNEL_MEM_BEEPS_1){
    return mixerFxGains[AUDIO_MIXER_LOCATION_MEMORY_1];
  }
  if(audioChannel == AUDIO_CHANNEL_MEM_BEEPS_2){
    return mixerFxGains[AUDIO_MIXER_LOCATION_MEMORY_2];
  }
  if(audioChannel == AUDIO_CHANNEL_NOTIFICATIONS){
    return mixerFxGains[AUDIO_MIXER_LOCATION_NOTIFICATIONS];
  }
  if(audioChannel == AUDIO_CHANNEL_BACKGROUND_MUSIC_1){
    return mixerBackgroundGains[AUDIO_MIXER_LOCATION_BACKGROUND_1];
  }
  if(audioChannel == AUDIO_CHANNEL_BACKGROUND_MUSIC_2){
    return mixerBackgroundGains[AUDIO_MIXER_LOCATION_BACKGROUND_2];
  }
  return 0.0f;
}

void AudioProcessing::loopBackgroundMusic(bool breakLoop){
  if(playBackground == false){
    return;
  }
  if(backgroundSoundFile == NULL || backgroundSoundFile == constants->emptyString){
    return;
  }
  if((breakLoop == true) || (getLastPlayedFileNameForChannel(AUDIO_CHANNEL_BACKGROUND_MUSIC_1) != backgroundSoundFile) || (channelIsPlaying(AUDIO_CHANNEL_BACKGROUND_MUSIC_1) == false)){
      playSound(backgroundSoundFile, AUDIO_CHANNEL_BACKGROUND_MUSIC_1, 1, 0);
  } 
}

void AudioProcessing::playBackgroundMusic(char * fileName, uint16_t audioChannel){
  if(audioChannel == AUDIO_CHANNEL_BACKGROUND_MUSIC_1){
   backgroundSoundFile = fileName;
  } else {
    playSound(fileName, AUDIO_CHANNEL_BACKGROUND_MUSIC_2);
  }
  playBackground = true;
}


void AudioProcessing::stopBackgroundMusic(){
  playBackground = false;
  playBackgroundSdWav1.stop();
}


void AudioProcessing::lowerGainBackgroundMusic(float fraction){
  setTemporaryGain(AUDIO_CHANNEL_BACKGROUND_MUSIC_1, (float) (mixerBackgroundGains[AUDIO_MIXER_LOCATION_BACKGROUND_1] * fraction));
  setTemporaryGain(AUDIO_CHANNEL_BACKGROUND_MUSIC_2, (float) (mixerBackgroundGains[AUDIO_MIXER_LOCATION_BACKGROUND_2] * fraction));
}

void AudioProcessing::resetGainBackgroundMusic(){
  float gain = (isPlayingIntensifiedBackgroundMusic == true) ? 0.0f : mixerBackgroundGains[AUDIO_MIXER_LOCATION_BACKGROUND_1];
  setTemporaryGain(AUDIO_CHANNEL_BACKGROUND_MUSIC_1, gain);
  setTemporaryGain(AUDIO_CHANNEL_BACKGROUND_MUSIC_2, mixerBackgroundGains[AUDIO_MIXER_LOCATION_BACKGROUND_2]);
}

void AudioProcessing::interruptBackgroundMusic(char * fileName){
  isPlayingIntensifiedBackgroundMusic = true;
  setTemporaryGain(AUDIO_CHANNEL_BACKGROUND_MUSIC_1, 0.0f);
  setGain(AUDIO_CHANNEL_BACKGROUND_MUSIC_2, mixerBackgroundGains[AUDIO_MIXER_LOCATION_BACKGROUND_2]);
  playBackgroundMusic(fileName, AUDIO_CHANNEL_BACKGROUND_MUSIC_2);
}

void AudioProcessing::resumeFromInterruptedBackgroundMusic(){
  isPlayingIntensifiedBackgroundMusic = false;
  setTemporaryGain(AUDIO_CHANNEL_BACKGROUND_MUSIC_1, mixerBackgroundGains[AUDIO_MIXER_LOCATION_BACKGROUND_1]);
  setGain(AUDIO_CHANNEL_BACKGROUND_MUSIC_2, 0.0f);
  playBackgroundSdWav2.stop();
}


bool AudioProcessing::getIsPlayingIntensifiedBackgroundMusic(){
  return isPlayingIntensifiedBackgroundMusic;
}

bool AudioProcessing::channelIsPlaying(uint16_t channel){
  if(channel == AUDIO_CHANNEL_MEM_BEEPS_1){return playMem1.isPlaying();}
  if(channel == AUDIO_CHANNEL_MEM_BEEPS_2){return playMem2.isPlaying();}
  if(channel == AUDIO_CHANNEL_NOTIFICATIONS){return playNotificationsSdWav.isPlaying();}
  if(channel == AUDIO_CHANNEL_BACKGROUND_MUSIC_1){return playBackgroundSdWav1.isPlaying();}
  if(channel == AUDIO_CHANNEL_BACKGROUND_MUSIC_2){return playBackgroundSdWav2.isPlaying();}
  return true;
}

uint16_t AudioProcessing::getMemBeepsChannel(){
  return AUDIO_CHANNEL_MEM_BEEPS_1;
}

uint16_t AudioProcessing::getNotificationsChannel(){
  return AUDIO_CHANNEL_NOTIFICATIONS;
}

uint16_t AudioProcessing::getBackgroundChannel(){
  return AUDIO_CHANNEL_BACKGROUND_MUSIC_1;
}

const char * AudioProcessing::getLastPlayedFileNameForChannel(uint16_t channel){
 if(channel < AUDIO_TOTAL_CHANNELS_COUNT){
   return lastPlayingFilename[channel];
 }
 return constants->emptyString;
}

bool AudioProcessing::isLastPlayedSoundOnChannel(char * file, uint16_t channel){
  if(strcmp(file, getLastPlayedFileNameForChannel(channel)) == 0){
    return true;
  }
  return false;
}

void AudioProcessing::stopIfThisFileIsPlaying(char * file, uint16_t channel){
  if(channel == AUDIO_CHANNEL_NOTIFICATIONS){
    if(playNotificationsSdWav.isPlaying()){
      if(isLastPlayedSoundOnChannel((char *) file, channel)){
        playNotificationsSdWav.stop();
      }
    }
  }
  if(channel == AUDIO_CHANNEL_BACKGROUND_MUSIC_1){
    if(playBackgroundSdWav1.isPlaying()){
      if(isLastPlayedSoundOnChannel((char *) file, channel)){
        playBackgroundSdWav1.stop();
      }
    }
  }
  if(channel == AUDIO_CHANNEL_BACKGROUND_MUSIC_2){
    if(playBackgroundSdWav2.isPlaying()){
      if(isLastPlayedSoundOnChannel((char *) file, channel)){
        playBackgroundSdWav2.stop();
      }
    }
  }
  if((channel == AUDIO_CHANNEL_MEM_BEEPS_1) || (channel == AUDIO_CHANNEL_MEM_BEEPS_2)){
    if(playMem1.isPlaying()){
      if(isLastPlayedSoundOnChannel((char *) file, channel)){
        playMem1.stop();
      }
    }
    if(playMem2.isPlaying()){
      if(isLastPlayedSoundOnChannel((char *) file, channel)){
        playMem2.stop();
      }
    }
  }
}




void AudioProcessing::playLoop(){
  //DEBUGGING .........................................................
  // uint16_t debugFound = false;
  // for(int i = 0; i < MAX_COUNT_PLAY_REQUEST_PACKAGES; i++){
  //     if(playRequestPackages[i].isNewRequest == true){
  //        Serial.print(i);Serial.print(": ");Serial.print(playRequestPackages[i].filePointer); Serial.print("  ");
  //        debugFound = true;
  //     }
  // } 
  // if(debugFound){
  //   Serial.println(); 
  // } 
  //...................................................................
  bool foundPackage = false;
  uint16_t packageId = 0;
  for(int i = 0; i < MAX_COUNT_PLAY_REQUEST_PACKAGES; i++){
      if(playRequestPackages[i].isNewRequest == true){
        packageId = i;
        foundPackage = true;
        playRequestPackages[i].isNewRequest = false;
        break;
      }
  }  
  if(foundPackage == false){
    return;
  }
  
  // Serial.print("Playing: ");
  // Serial.print(playRequestPackages[packageId].filePointer);
  // Serial.print ("  at channel: ");
  // Serial.print(playRequestPackages[packageId].channel);
  // Serial.print ("  @ time: ");
  // Serial.println(millis());
  // Serial.println(AudioMemoryUsageMax());
  // Serial.flush();
  //First check if the supplied audio string is one of the supplied memory audio files. If found, use sketch memory sound file on its own channel instead.
  //This will reduce calls to the SD card, which can be slow.
  if(
      strcmp(playRequestPackages[packageId].filePointer, constants->audioMemBeep) == 0
    || strcmp(playRequestPackages[packageId].filePointer, constants->audioMemBlip) == 0
    || strcmp(playRequestPackages[packageId].filePointer, constants->audioMemBlocked) == 0
    || strcmp(playRequestPackages[packageId].filePointer, constants->audioMemConfirm) == 0
    || strcmp(playRequestPackages[packageId].filePointer, constants->audioMemError) == 0
    || strcmp(playRequestPackages[packageId].filePointer, constants->audioMemPing) == 0
    || strcmp(playRequestPackages[packageId].filePointer, constants->audioMemPong) == 0
    || strcmp(playRequestPackages[packageId].filePointer, constants->audioMemTick) == 0
    || strcmp(playRequestPackages[packageId].filePointer, constants->audioMemTickSoft) == 0
    || strcmp(playRequestPackages[packageId].filePointer, constants->audioMemCancel) == 0
    )
  {
    playRequestPackages[packageId].channel = AUDIO_CHANNEL_MEM_BEEPS_1;
  } else if(playRequestPackages[packageId].channel == AUDIO_CHANNEL_MEM_BEEPS_1){
    playRequestPackages[packageId].channel = AUDIO_CHANNEL_NOTIFICATIONS;
  }

  //Serial.print("AudioProcessing::playSound>>> request to play sound: ");
  //Serial.println(fileName);
  if (playRequestPackages[packageId].killAllChannels == true) {
      playMem1.stop();
      playMem2.stop();
      playNotificationsSdWav.stop();
      playBackgroundSdWav1.stop();
      playBackgroundSdWav2.stop();
  }
  //If the sound is a memory audio file, just play it from channel 2 or 3, since it's a short little beep of some sorts.
  if ((playRequestPackages[packageId].channel == AUDIO_CHANNEL_MEM_BEEPS_1) || (playRequestPackages[packageId].channel == AUDIO_CHANNEL_MEM_BEEPS_2)) {
    unsigned int* getMemBeep = _beeps.getBeepByCharArray(playRequestPackages[packageId].filePointer);
    if(!playMem1.isPlaying()){
      if (getMemBeep) { playMem1.play(getMemBeep); }
    } else if(!playMem2.isPlaying()){
      if (getMemBeep) { playMem2.play(getMemBeep); }
    } else if (playRequestPackages[packageId].killCurrentChannel == true) {
      if (getMemBeep) { playMem1.play(getMemBeep); } 
    }
    return;
  }

  if (playRequestPackages[packageId].channel == AUDIO_CHANNEL_NOTIFICATIONS) {
    if ((playRequestPackages[packageId].killCurrentChannel == true) || (!playNotificationsSdWav.isPlaying())) {
      playNotificationsSdWav.play(playRequestPackages[packageId].filePointer);
      lastPlayingFilename[AUDIO_CHANNEL_NOTIFICATIONS] = playRequestPackages[packageId].filePointer;
      return;
    }
  }

  if (playRequestPackages[packageId].channel == AUDIO_CHANNEL_BACKGROUND_MUSIC_1) {
    if ((playRequestPackages[packageId].killCurrentChannel == 1) || (!playBackgroundSdWav1.isPlaying())) {
      playBackgroundSdWav1.play(playRequestPackages[packageId].filePointer);
      lastPlayingFilename[AUDIO_CHANNEL_BACKGROUND_MUSIC_1] = playRequestPackages[packageId].filePointer;
      return;
    }
  }

  if (playRequestPackages[packageId].channel == AUDIO_CHANNEL_BACKGROUND_MUSIC_2) {
    if ((playRequestPackages[packageId].killCurrentChannel == 1) || (!playBackgroundSdWav2.isPlaying())) {
      playBackgroundSdWav2.play(playRequestPackages[packageId].filePointer);
      lastPlayingFilename[AUDIO_CHANNEL_BACKGROUND_MUSIC_2] = playRequestPackages[packageId].filePointer;
      return;
    }
  }
  return;
}





void AudioProcessing::loop(){
  playLoop();
  loopBackgroundMusic(false);
}

