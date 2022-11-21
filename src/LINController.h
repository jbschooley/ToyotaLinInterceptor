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

    size_t writeLinBreak() {
        ser->flush();
        // configure to half baudrate --> a t_bit will be doubled
        ser->begin(baud >> 1);
        // write 0x00, including Stop-Bit (=1),
        // qualifies when writing in slow motion like a Break in normal speed
        size_t ret = ser->write(uint8_t(0x00));
        // ensure this is send
        ser->flush();
        // restore normal speed
        ser->begin(baud);
        return ret;
    }
public:
    explicit LINController(HardwareSerial* ser, unsigned long baud) {
        this->ser = ser;
        this->baud = baud;
    }

    void send(byte ident, byte data[], byte data_size) {
        Serial.print("sending data at ");
        Serial.println(millis());
        byte chksm = this->getChecksum(ident, data, data_size);
        writeLinBreak();
        ser->write(0x55);
        ser->write(ident);
        for(int i=0;i<data_size;i++) ser->write(data[i]);
        ser->write(chksm);
        ser->flush();
        delay(10);
    }

    void request(byte lin_id) {
        Serial.print("requesting data at ");
        Serial.println(millis());
        writeLinBreak();
        ser->write(0x55);
        ser->write(lin_id);
        while (ser->available()) {       // If anything comes in Serial1 (pins 0 & 1)
            Serial.print(ser->read(), HEX);  // read it and send it out Serial (USB)
            Serial.print(" ");
        }
        // SerialPanel.end();
        delay(10);
        // if (SerialPanel.available()) {       // If anything comes in Serial1 (pins 0 & 1)
        //   Serial.print(SerialPanel.read());  // read it and send it out Serial (USB)
        // }
        ser->flush();
    }

    static byte getChecksum(byte ident, const byte data[], byte data_size)
    {
        uint16_t sum = ident;
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
