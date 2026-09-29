# Hardware / wiring

RC car controlled by a Bluetooth gamepad.

- **Brain:** ESP32 (needs Bluetooth — the ESP8266 has none, so it can't run this).
- **Control:** Marvo GT-60 gamepad over Bluetooth (Bluepad32). Connect with HOME + X.
- **Steering:** SG90 servo — one signal wire, holds an angle.
- **Drive:** one DC motor through an H-bridge driver.

Steering is a servo now, so **only one DC motor** is left (the drive motor). Any
single-channel DC driver is enough.

## Pin map

From `src/managers/config.h`:

| Function | GPIO | Const |
| --- | --- | --- |
| Headlights | 4 | `HEAD_LIGHTS_PIN` |
| Left turn signal | 18 | `LEFT_TURN_PIN` |
| Right turn signal | 19 | `RIGHT_TURN_PIN` |
| Steering servo signal | 25 | `STEER_SERVO_PIN` |
| Drive speed (PWM) | 14 | `DRIVE_SPEED_PIN` |
| Drive forward | 13 | `DRIVE_FORWARD_PIN` |
| Drive backward | 12 | `DRIVE_BACKWARD_PIN` |

## Steering servo (SG90)

- Yellow (signal) → GPIO 25
- Red (power) → clean 5 V (its own supply, not the ESP32 5 V pin under load)
- Brown (ground) → common ground

## Drive motor driver

**Rule for every driver below:** the driver ground, the motor battery minus, and
the ESP32 GND must all be one **common ground**. Motor power stays separate from
the ESP32 — only grounds are shared.

### L298N — current, works, but big

Uses the Motor A side only.

| L298N | To | Note |
| --- | --- | --- |
| ENA | GPIO 14 | **remove the ENA jumper first**, or there's no speed control |
| IN1 | GPIO 13 | |
| IN2 | GPIO 12 | |
| OUT1 / OUT2 | drive motor | swap these two if forward/back is reversed |
| +12V | motor battery + (7–12 V) | |
| GND | common ground | |

Downsides: large board, wastes ~2 V as heat (hence the heatsink). Fine for testing.

### Smaller replacements (single DC motor)

All three are much smaller than the L298N. None are owned yet — these are buy
options to shrink the build.

| Driver | Size | Code change | Notes |
| --- | --- | --- | --- |
| **TB6612FNG** | small | almost none — just tie **STBY HIGH** | Best match. IN1/IN2 + PWM, same idea as the L298N. Efficient, no big heatsink. |
| **DRV8833** | tiny | yes — PWM the two inputs | No separate enable pin; drive by PWM-ing AIN1/AIN2. Tie nSLEEP HIGH. |
| **L9110S** | tiny / cheap | yes — PWM the two inputs | Same style as DRV8833 (inputs IA/IB). Cheapest, very compact. |

**TB6612FNG wiring** (near drop-in for the current code):

| TB6612 | To |
| --- | --- |
| PWMA | GPIO 14 |
| AIN1 | GPIO 13 |
| AIN2 | GPIO 12 |
| STBY | 3.3 V (or a GPIO held HIGH) — driver is off if this is LOW |
| VM | motor battery + |
| VCC | 3.3 V or 5 V (logic power) |
| GND | common ground |
| AO1 / AO2 | drive motor |

**DRV8833 / L9110S:** these have **no enable pin** — you set speed and direction by
PWM-ing the two inputs (one input PWM, the other LOW = one direction; swap for the
other). The current drive code uses "direction pins + one PWM enable," so it needs a
small rewrite for these two. TB6612 avoids that.

## Notes

- Drive = left stick up/down. Steering = right stick left/right.
- Steering feel/limits: `STEER_CENTER`, `STEER_LEFT`, `STEER_RIGHT`, `STEER_EXPO`
  in `config.h`.
