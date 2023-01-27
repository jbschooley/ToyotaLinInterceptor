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
        if (DataStore::idIsDataPanel(this->nextMsg)) {
            // if data, send data
            l->log(
                    "send data: "
                    + String(this->nextMsg, HEX)
                    + " - "
                    + DataStore::frameToString(this->ds->getFrame(this->nextMsg))
            );
            LINUtils::sendFrame(this->ser, this->nextMsg, this->ds->getFrame(this->nextMsg));
        } else if (DataStore::idIsRequestPanel(this->nextMsg)) {
            // if request, send id......................

            l->log(
                    "send request: "
                    + String(this->nextMsg, HEX)
            );
            LINUtils::sendRequest(this->ser, this->nextMsg);
        }

        // set next message
        switch (this->nextMsg) {
            case 0xb1:
                this->nextMsg = 0x32;
                break;
            case 0x32:
                this->nextMsg = 0x39;
                break;
            case 0x39:
                this->nextMsg = 0xba;
                break;
            case 0xba:
                this->nextMsg = 0xf5;
                break;
            case 0xf5:
                this->nextMsg = 0x76;
                break;
            case 0x76:
                this->nextMsg = 0x78;
                break;
            case 0x78:
                this->nextMsg = 0xb1;
                break;
        }
    }

    void handleByte(const uint8_t* b) override {
        switch (this->state) {
            case IDLE:
                if (*b == 0x55) this->state = WAIT_ID;
                break;
            case WAIT_ID:
                this->currID = *b;
                if (DataStore::idIsRequest(this->currID)) {
                    // if expecting response, go to next state
                    this->state = WAIT_BYTE_0;
                } else {
                    // otherwise, go back to idle
                    this->reset();
                }
                break;
            case WAIT_BYTE_0:
                this->currFrame[0] = *b;
                this->state = WAIT_BYTE_1;
                break;
            case WAIT_BYTE_1:
                this->currFrame[1] = *b;
                this->state = WAIT_BYTE_2;
                break;
            case WAIT_BYTE_2:
                this->currFrame[2] = *b;
                this->state = WAIT_BYTE_3;
                break;
            case WAIT_BYTE_3:
                this->currFrame[3] = *b;
                this->state = WAIT_BYTE_4;
                break;
            case WAIT_BYTE_4:
                this->currFrame[4] = *b;
                this->state = WAIT_BYTE_5;
                break;
            case WAIT_BYTE_5:
                this->currFrame[5] = *b;
                this->state = WAIT_BYTE_6;
                break;
            case WAIT_BYTE_6:
                this->currFrame[6] = *b;
                this->state = WAIT_BYTE_7;
                break;
            case WAIT_BYTE_7:
                this->currFrame[7] = *b;
                this->state = WAIT_CHECKSUM;
                break;
            case WAIT_CHECKSUM:
                uint8_t calculatedChecksum = LINUtils::getChecksum(&this->currID, this->currFrame);
                if (calculatedChecksum == *b) {
                    // response is good, send to datastore
                    l->log(
                            "received response: "
                            + String(this->currID, HEX)
                            + " - "
                            + DataStore::frameToString(this->currFrame)
                            + " - "
                            + String(calculatedChecksum, HEX)
                    );
                    // TODO modify buttons at receive?
                    this->ds->saveFrame(this->currID, this->currFrame);
                    mod->testButtons();
                } else {
                    // checksum is bad, log error
                    l->log(
                            "Checksum error ID "
                            + String(this->currID, HEX)
                            + " - "
                            + DataStore::frameToString(this->currFrame)
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
