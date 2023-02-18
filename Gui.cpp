#include "Gui.h"


Gui::Gui(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Interface *interfaceObj, Constants *constantsObj, Globals *globalsObj, GuiButtons *guiButtonsObj, NotificationBadge *notificationBadgeObj, Prompt ** promptsObj, uint16_t promptsCnt)
  : tgxImage(tgxImageObj), sprites(spritesObj), interface(interfaceObj), constants(constantsObj), globals(globalsObj), guiButtons(guiButtonsObj), notificationBadge(notificationBadgeObj), prompts(promptsObj) {
    promptsCount = promptsCnt;
    c_menuEntries = constants->c_menuEntries;
    init();
}

void Gui::init(){
  for(int i = 0; i < GUI_COUNT_MENUS_TOTAL; i++){
    for(int j = 0; j < GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT; j++){
      menuEntryIdMatrix[i][j] = GUI_MENU_ENTRY_ID_EMPTY;
    }
  }
}

void Gui::resetToWelcomeScreen(){
  interface->setSelectedInputConsumer(INPUT_CONSUMER_GUI);
  openPrompt(GUI_PROMPT, constants->PROMPT_WELCOME_SCREEN);
}



void Gui::drawMenu(uint16_t menu) {
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return;
  }
  Gui::FrameBox box;
  uint16_t animation;
  uint16_t frameColor;

  //Decide what kind of menu or content box is shown
  box = getMenuDimensions(menu);

  //If the menu is active, it needs to be shown in vibrant colors, otherwise in darker, duller colors.
  if(menu == activeMenu){
    frameColor = constants->colorGuiMenuFrame[constants->getColorScheme()];
  } else {
    frameColor = constants->colorGuiMenuFrameDisabled[constants->getColorScheme()];
  }

  drawAnimatedMenuFrame(menu, box, frameColor, constants->colorGuiMenuBackground[constants->getColorScheme()]);
  if(animations[menu].animationOngoing == true){
    return;
  }
  drawMenuHeader(box, menu);
  if((menu != GUI_PROMPT) && (menu != GUI_ALERT_BADGE)){ //The prompts and badges have their own GfxLoop content manager
    drawMenuContent(box, menu);
  }
}



void Gui::drawAnimatedMenuFrame(uint16_t menu, Gui::FrameBox box, uint16_t frameColor, uint16_t backgroundColor, float opacity) {
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return;
  }
  if(animations[menu].animationFramesDelay > 0 && animations[menu].animationCurrentFrame == 0 && animations[menu].animationOngoing == true && (animations[menu].animationVariantCurrent == ANIMATION_VARIANT_LEFT_TO_RIGHT_OPEN || animations[menu].animationVariantCurrent == ANIMATION_VARIANT_RIGHT_TO_LEFT_OPEN || animations[menu].animationVariantCurrent == ANIMATION_VARIANT_CENTER_OPEN)){
    return;
  }
  Gui::FrameBox headerBox = animationBoxGetFrame(menu, Gui::FrameBox {box.x, box.y, 0, constants->guiMenuHeaderHeight}, Gui::FrameBox {box.x, box.y, box.w, constants->guiMenuHeaderHeight});
  Gui::FrameBox menuBox = animationBoxGetFrame(menu, Gui::FrameBox {box.x, box.y+constants->guiMenuHeaderHeight, 0, box.h-constants->guiMenuHeaderHeight}, Gui::FrameBox {box.x, box.y+constants->guiMenuHeaderHeight, box.w, box.h-constants->guiMenuHeaderHeight});
  if (opacity == 1.0f) {
    tgxImage->fillRect(headerBox.x, headerBox.y, headerBox.w, headerBox.h, frameColor);
    tgxImage->fillRect(menuBox.x, menuBox.y, menuBox.w, menuBox.h, backgroundColor);
    tgxImage->drawRect(menuBox.x, menuBox.y, menuBox.w, menuBox.h, frameColor);
  } else {
    tgxImage->fillRect(menuBox.x, menuBox.y, menuBox.w, menuBox.h, backgroundColor, opacity);
    tgxImage->drawRect(menuBox.x, menuBox.y, menuBox.w, menuBox.h, frameColor, opacity);
  }
}

//MAIN METHOD FOR MENU HEADER TEXT DRAWING.
void Gui::drawMenuHeader(Gui::FrameBox box, uint16_t menu){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return;
  }
  char * headerPtr = constants->labelMenu[globals->getLang()]; //Use some generic value to start with
  if((menu == GUI_PROMPT) && (activePrompt < promptsCount)){
    headerPtr = prompts[activePrompt]->getName();
  }
  else if((menu != GUI_GAME_MENU) && (menu != GUI_MAIN_MENU)){ //In this case it's a main menu type, so just draw something like 'Menu' if this condition is not met
    if((menu == GUI_GAME_SUB_MENU) && (selectedCurrent[GUI_GAME_MENU] != GUI_MENU_SELECTED_NONE)){ //In the case of when the title of a sub menu is requested.
      headerPtr = c_menuEntries[selectedCurrentId[GUI_GAME_MENU]].label[globals->getLang()];
    }
    else if((menu == GUI_MAIN_SUB_MENU) && (selectedCurrent[GUI_MAIN_MENU] != GUI_MENU_SELECTED_NONE)){ //In the case of when the title of a sub menu is requested.
      headerPtr = c_menuEntries[selectedCurrentId[GUI_MAIN_MENU]].label[globals->getLang()];
    }
    else if(menu == GUI_ALERT_BADGE){
      headerPtr = constants->labelMessages[globals->getLang()];
    }
  }
  tgxImage->drawText(headerPtr, iVec2{ box.x+4, box.y+14 }, constants->colorGuiMenuTextHeader[constants->getColorScheme()], font_tgx_Arial_Bold_9, true);
}


//MAIN METHOD FOR MENU CONTENT TEXT DRAWING.
void Gui::drawMenuContent(FrameBox box, uint16_t menu){
  if(menu == GUI_PROMPT || menu >= GUI_COUNT_MENUS_TOTAL){
    return;
  }
  uint16_t pos = 0;
  bool highlighted = false;
  bool isActive = (menu == activeMenu) ? true : false; //Is this menu active and thus is the text shown as active?
  for(int i = 0; i < GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT; i++){
    uint16_t id = menuEntryIdMatrix[menu][i];
    if(id == GUI_MENU_ENTRY_ID_EMPTY){
      continue;
    }
    highlighted = (cursorCurrent[menu] == pos) ? true : false; //check if the menu-option is selected
    isActive = ((isActive == true) && (c_menuEntries[id].enabled == true)) ? true : false; //check if the menu-option is selectable if the menu is active
    drawTextLineAtYPosIncrement(box, c_menuEntries[id].label[globals->getLang()], pos, false, highlighted, isActive);
    pos++;
  }
} //END drawContent()


//Draws a line of text as part of a y-axis sorted list. Can account for headers and highlighted texts
void Gui::drawTextLineAtYPosIncrement(Gui::FrameBox box, char* text, uint16_t pos, bool isHeader, bool isHighlighted, bool isActive){
  box = _getCalculatedContentTextPos(box);
  box.y += (pos * MENU_ITEM_LINE_H);
  ILI9341_t3_font_t font;   
  uint16_t color;
  if(isHighlighted || isHeader){
    font = font_tgx_Arial_Bold_9;
  } else {
    font = font_tgx_Arial_9;
  }
  if(isHeader){
    color = constants->colorGuiMenuTextHeader[constants->getColorScheme()];
  } else if(isActive) {
    color = constants->colorGuiMenuTextContent[constants->getColorScheme()];
  } else {
    color = constants->colorGuiMenuTextContentDisabled[constants->getColorScheme()];
  }
  if(isHighlighted){
    tgxImage->fillRect(box.x-2, box.y-10, 6, 10, color);
    box.x += 7;
  }
  tgxImage->drawText(text, iVec2{ box.x, box.y }, color, font, true);
  if(isHighlighted){

  }
}





void Gui::setMenuSplitX(uint16_t split){
  menuSplitX = split;  
}

uint16_t Gui::getMenuSplitX(){
  return menuSplitX;
}

uint16_t Gui::getMenuSplitXPadding(){
  return menuSplitXPadding;
}



Gui::FrameBox Gui::getMenuDimensions(uint16_t menu){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return FrameBox {0,0,0,0};
  }
    Gui::FrameBox box = getFittingMenuBox(); 
  if(menu == GUI_ALERT_BADGE){
    box.x += BADGE_PADDING;
    box.w -= (BADGE_PADDING*2);
    box.y -= min((box.y+STATUS_BAR_H), BADGE_PADDING);
    box.h = min((notificationBadge->getContentHeight()+(padding*2)+MENU_HEADER_H), (DISPLAY_H - box.y - BADGE_PADDING));
  } else if(menu == GUI_PROMPT){
    box.x += PROMPT_PADDING;
    box.w -= (PROMPT_PADDING*2);
    box.y -= min((box.y+STATUS_BAR_H), PROMPT_PADDING);
    box.h = DISPLAY_H - box.y - PROMPT_PADDING;
  } else if(menu == GUI_MAIN_MENU || menu == GUI_GAME_MENU){
    box.x += padding;
    box.w = getMenuSplitX() - (padding * 2);
  } else if(menu == GUI_MAIN_SUB_MENU || menu == GUI_GAME_SUB_MENU){
    box.x = getMenuSplitX() + padding + getMenuSplitXPadding();
    box.w -= getMenuSplitX() + (padding*2) + getMenuSplitXPadding();
  }
  return box;
}





uint16_t Gui::getGameTimerObjectH(){
  return gameTimerHeight;
}


Gui::FrameBox Gui::getFittingMenuBox(){
  Gui::FrameBox box = boxMenu;
  box.h -= STATUS_BAR_H;
  box.y += STATUS_BAR_H;
  if(interface->getStatusIsIngame()){
    uint16_t timerH = getGameTimerObjectH();
    box.h -= timerH;
    box.y += timerH;
  }
  return box;
}


//BOTTOM MENU BUTONS
#define BTN_B_AMOUNT 3
#define BTN_B_LEFT 0
#define BTN_B_MIDDLE 1
#define BTN_B_RIGHT 2

void Gui::drawBottomButtons() {
  if(showBottomButtons == false){
    return;
  }

  uint16_t buttonPositionX = 0;
  uint16_t buttonPositionY = DISPLAY_H-BUTTON_BAR_H;
  char buttonChars[4];
  char const * labels[4] =    { constants->emptyString, constants->emptyString, constants->emptyString, constants->emptyString }; //max 4 bottom buttons
  bool backButtonHasBeenPlaced = false;
  bool confirmButtonHasBeenPlaced = false;
  bool upButtonHasBeenPlaced = false;
  bool downButtonHasBeenPlaced = false;
  bool isEnabled = true;
  for(int i = 0; i < 4; i++){
    if((backButtonHasBeenPlaced == false) && (getBackButtonLabel()[0] != 0)){
      labels[i] = getBackButtonLabel();
      buttonChars[i] = constants->keypadStar;
      backButtonHasBeenPlaced = true;

    }
    else if((confirmButtonHasBeenPlaced == false) && (getConfirmButtonLabel()[0] != 0)){
      labels[i] = getConfirmButtonLabel();
      buttonChars[i] = constants->keypadHash;
      confirmButtonHasBeenPlaced = true;
    }
    else if((upButtonHasBeenPlaced == false) && (getUpButtonLabel()[0] != 0)){
      labels[i] = getUpButtonLabel();
      buttonChars[i] = constants->keypadUp;
      upButtonHasBeenPlaced = true;
      isEnabled = canNavigatePreviousMenuItem(activeMenu);
    }
    else if((downButtonHasBeenPlaced == false) && (getDownButtonLabel()[0] != 0)){
      labels[i] = getDownButtonLabel();
      buttonChars[i] = constants->keypadDown;
      downButtonHasBeenPlaced = true;
      isEnabled = canNavigateNextMenuItem(activeMenu);
    } else {
      continue;
    }
    buttonPositionX += guiButtons->drawKeypadButtonWithLabel(buttonPositionX, buttonPositionY, buttonChars[i], labels[i], ICON_BUTTON_SIZE_MEDIUM, isEnabled);
  }

}





const char * const Gui::getBackButtonLabel(){
  if ((!showBottomButtons) || (activeMenu == GUI_PROMPT)) {
    return constants->emptyString;
  }
  if (interface->getStatusIsIngame()) {
    if (interface->getKeyStateBackIsCaptured()) {
      if (interface->getKeyStateBackIsShown()) {
        return interface->getKeyStateBackString();
      } else {
        return constants->emptyString;
      }
    }
    if ((showMenu[GUI_GAME_MENU] == true) || (showMenu[GUI_GAME_SUB_MENU] == true)) {
      return constants->labelClose[globals->getLang()];
    }
    return constants->labelOptions[globals->getLang()];
  }
  if ((showMenu[GUI_MAIN_MENU] == true) || (showMenu[GUI_MAIN_SUB_MENU] == true)) {
    return constants->labelOpenMenu[globals->getLang()];
  }
  return constants->labelClose[globals->getLang()];
}


const char * const Gui::getConfirmButtonLabel(){
  if (!showBottomButtons) {
    return constants->emptyString;
  }
  if(((activeMenu == GUI_GAME_MENU) || (activeMenu == GUI_GAME_SUB_MENU) || (activeMenu == GUI_MAIN_SUB_MENU) || (activeMenu == GUI_MAIN_MENU)) && (cursorCurrent[activeMenu] != GUI_MENU_SELECTED_NONE)){
    return constants->labelSelect[globals->getLang()];
  }
  if(interface->getStatusIsIngame()){
    if (interface->getKeyStateConfirmIsShown()) {
      return interface->getKeyStateConfirmString();
    }
  }
  return constants->emptyString;
}


const char * const Gui::getUpButtonLabel(){
  if((activeMenu == GUI_GAME_MENU) 
  || (activeMenu == GUI_GAME_SUB_MENU) 
  || (activeMenu == GUI_MAIN_MENU) 
  || (activeMenu == GUI_MAIN_SUB_MENU))
  {
       return constants->labelUpSymbol; 
  }
  return constants->emptyString;
}


const char * const Gui::getDownButtonLabel(){
  if((activeMenu == GUI_GAME_MENU) 
  || (activeMenu == GUI_GAME_SUB_MENU) 
  || (activeMenu == GUI_MAIN_MENU) 
  || (activeMenu == GUI_MAIN_SUB_MENU))
  {
       return constants->labelDownSymbol;
  }
  return constants->emptyString;
}


void Gui::parseUserInput() {
  if (interface->getSelectedInputConsumerIsGui() == false) {
    if ((interface->getKeyStateBackIsCaptured() == true) || (interface->getLastReceivedKey() != constants->keypadStar)) {
      return;
    }
  }
  lastUserInput = interface->consumeLastReceivedKey();
  if (lastUserInput == constants->emptyChar) {
    return;
  }
  if((activeMenu == GUI_ALERT_BADGE) && (showMenu[GUI_ALERT_BADGE] == true) && ((lastUserInput == constants->keypadStar) || (lastUserInput == constants->keypadHash))){
    notificationBadge->closeForced();
    return;
  }
  if(activePrompt <= promptsCount && activeMenu == GUI_PROMPT && showMenu[GUI_PROMPT] == true){
    prompts[activePrompt]->inputChar(lastUserInput);
  }
  else if (lastUserInput == constants->keypadStar) {
    handleBackButtonPressed();
  }
  else if (lastUserInput == constants->keypadHash) {
    handleHashButtonPressed();
  }
  else if(interface->isNumeric(lastUserInput)) {
    handleNumericButtonPressed(lastUserInput);    
  }
  lastUserInput = constants->emptyChar;
}


void Gui::handleBackButtonPressed() {
  if (interface->getStatusIsIngame()) { //MODULE MENU TOGGLE
    if (showMenu[GUI_GAME_MENU] == false) {
      openMenu(GUI_GAME_MENU);
      interface->setSelectedInputConsumer(INPUT_CONSUMER_GUI);
      interface->playConfirm();
    } else {
      closeActiveMenu();
      interface->playCancel();
      if(activeMenu == GUI_MENU_SELECTED_NONE){
        interface->setSelectedInputConsumer(INPUT_CONSUMER_GAME); //give the input control back to the game object  
      }
    }
  }
}


void Gui::handleHashButtonPressed() {
  if(selectMenuItem(activeMenu, cursorCurrent[activeMenu]) == false){
    interface->playError();
    return;
  }

  if(cursorCurrent[activeMenu] < GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT && menuEntryIdMatrix[activeMenu][cursorCurrent[activeMenu]] < GUI_MENU_ENTRIES_COUNT){ //This first guard is to prevent random memory access to imaginary not assigned Array indexes. ^Alexander
    if(c_menuEntries[selectedCurrentId[activeMenu]].parentId != GUI_MENU_ENTRY_IS_CATEGORY){
      //If the selected item is not a category, but an actual menu entry, we need to handle their proper action
      if(performUserAction(c_menuEntries[selectedCurrentId[activeMenu]].targetId) == true){
        interface->playConfirm();
      } else {
        interface->playError();
      }
      return;
    }
  }

  if(activeMenu == GUI_GAME_MENU){
    openMenu(GUI_GAME_SUB_MENU);
    interface->playConfirm();
    return;
  }

  if(activeMenu == GUI_GAME_SUB_MENU){
    //For some reasong there's a category in the game sub menu. Not sure what to do in that situation ^Alexander
    interface->playBlocked();
    return;
  }
}


void Gui::handleNumericButtonPressed(char inputChar){
  if((activeMenu == GUI_GAME_MENU) 
  || (activeMenu == GUI_GAME_SUB_MENU) 
  || (activeMenu == GUI_MAIN_MENU) 
  || (activeMenu == GUI_MAIN_SUB_MENU))
  {
    if(inputChar == constants->keypadUp){
      navigatePreviousMenuItem(activeMenu);
      return;
    }
    if(inputChar == constants->keypadDown){
      navigateNextMenuItem(activeMenu);
      return;
    }
  }  
}


bool Gui::performUserAction(uint16_t actionId){
  uint16_t actionTarget = actionId;
  switch(actionId) {
    case TARGET_ID_SHOW_DEVICE_INFO_INGAME:
      actionTarget = constants->PROMPT_DEVICE_INFO;
      break;
    case TARGET_ID_SHOW_HINTS:
      actionTarget = constants->PROMPT_SHOW_HINTS;
      break;
    default:
      return false; 
  }
  if(actionTarget < promptsCount){
    openPrompt(GUI_PROMPT, actionTarget);
  } else {
    //Do some custom action
  }
  return true;
}



void Gui::updateMenuEntries(uint16_t menu){
    if(menu >= GUI_COUNT_MENUS_TOTAL){
      return;
    }
    uint16_t pos = 0;
    cursorMax[menu] = GUI_MENU_SELECTED_NONE;
    for(int i = 0; i < GUI_MENU_ENTRIES_COUNT; i++){


      uint16_t parentIdSelector = GUI_MENU_ENTRY_IS_CATEGORY;
      //A. Get the parentId if this menu is a sub-menu
      if((menu == GUI_MAIN_SUB_MENU) && (selectedCurrentId[GUI_MAIN_MENU] != GUI_MENU_SELECTED_NONE)){ //In the case of when the title of a sub menu is requested.
        parentIdSelector = c_menuEntries[selectedCurrentId[GUI_GAME_MENU]].id;
      }
      if((menu == GUI_GAME_SUB_MENU) && (selectedCurrentId[GUI_GAME_MENU] != GUI_MENU_SELECTED_NONE)){ //In the case of when the title of a sub menu is requested.
        parentIdSelector = c_menuEntries[selectedCurrentId[GUI_GAME_MENU]].id;
      }

      //1. When there are somehow more menu items available for this menu than permitted in the array, just truncate (return).
      if(pos >= GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT){
        return; 
      }
      if((c_menuEntries[i].visible == true)
        && (c_menuEntries[i].menu == menu)
        && (c_menuEntries[i].parentId == parentIdSelector) 
        && ((interface->getCurrentAccessLevel() & c_menuEntries[i].accessRequired) > 0 )
      ){
        menuEntryIdMatrix[menu][pos] = c_menuEntries[i].id;
        pos++;
      }
    }

    //3. Set the max cursor to the max value to highlight/select an item
    if(pos > 0){
      cursorMax[menu] = pos-1;
    }
    
    //4. fill the rest of the array with 'empty' items
    for(int p = pos; p < GUI_MENU_ENTRIES_COUNT; p++){
      menuEntryIdMatrix[menu][p] = GUI_MENU_ENTRY_ID_EMPTY; 
    }

    //5. Debugging
    // for(int m = 0; m < GUI_MENU_ENTRIES_COUNT; m++){
    //   Serial.print("MENU ");
    //   Serial.print(m);
    //   Serial.print(": ");
    //   for(int i = 0; i < GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT; i++){
    //     if(menuEntryIdMatrix[m][i] != GUI_MENU_ENTRY_ID_EMPTY){
    //       Serial.print(c_menuEntries[menuEntryIdMatrix[m][i]].id);
    //     } else {
    //       Serial.print('.');
    //     }
    //       Serial.print("  ");
    //   } 
    //   Serial.println("");
    // }
} // END of  Gui::updateMainMenuContents(uint16_t menu)


void Gui::updateSubMenuContents(uint16_t menu){

} // END of  Gui::updateSubMenuContents(uint16_t selectedParentEntryId)







//MENU METHODS

void Gui::openMenu(uint16_t menu){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return;
  }
  activeMenu = menu;
  updateMenuEntries(menu);
  showMenu[menu] = true;
  runAnimationForMenu(menu, ANIMATE_OPEN);
  navigateToMenuItem(menu);
}


void Gui::openPrompt(uint16_t menu, uint16_t actionTarget){
  if(actionTarget >= promptsCount){ //To prevent unasigned memory access
    return;
  }
  activePrompt = actionTarget; //Set the active prompt to the action target. This will be used for the prompt GfxLoop and normal loop
  Gui::FrameBox box = getMenuDimensions(menu); //get the proper dimensions for this prompt window
  prompts[activePrompt]->setPosition(box.x, box.y + MENU_HEADER_H, box.w, box.h - MENU_HEADER_H); //Set the available dimension space for this prompt
  openMenu(menu); //Open the prompt
  showBottomButtons = false;
}


void Gui::closeMenu(uint16_t menu){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return;
  }
  if(menu == GUI_ALERT_BADGE){
    resetAndCloseMenu(menu);
    if(showMenu[GUI_PROMPT] ==  true){
      activeMenu = GUI_PROMPT;
      return;
    }
    if(showMenu[GUI_MAIN_SUB_MENU] ==  true){
      activeMenu = GUI_MAIN_SUB_MENU;
      return;
    }
    if(showMenu[GUI_GAME_SUB_MENU] ==  true){
      activeMenu = GUI_GAME_SUB_MENU;
      return;
    }
    if(showMenu[GUI_MAIN_MENU] ==  true){
      activeMenu = GUI_MAIN_MENU;
      return;
    }
    if(showMenu[GUI_GAME_MENU] ==  true){
      activeMenu = GUI_GAME_MENU;
      return;
    }
  }
  if(menu == GUI_PROMPT){
    resetAndCloseMenu(menu);
    if(showMenu[GUI_MAIN_SUB_MENU] ==  true){
      activeMenu = GUI_MAIN_SUB_MENU;
      return;
    }
    if(showMenu[GUI_GAME_SUB_MENU] ==  true){
      activeMenu = GUI_GAME_SUB_MENU;
      return;
    }
    if(showMenu[GUI_MAIN_MENU] ==  true){
      activeMenu = GUI_MAIN_MENU;
      return;
    }
    if(showMenu[GUI_GAME_MENU] ==  true){
      activeMenu = GUI_GAME_MENU;
      return;
    }
  }
  if(menu == GUI_MAIN_SUB_MENU){
    resetAndCloseMenu(menu);
    activeMenu = GUI_MAIN_MENU;
    return;    
  }
  if(menu == GUI_GAME_SUB_MENU){
    resetAndCloseMenu(menu);
    activeMenu = GUI_GAME_MENU;
    return;    
  }
  resetAndCloseMenu(menu);
  interface->setSelectedInputConsumer((isAnyMenuStillOpen() == true) ? INPUT_CONSUMER_GUI : INPUT_CONSUMER_GAME);
}


void Gui::closeActiveMenu(){
  closeMenu(activeMenu);
}


void Gui::resetAndCloseMenu(uint16_t menu){
  if(activeMenu == menu){
    activeMenu = GUI_MENU_SELECTED_NONE;
  }
  showMenu[menu] = false;     
  selectedCurrent[menu] = GUI_MENU_SELECTED_NONE;
  selectedCurrentId[menu] = GUI_MENU_SELECTED_NONE;
  cursorCurrent[menu] = GUI_MENU_SELECTED_NONE;
  cursorCurrentId[menu] = GUI_MENU_SELECTED_NONE;
  cursorMin[menu] = 0;
  cursorMax[menu] = GUI_MENU_SELECTED_NONE;
  runAnimationForMenu(menu, ANIMATE_CLOSE);
}


void Gui::openBadge(){
  Gui::FrameBox box = getMenuDimensions(GUI_ALERT_BADGE); //get the proper dimensions for this badge window
  notificationBadge->setPosition(box.x, box.y + MENU_HEADER_H, box.w, box.h - MENU_HEADER_H); //Set the available dimension space for this prompt
  notificationBadge->init();
  openMenu(GUI_ALERT_BADGE);
}

void Gui::closeBadge(){
  closeMenu(GUI_ALERT_BADGE);
}



bool Gui::isAnyMenuStillOpen(){
  for(int i = 0; i < GUI_COUNT_MENUS_TOTAL; i++){
    if(showMenu[i] == true){
      return true;
    }
  }
  return false;
}



//MENU NAVIGATION METHODS
bool Gui::navigateToMenuItem(uint16_t menu, uint16_t index, bool searchUp, bool isCheckOnly){
  uint16_t maxIdx = min((GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT-1), cursorMax[menu]);
  if(index > maxIdx){
    return false;
  }
  if(searchUp == true){
    for(int i = index; i>0; i--){
      if(menuEntryIdMatrix[menu][i] == GUI_MENU_ENTRY_ID_EMPTY){
        continue;
      }
      if(c_menuEntries[menuEntryIdMatrix[menu][i]].enabled == true){
        if(isCheckOnly == true){
          return true;
        }
        cursorCurrent[menu] = i;
        cursorCurrentId[menu] = c_menuEntries[menuEntryIdMatrix[menu][i]].id;
        return true;
      }
    }    
  } else {
    for(int i = index; i<=maxIdx; i++){
      if(menuEntryIdMatrix[menu][i] == GUI_MENU_ENTRY_ID_EMPTY){
        continue;
      }
      if(c_menuEntries[menuEntryIdMatrix[menu][i]].enabled == true){
        if(isCheckOnly == true){
          return true;
        }
        cursorCurrent[menu] = i;
        cursorCurrentId[menu] = c_menuEntries[menuEntryIdMatrix[menu][i]].id;
        return true;
      }
    }
  }
  return false;
}

void Gui::navigateNextMenuItem(uint16_t menu){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return;
  }
  if(navigateToMenuItem(menu, cursorCurrent[menu]+1, false)){
    interface->playTickSoft();
  } else {
    interface->playBlocked();
  }
}

void Gui::navigatePreviousMenuItem(uint16_t menu){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return;
  }
  if(cursorCurrent[menu] < 1){
    interface->playBlocked();
    return;
  }
  if(navigateToMenuItem(menu, cursorCurrent[menu]-1, false)){
    interface->playTickSoft();
  } else {
    interface->playBlocked();
  }
}



bool Gui::canNavigateNextMenuItem(uint16_t menu){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return false;
  }
  if(navigateToMenuItem(menu, cursorCurrent[menu]+1, false, true)){
    return true;
  }
  return false;
}

bool Gui::canNavigatePreviousMenuItem(uint16_t menu){
    if(menu >= GUI_COUNT_MENUS_TOTAL){
    return false;
  }
  if(cursorCurrent[menu] < 1){
    return false;
  }
  if(navigateToMenuItem(menu, cursorCurrent[menu]-1, false, true)){
    return true;
  } 
  return false;
}


bool Gui::selectMenuItem(uint16_t menu, uint16_t pos){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return false;
  }
  if(pos >= GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT){
    return false;
  }
  if(menuEntryIdMatrix[menu][pos] >= GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT){
    return false;
  }
  if(c_menuEntries[menuEntryIdMatrix[menu][pos]].enabled == false){
    return false;    
  }
  if(pos >= GUI_MAX_MENU_ENTRIES_PER_MENU_COUNT ){
    return false;
  }
  selectedCurrent[menu] = pos;
  selectedCurrentId[menu] = menuEntryIdMatrix[menu][pos];
  return true;
}



//ANIMATION STUFF

void Gui::runAnimationForMenu(uint16_t menu, bool isForClosing){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return;
  }
  uint16_t animIdx = (isForClosing) ? 1 : 0;
  animations[menu].animationVariantCurrent = animations[menu].animationVariantSet + animIdx;
  animations[menu].animationOngoing = true;
  if(animations[menu].animationCurrentFrame != 0){
    animations[menu].animationCurrentFrame = animations[menu].animationMaxFrames - animations[menu].animationCurrentFrame;
  } else {
    animations[menu].animationFramesDelay = animations[menu].animationFramesDelaySet;
  }
}



Gui::FrameBox Gui::animationBoxGetFrame(uint16_t menu, Gui::FrameBox startBox, Gui::FrameBox endBox){
  if(menu >= GUI_COUNT_MENUS_TOTAL){
    return Gui::FrameBox {0,0,0,0};
  }
  if(animations[menu].animationOngoing == false){
    return endBox;
  }
  Gui::FrameBox returnBox;
  if(animations[menu].animationVariantCurrent == ANIMATION_VARIANT_LEFT_TO_RIGHT_OPEN || animations[menu].animationVariantCurrent == ANIMATION_VARIANT_RIGHT_TO_LEFT_OPEN || animations[menu].animationVariantCurrent == ANIMATION_VARIANT_CENTER_OPEN){
    returnBox.x = map(animations[menu].animationCurrentFrame, 0, animations[menu].animationMaxFrames, startBox.x, endBox.x);
    returnBox.y = map(animations[menu].animationCurrentFrame, 0, animations[menu].animationMaxFrames, startBox.y, endBox.y);
    returnBox.w = map(animations[menu].animationCurrentFrame, 0, animations[menu].animationMaxFrames, startBox.w, endBox.w);
    returnBox.h = map(animations[menu].animationCurrentFrame, 0, animations[menu].animationMaxFrames, startBox.h, endBox.h);
  } else if(animations[menu].animationVariantCurrent == ANIMATION_VARIANT_LEFT_TO_RIGHT_CLOSE || animations[menu].animationVariantCurrent == ANIMATION_VARIANT_RIGHT_TO_LEFT_CLOSE || animations[menu].animationVariantCurrent == ANIMATION_VARIANT_CENTER_CLOSE){
    returnBox.x = map(animations[menu].animationCurrentFrame, 0, animations[menu].animationMaxFrames, endBox.x, startBox.x);
    returnBox.y = map(animations[menu].animationCurrentFrame, 0, animations[menu].animationMaxFrames, endBox.y, startBox.y);
    returnBox.w = map(animations[menu].animationCurrentFrame, 0, animations[menu].animationMaxFrames, endBox.w, startBox.w);
    returnBox.h = map(animations[menu].animationCurrentFrame, 0, animations[menu].animationMaxFrames, endBox.h, startBox.h);
  }
  if(animations[menu].animationVariantCurrent == ANIMATION_VARIANT_RIGHT_TO_LEFT_OPEN || animations[menu].animationVariantCurrent ==  ANIMATION_VARIANT_RIGHT_TO_LEFT_CLOSE){
    returnBox.x = endBox.x+endBox.w-returnBox.w;
  } else if(animations[menu].animationVariantCurrent == ANIMATION_VARIANT_CENTER_OPEN || animations[menu].animationVariantCurrent ==  ANIMATION_VARIANT_CENTER_CLOSE){
    returnBox.x = returnBox.x + (endBox.w/2) - (returnBox.w/2);
  }
  return returnBox;
}

void Gui::loopAnimations(){
  for(int i = 0; i<GUI_COUNT_MENUS_TOTAL; i++){
    if((animations[i].animationCurrentFrame < animations[i].animationMaxFrames) && (animations[i].animationOngoing == true)){
      if(animations[i].animationFramesDelay > 0){
        animations[i].animationFramesDelay--;
        continue;
      }
      animations[i].animationCurrentFrame++;
      continue;
    } else if(animations[i].animationOngoing == true){
      animations[i].animationOngoing = false;
      animations[i].animationCurrentFrame = 0;
      animations[i].animationFramesDelay = animations[i].animationFramesDelaySet;
    }
    continue;
  }
}


void Gui::drawGui() {
  loopAnimations();
  if (showBottomButtons == true) {
    drawBottomButtons();
  }
  for(int menu = 0; menu < GUI_COUNT_MENUS_TOTAL; menu++){
    if ((showMenu[menu] == true) || (animations[menu].animationOngoing == true)) {
      drawMenu(menu);
    }
  }
  if(activePrompt <= promptsCount && activeMenu == GUI_PROMPT && showMenu[GUI_PROMPT] == true && animations[GUI_PROMPT].animationOngoing == false){
    prompts[activePrompt]->loopGfx();
  }
  if((activeMenu == GUI_ALERT_BADGE) && (showMenu[GUI_ALERT_BADGE] == true) && (animations[GUI_ALERT_BADGE].animationOngoing == false)){
    notificationBadge->loopGfx();
  }
}


void Gui::loop() {
  parseUserInput();
  if(activePrompt <= promptsCount && activeMenu == GUI_PROMPT && showMenu[GUI_PROMPT] == true){
    prompts[activePrompt]->loop();
    if(prompts[activePrompt]->isRequestingToClose()){
      prompts[activePrompt]->close();
      closeActiveMenu();
      showBottomButtons = true;
    }
  }
  uint16_t countAvailableMessages = notificationBadge->getAvailableMessagesCount();
  if((showMenu[GUI_ALERT_BADGE] == false) && (countAvailableMessages > 0)){
    showMenu[GUI_ALERT_BADGE] = true;
    openBadge();
    showBottomButtons = false;
    interface->setSelectedInputConsumer(INPUT_CONSUMER_GUI);
  } else if(showMenu[GUI_ALERT_BADGE] == true && (countAvailableMessages < 1) ){
    showMenu[GUI_ALERT_BADGE] = false;
    closeBadge();
    showBottomButtons = true;
  }
  if((showMenu[GUI_ALERT_BADGE] == true) && (animations[GUI_ALERT_BADGE].animationOngoing == false)){
    notificationBadge->loop();
  }
}

void Gui::loopGfx() {
  drawGui();
}



//HELPER METHODS

void Gui::_drawProgressBarSimpleHorizontal(Gui::FrameBox box, uint32_t dataMin, uint32_t dataMax, uint32_t dataCurrent){
    uint16_t spacer = 2;
    uint16_t progressBarW = map(dataCurrent, dataMin, dataMax, 0, box.w - (2*spacer));
    tgxImage->drawRect(box.x, box.y, box.w, box.h, constants->colorGuiMenuFrame[constants->getColorScheme()]);
    tgxImage->fillRect(box.x + spacer, box.y + spacer, progressBarW, box.h - (2*spacer), constants->colorGuiMenuFrame[constants->getColorScheme()]);
}


Gui::FrameBox Gui::_getCalculatedHeaderTextPos(Gui::FrameBox box){
  Gui::FrameBox boxR = box;
  boxR.x += OFFSET_TEXT_X;
  boxR.y += OFFSET_TEXT_Y;
  return boxR;
}

Gui::FrameBox Gui::_getCalculatedContentTextPos(Gui::FrameBox box){
  Gui::FrameBox boxR = box;
  boxR.x += OFFSET_TEXT_X;
  boxR.y += MENU_ITEM_LINE_H + OFFSET_TEXT_Y;
  return boxR;
}



