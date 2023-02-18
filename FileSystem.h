#ifndef FileSystem_h
#define FileSystem_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <StreamUtils.h>
#include "Constants.h"
#include "Globals.h"

using namespace ArduinoJson;

#define JSON_DOCUMENT_CONFIG_FILE_SIZE 8192

class FileSystem{

  private:
    Constants *constants;
    Globals *globals;

    bool doingTask = false;

  public:
    enum ReturnStates { RETURN_ERROR = 0, RETURN_SUCCESS, RETURN_ERROR_OPEN_SD_HARDWARE, RETURN_ERROR_OPEN_SD_FILE, RETURN_ERROR_WRITE_SD_FILE, RETURN_ERROR_READ_SD_FILE };
    FileSystem(Constants *constantsObj, Globals *globalsObj);

    ReturnStates begin();

    ReturnStates readConfigFileFromSd();
    ReturnStates saveConfigFileToSd();
    ReturnStates dumpConfigToSerial();
    bool isDoingTask();
    void startTask();
    void endTask();

};

#endif
