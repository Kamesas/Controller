#pragma once // Prevents the file from being included multiple times

// --- Light Pins ---
const int HEAD_LIGHTS_PIN = 4;
const int LEFT_TURN_PIN = 18;
const int RIGHT_TURN_PIN = 19;

// --- Steering Servo ---
// One signal wire, any output pin. The servo holds an angle, so it steers
// smoothly instead of the old DC motor that only spun left/right.
const int STEER_SERVO_PIN = 25;

// Wheel angles in degrees. Center is straight ahead. LEFT and RIGHT are the
// end angles. Keep them just inside where the wheels hit their mechanical stop,
// so the servo never forces against a locked linkage (that strains the servo).
// Move LEFT and RIGHT closer to 90 to turn less, further from 90 to turn more.
// If the wheels turn the wrong way, swap the LEFT and RIGHT values.
const int STEER_CENTER = 90;
const int STEER_LEFT = 75;
const int STEER_RIGHT = 102;

// Steering feel. 0.0 = linear (twitchy, 1-to-1). Higher = gentler near center,
// so you use more of the stick for small turns. Full stick still reaches lock.
// Try 0.4-0.7. Raise it if steering is still too sensitive.
const float STEER_EXPO = 0.6;

// --- Drive Motor ---
const int DRIVE_SPEED_PIN = 14; // Must be a PWM-capable pin
const int DRIVE_FORWARD_PIN = 13;
const int DRIVE_BACKWARD_PIN = 12;

// --- PWM Configuration for Drive Motor Speed ---
const int PWM_FREQ = 5000;      // 5 kHz
const int PWM_RESOLUTION = 8;   // 8-bit (0-255)
// Channel 4 uses LEDC timer 2. The servo library takes timers 0 and 1,
// so this keeps the drive motor's PWM off the servo's timers.
const int DRIVE_SPEED_CHANNEL = 4;
