//
// Created by Jacob on 1/23/2023.
//

#ifndef TOYOTALININTERCEPTOR_CARHANDLER_H
#define TOYOTALININTERCEPTOR_CARHANDLER_H

#include "PanelHandler.h"
#include "Handler.h"

class CarHandler : public Handler {
private:
    Menu* menu;
    Toggle* toggle;
    PresetController* pc;
public:
    PanelHandler* panelHandler{};

    explicit CarHandler(DataStore* ds, Modifier* mod, Menu* menu, Toggle* toggle, PresetController* pc, HardwareSerial* ser)
            : Handler(ds, mod, ser, new Logger("Car", false)) {
        this->menu = menu;
        this->toggle = toggle;
        this->pc = pc;
    }

    void onReceiveID() override {
        if (DataStore::idIsData(currID)) {
            // if data, go to next state
            state = WAIT_BYTE_0;
        } else if (DataStore::idIsRequest(currID)) {
            // if request, send response and go back to idle
            // TODO modify buttons at send?
            lin->sendResponse(currID, ds->getFrame(currID));
            reset();
        } else {
            // if neither, go back to idle
            reset();
        }
        // Request button status from panel if car sent 0x78
        // 0x78 is 3 messages before 0x39 for buffer; seems to be fastest and most reliable
        if (currID == 0x78) panelHandler->sendMsg(0x39);
    }

    void onReceiveData() override {
        // if car sent climate status, forward to panel
        if (currID == 0xb1) {
            //l->log(
            //        "received data: "
            //        + String(currID, HEX)
            //        + " - "
            //        + DataStore::frameToString(currFrame)
            //        + " - "
            //        + String(calculatedChecksum, HEX)
            //);
            menu->run();
            toggle->run();
            panelHandler->sendMsg(currID);
            // TODO modify buttons after climate status received
            pc->presetAfter1s();
        }
    }

};

#endif //TOYOTALININTERCEPTOR_CARHANDLER_H
