#include <Arduino.h>
#include <HardwareSerial.h>
#include "LINUtils.h"
#include "Logger.h"
#include "DataStore.h"
#include "Modifier.h"
#include "CarHandlerSM.h"
#include "PanelHandlerSM.h"

#define SerialUSB Serial
#define SerialCar Serial1
#define SerialPanel Serial2

#define PinEnPanel 53
#define PinEnCar 46

unsigned long baud = 19200;

Logger* l;
DataStore* ds;
Modifier* mod;
CarHandlerSM* carHandlerSM;
PanelHandlerSM* panelHandlerSM;

void setup() {

    // open debug serial connection
    SerialUSB.begin(115200);

    // initialize logger, data store, handlers
    l = new Logger("main");
    ds = new DataStore();
    mod = new Modifier(ds);
    carHandlerSM = new CarHandlerSM(ds, mod, &SerialCar);
    panelHandlerSM = new PanelHandlerSM(ds, mod, &SerialPanel);

    // enable lin chips
    pinMode(PinEnPanel, OUTPUT);
    digitalWrite(PinEnPanel, HIGH);
    pinMode(PinEnCar, OUTPUT);
    digitalWrite(PinEnCar, HIGH);
}

bool testPanelChanged = false;
//void testChangePanelAfter3s() {
//    if (!testPanelChanged && millis() > 3000) {
//        testPanelChanged = true;
//        panelHandler.dataB1status[1] = 0x02;
//    }
//}

void loop() {
    carHandlerSM->handleRead();
    panelHandlerSM->tick();
    // testChangePanelAfter3s();
}
