#include <Arduino.h>
#include <Bluepad32.h>
#include "ControllerManager.h"
#include "config.h"

// Define the pin for the headlights. This is the single source of truth.
const int HEAD_LIGHTS_PIN = 4;
const int BREAK_LIGHTS_PIN = 5;
const int LEFT_TURN_PIN = 18;
const int RIGHT_TURN_PIN = 19;

void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    pinMode(HEAD_LIGHTS_PIN, OUTPUT);
    pinMode(BREAK_LIGHTS_PIN, OUTPUT);
    pinMode(LEFT_TURN_PIN, OUTPUT);
    pinMode(RIGHT_TURN_PIN, OUTPUT);

    digitalWrite(HEAD_LIGHTS_PIN, LOW); // Start with the light off
    digitalWrite(BREAK_LIGHTS_PIN, LOW); // Start with the light off
    digitalWrite(LEFT_TURN_PIN, LOW); // Start with the light off
    digitalWrite(RIGHT_TURN_PIN, LOW); // Start with the light off

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // Forget Bluetooth keys for easier testing
    BP32.forgetBluetoothKeys();

    // Disable virtual device by default
    BP32.enableVirtualDevice(false);
}

void loop() {
    // Fetch all the controllers' data
    if (BP32.update()) {
        processControllers();
    }
    
    // A small delay to prevent watchdog timeout
    delay(150);
}