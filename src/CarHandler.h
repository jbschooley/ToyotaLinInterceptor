//
// Created by Jacob on 1/23/2023.
//

#ifndef TOYOTALININTERCEPTOR_CARHANDLER_H
#define TOYOTALININTERCEPTOR_CARHANDLER_H

#include "PanelHandler.h"
#include "Handler.h"

class CarHandler : public Handler {
public:
    PanelHandler* panelHandler{};

    explicit CarHandler(DataStore* ds, Modifier* mod, HardwareSerial* ser)
            : Handler(ds, mod, ser, new Logger("Car", false)) {}

    void handleByte(const uint8_t* b) override {
        switch (state) {
            case IDLE:
                if (*b == 0x55) state = WAIT_ID;
                break;
            case WAIT_ID:
                currID = *b;
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
                uint8_t calculatedChecksum = LINController::getChecksum(&currID, currFrame);
                // if checksum is good, save frame to data store
                if (calculatedChecksum == *b) {
                    // this is reached after frame has been received and verified
                    // print frame to serial
                    //l->log(
                    //        "received data: "
                    //        + String(currID, HEX)
                    //        + " - "
                    //        + DataStore::frameToString(currFrame)
                    //        + " - "
                    //        + String(calculatedChecksum, HEX)
                    //);
                    ds->saveFrame(currID, currFrame);
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
                        panelHandler->sendMsg(currID);
                        // TODO modify buttons after climate status received
                        mod->presetAfter1s();
                    }
                    //l->log(
                    //        "saved data: "
                    //        + String(currID, HEX)
                    //        + " - "
                    //        + DataStore::frameToString(ds->getFrame(currID))
                    //        + " to "
                    //        + String((int)ds->getFrame(currID), HEX)
                    //);
                } else {
                    // if checksum is bad, log error
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

#endif //TOYOTALININTERCEPTOR_CARHANDLER_H
