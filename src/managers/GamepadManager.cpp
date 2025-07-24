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
    static bool leftTurnBlinking = false;
    static unsigned long lastBlinkTimeL = 0;
    static bool leftLedState = LOW;
    const int blinkInterval = 150; // 150ms interval for blinking

    bool btnL1 = ctl->buttons() & 0x0010;
    static bool btnL1PrevState = false;

    if (btnL1 && !btnL1PrevState) {
        leftTurnBlinking = !leftTurnBlinking;
        if (!leftTurnBlinking) {
            digitalWrite(LEFT_TURN_PIN, LOW); // Turn off LED when blinking is disabled
        }
    }
    btnL1PrevState = btnL1;

    if (leftTurnBlinking) {
        unsigned long currentTime = millis();
        if (currentTime - lastBlinkTimeL >= blinkInterval) {
            lastBlinkTimeL = currentTime;
            leftLedState = !leftLedState;
            digitalWrite(LEFT_TURN_PIN, leftLedState);
        }
    }

    // --- Button R1 ---
    static bool rightTurnBlinking = false;
    static unsigned long lastBlinkTimeR = 0;
    static bool rightLedState = LOW;

    bool btnR1 = ctl->buttons() & 0x0020;
    static bool btnR1PrevState = false;

    if (btnR1 && !btnR1PrevState) {
        rightTurnBlinking = !rightTurnBlinking;
        if (!rightTurnBlinking) {
            digitalWrite(RIGHT_TURN_PIN, LOW); // Turn off LED when blinking is disabled
        }
    }
    btnR1PrevState = btnR1;

    if (rightTurnBlinking) {
        unsigned long currentTime = millis();
        if (currentTime - lastBlinkTimeR >= blinkInterval) {
            lastBlinkTimeR = currentTime;
            rightLedState = !rightLedState;
            digitalWrite(RIGHT_TURN_PIN, rightLedState);
        }
    }

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