#include "PromptWelcomeScreen.h"

PromptWelcomeScreen::PromptWelcomeScreen(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Constants *constantsObj, Globals *globalsObj, Interface *interfaceObj, GuiButtons *guiButtonsObj) 
  : tgxImage(tgxImageObj), sprites(spritesObj), constants(constantsObj), globals(globalsObj), interface(interfaceObj), guiButtons(guiButtonsObj){
      setId(constants->PROMPT_WELCOME_SCREEN);
      setName(constants->labelWelcomeScreen[globals->getLang()]);
  }

void PromptWelcomeScreen::initCommonDataPackage(){
  dataPackage.sourceId = constants->PROMPT_WELCOME_SCREEN;
}

void PromptWelcomeScreen::reset(){

}
void PromptWelcomeScreen::enable(){
  
}
void PromptWelcomeScreen::disable(){
  
}

uint32_t PromptWelcomeScreen::getId(){
  return id;
}

void PromptWelcomeScreen::setId(uint32_t newId){
  id = newId;
}

char * PromptWelcomeScreen::getName(){
  return name;
}

void PromptWelcomeScreen::setName(char * newName){
  name = newName;
}

char * PromptWelcomeScreen::getDescription(){
  
}
bool PromptWelcomeScreen::isVisible(){
  
}
void PromptWelcomeScreen::show(){
  
}
void PromptWelcomeScreen::hide(){
  
}
Prompt::Position PromptWelcomeScreen::getPosition(){
  
}

void PromptWelcomeScreen::setPosition(Prompt::Position pos){
  position.x = pos.x;
  position.y = pos.y;
  position.width = pos.width;
  position.height = pos.height;
}

void PromptWelcomeScreen::setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
  position.x = x;
  position.y = y;
  position.width = w;
  position.height = h;
}

void PromptWelcomeScreen::inputChar(char key){
  if(key == constants->keypadStar && captureBackspace == false){
    //We cannot exit the welcome screen...
  }
  if(key == '1'){
    dataPackage.command = constants->COMMAND_CHANGE_GAME_STATUS_REQUEST;
    dataPackage.dataInt[0] = GAME_STATE_STARTED;
    if(Prompt::sendDataPackageToInterfaceAndClosePrompt(interface, dataPackage)){
      interface->playConfirm();
    } else {
      interface->playError();      
    }
    
  }
}

void PromptWelcomeScreen::inputInteger(uint32_t){
  
}

void PromptWelcomeScreen::drawButtons(){
  char * chrptrs[5];
  chrptrs[0] = constants->labelStartTimer[globals->getLang()]; 
  chrptrs[1] = constants->labelQuickSetupTimer[globals->getLang()];
  chrptrs[2] = constants->labelStatistics[globals->getLang()];
  chrptrs[3] = constants->labelSettings[globals->getLang()];
  chrptrs[4] = (interface->hasAccessAsUser()) ? constants->labelLogOut[globals->getLang()] : constants->labelLogIn[globals->getLang()];
  char buttonChars[5] = {'1', '2', '3', '4', '5'};
  uint16_t padding = 10;
  for(int i = 0; i <5; i++){
    guiButtons->drawKeypadButtonWithLabel(position.x + padding, position.y + padding + (LINE_HEIGHT_MEGA * i), buttonChars[i], chrptrs[i], ICON_BUTTON_SIZE_BIG, true);
  }
}

void PromptWelcomeScreen::loop(){
  
}

void PromptWelcomeScreen::loopGfx(){
  drawButtons();
  tgxImage->drawText(constants->DEVICE_SOFTWARE_VERSION, iVec2{ position.x+3, position.y+position.height-2 }, constants->colorGuiMenuTextContentDisabled[constants->getColorScheme()], font_tgx_Arial_8, true);
  //tgxImage->drawRect(position.x+5, position.y+5, 20, 20, constants->colorGuiMenuFrame[constants->getColorScheme()]);
}