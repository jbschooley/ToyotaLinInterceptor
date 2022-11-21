#include <Arduino.h>
#include <HardwareSerial.h>
#include <arduino-timer.h>
#include "LINController.h"
#define SerialUSB Serial
#define SerialPanel Serial1
#define SerialCar Serial2

unsigned long baud = 19200;

Timer<> default_timer;
LINController LINPanel(&SerialPanel, 19200);

void processRead() {

}

void writeToPanel() {
    byte dataB1status[8] = {0x00, 0x06, 0x14, 0x00, 0x34, 0x37, 0x00, 0xc1};
    LINPanel.send(0xb1, dataB1status, 8);
    byte data32[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x10};
    LINPanel.send(0x32, data32, 8);
    LINPanel.request(0x39);
    LINPanel.request(0xba);
    byte dataF5[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00};
    LINPanel.send(0xf5, dataF5, 8);
    LINPanel.request(0x76);
    LINPanel.request(0x78);
}

void setup() {
    SerialUSB.begin(115200);
    SerialPanel.begin(19200);
}

void loop() {
    writeToPanel();
}
