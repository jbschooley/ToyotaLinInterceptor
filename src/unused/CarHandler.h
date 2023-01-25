//
// Created by Jacob on 12/25/2022.
//

#ifndef TOYOTALININTERCEPTOR_CARHANDLER_H
#define TOYOTALININTERCEPTOR_CARHANDLER_H
#include <Arduino.h>
#include "LINController.h"
#include "PanelHandler.h"

class CarHandler {
private:
    byte currFrame[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    bool isInsideFrame = false;
    uint8_t currFrameSize = 8;
    uint8_t currFramePos = 0;

    enum receiveType {
        NONE,
        ID,
        DATA,
        RESPONSE,
    };

    receiveType currentlyReceiving = NONE;
public:
    LINController* LINCar;
    LINController* LINPanel;

    explicit CarHandler(HardwareSerial* serCar, HardwareSerial* serPanel) {
        LINCar = new LINController(serCar, 19200, currFrame);
        LINPanel = new LINController(serPanel, 19200, currFrame);
    }

    void handleIDByte(const byte* idByte) {
        switch (*idByte) {
            case 0xb1:
            case 0x32:
            case 0xf5:
                currentlyReceiving = DATA;
                break;
            case 0x39:
            case 0xba:
            case 0x78:
                currentlyReceiving = RESPONSE;
                LINPanel->request(*idByte);
                break;
            case 0x76:
                // send request to panel, do NOT get response because panel does not respond
                this->resetFrame();
                break;
        }
    }

    void resetFrame() {
        currentlyReceiving = NONE;
        currFramePos = 0;
    }

    void processByteCar() {
        byte currByte = LINCar->ser->read();

        if (currentlyReceiving == NONE) {
            if (currByte == 0x55) {
                // beginning of frame
                currentlyReceiving = ID;
            }
        } else if (currentlyReceiving == ID) {
            currFrame[0] = currByte;
            currFramePos++;
            handleIDByte(&currByte);
        } else if (currentlyReceiving == DATA) {
            if (currFramePos < currFrameSize+2) {
                currFrame[currFramePos] = currByte;
                currFramePos++;
            } else {
                // buffer full
                // send to panel
                LINPanel->send(currFrame[0], currFrame+1, currFrameSize);
                resetFrame();
            }
        }

    }

    void processBytePanel() {
        byte currByte = LINPanel->ser->read();

        if (currentlyReceiving == RESPONSE) {
            if (currFramePos < currFrameSize+2) {
                currFrame[currFramePos] = currByte;
                currFramePos++;
            } else {
                // buffer full
                // send to car
                LINCar->sendResponse(currFrame[0], currFrame+1, currFrameSize);
                resetFrame();
            }
        }

    }

    void handleRead() {
        while (LINCar->available()) {
            this->processByteCar();
        }
        while (LINPanel->available()) {
            this->processBytePanel();
        }
    }
};

#endif //TOYOTALININTERCEPTOR_CARHANDLER_H