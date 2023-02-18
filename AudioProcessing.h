
#ifndef AudioProcessing_h
#define AudioProcessing_h


#include <stdint.h>
#include <Arduino.h>
#include <AudioSample001_beep_a_normal_normaal_piepje.h>
#include <AudioSample002_beep_b_confirm_ok_piepje.h>
#include <AudioSample003_beep_c_error_error_piepje.h>
#include <AudioSample028_keypad_tick_toetsgeluid.h>
#include <AudioSample036_punish_second_tick_softtick.h>
#include <AudioSamplePingsound.h>
#include <AudioSamplePongsound.h>
#include <AudioSampleBlipsound.h>
#include <AudioSampleBlocked.h>
#include <AudioSampleCancel.h>
#include <SimpleInterfaceBeeps.h>
#include <AudioStream.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <SoftwareSerial.h>
#include "TeensyThreads.h"
#include <Audio.h>
#include "Constants.h"
#include "Globals.h"

    // AudioPlaySdWav playSdWav1;          //xy=212,424
    // AudioPlaySdWav playSdWav2;          //xy=216,484
    // AudioPlayMemory playMem1;           //xy=219,365
    // AudioMixer4 mixer1;                 //xy=488,419
    // AudioMixer4 mixer2;                 //xy=488,506
    // AudioOutputI2S2 i2s2_1;             //xy=707,454
    // AudioAnalyzePeak measuredPeaks[2];  //xy=756,525
    // AudioConnection patchCord1(AudioPlaySdWav playSdWav1, uint16_t, AudioMixer4 mixer1, uint16_t);
    // AudioConnection patchCord2(AudioPlaySdWav playSdWav1, uint16_t, AudioMixer4 mixer2, uint16_t);
    // AudioConnection patchCord3(AudioPlaySdWav playSdWav2, uint16_t, AudioMixer4 mixer1, uint16_t);
    // AudioConnection patchCord4(AudioPlaySdWav playSdWav2, uint16_t, AudioMixer4 mixer2, uint16_t);
    // AudioConnection patchCord5(AudioPlayMemory playMem1, uint16_t, AudioMixer4 mixer1, uint16_t);
    // AudioConnection patchCord6(AudioPlayMemory playMem1, uint16_t, AudioMixer4 mixer2, uint16_t);
    // AudioConnection patchCord7(AudioMixer4 mixer1, uint16_t, AudioOutputI2S2 i2s2_1, uint16_t);
    // AudioConnection patchCord8(AudioMixer4 mixer1, AudioAnalyzePeak measuredPeaks[0]);
    // AudioConnection patchCord9(AudioMixer4 mixer2, uint16_t, AudioOutputI2S2 i2s2_1, uint16_t);
    // AudioConnection patchCord10(AudioMixer4 mixer2, AudioAnalyzePeak measuredPeaks[1]);



#define AUDIO_TOTAL_CHANNELS_COUNT 5

#define AUDIO_CHANNEL_MEM_BEEPS_1 0
#define AUDIO_CHANNEL_MEM_BEEPS_2 1
#define AUDIO_CHANNEL_NOTIFICATIONS 2
#define AUDIO_CHANNEL_BACKGROUND_MUSIC_1 3
#define AUDIO_CHANNEL_BACKGROUND_MUSIC_2 4

#define AUDIO_CHANNELS_MASTER_COUNT 2
#define AUDIO_CHANNELS_BACKGROUND_COUNT 2
#define AUDIO_CHANNELS_FX_COUNT 3

#define AUDIO_MIXER_LOCATION_BACKGROUND_1 0
#define AUDIO_MIXER_LOCATION_BACKGROUND_2 1
#define AUDIO_MIXER_LOCATION_NOTIFICATIONS 2
#define AUDIO_MIXER_LOCATION_MEMORY_1 0
#define AUDIO_MIXER_LOCATION_MEMORY_2 1

#define AUDIO_GAIN_MASTER 0.20f //TODO: set this between 0.8-1.0
#define AUDIO_GAIN_MEMORY 0.7f
#define AUDIO_GAIN_NOTIFICATIONS 0.7f
#define AUDIO_GAIN_BACKGROUND 0.3f


class AudioProcessing {
    

  private:
    #define MAX_COUNT_PLAY_REQUEST_PACKAGES 5
    struct PlayRequestPackage {
      bool isNewRequest;
      char * filePointer;
      uint16_t channel;
      bool killCurrentChannel;
      bool killAllChannels;
    };
    PlayRequestPackage playRequestPackages[MAX_COUNT_PLAY_REQUEST_PACKAGES];

    SimpleInterfaceBeeps _beeps;
    Constants *constants;
    Globals *globals;
    float mixerMasterGains[AUDIO_CHANNELS_MASTER_COUNT];
    float mixerBackgroundGains[AUDIO_CHANNELS_BACKGROUND_COUNT];
    float mixerFxGains[AUDIO_CHANNELS_FX_COUNT];

    bool playBackground;
    const char * lastPlayingFilename[AUDIO_TOTAL_CHANNELS_COUNT];
    const char * backgroundSoundFile;
    bool allowRequests = true;
    const char * backgroundMusicInterruptedFile;
    bool isPlayingIntensifiedBackgroundMusic = false;
    uint32_t backgroundMusicInterruptTimeSlice = 0; //This has no usecase yet.
    uint16_t background1MixerLocation = AUDIO_MIXER_LOCATION_BACKGROUND_1;
    uint16_t background2MixerLocation = AUDIO_MIXER_LOCATION_BACKGROUND_2;
  public:

    AudioProcessing(Constants *constantsObj, Globals *globalsObj);
    void begin();
    void blockInterrupts();
    void allowInterupts();
    void playSound(const char * fileName, uint16_t channel, uint8_t killConcurrentChannel = 1, uint8_t killAllChannels = 0);
    bool requestAlreadyExistsInRequestPackages(const char *fileName, uint16_t channel);
    float getPeak(uint16_t peak);
    float getGain(uint16_t mixer);
    void setGain(uint16_t audioChannel, float gain, bool isTemporarely = false);
    void setTemporaryGain(uint16_t audioChannel, float gain);
    void loopBackgroundMusic(bool breakLoop = false);
    void playBackgroundMusic(char * fileName, uint16_t audioChannel = AUDIO_CHANNEL_BACKGROUND_MUSIC_1);
    void stopBackgroundMusic();
    void lowerGainBackgroundMusic(float fraction = 0.35);
    void resetGainBackgroundMusic();
    void interruptBackgroundMusic(char * fileName);
    void resumeFromInterruptedBackgroundMusic();
    bool getIsPlayingIntensifiedBackgroundMusic();
    bool channelIsPlaying(uint16_t channel);
    uint16_t getMemBeepsChannel();
    uint16_t getNotificationsChannel();
    uint16_t getBackgroundChannel();
    const char * getLastPlayedFileNameForChannel(uint16_t channel);
    bool isLastPlayedSoundOnChannel(char * file, uint16_t channel);
    void stopIfThisFileIsPlaying(char * file, uint16_t channel);
    void playLoop();
    void loop();
};



 #endif