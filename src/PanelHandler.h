//
// Created by Jacob on 1/24/2023.
//

#ifndef TOYOTALININTERCEPTOR_PANELHANDLER_H
#define TOYOTALININTERCEPTOR_PANELHANDLER_H

#include "Handler.h"

class PanelHandler : public Handler {
public:

    explicit PanelHandler(DataStore* ds, Modifier* mod, HardwareSerial* ser)
            : Handler(ds, mod, ser, new Logger("Panel", false)){
    }

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
            LINUtils::sendFrame(ser, id, ds->getFrame(id));
        } else if (DataStore::idIsRequestPanel(id)) {
            // if request, send id
            l->log(
                    "send request: "
                    + String(id, HEX)
            );
            LINUtils::sendRequest(ser, id);
        }
    }

    void handleByte(const uint8_t* b) override {
        switch (state) {
            case IDLE:
                if (*b == 0x55) state = WAIT_ID;
                break;
            case WAIT_ID:
                currID = *b;
                if (DataStore::idIsRequest(currID)) {
                    // if expecting response, go to next state
                    state = WAIT_BYTE_0;
                } else {
                    // otherwise, go back to idle
                    reset();
                }
                break;
            case WAIT_BYTE_0:
                currFrame[0] = *b;
                state = WAIT_BYTE_1;
                break;
            case WAIT_BYTE_1:
                currFrame[1] = *b;
                state = WAIT_BYTE_2;
                break;
            case WAIT_BYTE_2:
                currFrame[2] = *b;
                state = WAIT_BYTE_3;
                break;
            case WAIT_BYTE_3:
                currFrame[3] = *b;
                state = WAIT_BYTE_4;
                break;
            case WAIT_BYTE_4:
                currFrame[4] = *b;
                state = WAIT_BYTE_5;
                break;
            case WAIT_BYTE_5:
                currFrame[5] = *b;
                state = WAIT_BYTE_6;
                break;
            case WAIT_BYTE_6:
                currFrame[6] = *b;
                state = WAIT_BYTE_7;
                break;
            case WAIT_BYTE_7:
                currFrame[7] = *b;
                state = WAIT_CHECKSUM;
                break;
            case WAIT_CHECKSUM:
                uint8_t calculatedChecksum = LINUtils::getChecksum(&currID, currFrame);
                if (calculatedChecksum == *b) {
                    // response is good, send to datastore
                    l->log(
                            "received response: "
                            + String(currID, HEX)
                            + " - "
                            + DataStore::frameToString(currFrame)
                            + " - "
                            + String(calculatedChecksum, HEX)
                    );
                    // TODO modify buttons at receive?
                    ds->saveFrame(currID, currFrame);
                    mod->testButtons();
                } else {
                    // checksum is bad, log error
                    l->log(
                            "Checksum error ID "
                            + String(currID, HEX)
                            + " - "
                            + DataStore::frameToString(currFrame)
                            + " - "
                            + String(calculatedChecksum, HEX)
                            + " - "
                            + String(*b, HEX)
                    );
                }
                reset();
                break;
        }
    }

};

#endif //TOYOTALININTERCEPTOR_PANELHANDLER_H
