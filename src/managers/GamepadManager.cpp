#include "managers/GamepadManager.h"
#include <Bluepad32.h>
#include <ESP32Servo.h>
#include "config.h"
#include <Arduino.h>

// Steering servo, created and attached in main.cpp.
extern Servo steerServo;

// Blinking state variables
volatile bool leftTurnBlinking = false;
volatile bool rightTurnBlinking = false;
volatile bool emergencyMode = false;
volatile bool leftLedState = false;
volatile bool rightLedState = false;

// Helper function to log all controller inputs for debugging
void logAllControls(ControllerPtr ctl) {
    static int32_t last_buttons = 0;
    static int32_t last_axis_x = 0;
    static int32_t last_axis_y = 0;
    static int32_t last_axis_rx = 0;
    static int32_t last_axis_ry = 0;

    if (ctl->buttons() != last_buttons) {
        Serial.printf("Buttons changed: 0x%04X\n", ctl->buttons());
        last_buttons = ctl->buttons();
    }
    if (ctl->axisX() != last_axis_x || ctl->axisY() != last_axis_y) {
        Serial.printf("Left Stick Axis: X=%d, Y=%d\n", ctl->axisX(), ctl->axisY());
        last_axis_x = ctl->axisX();
        last_axis_y = ctl->axisY();
    }
    if (ctl->axisRX() != last_axis_rx || ctl->axisRY() != last_axis_ry) {
        Serial.printf("Right Stick Axis: X=%d, Y=%d\n", ctl->axisRX(), ctl->axisRY());
        last_axis_rx = ctl->axisRX();
        last_axis_ry = ctl->axisRY();
    }
}

// Handle blinking logic - called by a timer
void handleBlinking() {
    // Emergency mode overrides individual signals
    if (emergencyMode) {
        leftLedState = !leftLedState;
        rightLedState = leftLedState; // Synchronize blinking
        digitalWrite(LEFT_TURN_PIN, leftLedState);
        digitalWrite(RIGHT_TURN_PIN, rightLedState);
        return; // Skip individual signal logic
    }
    
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
    // Log all controls for debugging
    logAllControls(ctl);

    // --- Button Y (Headlights) ---
    static bool headlights = false;
    static bool btnY_prev = false;
    bool btnY_curr = (ctl->buttons() & 0x0008); // Hardcoded value for BUTTON_Y
    if (btnY_curr && !btnY_prev) { // Check for the rising edge (press event)
        headlights = !headlights;
        digitalWrite(HEAD_LIGHTS_PIN, headlights);
        Serial.printf("Headlights: %s\n", headlights ? "ON" : "OFF");
    }
    btnY_prev = btnY_curr;

    // --- L1/R1 for Turn Signals and Emergency ---
    static bool btnL1_prev = false;
    static bool btnR1_prev = false;
    bool btnL1_curr = (ctl->buttons() & 0x0010); // Hardcoded value for BUTTON_L1
    bool btnR1_curr = (ctl->buttons() & 0x0020); // Hardcoded value for BUTTON_R1

    // Emergency Mode (one button is pressed while the other is already held)
    if ((btnL1_curr && !btnL1_prev && btnR1_curr) || (btnR1_curr && !btnR1_prev && btnL1_curr)) {
        emergencyMode = !emergencyMode;
        Serial.printf("Emergency Mode: %s\n", emergencyMode ? "ON" : "OFF");
        if (emergencyMode) {
            // Turn off individual signals when entering emergency mode
            leftTurnBlinking = false;
            rightTurnBlinking = false;
        } else {
             // Turn off all lights when exiting emergency mode
            digitalWrite(LEFT_TURN_PIN, LOW);
            digitalWrite(RIGHT_TURN_PIN, LOW);
        }
    }

    // Left Turn Signal (L1 pressed, but not R1, and not in emergency mode)
    if (btnL1_curr && !btnL1_prev && !emergencyMode && !btnR1_curr) {
        leftTurnBlinking = !leftTurnBlinking;
        if(leftTurnBlinking) rightTurnBlinking = false; // Cancel opposite signal
        Serial.printf("Left Signal: %s\n", leftTurnBlinking ? "ON" : "OFF");
    }

    // Right Turn Signal (R1 pressed, but not L1, and not in emergency mode)
    if (btnR1_curr && !btnR1_prev && !emergencyMode && !btnL1_curr) {
        rightTurnBlinking = !rightTurnBlinking;
        if(rightTurnBlinking) leftTurnBlinking = false; // Cancel opposite signal
        Serial.printf("Right Signal: %s\n", rightTurnBlinking ? "ON" : "OFF");
    }
    
    // Update previous states for next loop
    btnL1_prev = btnL1_curr;
    btnR1_prev = btnR1_curr;


    // --- Steering (Right Joystick X-axis, servo) ---
    // Stick position sets a servo angle. An expo curve makes the center gentler,
    // so small stick moves turn the wheels only a little. Full stick still reaches
    // full lock. The servo holds the angle, so steering stays smooth.
    int steer_axis = ctl->axisRX(); // Right stick X, about -512 (left) to 512 (right)
    const int STEER_DEADZONE = 60;  // Snap to center when the stick is near the middle

    int angle;
    if (abs(steer_axis) <= STEER_DEADZONE) {
        angle = STEER_CENTER; // Wheels straight
    } else {
        // How far past the deadzone, from 0.0 (edge) to 1.0 (full stick).
        float amount = (float)(abs(steer_axis) - STEER_DEADZONE) / (512 - STEER_DEADZONE);
        if (amount > 1.0f) amount = 1.0f;

        // Bend the response: soft near center, full at the ends.
        float shaped = STEER_EXPO * amount * amount * amount + (1 - STEER_EXPO) * amount;

        if (steer_axis > 0) {
            angle = STEER_CENTER + shaped * (STEER_RIGHT - STEER_CENTER);
        } else {
            angle = STEER_CENTER - shaped * (STEER_CENTER - STEER_LEFT);
        }
    }
    steerServo.write(angle);


    // --- Driving (Left Joystick Y-axis) ---
    int drive_axis = ctl->axisY(); // Left stick Y
    const int DRIVE_DEADZONE = 25; // A small deadzone

    // Note: On most controllers, UP is negative, DOWN is positive.
    if (drive_axis < -DRIVE_DEADZONE) {
        // Drive Forward (joystick pushed up)
        digitalWrite(DRIVE_FORWARD_PIN, HIGH);
        digitalWrite(DRIVE_BACKWARD_PIN, LOW);
        int speed = map(abs(drive_axis), DRIVE_DEADZONE, 512, 0, 255);
        ledcWrite(DRIVE_SPEED_CHANNEL, speed);
    } else if (drive_axis > DRIVE_DEADZONE) {
        // Drive Backward (joystick pulled down)
        digitalWrite(DRIVE_FORWARD_PIN, LOW);
        digitalWrite(DRIVE_BACKWARD_PIN, HIGH);
        int speed = map(drive_axis, DRIVE_DEADZONE, 511, 0, 255);
        ledcWrite(DRIVE_SPEED_CHANNEL, speed);
    } else {
        // Stop Driving
        digitalWrite(DRIVE_FORWARD_PIN, LOW);
        digitalWrite(DRIVE_BACKWARD_PIN, LOW);
        ledcWrite(DRIVE_SPEED_CHANNEL, 0);
    }
}
