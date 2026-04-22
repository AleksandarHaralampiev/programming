/*
 * Kenbot — Battle Bot Controller
 *
 * Hardware:
 *   - ESP32 WROOM-32
 *   - PS4 controller via Bluepad32 (Bluetooth)
 *   - Hoverboard mainboard (2× BLDC wheel motors) via UART2 TX only (GPIO 17)
 *   - Custom brushed motor PCB (36V 500W weapon motor) via PWM + enable pin
 *
 * PS4 Button Map:
 *   L2              — drive reverse
 *   R2              — drive forward
 *   Left stick X    — steer left/right
 *   L1              — gear down (3→2→1)
 *   R1              — gear up  (1→2→3)
 *   O  (Circle)     — toggle hoverboard mainboard power (GPIO 22)
 *   Square          — weapon hard stop (force to 0%)
 *   D-pad UP        — weapon speed +1 stage (0→25→50→75→100%)
 *   D-pad DOWN      — weapon speed -1 stage, wraps 0%→100%
 */

#include <Arduino.h>
#include <Bluepad32.h>

// ─────────────────────────────────────────────
//  Pin mapping
// ─────────────────────────────────────────────
constexpr uint8_t kLEDPin             = 2;   // Built-in LED on ESP32 WROOM-32
constexpr uint8_t kEBrakePin          = 15;  // Emergency brake (future use)
constexpr uint8_t kDirectionPin       = 12;  // Direction pin (future use)
constexpr uint8_t kHoverboardPowerPin = 22;  // Hoverboard mainboard ON/OFF
constexpr uint8_t kWeaponPWMPin       = 25;  // PWM → J1 on weapon PCB

// ─────────────────────────────────────────────
//  Weapon PWM config
// ─────────────────────────────────────────────
constexpr uint32_t kWeaponPWMFreq  = 100;  // 100 Hz
constexpr uint8_t  kWeaponPWMRes   = 8;      // 8-bit → duty 0–255
constexpr uint8_t  kWeaponPWMChan  = 0;      // LEDC channel (0–15 available)

// Duty values for each of the 5 speed stages (0%, 25%, 50%, 75%, 100%)
constexpr uint8_t kWeaponDuty[5] = {0, 64, 128, 192, 255};

// ─────────────────────────────────────────────
//  Hoverboard UART config (TX only — GPIO 17)
// ─────────────────────────────────────────────
constexpr uint32_t HOVER_SERIAL_BAUD = 115200;
constexpr uint32_t SERIAL_BAUD       = 115200;
constexpr uint16_t START_FRAME       = 0xABCD;  // Magic bytes that mark start of packet
constexpr uint32_t TIME_SEND_MS      = 100;      // Send new command every 100 ms

// ─────────────────────────────────────────────
//  Deadzone — sticks within ±5% of centre = 0
// ─────────────────────────────────────────────
constexpr int DEAD_ZONE = 50;  // out of mapped range ±1000

static inline int applyDeadzone(int val) {
    return (abs(val) < DEAD_ZONE) ? 0 : val;
}

// ─────────────────────────────────────────────
//  Hoverboard serial command packet
//  Must match hoverboard firmware exactly (byte-for-byte)
// ─────────────────────────────────────────────
struct SerialCommand {
    uint16_t start;     // Always START_FRAME
    int16_t  steer;     // Steer value  −1000 … +1000
    int16_t  speed;     // Speed value  −1000 … +1000
    uint16_t checksum;  // XOR of start ^ steer ^ speed
};

// ─────────────────────────────────────────────
//  Global instances
// ─────────────────────────────────────────────
HardwareSerial HoverSerial(2);                   // UART2: TX only to hoverboard
ControllerPtr  myControllers[BP32_MAX_GAMEPADS]; // Up to 4 gamepads

SerialCommand Command;

// Drive state
unsigned long iTimeSend = 0;
int iSpeed = 0;
int iSteer = 0;
int gear   = 1;  // 1–3: scales max speed to 33% / 66% / 100%

// Toggle states
bool hoverboardOn = false;  // Hoverboard mainboard power state
int  weaponStage  = 0;      // 0=off, 1=25%, 2=50%, 3=75%, 4=100%

// Weapon ramp — target is set instantly, current duty steps toward it each tick
int weaponTargetDuty  = 0;
int weaponCurrentDuty = 0;
constexpr int kWeaponRampStep = 4;  // duty units per 15ms tick (~960ms full 0→100% ramp)

// ─────────────────────────────────────────────
//  LED status indicator (non-blocking)
//
//  Fast blink  (100ms)       — searching for controller
//  Slow blink  (800ms)       — connected, standby
//  Solid ON                  — hoverboard powered, ready to drive
//  Double-blink every second — weapon armed (stage > 0)
// ─────────────────────────────────────────────
void updateLED() {
    static unsigned long lastToggle = 0;
    static int blinkStep = 0;
    const unsigned long now = millis();

    bool anyConnected = false;
    for (auto ctl : myControllers) {
        if (ctl && ctl->isConnected()) { anyConnected = true; break; }
    }

    if (weaponStage > 0) {
        // Double-blink: ON-OFF-ON-PAUSE (100ms each, 600ms pause)
        const unsigned long doubleBlink[] = {100, 100, 100, 600};
        if (now - lastToggle >= doubleBlink[blinkStep % 4]) {
            lastToggle = now;
            blinkStep++;
            digitalWrite(kLEDPin, (blinkStep % 4 < 2) ? HIGH : LOW);
        }
    } else if (!anyConnected) {
        // Fast blink — searching
        if (now - lastToggle >= 100) {
            lastToggle = now;
            digitalWrite(kLEDPin, !digitalRead(kLEDPin));
        }
    } else if (hoverboardOn) {
        // Solid ON — hoverboard live
        digitalWrite(kLEDPin, HIGH);
    } else {
        // Slow blink — connected but idle
        if (now - lastToggle >= 800) {
            lastToggle = now;
            digitalWrite(kLEDPin, !digitalRead(kLEDPin));
        }
    }
}

// ─────────────────────────────────────────────
//  PS4 lightbar
//  Red   — weapon armed
//  Green — hoverboard on, weapon off
//  Blue  — connected standby
// ─────────────────────────────────────────────
void updateLightbar(ControllerPtr ctl) {
    if (!ctl) return;
    if (weaponStage > 0) {
        ctl->setColorLED(255, 0, 0);    // red — weapon armed
    } else if (hoverboardOn) {
        ctl->setColorLED(0, 255, 0);    // green — hoverboard live
    } else {
        ctl->setColorLED(0, 0, 64);     // dim blue — standby
    }
}

// ─────────────────────────────────────────────
//  Weapon ramp (non-blocking)
//  Spin-up ramps gradually to protect motor and PCB.
//  Spin-down cuts instantly for safety.
// ─────────────────────────────────────────────
void updateWeaponRamp() {
    if (weaponCurrentDuty == weaponTargetDuty) return;

    if (weaponCurrentDuty < weaponTargetDuty) {
        weaponCurrentDuty = min(weaponTargetDuty, weaponCurrentDuty + kWeaponRampStep);
    } else {
        weaponCurrentDuty = weaponTargetDuty;  // drop instantly
    }
    ledcWrite(kWeaponPWMChan, weaponCurrentDuty);
}

// ─────────────────────────────────────────────
//  Forward declarations
// ─────────────────────────────────────────────
void Send(int16_t uSteer, int16_t uSpeed);
void processControllers();
void processGamepad(ControllerPtr ctl);
void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);

// ─────────────────────────────────────────────
//  Setup
// ─────────────────────────────────────────────
void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.println("Kenbot booting...");

    // UART2 TX only to hoverboard (RX=-1 unused, TX=17)
    HoverSerial.begin(HOVER_SERIAL_BAUD, SERIAL_8N1, -1, 17);

    // Bluepad32 — register callbacks then start BT scan
    BP32.setup(&onConnectedController, &onDisconnectedController);
    delay(1000);  // Give Bluepad32 time to stabilize
    // BP32.forgetBluetoothKeys();  // DISABLED: causes re-pairing loop on every boot

    pinMode(kLEDPin, OUTPUT);
    digitalWrite(kLEDPin, LOW);

    // Hoverboard pins (reserved for future use)
    pinMode(kEBrakePin,    OUTPUT);
    pinMode(kDirectionPin, OUTPUT);

    // Hoverboard power toggle
    pinMode(kHoverboardPowerPin, OUTPUT);
    digitalWrite(kHoverboardPowerPin, LOW);

    // Weapon motor controller — PWM on GPIO 25 → J1
    ledcSetup(kWeaponPWMChan, kWeaponPWMFreq, kWeaponPWMRes);
    ledcAttachPin(kWeaponPWMPin, kWeaponPWMChan);
    ledcWrite(kWeaponPWMChan, 0);

    Serial.println("Ready.");
}

// ─────────────────────────────────────────────
//  Main loop
// ─────────────────────────────────────────────
void loop() {
    BP32.update();
    processControllers();
    updateWeaponRamp();
    updateLED();
    delay(15);
}

// ─────────────────────────────────────────────
//  Bluepad32 callbacks
// ─────────────────────────────────────────────
void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("Controller connected: slot %d\n", i);
            myControllers[i] = ctl;
            return;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("Controller disconnected: slot %d\n", i);
            myControllers[i] = nullptr;

            // Safety: stop drive and weapon immediately on disconnect
            Send(0, 0);
            weaponStage       = 0;
            weaponTargetDuty  = 0;
            weaponCurrentDuty = 0;
            ledcWrite(kWeaponPWMChan, 0);
            return;
        }
    }
}

// ─────────────────────────────────────────────
//  Controller processing
// ─────────────────────────────────────────────
void processControllers() {
    for (auto ctl : myControllers) {
        if (ctl && ctl->isConnected() && ctl->hasData() && ctl->isGamepad()) {
            processGamepad(ctl);
        }
    }
}

void processGamepad(ControllerPtr ctl) {
    // ── Drive ──────────────────────────────────────────────────────
    // R2 = forward (0–1023), L2 = reverse (0–1023)
    iSpeed = ctl->throttle() - ctl->brake();                         // −1023 … +1023
    iSpeed = applyDeadzone(map(iSpeed, -1023, 1023, -1000, 1000));

    // Left stick X: −512 … +511
    iSteer = applyDeadzone(map(ctl->axisX(), -512, 511, -1000, 1000));

    // Apply gear: gear 1=33%, gear 2=66%, gear 3=100% max speed
    iSpeed = (iSpeed * gear) / 3;
    iSteer = (iSteer * gear) / 3;

    const unsigned long timeNow = millis();
    if (iTimeSend <= timeNow) {
        iTimeSend = timeNow + TIME_SEND_MS;
        Send(iSteer, iSpeed);
    }

    // ── Gear shifting — R1 / L1 ────────────────────────────────────
    if (ctl->buttons() == 0x0020 && gear < 3) {         // R1 — gear up
        gear++;
        Serial.printf("Gear: %d\n", gear);
        delay(200);
    } else if (ctl->buttons() == 0x0010 && gear > 1) {  // L1 — gear down
        gear--;
        Serial.printf("Gear: %d\n", gear);
        delay(200);

    // ── O (Circle) — toggle hoverboard power ───────────────────────
    } else if (ctl->buttons() == 0x0002) {
        hoverboardOn = !hoverboardOn;
        digitalWrite(kHoverboardPowerPin, hoverboardOn ? HIGH : LOW);
        Serial.printf("Hoverboard: %s\n", hoverboardOn ? "ON" : "OFF");
        delay(200);

    // ── Square — weapon hard stop ───────────────────────────────────
    } else if (ctl->buttons() == 0x0004) {
        weaponStage = 0;
        weaponTargetDuty  = 0;
        weaponCurrentDuty = 0;  // also zero current so ramp doesn't continue
        ledcWrite(kWeaponPWMChan, 0);
        Serial.println("Weapon: HARD STOP");
        delay(200);
    }

    // ── D-pad — weapon speed stages ────────────────────────────────
    // UP ramps to next stage, DOWN cuts instantly to lower stage (wraps 0%→100%)
    const uint8_t dpad = ctl->dpad();
    if (dpad == 0x01) {  // D-pad UP — ramp up to new target
        weaponStage = min(4, weaponStage + 1);
        weaponTargetDuty = kWeaponDuty[weaponStage];

        Serial.printf("Weapon: %d%%\n", weaponStage * 25);
        delay(200);
    } else if (dpad == 0x02) {  // D-pad DOWN — instant cut to lower stage
        weaponStage = (weaponStage == 0) ? 4 : weaponStage - 1;
        weaponTargetDuty = kWeaponDuty[weaponStage];

        Serial.printf("Weapon: %d%%\n", weaponStage * 25);
        delay(200);
    }

    updateLightbar(ctl);
}

// ─────────────────────────────────────────────
//  Hoverboard UART TX
//  Packs steer + speed into framed protocol and sends over UART2.
// ─────────────────────────────────────────────
void Send(int16_t uSteer, int16_t uSpeed) {
    Command.start    = START_FRAME;
    Command.steer    = uSteer;
    Command.speed    = uSpeed;
    Command.checksum = (uint16_t)(Command.start ^ Command.steer ^ Command.speed);

    HoverSerial.write((uint8_t*)&Command, sizeof(Command));
}
