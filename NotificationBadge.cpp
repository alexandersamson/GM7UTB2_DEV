#include "core_pins.h"
#include <stdint.h>
#include "NotificationBadge.h"

NotificationBadge::NotificationBadge(Constants* constantsObj, Globals* globalsObj, Image<RGB565>* tgxImageObj, Sprites* spritesObj, GuiButtons* guiButtonsObj)
  : constants(constantsObj), globals(globalsObj), tgxImage(tgxImageObj), sprites(spritesObj), guiButtons(guiButtonsObj) {
}

void NotificationBadge::init() {
  lastMillis = millis();
}

void NotificationBadge::setPosition(NotificationBadge::Position pos) {
  position.x = pos.x;
  position.y = pos.y;
  position.width = pos.width;
  position.height = pos.height;
}

void NotificationBadge::setPosition(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  position.x = x;
  position.y = y;
  position.width = w;
  position.height = h;
}


uint16_t NotificationBadge::getAvailableMessagesCount() {
  uint16_t counter = 0;
  for (int i = 0; i < MAX_MESSAGES_COUNT; i++) {
    if ((globals->notificationMessages[i].isNew == true) || (globals->notificationMessages[i].durationLeft > 0)) {
      counter++;
    }
  }
  return counter;
}

uint16_t NotificationBadge::getContentHeight() {
  return (getAvailableMessagesCount() * (constants->mediumFont.lineHeight + 5)) + 4;
}


void NotificationBadge::drawTextLines() {
}


void NotificationBadge::closeForced() {
  for (int i = 0; i < MAX_MESSAGES_COUNT; i++) {
    if (globals->notificationMessages[i].durationLeft > 0) {
      globals->notificationMessages[i].durationLeft = 0;
      globals->notificationMessages[i].isNew = false;
    }
  }
}

void NotificationBadge::loop() {
  uint32_t millisseconds = millis();
  uint32_t deltaMillis = millisseconds - lastMillis;
  lastMillis = millisseconds;
  for (int i = 0; i < MAX_MESSAGES_COUNT; i++) {
    if (globals->notificationMessages[i].isNew) {
      globals->notificationMessages[i].isNew = false;
    }
    if (globals->notificationMessages[i].durationLeft > 0) {
      globals->notificationMessages[i].durationLeft -= (globals->notificationMessages[i].durationLeft >= deltaMillis) ? deltaMillis : globals->notificationMessages[i].durationLeft;
    }
  }
}

void NotificationBadge::loopGfx() {
  uint16_t messageCount = 0;
  uint16_t progressLineWidth = 0;
  for (int i = 0; i < MAX_MESSAGES_COUNT; i++) {
    if (globals->notificationMessages[i].durationLeft > 0) {
      tgxImage->drawText(globals->notificationMessages[i].message, iVec2{ position.x + 3, position.y + 15 + (messageCount * (constants->mediumFont.lineHeight + 5)) }, constants->colorGuiMenuTextContent[constants->getColorScheme()], constants->mediumFont.font, false);
      progressLineWidth = max(progressLineWidth, map(globals->notificationMessages[i].durationLeft, 0, globals->notificationMessages[i].durationSet, 0, position.width - 2));
      messageCount++;
    }
  }
  tgxImage->drawFastHLine(iVec2{ position.x + 1, position.y + (messageCount * (constants->mediumFont.lineHeight + 5) + 4) }, progressLineWidth, constants->colorGuiMenuTextContentDisabled[constants->getColorScheme()]);
  tgxImage->drawFastHLine(iVec2{ position.x + 1, position.y + (messageCount * (constants->mediumFont.lineHeight + 5) + 5) }, progressLineWidth, constants->colorGuiMenuTextContentDisabled[constants->getColorScheme()]);
  tgxImage->drawFastHLine(iVec2{ position.x + 1, position.y + (messageCount * (constants->mediumFont.lineHeight + 5) + 6) }, progressLineWidth, constants->colorGuiMenuTextContentDisabled[constants->getColorScheme()]);
}
