#include <Arduino.h>
#include <Bluepad32.h>
#include "ControllerManager.h"
#include "config.h"
#include "managers/GamepadManager.h"

// Define the pin for the headlights. This is the single source of truth.
const int HEAD_LIGHTS_PIN = 4;
const int LEFT_TURN_PIN = 18;
const int RIGHT_TURN_PIN = 19;

// Hardware timer for precise blinking
hw_timer_t * blinkTimer = NULL;
volatile bool timerTick = false;

// Timer interrupt function
void IRAM_ATTR onBlinkTimer() {
    timerTick = true;
}

void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    pinMode(HEAD_LIGHTS_PIN, OUTPUT);
    pinMode(LEFT_TURN_PIN, OUTPUT);
    pinMode(RIGHT_TURN_PIN, OUTPUT);

    digitalWrite(HEAD_LIGHTS_PIN, LOW);
    digitalWrite(LEFT_TURN_PIN, LOW);
    digitalWrite(RIGHT_TURN_PIN, LOW);

    // Setup hardware timer for precise 100ms intervals
    blinkTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1MHz), count up
    timerAttachInterrupt(blinkTimer, &onBlinkTimer, true);
    timerAlarmWrite(blinkTimer, 800000, true); // 800ms = 100,000 microseconds
    timerAlarmEnable(blinkTimer);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);
}

void loop() {
    // Fetch all the controllers' data
    if (BP32.update()) {
        processControllers();
    }
    
    // Handle blinking logic on timer tick
    if (timerTick) {
        timerTick = false;
        handleBlinking();
    }
    
    // Larger delay is fine now - blinking runs on hardware timer
    delay(50); // Or even delay(100) would work fine
}