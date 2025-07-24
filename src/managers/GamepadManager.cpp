#include "GamepadManager.h"
#include "config.h"
#include "GamepadLogger.h"
#include <Arduino.h>

void processGamepad(ControllerPtr ctl) {
    // The manager is now only responsible for logging and game logic,
    // not the implementation of how logging is done.
    logGamepadOnStateChange(ctl);

    // --- Button Y ---
    static bool headlights = false;
    bool btnY = ctl->buttons() & 0x0008;
    static bool btnYPrevState = false;
    if (btnY && !btnYPrevState) {
        headlights = !headlights;
        digitalWrite(HEAD_LIGHTS_PIN, headlights ? HIGH : LOW);
        Serial.printf("HEAD_LIGHTS_PIN %s\n", headlights ? "ON" : "OFF");
    }
    btnYPrevState = btnY;

    // --- Button A ---
    bool btnA = ctl->buttons() & 0x0001;
    digitalWrite(BREAK_LIGHTS_PIN, btnA ? HIGH : LOW);

    // --- Button L1 ---
    static bool leftTernLights = false;
    bool btnL1 = ctl->buttons() &  0x0010;
    static bool btnL1PrevState = false;
    if (btnL1 && !btnL1PrevState) {
        leftTernLights = !leftTernLights;
        digitalWrite(LEFT_TURN_PIN, leftTernLights ? HIGH : LOW);
        Serial.printf("LEFT_TURN_PIN %s\n", leftTernLights ? "ON" : "OFF");
    }
    btnL1PrevState = btnL1;

    // --- Button R2 ---
    static bool rightTernLights = false;
    bool btnR1 = ctl->buttons() &  0x0020;
    static bool btnR1PrevState = false;
    if (btnR1 && !btnR1PrevState) {
        rightTernLights = !rightTernLights;
        digitalWrite(RIGHT_TURN_PIN, rightTernLights ? HIGH : LOW);
        Serial.printf("RIGHT_TURN_PIN %s\n", rightTernLights ? "ON" : "OFF");
    }
    btnR1PrevState = btnR1;

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