#include "GamepadLogger.h"
#include <Arduino.h>

// This function is now internal to the logger and is only called when there's a change.
static void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d\n",
        ctl->dpad(), ctl->buttons(), ctl->axisX(), ctl->axisY(),
        ctl->axisRX(), ctl->axisRY()
    );
}

void logGamepadOnStateChange(ControllerPtr ctl) {
    // --- State tracking is now correctly encapsulated within the logger ---
    static uint32_t lastButtons = 0;
    static uint8_t lastDpad = 0;
    static int16_t lastAxisX = 0, lastAxisY = 0, lastAxisRX = 0, lastAxisRY = 0;

    // --- Get current state ---
    uint32_t currentButtons = ctl->buttons();
    uint8_t currentDpad = ctl->dpad();
    int16_t currentAxisX = ctl->axisX(), currentAxisY = ctl->axisY();
    int16_t currentAxisRX = ctl->axisRX(), currentAxisRY = ctl->axisRY();

    // --- Check for any change ---
    if (currentButtons != lastButtons || currentDpad != lastDpad ||
        currentAxisX != lastAxisX || currentAxisY != lastAxisY ||
        currentAxisRX != lastAxisRX || currentAxisRY != lastAxisRY) {
        
        dumpGamepad(ctl);

        // --- Update last known state ---
        lastButtons = currentButtons;
        lastDpad = currentDpad;
        lastAxisX = currentAxisX; lastAxisY = currentAxisY;
        lastAxisRX = currentAxisRX; lastAxisRY = currentAxisRY;
    }
}