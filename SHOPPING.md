# Shopping list — make the car run on one battery

Goal: cut the bench supplies. **One battery pack powers the whole car** — the
motor directly, and a step-down converter makes 5 V for the ESP32 and servo. All
on a common ground. Then the car is truly wireless.

The TX12 18650 cells were only borrowed for testing, so the car needs its own
pack. That means we pick the best battery for a car, not a reused one.

## Power plan

```
2S battery (7.4 V) ──┬── L298N +12V  → drive motor
                     └── buck (5 V)  → ESP32 (5V/VIN) + servo
                    (all grounds joined)
```

## Must buy

| Item | Pick | Why |
| --- | --- | --- |
| **Battery** | **2S LiPo, 7.4 V, ~1500–2200 mAh**, with an **XT30** or **XT60** connector | Right voltage for the motor + buck. Light, compact, high current, made for RC. |
| **Charger** | **2S LiPo balance charger** (cheap USB 2S charger, or an IMAX B6 clone) | LiPo must be balance-charged. Skip only if you already own one. |
| **Step-down (5 V rail)** | **Buck converter** (LM2596 or MP1584 module), **or a UBEC** (RC 5 V/6 V) | Turns 7.4 V into clean 5 V for the ESP32 + servo. Set to 5 V *before* wiring. 2–3 A rating covers servo spikes. UBEC handles spikes best. |
| **Low-voltage alarm** | **2S LiPo alarm buzzer** (a few coins) | Beeps before the LiPo drops too low. LiPo is damaged/unsafe if over-drained (below ~3.0 V/cell). |
| **Connector / power switch** | Matching **XT30/XT60 pigtail** + a small **on/off switch** | Easy battery swaps and a way to turn the car off. |

## Probably already have (from the parts box)

- **Capacitor 470–1000 µF** — put on the buck's 5 V output near the servo to smooth
  its current spikes (the resets we saw earlier).
- Jumper wires, the L298N, the SG90 servos.

## Optional / later

- **TB6612FNG motor driver** — much smaller than the L298N and more efficient, for a
  tidier build. Near drop-in with the current code (see [HARDWARE.md](HARDWARE.md)).

## To drive with the TX12 (ELRS) instead of the gamepad

The TX12 is an ExpressLRS radio. It sends to a **receiver on the car**, and the
receiver feeds the controls in. Two ways, depending on whether you keep the ESP32.

### Path A — keep the ESP32 (smart car: keeps lights + logic)

| Item | Pick | Why |
| --- | --- | --- |
| **ELRS 2.4 GHz receiver** | RadioMaster RP1/RP2, Happymodel EP1/EP2, BetaFPV ELRS Lite (~$8–15) | Binds to the TX12, outputs **CRSF** (one wire) to the ESP32. |

- Powers from the **5 V rail** (~0.1 A — negligible), common ground.
- Wire its **TX / CRSF out → ESP32 GPIO 16**. Needs new firmware to decode CRSF.

### Path B — no ESP32 (plain RC car, no lights/logic)

| Item | Pick | Why |
| --- | --- | --- |
| **ELRS receiver with PWM outputs** | RadioMaster RP4TD or another PWM-capable ELRS RX (~$12–20) | Servo plugs **straight in** — no code for steering. |
| **Brushed ESC** (bidirectional, 2S) | matched to the motor's current | Replaces the L298N; reads the throttle channel and drives the DC motor. |

- The ESC's **built-in BEC** (5 V) can power the receiver + servo — may remove the
  need for the buck converter.
- You lose the headlights, turn signals, and any custom logic.

**Which:** keep the ESP32 for a programmable car; go Path B for a simple, rock-solid
RC car.

## FPV / camera — parked for now

Decided to skip FPV for now (too expensive for what it adds to a car).

- If ever revisited: **ESP32-CAM** streaming to a phone over WiFi is the only FPV
  worth adding here — cheap (~$7) and a nice ESP32 sub-project. Short range though.
- **Analog 5.8 GHz FPV is not worth it for this car** — the goggles/monitor cost
  too much. For real FPV, buy a dedicated tiny FPV drone (e.g. BetaFPV Air65)
  instead of kitting out the car.

## If you prefer less fuss over max performance

- A **2S Li-ion (18650) pack with a protection board** works too — more forgiving
  than LiPo, but heavier and lower current. Needs an 18650 charger. LiPo is the
  better car battery; Li-ion is the safer/simpler one.

## Safety notes

- Set the buck to **5 V before** connecting the ESP32 (don't feed it 7–8 V).
- Never join any **+** wires; only grounds are shared.
- Don't over-drain or short a LiPo; store it around half charge.
