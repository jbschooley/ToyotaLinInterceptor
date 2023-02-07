#include <Arduino.h>
#include <HardwareSerial.h>
#include "LINController.h"
#include "Logger.h"
#include "Timer.h"
#include "DataStore.h"
#include "Modifier.h"
#include "PresetController.h"
#include "Menu.h"
#include "CarHandler.h"
#include "PanelHandler.h"

#define SerialUSB Serial
#define SerialCar Serial1
#define SerialPanel Serial2

#define PinEnPanel 53
#define PinEnCar 46

//unsigned long baud = 19200;

Logger* l;
DataStore* ds;
Modifier* mod;
PresetController* pc;
Menu* menu;

CarHandler* carHandler;
PanelHandler* panelHandler;

void setup() {

    // open debug serial connection
    SerialUSB.begin(115200);

    // initialize logger, data store, handlers
    l = new Logger("main");
    l->log("begin setup");
    ds = new DataStore();
    mod = new Modifier(ds);
    pc = new PresetController(ds, mod);
    menu = new Menu(ds, mod, pc);
    carHandler = new CarHandler(ds, mod, menu, pc, &SerialCar);
    panelHandler = new PanelHandler(ds, mod, &SerialPanel);
    carHandler->panelHandler = panelHandler;

    // enable lin chips
    pinMode(PinEnPanel, OUTPUT);
    digitalWrite(PinEnPanel, HIGH);
    pinMode(PinEnCar, OUTPUT);
    digitalWrite(PinEnCar, HIGH);

    l->log("end setup");

    //l->log("testing defrost");
    //mod->testTemp();
    //mod->printButton(mod->BUTTON_OFF);
}

bool testPanelChanged = false;
//void testChangePanelAfter3s() {
//    if (!testPanelChanged && millis() > 3000) {
//        testPanelChanged = true;
//        panelHandler.dataB1status[1] = 0x02;
//    }
//}

void loop() {
    carHandler->handleRead();
    panelHandler->handleRead();
    //testChangePanelAfter3s();
}
