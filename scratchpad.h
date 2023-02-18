//SCRATCHPAD FOR UNUSED BUT POSSIBLE USEFUL SNIPPETS

#include "wiring.h"
loop(){
  usbHostController.Task();

    // if (*hiddrivers[0] != hid_driver_active[0]) {
    //   if (hid_driver_active[0]) {
    //     Serial.printf("*** HID Device %s - disconnected ***\n", hid_driver_names[0]);
    //     hid_driver_active[0] = false;
    //   } else {
    //     Serial.printf("*** HID Device %s %x:%x - connected ***\n", hid_driver_names[0], hiddrivers[0]->idVendor(), hiddrivers[0]->idProduct());
    //     hid_driver_active[0] = true;

    //     const uint8_t *psz = hiddrivers[0]->manufacturer();
    //     if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
    //     psz = hiddrivers[0]->product();
    //     if (psz && *psz) Serial.printf("  product: %s\n", psz);
    //     psz = hiddrivers[0]->serialNumber();
    //     if (psz && *psz) Serial.printf("  Serial: %s\n", psz);
    //   }
    // }

    if (mouse1.available()) {
    Serial.print("Mouse: buttons = ");
    Serial.print(mouse1.getButtons());
    Serial.print(",  mouseX = ");
    mousePosX += mouse1.getMouseX();
    if(mousePosX < 0){
      mousePosX = 0;
    }
    if(mousePosX >= LX){
      mousePosX = LX-1;
    }
    Serial.print(mouse1.getMouseX());
    Serial.print(",  mouseY = ");
        mousePosY += mouse1.getMouseY();
    if(mousePosY < 0){
      mousePosY = 0;
    }
    if(mousePosY >= LY){
      mousePosY = LY-1;
    }    
    Serial.print(mouse1.getMouseY());
    Serial.print(",  wheel = ");
    Serial.print(mouse1.getWheel());
    Serial.print(",  wheelH = ");
    Serial.print(mouse1.getWheelH());
    Serial.println();
    mouse1.mouseDataClear();
    }
    drawMousePointer(frameBuffer);
}

int16_t mousePosX = 0;
int16_t mousePosY = 0;
void drawMousePointer(uint16_t* fb){
    tgxImage.blitMasked(spriteMousePointer.sprite, spriteMousePointer.alphaMaskColor, iVec2{mousePosX, mousePosY}, 1.0f);
}