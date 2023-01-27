//
// Created by Jacob on 1/24/2023.
//

#ifndef TOYOTALININTERCEPTOR_PANELHANDLER_H
#define TOYOTALININTERCEPTOR_PANELHANDLER_H

#include "Handler.h"

class PanelHandler : protected Handler {
private:
    // track next message to send
    unsigned long lastMillis = 0;

public:
    uint8_t nextMsg = 0xb1;

    explicit PanelHandler(DataStore* ds, Modifier* mod, HardwareSerial* ser)
            : Handler(ds, mod, ser, new Logger("Panel", false)){
    }

    void tick() {
//        sendEvery10ms();
        handleRead();
    }

    void sendEvery10ms() {
        if (millis() - lastMillis >= 10) {
            lastMillis = millis();
            sendNext();
        }
    }

    void sendMsg(uint8_t id) {
        nextMsg = id;
        sendNext();
    }

    void sendNext() {
        // send message
        if (DataStore::idIsDataPanel(nextMsg)) {
            // if data, send data
            l->log(
                    "send data: "
                    + String(nextMsg, HEX)
                    + " - "
                    + DataStore::frameToString(ds->getFrame(nextMsg))
            );
            LINUtils::sendFrame(ser, nextMsg, ds->getFrame(nextMsg));
        } else if (DataStore::idIsRequestPanel(nextMsg)) {
            // if request, send id......................

            l->log(
                    "send request: "
                    + String(nextMsg, HEX)
            );
            LINUtils::sendRequest(ser, nextMsg);
        }

        // set next message
        switch (nextMsg) {
            case 0xb1:
                nextMsg = 0x32;
                break;
            case 0x32:
                nextMsg = 0x39;
                break;
            case 0x39:
                nextMsg = 0xba;
                break;
            case 0xba:
                nextMsg = 0xf5;
                break;
            case 0xf5:
                nextMsg = 0x76;
                break;
            case 0x76:
                nextMsg = 0x78;
                break;
            case 0x78:
                nextMsg = 0xb1;
                break;
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
