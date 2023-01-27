//
// Created by Jacob on 1/23/2023.
//

#ifndef TOYOTALININTERCEPTOR_CARHANDLERSM_H
#define TOYOTALININTERCEPTOR_CARHANDLERSM_H

#include "PanelHandlerSM.h"

class CarHandlerSM {
private:
    enum CarState {
        IDLE,
        WAIT_ID,
        WAIT_BYTE_0,
        WAIT_BYTE_1,
        WAIT_BYTE_2,
        WAIT_BYTE_3,
        WAIT_BYTE_4,
        WAIT_BYTE_5,
        WAIT_BYTE_6,
        WAIT_BYTE_7,
        WAIT_CHECKSUM,
    };

    Logger* l;
    DataStore* ds;
    Modifier* mod;
    HardwareSerial* ser;
    CarState state = IDLE;

    // current frame to store received data
    uint8_t currID = 0;
    uint8_t currFrame[8]{};

public:
    PanelHandlerSM* panelHandlerSM;

    explicit CarHandlerSM(DataStore* ds, Modifier* mod, HardwareSerial* ser) {
        this->l = new Logger("Car", false);
        this->ds = ds;
        this->mod = mod;
        this->ser = ser;
        ser->begin(19200);
    }

    void handleRead() {
//        l->log("handleRead: " + String(ser->available()) + " bytes available");
        while (ser->available()) {
            uint8_t b = ser->read();
//            l->log("read byte: " + String(b, HEX));
            handleByte(&b);
        }
    }

    void handleByte(const uint8_t* b) {
        switch (this->state) {
            case IDLE:
                if (*b == 0x55) this->state = WAIT_ID;
                break;
            case WAIT_ID:
                this->currID = *b;
                if (DataStore::idIsData(this->currID)) {
                    // if data, go to next state
                    this->state = WAIT_BYTE_0;
                } else if (DataStore::idIsRequest(this->currID)) {
                    // if request, send response and go back to idle
                    // TODO modify buttons at send?
                    LINUtils::sendResponse(this->ser, this->currID, this->ds->getFrame(this->currID));
                    this->reset();
                } else {
                    // if neither, go back to idle
                    this->reset();
                }
                // Request button status from panel if car sent 0x78
                // 0x78 is 3 messages before 0x39 for buffer; seems to be fastest and most reliable
                if (this->currID == 0x78) panelHandlerSM->sendMsg(0x39);
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
                // if checksum is good, save frame to data store
                if (calculatedChecksum == *b) {
                    // this is reached after frame has been received and verified
                    // print frame to serial
//                    l->log(
//                            "received data: "
//                            + String(this->currID, HEX)
//                            + " - "
//                            + DataStore::frameToString(this->currFrame)
//                            + " - "
//                            + String(calculatedChecksum, HEX)
//                    );
                    this->ds->saveFrame(this->currID, this->currFrame);
                    // if car sent climate status, forward to panel
                    if (this->currID == 0xb1) panelHandlerSM->sendMsg(this->currID);
//                    l->log(
//                            "saved data: "
//                            + String(this->currID, HEX)
//                            + " - "
//                            + DataStore::frameToString(this->ds->getFrame(this->currID))
//                            + " to "
//                            + String((int)this->ds->getFrame(this->currID), HEX)
//                    );
                } else {
                    // if checksum is bad, log error
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
                this->reset();
                break;
        }
    }

    void reset() {
        this->state = IDLE;
    }
};

#endif //TOYOTALININTERCEPTOR_CARHANDLERSM_H
