
//GAME AND SYS DEFINES
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





#include <Arduino.h>
#include <ILI9341_T4.h>
#include <tgx.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <SoftwareSerial.h>
#include "TeensyThreads.h"
#include <Chrono.h>
#include "USBHost_t36.h"
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include "Constants.h"
#include "Globals.h"
#include "FileSystem.h"
#include "AudioProcessing.h"
#include <WS2812Serial.h>
#include "Gm7Can.h"
#include "GameTimer.h"
#include "Gui.h"
#include "GuiButtons.h"
#include "Game.h"
#include "Interface.h"
#include "Sprites.h"
#include "GameModule.h"
#include "GMDisarmCode.h"
#include "GMExternalModule.h"
#include "Prompt.h"
#include "PromptDeviceInfo.h"
#include "PromptWelcomeScreen.h"
#include "PromptShowHints.h"
#include "NotificationBadge.h"




//setting_timer_main=3610
//setting_client_api_key=5y654y5v546647v6trvtyy6thv6hgjgh


//
// DEFAULT WIRING USING SPI 0 ON TEENSY 4/4.1
//
#define PIN_SCK 13   // mandatory
#define PIN_MISO 12  // mandatory
#define PIN_MOSI 11  // mandatory
#define PIN_DC 10    // mandatory, can be any pin but using pin 10 (or 36 or 37 on T4.1) provides greater performance

#define PIN_CS 9           // optional (but recommended), can be any pin.
#define PIN_RESET 6        // optional (but recommended), can be any pin.
#define PIN_BACKLIGHT 255  // optional, set this only if the screen LED pin is connected directly to the Teensy.
#define PIN_TOUCH_IRQ 255  // optional, set this only if the touchscreen is connected on the same SPI bus
#define PIN_TOUCH_CS 255   // optional, set this only if the touchscreen is connected on the same spi bus




// the screen driver object
ILI9341_T4::ILI9341Driver tft(PIN_CS, PIN_DC, PIN_SCK, PIN_MOSI, PIN_MISO, PIN_RESET, PIN_TOUCH_CS, PIN_TOUCH_IRQ);


// 2 diff buffers with 12K memory each
ILI9341_T4::DiffBuffStatic<12288> diff1;
ILI9341_T4::DiffBuffStatic<12288> diff2;


// Screen size in landscape mode
#define LX 320
#define LY 240

// 30MHz SPI. can do better with short wires
#define SPI_SPEED 78000000
#define SPI_SPEED_READ 32000000

// the framebuffers
DMAMEM uint16_t internal_fb[LX * LY];  // used by the library for buffering (in DMAMEM)
DMAMEM uint16_t frameBuffer[LX * LY];  // the main framebuffer we draw onto.


//interval timers
Chrono chronoTimerFps2;
Chrono chronoTimerFps60;
Chrono chronoTimerFps100;
Chrono chronoTimerFps200;

// namespace for draw graphics primitives
using namespace tgx;

// image that encapsulates the framebuffer fb.
Image<RGB565> tgxImage(frameBuffer, LX, LY);


//CAN BUS
Gm7Can gm7Can;



// Finally include the sketch includes
Constants constants = Constants();
Globals globals = Globals(&constants);

//FILESYSTEM
FileSystem fileSystem = FileSystem(&constants, &globals);
Sprites sprites = Sprites(&constants);
AudioProcessing audioProcessor = AudioProcessing(&constants, &globals);
GameTimer gameTimerMain = GameTimer(&constants, &globals);


//INTERFACE
Interface interface = Interface(&fileSystem, &gm7Can, &tgxImage, &sprites, &audioProcessor, &constants, &globals, &tft, frameBuffer);





//GAME MODULES & GAME
GameTimer validationTimerDisarmCode = GameTimer(&constants, &globals);
GameTimer internalModuleTimerDisarmCode = GameTimer(&constants, &globals);

//Internal game modules
GMDisarmCode moduleDisarmCode = GMDisarmCode(&tgxImage, &sprites, &interface, &internalModuleTimerDisarmCode, &validationTimerDisarmCode, &constants, &globals);

//External game module slots
GMExternalModule externalModule0 = GMExternalModule(&tgxImage, &sprites, &interface, &internalModuleTimerDisarmCode, &validationTimerDisarmCode, &constants, &globals);
GMExternalModule externalModule1 = GMExternalModule(&tgxImage, &sprites, &interface, &internalModuleTimerDisarmCode, &validationTimerDisarmCode, &constants, &globals);
GMExternalModule externalModule2 = GMExternalModule(&tgxImage, &sprites, &interface, &internalModuleTimerDisarmCode, &validationTimerDisarmCode, &constants, &globals);
GMExternalModule externalModule3 = GMExternalModule(&tgxImage, &sprites, &interface, &internalModuleTimerDisarmCode, &validationTimerDisarmCode, &constants, &globals);

GameModule * modules[MODULES_COUNT] = { &moduleDisarmCode, &externalModule0, &externalModule1, &externalModule2, &externalModule3 };

Game game = Game(&interface, &gameTimerMain, &audioProcessor, &constants, &globals, modules, MODULES_COUNT);


//PROMPTS & GUI
GuiButtons guiButtons = GuiButtons(&tgxImage, &sprites, &constants, &globals);

NotificationBadge notificationBadge = NotificationBadge(&constants, &globals, &tgxImage, &sprites, &guiButtons);

const uint16_t promptsCount = 3;
PromptWelcomeScreen promptWelcomeScreen = PromptWelcomeScreen(&tgxImage, &sprites, &constants, &globals, &interface, &guiButtons);
PromptDeviceInfo    promptDeviceInfo =    PromptDeviceInfo(&tgxImage, &sprites, &constants, &globals, &interface, &guiButtons);
PromptShowHints     promptShowHints =    PromptShowHints(&tgxImage, &sprites, &constants, &globals, &interface, &guiButtons);
Prompt * prompts[promptsCount] = { &promptWelcomeScreen, &promptDeviceInfo, &promptShowHints };



Gui gui = Gui(&tgxImage, &sprites, &interface, &constants, &globals, &guiButtons, &notificationBadge, prompts, promptsCount);






//USB
USBHost usbHostController;
USBHub hub1(usbHostController);
MouseController mouse1(usbHostController);
USBHIDParser hid1(usbHostController);
USBHIDParser hid2(usbHostController);
USBDriver *drivers[] = { &hub1, &hid1, &hid2 };
#define CNT_DEVICES (sizeof(drivers) / sizeof(drivers[0]))
const char *driver_names[CNT_DEVICES] = { "Hub1", "HID1", "HID2" };
bool driver_active[CNT_DEVICES] = { false, false, false };
USBHIDInput *hiddrivers[] = { &mouse1 };
#define CNT_HIDDEVICES (sizeof(hiddrivers) / sizeof(hiddrivers[0]))
const char *hid_driver_names[CNT_HIDDEVICES] = { "Mouse1" };
bool hid_driver_active[CNT_HIDDEVICES] = { false };
bool show_changed_only = false;




//NEOPIXEL
#define NEOPIXEL_PIN 20 //Teensy 4.1 TX pin on Serial5
#define NEOPIXEL_NUMPIXELS 10
byte drawingMemory[NEOPIXEL_NUMPIXELS*3];         //  3 bytes per LED
DMAMEM byte displayMemory[NEOPIXEL_NUMPIXELS*12]; // 12 bytes per LED
WS2812Serial neopixelSerial(NEOPIXEL_NUMPIXELS, displayMemory, drawingMemory, NEOPIXEL_PIN, WS2812_GRB);







//MAIN FUNCTIONS

// draw a filled rectangle in the framebuffer
void drawRectFilled(uint16_t *fb, int x, int y, int lx, int ly, uint16_t color) {
  uint16_t *p = fb + x + (LX * y);
  for (int j = 0; j < ly; j++) {
    for (int i = 0; i < lx; i++) p[i] = color;
    p += LX;
  }
}


/** fill a framebuffer with a given color */
void clear(uint16_t *fb, uint16_t color = 0) {
  for (int i = 0; i < LX * LY; i++) fb[i] = color;
}

/** draw a disk centered at (x,y) with radius r and color col on the framebuffer fb */
void drawDisk(uint16_t *fb, double x, double y, double r, uint16_t col) {
  int xmin = (int)(x - r);
  int xmax = (int)(x + r);
  int ymin = (int)(y - r);
  int ymax = (int)(y + r);
  if (xmin < 0) xmin = 0;
  if (xmax >= LX) xmax = LX - 1;
  if (ymin < 0) ymin = 0;
  if (ymax >= LY) ymax = LY - 1;
  const double r2 = r * r;
  for (int j = ymin; j <= ymax; j++) {
    double dy2 = (y - j) * (y - j);
    for (int i = xmin; i <= xmax; i++) {
      const double dx2 = (x - i) * (x - i);
      if (dx2 + dy2 <= r2) fb[i + (j * LX)] = col;
    }
  }
}



//AUDIO VISUALIZER
//drawStereoPeakVisualizer
int nbf = 0;  // count the number of frames drawn.
float peaks[2] = { 0.0, 0.0 };
uint16_t peakColors[2] = { ILI9341_T4_COLOR_WHITE, ILI9341_T4_COLOR_WHITE };

void drawStereoPeakVisualizer(double x, double y, double barWidth, double barHeight, double bars = 2, double vertSpacer = 1, double border = 1, uint16_t bgColor = ILI9341_T4_COLOR_NAVY) {
  tgxImage.fillRect(x, y, barWidth + (border * 2), (barHeight * bars) + (vertSpacer * (bars - 1)) + (border * 2), bgColor);
  for (int i = 0; i < bars; i++) {
    peaks[i] = audioProcessor.getPeak(i);
    if (peaks[i] > 0.90) {
      peakColors[i] = ILI9341_T4_COLOR_RED;
    } else if (peaks[i] > 0.75) {
      peakColors[i] = ILI9341_T4_COLOR_ORANGE;
    } else if (peaks[i] > 0.01) {
      peakColors[i] = ILI9341_T4_COLOR_WHITE;
    } else {
      peakColors[i] = ILI9341_T4_COLOR_GRAY;
    }
   tgxImage.fillRect(x + border, y + border + (i * barHeight) + (i * vertSpacer), 1 + int(peaks[i] * (barWidth - 1)), 3, peakColors[i]);
  }
}

//Method to draw simple horizontal progress bars
void drawSimpleProgressBarHorizontal(uint16_t *fb, long curValue, long valueMin, long valueMax, double x, double y, double width, double height, uint16_t fgColor = ILI9341_T4_COLOR_RED, uint16_t bgColor = ILI9341_T4_COLOR_BLACK) {
  uint16_t barWidth = map(curValue, valueMin, valueMax, 1, width);
  drawRectFilled(fb, x, y, width, height, bgColor);
  drawRectFilled(fb, x, y, barWidth, height, fgColor);
}




//MAIN COUNTDOWN TIMER
uint16_t timerColor =    0b0000011111100001;
uint16_t timerBgColor =  0b0000000001100000;
uint16_t timerBarColor = 0b0000000010000001;

void drawGameTimerMain() {
  uint16_t startX = 3;
  uint16_t startY = 14;
  uint16_t timerStringStartX = 0;
  uint16_t timerStringStartY = 48;
  uint16_t timerSubstringStartX = 0;
  uint16_t timerSubstringStartY = 68;
  uint16_t glyphStartX = 1;
  uint16_t glyphStartY = 0;
  uint16_t glyphDist = 37;
  uint16_t dotsDist = 19;
  uint16_t glyphW = 34;
  uint16_t glyphH = 49;
  uint16_t dotsBgW = 16;
  uint16_t dotW = 8;
  uint16_t dotH = 8;
  uint16_t dotTopY = 9;
  uint16_t dotBotY = 32;
  uint16_t dotsX = 4;
  uint16_t dotsBarMaxH = 30;
  uint16_t dotsBarTopY = 9;
  uint16_t topBarStartX = 1;  
  uint16_t topBarStartY = 55;  
  uint16_t topBarW = 313;  
  uint16_t topBarH = 13;  

  if (gameTimerMain.getTimerStatus() == FLAG_UNPREPARED_TIMER) {
    tgxImage.drawText(gameTimerMain.getTimerString(), iVec2{ 3, 76 }, RGB565_Green, font_tgx_Arial_Bold_48, true);
    return;
  }
  //Draw digit backgrounds
  tgxImage.fillRect(startX + glyphStartX, startY + glyphStartY, glyphW, glyphH, (uint16_t) timerBgColor);
  tgxImage.fillRect(startX + glyphStartX + glyphDist, startY + glyphStartY, glyphW, glyphH, (uint16_t) timerBgColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 2), startY + glyphStartY, dotsBgW, glyphH, (uint16_t )timerBgColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 2) + dotsDist, startY + glyphStartY, glyphW, glyphH, (uint16_t )timerBgColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 3) + dotsDist, startY + glyphStartY, glyphW, glyphH, (uint16_t) timerBgColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 4) + dotsDist, startY + glyphStartY, dotsBgW, glyphH, (uint16_t) timerBgColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 4) + (dotsDist * 2), startY + glyphStartY, glyphW, glyphH, (uint16_t) timerBgColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 5) + (dotsDist * 2), startY + glyphStartY, glyphW, glyphH, (uint16_t) timerBgColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 6) + (dotsDist * 2), startY + glyphStartY, dotsBgW, glyphH, (uint16_t) timerBgColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 6) + (dotsDist * 3), startY + glyphStartY, glyphW, glyphH, (uint16_t) timerBgColor);

  //draw dot seperators
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 2) + dotsX, startY + glyphStartY + dotTopY, dotW, dotH, timerColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 2) + dotsX, startY + glyphStartY + dotBotY, dotW, dotH, timerColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 4) + dotsDist + dotsX, startY + glyphStartY + dotTopY, dotW, dotH, timerColor);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 4) + dotsDist + dotsX, startY + glyphStartY + dotBotY, dotW, dotH, timerColor);
  uint16_t level = map(gameTimerMain.getMillisLeftThisSecond(),999,0,0,dotsBarMaxH);
  tgxImage.fillRect(startX + glyphStartX + (glyphDist * 6) + (dotsDist * 2) + dotsX, startY + glyphStartY + dotsBarTopY + level, dotW, dotsBarMaxH - level, timerColor);

  //draw on-top header bars
  tgxImage.fillRect(startX + topBarStartX, startY + topBarStartY, topBarW, topBarH, (uint16_t) timerBarColor);

  tgxImage.drawText(gameTimerMain.getTimerString(), iVec2{ startX + timerStringStartX, startY + timerStringStartY }, timerColor, font_tgx_Arial_Bold_48, true);
  tgxImage.drawText(gameTimerMain.getTimerSubString(), iVec2{ startX + timerSubstringStartX, startY + timerSubstringStartY }, timerColor, font_tgx_Arial_12, true);
  //tgxImage.drawText((char *) interface.getKeysReceivedBuffer(), iVec2{ 29, 10 }, RGB565_Blue, font_tgx_Arial_10, true);
  //Serial.println((char *) interface.getKeysReceivedBuffer());
}


/////////////////
// CAN VISUALIZER
/////////////////
uint16_t canVisualizerBaseX = 231;
void canVisualizerLoop(){
  uint16_t rxColor = (millis() < (gm7Can.getLastCanRxMillis()+gm7Can.getCanActivityTresholdInMillis())) ? ILI9341_T4_COLOR_WHITE : ILI9341_T4_COLOR_BLUE;
  uint16_t txColor = (millis() < (gm7Can.getLastCanTxMillis()+gm7Can.getCanActivityTresholdInMillis())) ? ILI9341_T4_COLOR_WHITE : ILI9341_T4_COLOR_BLUE;
  tgxImage.fillRect(205, 0, 41, 11, (uint16_t)ILI9341_T4_COLOR_NAVY);
  tgxImage.drawText((char*)"CAN", iVec2{ 207, 9 }, (uint16_t)ILI9341_T4_COLOR_WHITE, font_tgx_Arial_8, true);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+7,1}, 2, txColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+7,2}, 3, txColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+7,3}, 4, txColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+7,4}, 5, txColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+7,5}, 5, txColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+7,6}, 4, txColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+7,7}, 3, txColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+7,8}, 2, txColor);

  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+3,1}, 2, rxColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+2,2}, 3, rxColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+1,3}, 4, rxColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX,4}, 5, rxColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX,5}, 5, rxColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+1,6}, 4, rxColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+2,7}, 3, rxColor);
  tgxImage.drawFastHLine(iVec2{canVisualizerBaseX+3,8}, 2, rxColor);
}


//////////////////////////
//  DISPLAY UPDATE ASYNC
//////////////////////////

void updateDisplayAsync() {
  clear(frameBuffer);
  //tgxImage.drawFastHLine(iVec2{1,32},50,RGB565_Red);
  drawGameTimerMain();
  game.runModulesGfx();
  gui.loopGfx();
  canVisualizerLoop();
  drawStereoPeakVisualizer(250, 0, 28, 3, 2, 1, 2, ILI9341_T4_COLOR_NAVY);
  tft.overlayFPS(frameBuffer);
  tft.update(frameBuffer);
}





/////////////
//  NeoPixel
/////////////
void neopixelLoop(){
  uint16_t neopixelProgress = map(gameTimerMain.getMillisLeftThisSecond(), 0, 999, 0, NEOPIXEL_NUMPIXELS - 1);
  neopixelSerial.clear();
  neopixelSerial.setPixelColor(neopixelProgress, neopixelSerial.Color(20, 0, 0));
  neopixelSerial.show();
}



//////////////////////
// CAN CALLBACK METHOD
//////////////////////

////////////
//  SETUP
////////////

void setup() {

  //DISPLAY BOOTING
  while (!tft.begin(SPI_SPEED, SPI_SPEED_READ)); // initialize the display
  tft.setRotation(1);                  // landscape mode 320x240
  tft.setFramebuffer(internal_fb);     // set the internal framebuffer (enables double buffering)
  tft.setDiffBuffers(&diff1, &diff2);  // set the 2 diff buffers => activate differential updates.
  tft.setDiffGap(4);                   // use a small gap for the diff buffers (useful because cells are small...)
  tft.setRefreshRate(120);             // Set the refresh rate to around 120Hz
  tft.setVSyncSpacing(-1);             // set framerate = refreshrate (we must draw the fast for this to works: ok in our case).
  delay(10);

  //SERIAL
  Serial.begin(115200);
  delay(10);
  #ifdef DEBUG_TFT
  tft.output(&Serial);  // output debug infos to serial port.
  #endif


  //Boot up the interface, including the file system, SD card, Audio processor, etc.
  interface.boot();
  delay(50);



  //Let's activate some neopixels
  neopixelSerial.begin(); 
  for (int i = 0; i < NEOPIXEL_NUMPIXELS; i++) {
    neopixelSerial.setPixel(i, 0x000000);
  }
  neopixelSerial.show();
  delay(50);


  //Let's go to the welcome screen
  gui.resetToWelcomeScreen();
  delay(50);

  //Let's play some background sound
  //audioProcessor.playBackgroundMusic(globals.config.backgroundMusicMenuFile);

  //Prepare the game
  game.reset();

}




void loop() {
  
  if (chronoTimerFps200.hasPassed(5)){
    chronoTimerFps200.restart();
    
    interface.loop();
    if(globals.getSystemIsOnline() == true){
      gui.loop();    
    }
  }

  if (chronoTimerFps100.hasPassed(10)) {
    chronoTimerFps100.restart();
    if(globals.getSystemIsOnline() == true){
      gameTimerMain.loop();
      game.loop();
      neopixelLoop();
    }    
  }

  if (chronoTimerFps60.hasPassed(17)) {
    chronoTimerFps60.restart();

    updateDisplayAsync();
    if(globals.getSystemIsOnline() == true){
      audioProcessor.loop();
    }

  }

  if (chronoTimerFps2.hasPassed(500)) {
    chronoTimerFps2.restart();

  }

  //Loop CAN
  gm7Can.loop();


  if(Serial.available()){
    interface.processSerialRxChar(Serial.read());
  }

}
