//
// Created by Jacob on 1/23/2023.
//

#ifndef TOYOTALININTERCEPTOR_CARHANDLERSM_H
#define TOYOTALININTERCEPTOR_CARHANDLERSM_H

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

class CarHandlerSM {
private:
    DataStore* ds;
    CarState state = IDLE;
    HardwareSerial* ser;

    // current frame
    uint8_t currID{};
    uint8_t currFrame[8]{};

public:
    explicit CarHandlerSM(DataStore* ds, HardwareSerial* ser) {
        this->ds = ds;
        this->ser = ser;
    }

    void handleRead() {
        while (ser->available()) {
            uint8_t b = ser->read();
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
                    // TODO: send response, including override if necessary
                    this->state = IDLE;
                } else {
                    // if neither, go back to idle
                    this->state = IDLE;
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
                // TODO: verify checksum
                // if checksum is good, save frame to data store
                if (true) {
                    // this is reached after frame has been received and verified
                    ds->saveFrame(this->currID, this->currFrame);
                }
                this->state = IDLE;
                break;
        }
    }
};

#endif //TOYOTALININTERCEPTOR_CARHANDLERSM_H
