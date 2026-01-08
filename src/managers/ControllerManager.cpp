#include "ControllerManager.h"
#include "GamepadManager.h"
// #include "KeyboardManager.h"
// #include "MouseManager.h"
// #include "BalanceBoardManager.h"
#include <Arduino.h>

// Global array to hold controller pointers
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(),
                          properties.vendor_id, properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not find empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }
    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            // } else if (myController->isMouse()) {
            //     processMouse(myController);
            // } else if (myController->isKeyboard()) {
            //     processKeyboard(myController);
            // } else if (myController->isBalanceBoard()) {
            //     processBalanceBoard(myController);
            } else {
                // When other controller types are connected, they will be reported as "Unsupported"
                // but their specific logic will not be processed.
                if (!myController->isGamepad()) {
                    Serial.println("Unsupported controller");
                }
            }
        }
    }
}
