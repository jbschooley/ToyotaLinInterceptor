//
// Created by Jacob on 11/21/2022.
//

#ifndef TOYOTALININTERCEPTOR_LINCONTROLLER_H
#define TOYOTALININTERCEPTOR_LINCONTROLLER_H
#include <Arduino.h>

class LINController {
private:
    HardwareSerial* ser;
    unsigned long baud;

    byte currFrame[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t currFrameSize = 8;
    uint8_t currFramePos = 0;
    bool isRequest = false;
    bool isInsideFrame = false;

    void (*funcProcessReadId)(byte*, uint8_t*, bool*);
    byte* returnFrame;

    size_t writeBreak() {
        ser->flush();
        // configure to half baudrate --> a t_bit will be doubled
        ser->begin(baud >> 1);
        // write 0x00, including Stop-Bit (=1),
        // qualifies when writing in slow motion like a Break in normal speed
        size_t ret = ser->write(uint8_t(0x00));
        // ensure this is sent
        ser->flush();
        // restore normal speed
        ser->begin(baud);
        return ret;
    }
public:
    explicit LINController(
            HardwareSerial* ser,
            unsigned long baud,
            byte* returnFrame,
            void (*funcProcessReadId)(byte*, uint8_t*, bool*)) {
        this->ser = ser;
        this->baud = baud;
        this->funcProcessReadId = funcProcessReadId;
        this->returnFrame = returnFrame;

        // open serial connection
        ser->begin(19200);
    }

    int available() {
        return ser->available();
    }

    bool processRead() {
        while (ser->available()) {       // If anything comes in Serial1 (pins 0 & 1)
//            Serial.print(" ");
//            Serial.print(ser->read(), HEX);  // read it and send it out Serial (USB)
            byte currByte = ser->read();

            if (isInsideFrame) {
                if (currFramePos == 0) { // ID
                    funcProcessReadId(&currByte, &currFrameSize, &isRequest);
                }

                if (isRequest) {
                    currFrame[currFramePos] = currByte;
                    return handleFrame();
                    resetFrame();
                } else if (currFramePos < currFrameSize+2) {  // 2 extra for ID and checksum
                    currFrame[currFramePos] = currByte;
                    currFramePos++;
                } else {
                    // end of frame
                    return handleFrame();
                }

            } else {
                if (currByte == 0x55) {
                    // beginning of frame
                    isInsideFrame = true;
                }
            }
        }

        // not done with reading a frame yet
        return false;
    }

    void resetFrame() {
        isInsideFrame = false;
        currFramePos = 0;
    }

//    int read() {
//        return ser->read();
//    }

    bool handleFrame() {
        uint8_t frameSizeToReturn = currFrameSize + 1;
        if (isRequest) {
            returnFrame[0] = 1;
            frameSizeToReturn = 1;
        } else {
            returnFrame[0] = 0;
            // verify checksum
            byte chksm = LINController::getChecksum(&currFrame[0], &currFrame[1], 8);
            if (chksm != currFrame[currFrameSize+1]) {
                resetFrame();
                return false;
            }
        }

//        Serial.print(" [");
//        for (int i=0; i<frameSizeToReturn; i++) {
//            Serial.print(currFrame[i], HEX);
//            if (i < frameSizeToReturn-1) Serial.print(' ');
//        }
//        Serial.print("]");

        for (int i=0; i<frameSizeToReturn; i++) {
            returnFrame[i+1] = currFrame[i];
        }

        resetFrame();
        return true;
    }

    void send(byte ident, byte data[], byte data_size) {
        Serial.println();
        Serial.print(millis());
        Serial.print(" sending data");
        byte chksm = LINController::getChecksum(&ident, data, data_size);
        writeBreak();
        ser->write(0x55);
        ser->write(ident);
        for(int i=0;i<data_size;i++) ser->write(data[i]);
        ser->write(chksm);
        ser->flush();
//        processRead();
//        delay(10);
    }

    void request(byte lin_id) {
        Serial.println();
        Serial.print(millis());
        Serial.print(" request data");
        writeBreak();
        ser->write(0x55);
        ser->write(lin_id);
        // SerialPanel.end();
//        delay(10);
        // if (SerialPanel.available()) {       // If anything comes in Serial1 (pins 0 & 1)
        //   Serial.print(SerialPanel.read());  // read it and send it out Serial (USB)
        // }
        ser->flush();
//        processRead();
    }

    static byte getChecksum(byte* ident, const byte data[], byte data_size)
    {
        uint16_t sum = *ident;
        // test FrameID bits for classicChecksum
        if ((sum & 0x3F) >= 0x3C)
        {
            // LIN 1.x: legacy
            // LIN 2.0: don't include PID for ChkSum calculation on configuration and reserved frames
            sum = 0x00;
        }

        // sum up all bytes (including carryover to the high byte)
        // ID allready considered
        while (data_size-- > 0)
            sum += data[data_size];
        // add high byte (carry over) to the low byte
        while (sum >> 8)
            sum = (sum & 0xFF) + (sum >> 8);
        // inverting result
        return (~sum);
    }
};


#endif //TOYOTALININTERCEPTOR_LINCONTROLLER_H
