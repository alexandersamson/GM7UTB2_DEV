

#ifndef Constants_h
#define Constants_h

#include <Arduino.h>
#include <font_tgx_Arial.h>
#include <font_tgx_Arial_Bold.h>
#include "Gm7CanProtocol.h"


//DEBUG
#define DEBUG 1


//HARDWARE
#define SDCARD_CS_PIN BUILTIN_SDCARD


//MEMORY AND ARRAY SIZES
#define SIZE_ARRAY_TIMER_STRING 16
#define SIZE_ARRAY_BUTTON_LABEL_SMALL 16
#define SIZE_ARRAY_BUTTON_LABEL_BIG 32
#define SIZE_ARRAY_LABEL 16
#define SIZE_ARRAY_NAME 32
#define SIZE_ARRAY_FILENAME 64
#define SIZE_ARRAY_DESCRIPTION 256


//GAME
#define GAME_STATE_UNPREPARED 1  //UNARMED
#define GAME_STATE_PREPARED 2    //ARMED
#define GAME_STATE_STARTED 4     //COUNTDOWN
#define GAME_STATE_VALIDATING 8  //Validating code entry. Some sort of validating mode where userinput is disabled temporarely
#define GAME_STATE_DISARMING 16  //Validating actual disarming. Some sort of disarming mode where userinput is disabled temporarely
#define GAME_STATE_PAUSED 32
#define GAME_STATE_STOPPED 64
#define GAME_STATE_BLOCKED 128
#define GAME_STATE_ENDED_WIN 256
#define GAME_STATE_ENDED_LOSS 512
#define GAME_STATE_ENDED_DRAW 1024
#define GAME_STATE_ERROR 2048

#define GAME_LOOPABLE (GAME_STATE_STARTED + GAME_STATE_VALIDATING + GAME_STATE_DISARMING + GAME_STATE_PAUSED + GAME_STATE_BLOCKED + GAME_STATE_ENDED_WIN + GAME_STATE_ENDED_LOSS + GAME_STATE_ENDED_DRAW )


//GAME TIMER
#define FLAG_PREPARED_TIMER 1
#define FLAG_STARTED_TIMER 2
#define FLAG_STOPPED_TIMER 4
#define FLAG_PAUSED_TIMER 8
#define FLAG_DEFUSED_TIMER 16
#define FLAG_EXPIRED_TIMER 32
#define FLAG_BONUS_TIMER 64
#define FLAG_PUNISH_TIMER 128
#define FLAG_UNPREPARED_TIMER 256


//MODULES
#define MODULES_COUNT 5
#define MODULE_CONFIG_MAX_COMMAND_STRINGS 10  //Every module can use up to 10 strings per effect, such as contributing to disarmament, bonus time, actuators, or punishment
#define MODULE_CONFIG_STRING_MAX_LENGTH 16

#define GAME_MODULE_STATE_INACTIVE 1
#define GAME_MODULE_STATE_ACTIVE 2
#define GAME_MODULE_STATE_VALIDATING 4
#define GAME_MODULE_STATE_PAUSED 8
#define GAME_MODULE_STATE_SOLVED 16
#define GAME_MODULE_STATE_FAILED 32
#define GAME_MODULE_STATE_INFORMATIVE 64
#define GAME_MODULE_STATE_DISABLED 128
#define GAME_MODULE_STATE_SELECTED 256
#define GAME_MODULE_STATE_ALTERED 512
#define GAME_MODULE_STATE_SOLVABLE 1024
#define GAME_MODULE_STATE_RESERVED 2048
#define GAME_MODULE_STATE_ERROR 4096

#define GAME_MODULE_OUTPUT_NORMAL 1
#define GAME_MODULE_OUTPUT_SOLVED 2
#define GAME_MODULE_OUTPUT_FAILED 4
#define GAME_MODULE_OUTPUT_PUNISH_SMALL 8
#define GAME_MODULE_OUTPUT_PUNISH_MEDIUM 16
#define GAME_MODULE_OUTPUT_PUNISH_BIG 32
#define GAME_MODULE_OUTPUT_PUNISH_STREAM 64
#define GAME_MODULE_OUTPUT_REWARD_SMALL 128
#define GAME_MODULE_OUTPUT_REWARD_MEDIUM 256
#define GAME_MODULE_OUTPUT_REWARD_BIG 512
#define GAME_MODULE_OUTPUT_REWARD_STREAM 1024


//LANGUAGE STUFF
#define COUNT_LANGUAGES 2
#define LANG_EN 0
#define LANG_NL 1

//COLOR STUFF
#define COLOR_SCHEMES 4
#define COLOR_SCHEME_RED 0
#define COLOR_SCHEME_GREEN 1
#define COLOR_SCHEME_BLUE 2
#define COLOR_SCHEME_YELLOW 3

//FONTS
#define FONT_BOLD_SMALL font_tgx_Arial_Bold_9
#define FONT_BOLD_MEDIUM font_tgx_Arial_Bold_12
#define FONT_BOLD_BIG font_tgx_Arial_Bold_16
#define FONT_BOLD_MEGA font_tgx_Arial_Bold_28
#define FONT_MICRO font_tgx_Arial_8
#define FONT_SMALL font_tgx_Arial_9
#define FONT_MEDIUM font_tgx_Arial_12
#define FONT_BIG font_tgx_Arial_16
#define FONT_MEGA font_tgx_Arial_Bold_28
#define LINE_HEIGHT_MICRO 11
#define LINE_HEIGHT_SMALL 13
#define LINE_HEIGHT_MEDIUM 16
#define LINE_HEIGHT_BIG 24
#define LINE_HEIGHT_MEGA 32

//CONSTANTS CLASS
class Constants {
  private:
    uint16_t language = LANG_EN;
    uint16_t colorScheme = COLOR_SCHEME_YELLOW;
  public:
    //PATHS AND FILES
    const char * const CONFIG_FILE_NAME =         "config.json";
    const char * const CONFIG_BACKUP_FILE_NAME =  "config_backup.json";
    const char * const CONFIG_TEMP_FILE_NAME =    "config_temp.json";

    //DEVICE
    const char * const DEVICE_MODEL =             "GM7UTB2";
    const char * const DEVICE_SERIAL_NUMBER =     "10101";
    const uint32_t DEVICE_UID_32 =                atoi(DEVICE_SERIAL_NUMBER);
    const uint64_t DEVICE_UID_64 =                atoi(DEVICE_SERIAL_NUMBER);
    const char * const DEVICE_SOFTWARE_VERSION =  "0.1.0-ALPHA.1";
    const char * const DEVICE_PRODUCTION_DATE =   "TBA";
    const char * const DEVICE_OWNER =             "GM7 Engineering";
    const char * const DEVICE_VENDOR =            "GM7 NL";
    const char * const DEVICE_SHORT_NAME =        "TIMEBMB";
    const uint16_t     DEVICE_TYPE_ID =           Gm7CanProtocol::DEVICE_TYPE_CONTROLLER_GM7UTB;

    //GENERIC
    const char* const emptyString = "\0";
    const char emptyChar = '\0';

    //FONT PACKAGES
    struct FontPackage {
      ILI9341_t3_font_t font;
      uint16_t lineHeight;
      uint16_t offsetX;
      uint16_t offsetY;
    };

    Constants::FontPackage microFont = {
      font_tgx_Arial_8,
      9,
      0,
      9
    };

    Constants::FontPackage smallFont = {
      font_tgx_Arial_9,
      10,
      0,
      11
    };
  
    Constants::FontPackage mediumFont = {
      font_tgx_Arial_12,
      14,
      0,
      15
    };
  
    Constants::FontPackage bigFont = {
      font_tgx_Arial_16,
      17,
      0,
      20
    };
    
    //KEYPAD
    const char keypadStar = '*';
    const char keypadHash = '#';
    const char keypadUp = '8';
    const char keypadDown = '0';
    const char keypadLeft = '7';
    const char keypadRight = '9';

    
    //LABELS
    const char* const labelUpSymbol = "^";
    const char* const labelDownSymbol = "_";

    const char* const labelLanguages[COUNT_LANGUAGES] = {"English", "Nederlands"};
    const char* const labelMenu[COUNT_LANGUAGES] = {"MENU", "MENU"};
    const char* const labelConfirmed[COUNT_LANGUAGES] = {"CONFIRMED", "BEVESTIGD"};
    const char* const labelConfirm[COUNT_LANGUAGES] = {"CONFIRM", "BEVESTIG"};
    const char* const labelSelect[COUNT_LANGUAGES] = {"SELECT", "OPENEN"};
    const char* const labelCancel[COUNT_LANGUAGES] = {"CANCEL", "AFBREKEN"};
    const char* const labelOpenMenu[COUNT_LANGUAGES] = {"OPEN MENU", "OPEN MENU"};
    const char* const labelOptions[COUNT_LANGUAGES] = {"OPTIONS", "OPTIES"};
    const char* const labelGoBack[COUNT_LANGUAGES] = {"GO BACK", "GA TERUG"};
    const char* const labelClose[COUNT_LANGUAGES] = {"CLOSE", "SLUITEN"};
    const char* const labelBackspace[COUNT_LANGUAGES] = {"CLEAR", "WISSEN"};
    const char* const labelModules[COUNT_LANGUAGES] = {"MODULES", "MODULES"};
    const char* const labelSettings[COUNT_LANGUAGES] = {"SETTINGS", "INSTELLINGEN"};
    const char* const labelMessages[COUNT_LANGUAGES] = {"MESSAGES", "BERICHTEN"};

    const char* const labelStartTimer[COUNT_LANGUAGES] = {"START TIMER", "START DE KLOK"};
    const char* const labelQuickSetupTimer[COUNT_LANGUAGES] = {"SET TIMER", "STEL KLOK IN"};
    const char* const labelStatistics[COUNT_LANGUAGES] = {"STATISTICS", "STATISTIEKEN"};
    const char* const labelLogIn[COUNT_LANGUAGES] = {"LOG IN", "INLOGGEN"};
    const char* const labelLogOut[COUNT_LANGUAGES] = {"LOG OUT", "UITLOGGEN"};

    const char* const labelDeviceInformation[COUNT_LANGUAGES] = {"Device information", "Toestelinformatie"};
    const char* const labelWelcomeScreen[COUNT_LANGUAGES] = {"Welcome screen", "Startscherm"};
    const char* const labelAvailableHints[COUNT_LANGUAGES] = {"Available hints", "Beschikbare hints"};

    const char* const labelLoggedInAsGuest[COUNT_LANGUAGES] = {"Logged in as guest", "Ingelogd als gast"};
    const char* const labelLoggedInAsUser[COUNT_LANGUAGES] = {"Logged in as user", "Ingelogd als gebruiker"};
    const char* const labelLoggedInAsAdmin[COUNT_LANGUAGES] = {"Logged in as admin", "Ingelogd als administrator"};
    const char* const labelLoggedInAsDev[COUNT_LANGUAGES] = {"Logged in as developer", "Ingelogd als ontwikkelaar"};

    //TIMER STRINGS
    const char* const timerExpired[COUNT_LANGUAGES] = {"Expired", "Verlopen"};
    const char* const timerInactive[COUNT_LANGUAGES] = {"Inactive", "Inactief"};      
    const char* const timerSubtitles[COUNT_LANGUAGES][9] = {{"STAND BY", "ARMED", "STOPPED", "PAUSED", "DISARMED", "EXPIRED", "ARMED - SLOWED", "ARMED - ACCELERATED", "DEACTIVATED"},
                                                            {"STAND BY", "ACTIEF", "GESTOPT", "GEPAUZEERD", "ONTMANTELD", "VERLOPEN", "ACTIEF - LANGZAMER", "ACTIEF - VERSNELD", "INACTIEF"}};

    //MEM AUDIO FILES
    const char* const audioMemBeep = "beep";
    const char* const audioMemTick = "tickLoud";
    const char* const audioMemTickSoft = "tickSoft";
    const char* const audioMemBlip = "blip";
    const char* const audioMemPing = "ping";
    const char* const audioMemConfirm = "confirm";
    const char* const audioMemError = "error";
    const char* const audioMemBlocked = "blocked";
    const char* const audioMemPong = "pong";
    const char* const audioMemCancel = "cancel";

    //COLORS
    uint16_t getColorScheme() const {return colorScheme;};  
                                                    //COLOR SCHEMES:   R       G       B       Y
    const uint16_t colorGuiMenuFrame[COLOR_SCHEMES] =               {0xF9E7, 0x07E0, 0x39DF, 0xFF40};
    const uint16_t colorGuiMenuFrameDisabled[COLOR_SCHEMES] =       {0x50C3, 0x0260, 0x18CA, 0x5240};
    const uint16_t colorGuiMenuTextContentDisabled[COLOR_SCHEMES] = {0x50C3, 0x0260, 0x18CA, 0x5240};
    const uint16_t colorGuiMenuTextContent[COLOR_SCHEMES] =         {0xF9E7, 0x07E0, 0x39DF, 0xFF40};
    const uint16_t colorGuiMenuTextHeader[COLOR_SCHEMES] =          {0x1000, 0x00A0, 0x0002, 0x10A0};
    const uint16_t colorGuiMenuBackground[COLOR_SCHEMES] =          {0x1000, 0x00A0, 0x0002, 0x10A0};

    //GUI standards
    const uint16_t guiMenuHeaderHeight = 16;


    //OPTIONS & MENU-LINES
    #define GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT 10 //Needs to be reasonable in order to not make too many array overhead
    #define GUI_COUNT_MENUS_TOTAL 7
    #define GUI_MENU_ENTRY_IS_CATEGORY 9999    
    #define GUI_MENU_ENTRY_ID_EMPTY 9999
    #define GUI_MENU_SELECTED_NONE 9999

    #define GUI_GAME_MENU 0
    #define GUI_GAME_SUB_MENU 1
    #define GUI_MAIN_MENU 2
    #define GUI_MAIN_SUB_MENU 3
    #define GUI_PROMPT 4
    #define GUI_ALERT_BADGE 5
    #define GUI_LOGIN 6

    #define TYPE_CATEGORY 0
    #define TYPE_PLACEHOLDER 1
    #define TYPE_NORMAL 2
    #define TYPE_SHOW_REDIRECT 3
    #define TYPE_SHOW_TOGGLE 4
    #define TYPE_SHOW_PROGRESS_BAR 5
    #define TYPE_SHOW_STRING 6

    #define CATEGORY_ID_GAME_OPTIONS 1
    #define CATEGORY_ID_GAME_MODULES 2
    #define CATEGORY_ID_LOGIN 3
    #define CATEGORY_ID_USER_MENU_GENERAL 4
    #define CATEGORY_ID_USER_MENU_GAME 5
    #define CATEGORY_ID_USER_MENU_AUDIO 6
    #define CATEGORY_ID_ADMIN_MENU_IO 7
    #define CATEGORY_ID_ADMIN_MENU_SYSTEM 8

    #define TARGET_ID_LOG_OUT 1000
    #define TARGET_ID_OPEN_SUBMENU_INGAME_OPTIONS 2001
    #define TARGET_ID_OPEN_SUBMENU_INGAME_MODULES 2002
    #define TARGET_ID_OPEN_SUBMENU_INGAME_HINTS 2003
    #define TARGET_ID_SHOW_HINTS 3001
    #define TARGET_ID_REQUEST_HINT 3002
    #define TARGET_ID_SELECT_INGAME_MODULE 3004
    #define TARGET_ID_SHOW_DEVICE_INFO_INGAME 3005

    #define ACCESS_REQUIRED_USER 1
    #define ACCESS_REQUIRED_ADMIN 2
    #define ACCESS_REQUIRED_DEV 4

    enum promptIds {
      PROMPT_WELCOME_SCREEN = 0,
      PROMPT_DEVICE_INFO,
      PROMPT_SHOW_HINTS,
      PROMPT_REQUEST_HINTS
    };


    struct MenuEntry {
      uint16_t id;                  //Must be incrementing, starting from 0 (will be used to direct access this menu entry from the array index)
      uint16_t menu;                // in what menu is this option showed?
      uint16_t menuChild;           // in what menu are the childeren shown?
      uint16_t type;                //What kind of menu entry is this?
      uint16_t parentId;            //Id of the parent, which need to be in selection in order to show this item
      uint16_t targetId;            //ID of the target to run when option is selected
      char const * label[COUNT_LANGUAGES];//Name of the item which is shown on the display
      bool visible;                 //Is this item visible at all?
      bool enabled;                 //Is this item enabled (and selectable)
      uint16_t accessRequired;      //What access level is required to be able to use (and see this item)
      uint16_t spriteId;            //Some id to a sprite for this item.
    };

    #define GUI_MENU_ENTRIES_COUNT 7
    MenuEntry c_menuEntries[GUI_MENU_ENTRIES_COUNT] = {              
      //                                                                                                                                       LABEL - MAX LENGTH 15 +  terminator
      //  MENU                    TARGET CHILD MENU   TYPE              PARENT ID                     TARGET ID                                "..............."                          VISI. ENABL.  ACCESS LEVEL REQUIRED   SPRITE
      {0, GUI_GAME_MENU,          GUI_GAME_SUB_MENU,  TYPE_CATEGORY,    GUI_MENU_ENTRY_IS_CATEGORY,   TARGET_ID_OPEN_SUBMENU_INGAME_MODULES,  {"Modules", "Modules"},               true, true,   ACCESS_REQUIRED_USER,   0     },
      {1, GUI_GAME_MENU,          GUI_GAME_SUB_MENU,  TYPE_CATEGORY,    GUI_MENU_ENTRY_IS_CATEGORY,   TARGET_ID_OPEN_SUBMENU_INGAME_HINTS,    {"Hints", "Hints"},                   true, true,   ACCESS_REQUIRED_USER,   0     },
      {2, GUI_GAME_MENU,          GUI_GAME_SUB_MENU,  TYPE_CATEGORY,    GUI_MENU_ENTRY_IS_CATEGORY,   TARGET_ID_OPEN_SUBMENU_INGAME_OPTIONS,  {"Options", "Opties"},                true, true,   ACCESS_REQUIRED_USER,   0     },
      {3, GUI_GAME_SUB_MENU,      GUI_GAME_SUB_MENU,  TYPE_NORMAL,      1,                            TARGET_ID_SHOW_HINTS,                   {"Show hints", "Bekijk hints"},       true, true,   ACCESS_REQUIRED_USER,   0     },
      {4, GUI_GAME_SUB_MENU,      GUI_GAME_SUB_MENU,  TYPE_NORMAL,      1,                            TARGET_ID_REQUEST_HINT,                 {"Request hint", "Vraag hint"},       true, true,   ACCESS_REQUIRED_USER,   0     },
      {5, GUI_GAME_SUB_MENU,      GUI_GAME_SUB_MENU,  TYPE_PLACEHOLDER, 0,                            TARGET_ID_SELECT_INGAME_MODULE,         {"[Game Modules]", "[Spelmodules]"},  true, true,   ACCESS_REQUIRED_USER,   0     },
      {6, GUI_GAME_SUB_MENU,      GUI_GAME_SUB_MENU,  TYPE_NORMAL,      2,                            TARGET_ID_SHOW_DEVICE_INFO_INGAME,      {"Device info", "Toestelinfo"},       true, true,   ACCESS_REQUIRED_USER,   0     }
    };


    //COMMANDS
    enum commands {
      COMMAND_CHANGE_GAME_STATUS_REQUEST = 0
    };

};







#endif
