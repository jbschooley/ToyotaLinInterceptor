//
// Created by Jacob on 1/9/2023.
//

#ifndef TOYOTALININTERCEPTOR_HANDLER_H
#define TOYOTALININTERCEPTOR_HANDLER_H

#include <Arduino.h>

class Handler {
protected:
    enum State {
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
    LINController* lin;
    State state = IDLE;

    // current frame to store received data
    uint8_t currID = 0;
    uint8_t currFrame[8]{};

public:

    explicit Handler(DataStore* ds, Modifier* mod, HardwareSerial* ser, Logger* l) {
        this->l = l;
        this->ds = ds;
        this->mod = mod;
        this->lin = new LINController(ser);
    }

    void reset() {
        state = IDLE;
    }

//    virtual void handleByte(const uint8_t* b) {}

    void handleRead() {
        while (lin->available()) {
            uint8_t b = lin->read();
            handleByte(&b);
        }
    }

    void handleByte(const uint8_t* b) {
        switch (state) {
            case IDLE:
                if (*b == 0x55) state = WAIT_ID;
                break;
            case WAIT_ID:
                currID = *b;
                onReceiveID();
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
                    //       "received data: "
                    //       + String(currID, HEX)
                    //       + " - "
                    //       + DataStore::frameToString(currFrame)
                    //       + " - "
                    //       + String(calculatedChecksum, HEX)
                    //);
                    ds->saveFrame(currID, currFrame);
                    onReceiveData();
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

    virtual void onReceiveID() {}
    virtual void onReceiveData() {}

};


#endif //TOYOTALININTERCEPTOR_HANDLER_H
