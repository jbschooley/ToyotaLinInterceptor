#ifndef TOYOTALININTERCEPTOR_PANELHANDLER_H
#define TOYOTALININTERCEPTOR_PANELHANDLER_H
#include <Arduino.h>
#include "LINController.h"


class PanelHandler {
private:
    uint8_t nextMsg = 0xb1;
    LINController* LINPanel;
    unsigned long lastMillis = 0;

    byte currFrame[10];

public:
    byte dataB1status[8] = {0x00, 0x06, 0x14, 0x00, 0x34, 0x37, 0x00, 0xc1};
    byte data32[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x10};
    byte dataF5[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00};
    byte dataResp39[9] = {0x40, 0x00, 0x00, 0x00, 0x10, 0x90, 0x00, 0x00, 0xe5};

    explicit PanelHandler(HardwareSerial* ser) {
        LINPanel = new LINController(ser, 19200, currFrame, specs);
    }

    void sendEvery10ms() {
        if (millis() - lastMillis >= 10) {
            lastMillis = millis();
            sendNext();
        }
    }

    void sendDataFrame(byte* frame, uint8_t size) {
        LINPanel->send(nextMsg, frame, size);
    }

    void sendRequest(byte msg) {
        LINPanel->request(msg);
    }

    void sendNext() {
        switch (this->nextMsg) {
            case 0xb1:
                Serial.print(" b1 memloc ");
                Serial.print((int)this->dataB1status, HEX);
                Serial.print(" current b1 status ");
                for (int i=0; i<8; i++) {
                    Serial.print(dataB1status[i], HEX);
                    if (i < 7) Serial.print(' ');
                }
                Serial.println();
                LINPanel->send(0xb1, dataB1status, 8);
                nextMsg = 0x32;
                break;
            case 0x32:
                LINPanel->send(0x32, data32, 8);
                nextMsg = 0x39;
                break;
            case 0x39:
                LINPanel->request(0x39);
                nextMsg = 0xba;
                // TODO read response
                break;
            case 0xba:
                LINPanel->request(0xba);
                nextMsg = 0xf5;
                // TODO read response
                break;
            case 0xf5:
                LINPanel->send(0xf5, dataF5, 8);
                nextMsg = 0x76;
                break;
            case 0x76:
                LINPanel->request(0x76);
                nextMsg = 0x78;
                // TODO read response
                break;
            case 0x78:
                LINPanel->request(0x78);
                nextMsg = 0xb1;
                // TODO read response
                break;
        }

        handleRead();

    }

    static void specs(byte* idByte, uint8_t* currFrameSize, bool* isRequest) {
        switch (*idByte) {
            case 0xb1:
            case 0x32:
            case 0xf5:
                *isRequest = false;
                break;
            case 0x39:
            case 0xba:
            case 0x78:
//                *currFrameSize = 8;
                break;
            case 0x76:
//                *currFrameSize = 1;
                *isRequest = true;
                break;
        }
    }

    void handleRead() {
        while (LINPanel->available()) {
            if (LINPanel->processRead()) {
                if (currFrame[1] == 0x39) {
                    Serial.print("39 RESPONSE: ");
                    for (int i=0; i<10; i++) {
                        Serial.print(currFrame[i], HEX);
                        if (i < 9) Serial.print(' ');
                    }
                    Serial.println();
                }
//                Serial.print(" [");
//                for (int i=0; i<10; i++) {
//                    Serial.print(currFrame[i], HEX);
//                    if (i < 10-1) Serial.print(' ');
//                }
//                Serial.println("]");
            }
        }
    }

//    void handleRead() {
//        while (LINPanel->available()) {
//            byte currByte = LINPanel->read();
//
//            if (isInsideFrame) {
//                if (currFramePos == 0) { // ID
//                    switch (currByte) {
//                        case 0x76:
//                            currFrameSize = 2;
//                            break;
//                        case 0xb1:
//                        case 0x32:
//                        case 0x39:
//                        case 0xba:
//                        case 0xf5:
//                        case 0x78:
//                        default:
//                            currFrameSize = 8;
//                            break;
//                    }
//                }
//                if (currFramePos < currFrameSize+2) {  // 2 extra for ID and checksum
//                    currFrame[currFramePos] = currByte;
//                    currFramePos++;
//                } else {
//                    handleFrame();
//                    // reset
//                    isInsideFrame = false;
//                    currFramePos = 0;
//                }
//            } else {
//                if (currByte == 0x55) {
//                    isInsideFrame = true;
//                }
//            }
//        }
//
//    }
//
//    void handleFrame() {
//        Serial.print(" " + currFrameStr);
//        Serial.print(" [");
//        for (int i=0; i<currFrameSize+2; i++) {
//            Serial.print(currFrame[i], HEX);
//            if (i < currFrameSize+1) Serial.print(' ');
//        }
//        Serial.print("]");
//    }

};


#endif //TOYOTALININTERCEPTOR_PANELHANDLER_H
