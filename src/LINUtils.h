//
// Created by Jacob on 1/24/2023.
//

#ifndef TOYOTALININTERCEPTOR_LINUTILS_H
#define TOYOTALININTERCEPTOR_LINUTILS_H

class LINUtils {
public:

//    AI generated, might be more efficient but needs testing
//    static uint8_t getChecksum(const uint8_t* frame, uint8_t frameSize) {
//        uint8_t sum = 0;
//        for (int i=0; i<8; i++) {
//            sum += frame[i];
//        }
//        return (uint8_t) (0xFF - sum);
//    }

    static uint8_t getChecksum(uint8_t* ident, const uint8_t* frame)
    {
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
};

#endif //TOYOTALININTERCEPTOR_LINUTILS_H
