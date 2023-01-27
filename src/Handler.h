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
    HardwareSerial* ser;
    State state = IDLE;

    // current frame to store received data
    uint8_t currID = 0;
    uint8_t currFrame[8]{};

public:

    explicit Handler(DataStore* ds, Modifier* mod, HardwareSerial* ser, Logger* l) {
        this->l = l;
        this->ds = ds;
        this->mod = mod;
        this->ser = ser;
        ser->begin(19200);
    }

    virtual void handleByte(const uint8_t* b) {}

    void handleRead() {
        while (ser->available()) {
            uint8_t b = ser->read();
            handleByte(&b);
        }
    }

    void reset() {
        state = IDLE;
    }

};


#endif //TOYOTALININTERCEPTOR_HANDLER_H
