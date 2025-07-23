#include "GamepadManager.h"
#include "config.h"
#include "GamepadLogger.h"
#include <Arduino.h>

void processGamepad(ControllerPtr ctl) {
    // The manager is now only responsible for logging and game logic,
    // not the implementation of how logging is done.
    logGamepadOnStateChange(ctl);

    // --- Button-specific actions ---
    static bool headlights = false;
    bool currentButtonState = ctl->buttons() & 0x0008;
    static bool prevButtonState = false;
    if (currentButtonState && !prevButtonState) {
        headlights = !headlights;
        digitalWrite(HEAD_LIGHTS_PIN, headlights ? HIGH : LOW);
        Serial.printf("Button toggled - LED %s\n", headlights ? "ON" : "OFF");
    }
    prevButtonState = currentButtonState;

    // Other button examples
    if (ctl->a()) {
        static int colorIdx = 0;
        switch (colorIdx % 3) {
            case 0: ctl->setColorLED(255, 0, 0); break;  // Red
            case 1: ctl->setColorLED(0, 255, 0); break;  // Green
            case 2: ctl->setColorLED(0, 0, 255); break;  // Blue
        }
        colorIdx++;
    }

    if (ctl->b()) {
        static int led = 0;
        led++;
        ctl->setPlayerLEDs(led & 0x0f);
    }

    if (ctl->x()) {
        ctl->playDualRumble(0, 250, 0x80, 0x40);
    }
}