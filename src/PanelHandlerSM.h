//
// Created by Jacob on 1/24/2023.
//

#ifndef TOYOTALININTERCEPTOR_PANELHANDLERSM_H
#define TOYOTALININTERCEPTOR_PANELHANDLERSM_H

class PanelHandlerSM {
private:
    enum PanelState {
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
    HardwareSerial* ser;
    PanelState state = IDLE;

    // current frame to store response
    uint8_t currID = 0;
    uint8_t currFrame[8]{};

    // track next message to send
    uint8_t nextMsg = 0xb1;
    unsigned long lastMillis = 0;

public:
    explicit PanelHandlerSM(DataStore* ds, HardwareSerial* ser) {
        this->l = new Logger("Panel", false);
        this->ds = ds;
        this->ser = ser;
        ser->begin(19200);
    }

    void tick() {
        sendEvery10ms();
        handleRead();
    }

    void sendEvery10ms() {
        if (millis() - lastMillis >= 10) {
            lastMillis = millis();
            sendNext();
        }
    }

    void sendNext() {
        // send message
        if (DataStore::idIsData(this->nextMsg)) {
            // if data, send data
            l->log(
                    "send data: "
                    + String(this->nextMsg, HEX)
                    + " - "
                    + DataStore::frameToString(this->ds->getFrame(this->nextMsg))
            );
            LINUtils::sendFrame(this->ser, this->nextMsg, this->ds->getFrame(this->nextMsg));
        } else if (DataStore::idIsRequest(this->nextMsg)) {
            // if request, send id
//            this->currID = this->nextMsg;
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
                    // TODO modify buttons at save?
                    this->ds->saveFrame(this->currID, this->currFrame);
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

    void reset() {
        this->state = IDLE;
    }
};

#endif //TOYOTALININTERCEPTOR_PANELHANDLERSM_H
