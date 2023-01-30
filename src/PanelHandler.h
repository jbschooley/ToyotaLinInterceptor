//
// Created by Jacob on 1/24/2023.
//

#ifndef TOYOTALININTERCEPTOR_PANELHANDLER_H
#define TOYOTALININTERCEPTOR_PANELHANDLER_H

#include "Handler.h"

class PanelHandler : public Handler {
public:

    explicit PanelHandler(DataStore* ds, Modifier* mod, HardwareSerial* ser)
            : Handler(ds, mod, ser, new Logger("Panel", false)) {}

    void sendMsg(uint8_t id) {
        // send message
        if (DataStore::idIsDataPanel(id)) {
            // if data, send data
            l->log(
                    "send data: "
                    + String(id, HEX)
                    + " - "
                    + DataStore::frameToString(ds->getFrame(id))
            );
            lin->sendFrame(id, ds->getFrame(id));
        } else if (DataStore::idIsRequestPanel(id)) {
            // if request, send id
            l->log(
                    "send request: "
                    + String(id, HEX)
            );
            lin->sendRequest(id);
        }
    }

    void onReceiveID() override {
        if (DataStore::idIsRequest(currID)) {
            // if expecting response, go to next state
            state = WAIT_BYTE_0;
        } else {
            // otherwise, go back to idle
            reset();
        }
    }

    void onReceiveData() override {
        // TODO modify buttons at receive?
    }

};

#endif //TOYOTALININTERCEPTOR_PANELHANDLER_H
