#ifndef TOYOTALININTERCEPTOR_PANELHANDLER_H
#define TOYOTALININTERCEPTOR_PANELHANDLER_H
#include <Arduino.h>
#include "LINController.h"
#include "arduino-timer.h"


class PanelHandler {
private:
    uint8_t nextMsg = 0xb1;
    LINController* LINPanel;
    unsigned long lastMillis = 0;

public:
    byte dataB1status[8] = {0x00, 0x06, 0x14, 0x00, 0x34, 0x37, 0x00, 0xc1};
    byte data32[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x10};
    byte dataF5[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00};

    explicit PanelHandler(HardwareSerial* ser) {
        LINPanel = new LINController(ser, 19200);
    }

    void sendEvery10ms() {
        if (millis() - lastMillis >= 10) {
            lastMillis = millis();
            sendNext();
        }
    }

    void sendNext() {

        switch (this->nextMsg) {
            case 0xb1:
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

    }
};


#endif //TOYOTALININTERCEPTOR_PANELHANDLER_H
