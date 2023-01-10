//
// Created by Jacob on 1/9/2023.
//

#ifndef TOYOTALININTERCEPTOR_HANDLER_H
#define TOYOTALININTERCEPTOR_HANDLER_H

#include <Arduino.h>

class Handler {
public:
    unsigned long baud = 19200;
    uint8_t frameSize = 8;

    HardwareSerial* serCar;
    HardwareSerial* serPanel;

    Handler(HardwareSerial* serCar, HardwareSerial* serPanel) {
        this->serCar = serCar;
        this->serPanel = serPanel;
    }

    void read() {

    }
};


#endif //TOYOTALININTERCEPTOR_HANDLER_H
