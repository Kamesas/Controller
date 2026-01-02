#include <Arduino.h>
#include <Bluepad32.h>
#include "ControllerManager.h"
#include "managers/config.h" // Include all our pin definitions and settings
#include "managers/GamepadManager.h"

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

    // --- PinMode Setup ---
    // Lights
    pinMode(HEAD_LIGHTS_PIN, OUTPUT);
    pinMode(LEFT_TURN_PIN, OUTPUT);
    pinMode(RIGHT_TURN_PIN, OUTPUT);

    digitalWrite(HEAD_LIGHTS_PIN, LOW);
    digitalWrite(LEFT_TURN_PIN, LOW);
    digitalWrite(RIGHT_TURN_PIN, LOW);
    
    // Motors
    pinMode(TERN_LEFT_PIN, OUTPUT);
    pinMode(TERN_RIGHT_PIN, OUTPUT);
    pinMode(DRIVE_FORWARD_PIN, OUTPUT);
    pinMode(DRIVE_BACKWARD_PIN, OUTPUT);

    // --- PWM (LEDC) Setup for ESP32 ---
    ledcSetup(TERN_SPEED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(TERN_SPEED_PIN, TERN_SPEED_CHANNEL);

    ledcSetup(DRIVE_SPEED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(DRIVE_SPEED_PIN, DRIVE_SPEED_CHANNEL);


    // Setup hardware timer for precise 100ms intervals
    blinkTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1MHz), count up
    timerAttachInterrupt(blinkTimer, &onBlinkTimer, true);
    timerAlarmWrite(blinkTimer, 800000, true); // 800ms
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
    
    // A small delay is good practice
    delay(20);
}

