//
// Created by Jacob on 1/24/2023.
//

#ifndef TOYOTALININTERCEPTOR_LINUTILS_H
#define TOYOTALININTERCEPTOR_LINUTILS_H

#include "DataStore.h"

class LINUtils {
private:
//    static unsigned long baud = 19200;

public:

//    AI generated, might be more efficient but needs testing
//    static uint8_t getChecksum(const uint8_t* frame, uint8_t frameSize) {
//        uint8_t sum = 0;
//        for (int i=0; i<8; i++) {
//            sum += frame[i];
//        }
//        return (uint8_t) (0xFF - sum);
//    }

    static uint8_t getChecksum(const uint8_t* ident, const uint8_t* frame) {
        uint16_t sum = *ident;
        // test FrameID bits for classicChecksum
        if ((sum & 0x3F) >= 0x3C) {
            // LIN 1.x: legacy
            // LIN 2.0: don't include PID for ChkSum calculation on configuration and reserved frames
            sum = 0x00;
        }

        // sum up all bytes (including carryover to the high byte)
        // ID already considered
        for (int i = 0; i < 8; i++) {
            sum += (uint16_t) frame[i];
        }
        // add high byte (carry over) to the low byte
        while (sum >> 8)
            sum = (sum & 0xFF) + (sum >> 8);
        // inverting result
        return (~sum);
    }

    static size_t writeBreak(HardwareSerial* ser) {
        ser->flush();
        // configure to half baudrate --> a t_bit will be doubled
        ser->begin(19200 >> 1);
        // write 0x00, including Stop-Bit (=1),
        // qualifies when writi+ng in slow motion like a Break in normal speed
        size_t ret = ser->write(uint8_t(0x00));
        // ensure this is sent
        ser->flush();
        // restore normal speed
        ser->begin(19200);
        return ret;
    }

    static void sendFrame(HardwareSerial* ser, byte ident, const byte data[]) {
        writeBreak(ser);
        ser->write(0x55);
        ser->write(ident);
        byte chksm = getChecksum(&ident, data);
        for(int i=0; i<8; i++) ser->write(data[i]);
        ser->write(chksm);
        ser->flush();
    }

    static void sendResponse(HardwareSerial* ser, byte ident, const byte data[]) {
        byte chksm = getChecksum(&ident, data);
        for(int i=0; i<8; i++) ser->write(data[i]);
        ser->write(chksm);
        ser->flush();
    }

    static void sendRequest(HardwareSerial* ser, byte ident) {
        writeBreak(ser);
        ser->write(0x55);
        ser->write(ident);
        ser->flush();
    }
};

#endif //TOYOTALININTERCEPTOR_LINUTILS_H
