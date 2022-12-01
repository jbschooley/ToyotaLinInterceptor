#include <Arduino.h>
#include <HardwareSerial.h>
#include <arduino-timer.h>
#include "LINController.h"
#include "PanelHandler.h"

#define SerialUSB Serial
#define SerialCar Serial1
#define SerialPanel Serial2

#define PinEnPanel 53
#define PinEnCar 46

unsigned long baud = 19200;

Timer<10> timer;
//LINController LINPanel(&SerialPanel, 19200);
PanelHandler panelHandler(&SerialPanel);

//void writeToPanel() {
//    byte dataB1status[8] = {0x00, 0x06, 0x14, 0x00, 0x34, 0x37, 0x00, 0xc1};
//    LINPanel.send(0xb1, dataB1status, 8);
//    byte data32[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x10};
//    LINPanel.send(0x32, data32, 8);
//    LINPanel.request(0x39);
//    LINPanel.request(0xba);
//    byte dataF5[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00};
//    LINPanel.send(0xf5, dataF5, 8);
//    LINPanel.request(0x76);
//    LINPanel.request(0x78);
//}

void setup() {
    // enable lin chips
    pinMode(PinEnPanel, OUTPUT);
    digitalWrite(PinEnPanel, HIGH);
    pinMode(PinEnCar, OUTPUT);
    digitalWrite(PinEnCar, HIGH);

    // open debug serial connection
    SerialUSB.begin(115200);

//    panelHandler.sendNext();
//    timer.every(10, panelHandler.sendNext)
}

bool testPanelChanged = false;
void testChangePanelAfter3s() {
    if (!testPanelChanged && millis() > 3000) {
        testPanelChanged = true;
        panelHandler.dataB1status[1] = 0x02;
    }
}

void loop() {
//    writeToPanel();
//    timer.tick();
    panelHandler.sendEvery10ms();
    testChangePanelAfter3s();
}
