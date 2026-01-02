#pragma once // Prevents the file from being included multiple times

// --- Light Pins ---
const int HEAD_LIGHTS_PIN = 4;
const int LEFT_TURN_PIN = 18;
const int RIGHT_TURN_PIN = 19;

// --- Motor Pins ---
// Turning/Steering Motor
const int TERN_SPEED_PIN = 26; // Must be a PWM-capable pin
const int TERN_LEFT_PIN = 25;
const int TERN_RIGHT_PIN = 33;

// Drive Motor
const int DRIVE_SPEED_PIN = 14; // Must be a PWM-capable pin
const int DRIVE_FORWARD_PIN = 13; 
const int DRIVE_BACKWARD_PIN = 12;

// --- PWM Configuration for Motor Speed ---
const int PWM_FREQ = 5000;      // 5 kHz
const int PWM_RESOLUTION = 8;   // 8-bit (0-255)
const int TERN_SPEED_CHANNEL = 0;
const int DRIVE_SPEED_CHANNEL = 1;
