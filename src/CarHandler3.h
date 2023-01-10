//
// Created by Jacob on 1/9/2023.
//

#ifndef TOYOTALININTERCEPTOR_CARHANDLER3_H
#define TOYOTALININTERCEPTOR_CARHANDLER3_H

class CarHandler3 {
private:
    LINController* LINCar;
    byte currFrame[10];
public:
    PanelHandler* panelHandler;
    byte* dataB1PanelStatus;

    explicit CarHandler3(HardwareSerial* serCar) {
        LINCar = new LINController(serCar, 19200, currFrame, specs);
    }

    void panelHandlerMemLoc() {
        Serial.print("MEMLOC ");
        Serial.println((int)this->panelHandler, HEX);
    }

    static void specs(byte* idByte, uint8_t* currFrameSize, bool* isRequest) {
        switch (*idByte) {
            case 0xb1:
            case 0x32:
            case 0xf5:
                *currFrameSize = 1;
                *isRequest = true;
                break;
            case 0x39:
            case 0xba:
            case 0x78:
                *currFrameSize = 8;
                *isRequest = false;
                break;
            case 0x76:
                *currFrameSize = 1;
                *isRequest = true;
                break;
        }
    }

    void handleRead() {
        while (LINCar->available()) {
            if (LINCar->processReadSlave()) {
                Serial.print(millis());

                bool isRequest = currFrame[0];
                uint8_t frameSizeToPrint = isRequest ? 1 : 10;
                Serial.print(" [");
                for (int i=0; i<=frameSizeToPrint; i++) {
                    Serial.print(currFrame[i], HEX);
                    if (i < frameSizeToPrint) Serial.print(' ');
                }
                Serial.print("] ");

                if (isRequest) {
                    Serial.print("request ");
                } else {
                    Serial.print("data ");
                }

                uint8_t idByte = currFrame[1];
                if (idByte == 0xb1) {
                    for (int i=0; i<8; i++) {
                        dataB1PanelStatus[i] = currFrame[i+2];
                    }
                }

                Serial.println();

            }
        }
    }
};

#endif //TOYOTALININTERCEPTOR_CARHANDLER3_H
