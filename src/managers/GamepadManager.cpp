#include "GamepadManager.h"
#include "config.h"
#include "GamepadLogger.h"
#include <Arduino.h>

// Blinking state variables
volatile bool leftTurnBlinking = false;
volatile bool rightTurnBlinking = false;
volatile bool emergencyMode = false;
volatile bool leftLedState = false;
volatile bool rightLedState = false;

// Handle blinking logic - called every 100ms by timer
void handleBlinking() {
    // Left turn signal
    if (leftTurnBlinking) {
        leftLedState = !leftLedState;
        digitalWrite(LEFT_TURN_PIN, leftLedState);
    } else {
        digitalWrite(LEFT_TURN_PIN, LOW);
        leftLedState = false;
    }
    
    // Right turn signal
    if (rightTurnBlinking) {
        rightLedState = !rightLedState;
        digitalWrite(RIGHT_TURN_PIN, rightLedState);
    } else {
        digitalWrite(RIGHT_TURN_PIN, LOW);
        rightLedState = false;
    }
}

void processGamepad(ControllerPtr ctl) {
    logGamepadOnStateChange(ctl);

    // --- Button Y (Headlights) ---
    static bool headlights = false;
    bool btnY = ctl->buttons() & 0x0008;
    static bool btnYPrevState = false;
    if (btnY && !btnYPrevState) {
        headlights = !headlights;
        digitalWrite(HEAD_LIGHTS_PIN, headlights ? HIGH : LOW);
        Serial.printf("HEAD_LIGHTS_PIN %s\n", headlights ? "ON" : "OFF");
    }
    btnYPrevState = btnY;

    // --- Button States ---
    bool btnL1 = ctl->buttons() & 0x0010;
    bool btnR1 = ctl->buttons() & 0x0020;
    static bool btnL1PrevState = false;
    static bool btnR1PrevState = false;

    // --- Emergency Mode (Both buttons pressed together) ---
    static bool emergencyPrevState = false;
    bool bothPressed = btnL1 && btnR1;
    
    if (bothPressed && !emergencyPrevState) {
        emergencyMode = !emergencyMode;
        if (emergencyMode) {
            // Turn off individual signals when entering emergency mode
            leftTurnBlinking = false;
            rightTurnBlinking = false;
        } else {
            // Turn off all lights when exiting emergency mode
            digitalWrite(LEFT_TURN_PIN, LOW);
            digitalWrite(RIGHT_TURN_PIN, LOW);
        }
        Serial.printf("Emergency Mode %s\n", emergencyMode ? "ON" : "OFF");
    }
    emergencyPrevState = bothPressed;

    // --- Left Turn Signal (Button L1) ---
    if (btnL1 && !btnL1PrevState && !emergencyMode) {
        leftTurnBlinking = !leftTurnBlinking;
        if (!leftTurnBlinking) {
            digitalWrite(LEFT_TURN_PIN, LOW);
            leftLedState = false;
        }
        Serial.printf("Left Turn Signal %s\n", leftTurnBlinking ? "ON" : "OFF");
    }
    btnL1PrevState = btnL1;

    // --- Right Turn Signal (Button R1) ---
    if (btnR1 && !btnR1PrevState && !emergencyMode) {
        rightTurnBlinking = !rightTurnBlinking;
        if (!rightTurnBlinking) {
            digitalWrite(RIGHT_TURN_PIN, LOW);
            rightLedState = false;
        }
        Serial.printf("Right Turn Signal %s\n", rightTurnBlinking ? "ON" : "OFF");
    }
    btnR1PrevState = btnR1;
}