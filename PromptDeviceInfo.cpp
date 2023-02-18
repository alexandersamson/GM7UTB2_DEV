#include "PromptDeviceInfo.h"

PromptDeviceInfo::PromptDeviceInfo(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Constants *constantsObj, Globals *globalsObj, Interface *interfaceObj, GuiButtons *guiButtonsObj) 
  : tgxImage(tgxImageObj), sprites(spritesObj), constants(constantsObj), globals(globalsObj), interface(interfaceObj), guiButtons(guiButtonsObj){
      setId(constants->PROMPT_DEVICE_INFO);
      setName(constants->labelDeviceInformation[globals->getLang()]);
  }

void PromptDeviceInfo::initCommonDataPackage(){
  dataPackage.sourceId = constants->PROMPT_DEVICE_INFO;
}

void PromptDeviceInfo::reset(){

}
void PromptDeviceInfo::enable(){
  
}
void PromptDeviceInfo::disable(){
  
}

uint32_t PromptDeviceInfo::getId(){
  return id;
}

void PromptDeviceInfo::setId(uint32_t newId){
  id = newId;
}

char * PromptDeviceInfo::getName(){
  return name;
}

void PromptDeviceInfo::setName(char * newName){
  name = newName;
}

char * PromptDeviceInfo::getDescription(){
  
}
bool PromptDeviceInfo::isVisible(){
  
}
void PromptDeviceInfo::show(){
  
}
void PromptDeviceInfo::hide(){
  
}
Prompt::Position PromptDeviceInfo::getPosition(){
  
}

void PromptDeviceInfo::setPosition(Prompt::Position pos){
  position.x = pos.x;
  position.y = pos.y;
  position.width = pos.width;
  position.height = pos.height;
}

void PromptDeviceInfo::setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
  position.x = x;
  position.y = y;
  position.width = w;
  position.height = h;
}

void PromptDeviceInfo::inputChar(char key){
  if(key == constants->keypadStar && captureBackspace == false){
    Prompt::requestToClose();
    interface->playCancel();
  }
}

void PromptDeviceInfo::inputInteger(uint32_t){
  
}

void PromptDeviceInfo::drawButtons(){
    guiButtons->drawKeypadButtonWithLabel(position.x+2, position.y + position.height - BUTTON_HEIGHT_MEDIUM-2, constants->keypadStar, constants->labelClose[globals->getLang()], ICON_BUTTON_SIZE_MEDIUM, true);
}

void PromptDeviceInfo::loop(){
  
}

void PromptDeviceInfo::loopGfx(){
  const char * chrptrs[5] = {
    constants->DEVICE_MODEL, 
    constants->DEVICE_SERIAL_NUMBER,
    constants->DEVICE_SOFTWARE_VERSION,
    constants->DEVICE_PRODUCTION_DATE,
    constants->DEVICE_OWNER
  };
  const char * labels[5] = {"Device model:", "Serial number:", "Version:", "Constr. date:", "Licensed to:"};
  for(int i = 0; i < 5; i++){
      tgxImage->drawText(labels[i], iVec2{ position.x+10, position.y+22 + (i*20) }, constants->colorGuiMenuTextContent[constants->getColorScheme()], font_tgx_Arial_12, true);
      tgxImage->drawText(chrptrs[i], iVec2{ position.x+130, position.y+22 + (i*20) }, constants->colorGuiMenuTextContent[constants->getColorScheme()], font_tgx_Arial_Bold_12, true);
  }
  drawButtons();
}