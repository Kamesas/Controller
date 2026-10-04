#include <Arduino.h>
#include <Bluepad32.h>
#include <ESP32Servo.h>
#include "managers/ControllerManager.h"
#include "managers/config.h" // Include all our pin definitions and settings
#include "managers/GamepadManager.h"

// Steering servo. GamepadManager.cpp writes angles to it.
Servo steerServo;

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
    
    // Drive motor direction pins
    pinMode(DRIVE_FORWARD_PIN, OUTPUT);
    pinMode(DRIVE_BACKWARD_PIN, OUTPUT);

    // --- Steering Servo Setup ---
    // Reserve LEDC timers 0 and 1 for the servo library, then attach the servo.
    // 50 Hz is the standard servo update rate. 500-2400 us is the pulse range;
    // widen or narrow it if your servo does not reach full travel.
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    steerServo.setPeriodHertz(50);
    steerServo.attach(STEER_SERVO_PIN, 500, 2400);
    steerServo.write(STEER_CENTER); // Start with wheels straight

    // --- PWM (LEDC) Setup for the drive motor speed ---
    ledcSetup(DRIVE_SPEED_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(DRIVE_SPEED_PIN, DRIVE_SPEED_CHANNEL);


    // Setup hardware timer for precise 100ms intervals
    blinkTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1MHz), count up
    timerAttachInterrupt(blinkTimer, &onBlinkTimer, true);
    timerAlarmWrite(blinkTimer, 800000, true); // 800ms
    timerAlarmEnable(blinkTimer);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);
    // Keep paired controllers across reboots so the gamepad reconnects on its
    // own. Uncomment to wipe all pairings (needed only to bond a different pad).
    // BP32.forgetBluetoothKeys();
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

