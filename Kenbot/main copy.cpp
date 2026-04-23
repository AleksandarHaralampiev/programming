#include <Arduino.h>
#include <Bluepad32.h>
#define DEBUG_RX

// ================= Pin mapping =================
constexpr uint8_t kEBrakePin = 15;
constexpr uint8_t kDirectionPin = 12;
constexpr uint8_t kWeaponPin = 22;

// ================= Serial/config =================
constexpr uint32_t HOVER_SERIAL_BAUD = 115200;
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint16_t START_FRAME = 0xABCD;
constexpr uint32_t TIME_SEND_MS = 100;

HardwareSerial HoverSerial(2);  // UART2
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// ================= Protocol types =================
struct SerialCommand {
    uint16_t start;
    int16_t steer;
    int16_t speed;
    uint16_t checksum;
};

struct SerialFeedback {
    uint16_t start;
    int16_t cmd1;
    int16_t cmd2;
    int16_t speedR_meas;
    int16_t speedL_meas;
    int16_t batVoltage;
    int16_t boardTemp;
    uint16_t cmdLed;
    uint16_t checksum;
};

// ================= Runtime state =================
SerialCommand Command;
SerialFeedback Feedback;
SerialFeedback NewFeedback;

uint8_t idx = 0;
uint16_t bufStartFrame = 0;
byte* p = nullptr;
byte incomingByte = 0;
byte incomingBytePrev = 0;

unsigned long iTimeSend = 0;
int iSpeed = 0;
int iSteer = 0;
int gear = 1;

// ================= Forward declarations =================
void Send(int16_t uSteer, int16_t uSpeed);
void Receive();
void processControllers();
void processGamepad(ControllerPtr ctl);
void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);

// ================= Arduino lifecycle =================
void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.println("Hoverboard Serial v1.0");

    HoverSerial.begin(HOVER_SERIAL_BAUD, SERIAL_8N1, 16, 17);
    Serial.println("Ready");

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);

    pinMode(kEBrakePin, OUTPUT);
    pinMode(kDirectionPin, OUTPUT);
    pinMode(kWeaponPin, OUTPUT);
}

void loop() {
    if (BP32.update()) {
        processControllers();
    }
    delay(10);
}

// ================= Controller callbacks =================
void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("Controller connected: %d\n", i);
            myControllers[i] = ctl;
            return;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("Controller disconnected: %d\n", i);
            myControllers[i] = nullptr;
            return;
        }
    }
}

// ================= Controller processing =================
void processControllers() {
    for (auto ctl : myControllers) {
        if (ctl && ctl->isConnected() && ctl->hasData() && ctl->isGamepad()) {
            processGamepad(ctl);
        }
    }
}

void processGamepad(ControllerPtr ctl) {
    Receive();

    iSpeed = ctl->throttle() - ctl->brake();
    iSpeed = map(iSpeed, -255, 255, -1000, 1000);
    iSteer = map(ctl->axisX(), -128, 127, -1000, 1000);
    //Serial.println(iSpeed);

    const unsigned long timeNow = millis();
    if (iTimeSend <= timeNow) {
        iTimeSend = timeNow + TIME_SEND_MS;
        Send(iSteer, iSpeed);
    }

    if (ctl->buttons() == 0x0020 && gear < 3) {
        gear++;
        delay(200);
    } else if (ctl->buttons() == 0x0010 && gear > 1) {
        gear--;
        delay(200);
    }


    //  Serial.printf(
    //     "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    //     "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
    //     ctl->index(),        // Controller Index
    //     ctl->dpad(),         // D-pad
    //     ctl->buttons(),      // bitmask of pressed buttons
    //     ctl->axisX(),        // (-511 - 512) left X Axis
    //     ctl->axisY(),        // (-511 - 512) left Y axis
    //     ctl->axisRX(),       // (-511 - 512) right X axis
    //     ctl->axisRY(),       // (-511 - 512) right Y axis
    //     ctl->brake(),        // (0 - 1023): brake button
    //     ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    //     ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    //     ctl->gyroX(),        // Gyro X
    //     ctl->gyroY(),        // Gyro Y
    //     ctl->gyroZ(),        // Gyro Z
    //     ctl->accelX(),       // Accelerometer X
    //     ctl->accelY(),       // Accelerometer Y
    //     ctl->accelZ()        // Accelerometer Z
    // );

}

// ================= Hoverboard serial TX =================
void Send(int16_t uSteer, int16_t uSpeed) {
    Command.start = START_FRAME;
    Command.steer = uSteer;
    Command.speed = uSpeed;
    Command.checksum = (uint16_t)(Command.start ^ Command.steer ^ Command.speed);

    HoverSerial.write((uint8_t*)&Command, sizeof(Command));
}

// ================= Hoverboard serial RX =================
void Receive() {
    if (HoverSerial.available()) {
        incomingByte = HoverSerial.read();
        bufStartFrame = ((uint16_t)(incomingByte) << 8) | incomingBytePrev;
    } else {
        return;
    }

#ifdef DEBUG_RX
    Serial.print(incomingByte);
    return;
#endif

    if (bufStartFrame == START_FRAME) {
        p = (byte*)&NewFeedback;
        *p++ = incomingBytePrev;
        *p++ = incomingByte;
        idx = 2;
    } else if (idx >= 2 && idx < sizeof(SerialFeedback)) {
        *p++ = incomingByte;
        idx++;
    }

    if (idx == sizeof(SerialFeedback)) {
        const uint16_t checksum = (uint16_t)(
                NewFeedback.start ^ NewFeedback.cmd1 ^ NewFeedback.cmd2 ^
                NewFeedback.speedR_meas ^ NewFeedback.speedL_meas ^ NewFeedback.batVoltage ^
                NewFeedback.boardTemp ^ NewFeedback.cmdLed);

        if (NewFeedback.start == START_FRAME && checksum == NewFeedback.checksum) {
            memcpy(&Feedback, &NewFeedback, sizeof(SerialFeedback));

            Serial.print("1: ");
            Serial.print(Feedback.cmd1);
            Serial.print(" 2: ");
            Serial.print(Feedback.cmd2);
            Serial.print(" 3: ");
            Serial.print(Feedback.speedR_meas);
            Serial.print(" 4: ");
            Serial.print(Feedback.speedL_meas);
            Serial.print(" 5: ");
            Serial.print(Feedback.batVoltage);
            Serial.print(" 6: ");
            Serial.print(Feedback.boardTemp);
            Serial.print(" 7: ");
            Serial.println(Feedback.cmdLed);
        } else {
            Serial.println("Non-valid data skipped");
        }
        idx = 0;
    }

    incomingBytePrev = incomingByte;
}