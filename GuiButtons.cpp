#include "GuiButtons.h"

GuiButtons::GuiButtons(Image<RGB565> *tgxImageObj, Sprites *spritesObj, Constants *constantsObj, Globals *globalsObj)
  : tgxImage(tgxImageObj), sprites(spritesObj), constants(constantsObj), globals(globalsObj) {

  }


  void GuiButtons::drawKeypadButtonIcon(char character, uint16_t x, uint16_t y, uint16_t size, bool showAsEnabled){
    uint16_t textOffsetY = 0;
    uint16_t textOffsetX = 0;
    uint16_t buttonCornerRadius = 0;
    uint16_t buttonHeight = 0;
    uint16_t buttonWidth = 0;
    ILI9341_t3_font_t font = FONT_SMALL;
    uint16_t color = 0b0100000100001000; 
    
    if(showAsEnabled == true) { color = constants->colorGuiMenuTextContent[constants->getColorScheme()]; } else { color = constants->colorGuiMenuTextContentDisabled[constants->getColorScheme()]; }

    if(size == ICON_BUTTON_SIZE_SMALL){
      textOffsetY = 12;
      textOffsetX = 5;
      buttonCornerRadius = 2;
      buttonHeight = 14;
      buttonWidth = 16;
      font = FONT_BOLD_SMALL;
    } else if (size == ICON_BUTTON_SIZE_MEDIUM){
      textOffsetY = 15;
      textOffsetX = 7;
      buttonCornerRadius = 2;
      buttonHeight = 18;
      buttonWidth = 22;
      font = FONT_BOLD_MEDIUM;
    } else if (size == ICON_BUTTON_SIZE_BIG){
      textOffsetY = 20;
      textOffsetX = 10;
      buttonCornerRadius = 3;
      buttonHeight = 25;
      buttonWidth = 31;
      font = FONT_BOLD_BIG;
    }
    tgxImage->fillRoundRect(x, y, buttonWidth, buttonHeight, buttonCornerRadius, color);
    if(character == '*'){
      if(size == ICON_BUTTON_SIZE_SMALL){
        font = font_tgx_Arial_Bold_16;
        textOffsetY += 7;
        textOffsetX += -1;
      }else if(size == ICON_BUTTON_SIZE_MEDIUM){
        font = font_tgx_Arial_Bold_20;
        textOffsetY += 9;
        textOffsetX += -1;
      }else if(size == ICON_BUTTON_SIZE_BIG){
        font = font_tgx_Arial_Bold_32;
        textOffsetY += 17;
        textOffsetX += -2;
      }
    }
    if(character == '#'){
      if(size == ICON_BUTTON_SIZE_MEDIUM){
        textOffsetX += -1;
      }
      if(size == ICON_BUTTON_SIZE_SMALL){
        tgxImage->drawPixel(x + textOffsetX + 6, y + textOffsetY - 7, constants->colorGuiMenuTextHeader[constants->getColorScheme()]);
        tgxImage->drawPixel(x + textOffsetX -1, y + textOffsetY - 4, constants->colorGuiMenuTextHeader[constants->getColorScheme()]);
      }
    }
    tgxImage->drawChar(character, iVec2{ x + textOffsetX, y + textOffsetY }, constants->colorGuiMenuTextHeader[constants->getColorScheme()], font);
  };



  uint16_t GuiButtons::drawKeypadButtonWithLabel(uint16_t x, uint16_t y, char buttonChar, const char * buttonLabel, uint16_t size, bool isEnabled){
    Constants::FontPackage fontPackage;
    uint16_t buttonWidth;
    uint16_t buttonHeight;

    if(size == ICON_BUTTON_SIZE_SMALL){
      fontPackage = constants->smallFont;
      buttonHeight = BUTTON_HEIGHT_SMALL;
      buttonWidth = 16;
    }
    else if(size == ICON_BUTTON_SIZE_BIG){
      fontPackage = constants->bigFont;
      buttonHeight = BUTTON_HEIGHT_BIG;
      buttonWidth = 31;
    } 
    else {
      fontPackage = constants->mediumFont;
      buttonHeight = BUTTON_HEIGHT_MEDIUM;
      buttonWidth = 22;
    }

    uint16_t cornerRounding = (size == ICON_BUTTON_SIZE_SMALL) ? 2 : 3;
    uint16_t buttonToTextPadding = (size == ICON_BUTTON_SIZE_SMALL) ? 2 : 3;
    uint16_t textToEdgePadding = (size == ICON_BUTTON_SIZE_SMALL) ? 4 : (size == ICON_BUTTON_SIZE_BIG) ? 6 : 5;
    uint16_t buttonEdgeToEdgePadding = (size == ICON_BUTTON_SIZE_SMALL) ? 4 : 5;
    uint16_t color = (isEnabled == true) ? constants->colorGuiMenuTextContent[constants->getColorScheme()] : constants->colorGuiMenuTextContentDisabled[constants->getColorScheme()];
    uint16_t colorBackground = constants->colorGuiMenuBackground[constants->getColorScheme()];
    uint16_t textWidth = max(buttonWidth-buttonToTextPadding, tgxImage->measureText(buttonLabel, iVec2 {x+fontPackage.offsetX, y+fontPackage.offsetY}, fontPackage.font, false).lx());
    
    tgxImage->fillRoundRect(x, y, buttonWidth + textWidth + textToEdgePadding + buttonToTextPadding, buttonHeight, cornerRounding, colorBackground);
    tgxImage->drawRoundRect(x, y, buttonWidth + textWidth + textToEdgePadding + buttonToTextPadding, buttonHeight, cornerRounding, color);
    drawKeypadButtonIcon(buttonChar, x, y, size, isEnabled);
    if((buttonLabel[0] == '^' || buttonLabel[0] == '_') && buttonLabel[1] == 0){
      iVec2 pointA = (buttonLabel[0] == '^') ? iVec2{x + buttonWidth + buttonToTextPadding, y+buttonHeight-buttonToTextPadding}                                      : iVec2{x + buttonWidth + buttonToTextPadding, y+buttonToTextPadding}; //left most part of the triangle
      iVec2 pointB = (buttonLabel[0] == '^') ? iVec2{x + buttonWidth + buttonToTextPadding + (buttonWidth/2)-1, y+buttonToTextPadding}                               : iVec2{x + buttonWidth + buttonToTextPadding + (buttonWidth/2)-1, y+buttonHeight-buttonToTextPadding}; //The point of the triangle
      iVec2 pointC = (buttonLabel[0] == '^') ? iVec2{x + buttonWidth + buttonToTextPadding + (buttonWidth-buttonToTextPadding), y+buttonHeight-buttonToTextPadding}  : iVec2{x + buttonWidth + buttonToTextPadding + (buttonWidth-buttonToTextPadding), y+buttonToTextPadding}; //Right most part of the triangle
      tgxImage->fillTriangle(pointA, pointB, pointC, color, color);
    } else {
      tgxImage->drawText(buttonLabel, iVec2{ x+fontPackage.offsetX + buttonWidth + buttonToTextPadding , y+fontPackage.offsetY }, color, fontPackage.font, true);
    }
    
    return buttonWidth + textWidth + textToEdgePadding + buttonToTextPadding + buttonEdgeToEdgePadding; //Return width of all what has been created
  }
