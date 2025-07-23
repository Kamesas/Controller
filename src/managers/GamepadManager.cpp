#include "GamepadManager.h"
#include "config.h" // Include the shared configuration
#include <Arduino.h>

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
}

void processGamepad(ControllerPtr ctl) {
    static bool ledState = false;
    static bool prevButtonState = false;

    bool currentButtonState = ctl->buttons() & 0x0008;

    if (currentButtonState && !prevButtonState) {
        ledState = !ledState;
        digitalWrite(LIGHTS_PIN, ledState ? HIGH : LOW);
        Serial.printf("Button toggled - LED %s\n", ledState ? "ON" : "OFF");
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

    dumpGamepad(ctl);
}