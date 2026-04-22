# Kenbot — Battle Bot Guide

## What is this?

Kenbot is a battle bot controlled by a PS4 controller over Bluetooth.
It uses hoverboard wheels for movement and a separate brushed motor for a weapon.
The brain is an ESP32 WROOM-32 running this firmware.

---

## Hardware Overview

| Component | Description |
|---|---|
| ESP32 WROOM-32 | Main controller — runs the firmware |
| PS4 Controller | Wireless input via Bluetooth (Bluepad32) |
| Hoverboard Mainboard | Controls 2× BLDC wheel motors via UART |
| Weapon Motor PCB | Custom brushed motor controller (36V 500W) |
| Brushed Motor | 36V 500W 2700RPM weapon motor |

---

## ESP32 Pin Wiring

| GPIO | Label | Connect To | Description |
|---|---|---|---|
| GPIO 2 | Built-in LED | — | Status indicator (onboard, no wiring needed) |
| GPIO 12 | Direction | — | Reserved for future use |
| GPIO 15 | E-Brake | — | Reserved for future use |
| GPIO 17 | UART TX | Hoverboard RX | Sends drive commands to hoverboard |
| GPIO 22 | Hoverboard Power | Hoverboard power button circuit | Toggles mainboard ON/OFF |
| GPIO 25 | Weapon PWM | Weapon PCB — J1 pin | Controls weapon motor speed via PWM |
| GND | Ground | Hoverboard GND + Weapon PCB GND | Common ground for all |

> NOTE: GPIO 17 connects to the hoverboard's RX pin (3-pin UART connector).
> You only need TX and GND — no RX wire is needed back to the ESP32.

---

## Weapon PCB Wiring (Custom Brushed Motor Controller)

The weapon PCB has the following connectors:

| PCB Label | Connect To | Description |
|---|---|---|
| J1 | ESP32 GPIO 25 | PWM speed signal — duty cycle controls motor speed |
| J2 (42V +) | Battery positive | Main power input (36–42V) |
| J3 (MOTOR) | Brushed motor terminals | Motor output |
| GND | ESP32 GND | Signal ground |

---

## Hoverboard Wiring

| Hoverboard Pin | Connect To | Description |
|---|---|---|
| RX (3-pin UART connector) | ESP32 GPIO 17 | Receives drive commands |
| GND (3-pin UART connector) | ESP32 GND | Common ground |
| Power button circuit | ESP32 GPIO 22 | Toggle mainboard power |

> The hoverboard firmware must have CONTROL_SERIAL_USART3 or USART2 enabled
> depending on which connector you use (right = USART3, left = USART2).

---

## PS4 Controller Button Map

| Button | Action |
|---|---|
| R2 (right trigger) | Drive forward |
| L2 (left trigger) | Drive reverse |
| Left stick X | Steer left / right |
| R1 | Gear up (1 → 2 → 3) |
| L1 | Gear down (3 → 2 → 1) |
| O (Circle) | Toggle hoverboard mainboard power ON/OFF |
| Square | Weapon HARD STOP — instantly cuts to 0% |
| D-pad UP | Weapon speed +1 stage |
| D-pad DOWN | Weapon speed -1 stage (at 0% wraps to 100%) |

---

## Gear System

The bot starts in **gear 1** on boot so it does not lunge when first connected.

| Gear | Max Speed |
|---|---|
| 1 | 33% |
| 2 | 66% |
| 3 | 100% |

Gear affects both forward/reverse speed and steering.
Use R1 to go up, L1 to go down.

---

## Weapon Speed Stages

The weapon has 5 stages controlled by D-pad UP/DOWN.
When spinning up, the speed ramps gradually (~1 second from 0 to 100%) to
protect the motor and PCB from current spikes.
When slowing down or hard stopping, power cuts instantly.

| Stage | Speed | PWM Duty |
|---|---|---|
| 0 | 0% (off) | 0 |
| 1 | 25% | 64/255 |
| 2 | 50% | 128/255 |
| 3 | 75% | 192/255 |
| 4 | 100% | 255/255 |

**Square** = instant kill regardless of current stage.

---

## PS4 Lightbar Colors

| Color | Meaning |
|---|---|
| Dim blue | Connected, standby (nothing active) |
| Green | Hoverboard powered, ready to drive |
| Red | Weapon armed (any stage above 0%) |

---

## ESP32 Built-in LED Patterns

| Pattern | Meaning |
|---|---|
| Fast blink (100ms) | Searching for PS4 controller |
| Slow blink (800ms) | Controller connected, standby |
| Solid ON | Hoverboard powered and ready |
| Double-blink | Weapon armed — most important to see in a fight |

---

## Safety Features

- **Controller disconnect** — if the PS4 controller disconnects for any reason,
  the drive immediately sends speed 0 and the weapon cuts to 0% instantly.
- **Weapon hard stop** — Square button kills weapon at any time.
- **Gear 1 on boot** — prevents the bot from lunging when first powered.
- **Weapon ramp** — gradual spin-up prevents current spikes that could blow the PCB.

---

## How to Flash the Firmware

### Requirements
- VS Code with PlatformIO IDE extension installed
- ESP32 connected via USB

### Steps
1. Open VS Code → File → Open Folder → select the Kenbot folder
2. Wait for PlatformIO to load the project
3. Click the **→ Upload** arrow at the bottom toolbar
4. Wait for `Hard resetting via RTS pin...` — done

### Serial Monitor
Click the plug icon in the PlatformIO toolbar, baud rate **115200**.
On boot you will see:
```
Kenbot booting...
Ready.
```
When PS4 connects:
```
Controller connected: slot 0
```

---

## Software Dependencies

All installed automatically by PlatformIO on first build — no manual downloads.

| Library | Purpose |
|---|---|
| Bluepad32 | PS4 Bluetooth controller support |
| Arduino ESP32 framework | GPIO, UART, PWM, timers |
| espressif32 toolchain | ESP32 compiler and build tools |

---

## Pairing the PS4 Controller

1. Flash and power the ESP32
2. Watch the LED — it will fast blink (searching)
3. On the PS4 controller, hold **PS + Share** until it flashes rapidly
4. The controller will pair automatically
5. LED changes to slow blink once paired
6. Pairing is remembered — next time just press the PS button

---

## Startup Sequence (in a fight)

1. Power on ESP32 — LED fast blinks
2. Connect PS4 controller — LED slow blinks
3. Press **O** to power on hoverboard — LED goes solid
4. Press **R1** to shift to desired gear
5. Use **R2/L2 + left stick** to drive
6. Press **D-pad UP** to spin up weapon gradually
7. Press **Square** at any time to kill weapon
8. Press **O** again to cut hoverboard power
