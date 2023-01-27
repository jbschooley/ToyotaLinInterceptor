//
// Created by Jacob on 1/23/2023.
//

#ifndef TOYOTALININTERCEPTOR_CARHANDLER_H
#define TOYOTALININTERCEPTOR_CARHANDLER_H

#include "PanelHandler.h"
#include "Handler.h"

class CarHandler : public Handler {
public:
    PanelHandler* panelHandlerSM;

    explicit CarHandler(DataStore* ds, Modifier* mod, HardwareSerial* ser)
            : Handler(ds, mod, ser, new Logger("Car", false)) {}

    void handleByte(const uint8_t* b) override {
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

};

#endif //TOYOTALININTERCEPTOR_CARHANDLER_H
