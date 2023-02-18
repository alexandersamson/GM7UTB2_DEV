#include "PromptShowHints.h"

PromptShowHints::PromptShowHints(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Constants *constantsObj, Globals *globalsObj, Interface *interfaceObj, GuiButtons *guiButtonsObj) 
  : tgxImage(tgxImageObj), sprites(spritesObj), constants(constantsObj), globals(globalsObj), interface(interfaceObj), guiButtons(guiButtonsObj){
      setId(constants->PROMPT_SHOW_HINTS);
      setName(constants->labelAvailableHints[globals->getLang()]);
  }

void PromptShowHints::initCommonDataPackage(){
  dataPackage.sourceId = constants->PROMPT_SHOW_HINTS;
}

void PromptShowHints::reset(){

}
void PromptShowHints::enable(){
  
}
void PromptShowHints::disable(){
  
}

uint32_t PromptShowHints::getId(){
  return id;
}

void PromptShowHints::setId(uint32_t newId){
  id = newId;
}

char * PromptShowHints::getName(){
  return name;
}

void PromptShowHints::setName(char * newName){
  name = newName;
}

char * PromptShowHints::getDescription(){
  
}
bool PromptShowHints::isVisible(){
  
}
void PromptShowHints::show(){
  
}
void PromptShowHints::hide(){
  
}
Prompt::Position PromptShowHints::getPosition(){
  
}

void PromptShowHints::setPosition(Prompt::Position pos){
  position.x = pos.x;
  position.y = pos.y;
  position.width = pos.width;
  position.height = pos.height;
}

void PromptShowHints::setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
  position.x = x;
  position.y = y;
  position.width = w;
  position.height = h;
}

void PromptShowHints::inputChar(char key){
  if(key == constants->keypadStar && captureBackspace == false){
    Prompt::requestToClose();
    interface->playCancel();
  }
}

void PromptShowHints::inputInteger(uint32_t){
  
}

void PromptShowHints::drawButtons(){
    guiButtons->drawKeypadButtonWithLabel(position.x+2, position.y + position.height - BUTTON_HEIGHT_MEDIUM-2, constants->keypadStar, constants->labelClose[globals->getLang()], ICON_BUTTON_SIZE_MEDIUM, true);
}

void PromptShowHints::loop(){
  
}

void PromptShowHints::loopGfx(){
  drawButtons();
  tgxImage->drawText("What about the serial number?", iVec2{ position.x+5, position.y+18 }, constants->colorGuiMenuTextContent[constants->getColorScheme()], font_tgx_Arial_14, true);
  //tgxImage->drawRect(position.x+5, position.y+5, 20, 20, constants->colorGuiMenuFrame[constants->getColorScheme()]);
}