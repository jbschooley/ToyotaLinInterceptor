/**
 * Handles low-level sending and receiving messages on the LIN bus.
 *
 * This is not specific to Toyota's implementation, but does assume that all
 * messages are 8 bytes long. It is designed for use with the TLE7259-3GE
 * LIN transceiver. Other transceivers may work differently.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_LINCONTROLLER_H
#define TOYOTALININTERCEPTOR_LINCONTROLLER_H

#define LIN_BAUD 19200

class LINController {
private:
    HardwareSerial* ser;

public:

    /**
     * Creates a new LINController.
     * @param ser the serial port to use
     */
    explicit LINController(HardwareSerial* ser) {
        this->ser = ser;
        this->ser->begin(LIN_BAUD);
    }

    /**
     * Returns the number of bytes available in the input stream.
     * @return the number of bytes available
     */
    int available() {
        return ser->available();
    }

    /**
     * Reads the next byte from the input stream.
     * @return the next byte
     */
    int read() {
        return ser->read();
    }

    /**
     * Writes a Break on the LIN bus. This is used to signal the start of a
     * frame.
     */
    size_t writeBreak() {
        ser->flush();
        // configure to half baudrate --> a t_bit will be doubled
        ser->begin(19200 >> 1);
        // write 0x00, including stop bit
        // this is recognized as a Break by the responder
        size_t ret = ser->write(uint8_t(0x00));
        // ensure this is sent before continuing
        ser->flush();
        // restore normal speed
        ser->begin(19200);
        return ret;
    }

    /**
     * Sends a full frame, including data, on the LIN bus. Used by a
     * Commander node.
     *
     * @param ident     The ID of the frame to send
     * @param data      An 8-byte array of data to send
     */
    void sendFrame(byte ident, const byte data[]) {
        writeBreak();
        ser->write(0x55);
        ser->write(ident);
        byte chksm = getChecksum(&ident, data);
        for(int i=0; i<8; i++) ser->write(data[i]);
        ser->write(chksm);
        ser->flush();
    }

    /**
     * Sends a request for data on the LIN bus. Used by a Commander node.
     *
     * @param ident     The ID of the frame to request
     */
    void sendRequest(byte ident) {
        writeBreak();
        ser->write(0x55);
        ser->write(ident);
        ser->flush();
    }

    /**
     * Sends a response to a request on the LIN bus. Used by a Responder
     * node.
     *
     * @param ident     The ID of the frame to respond to. This is not sent
     *                  on the bus, but is used to calculate the checksum.
     * @param data      An 8-byte array of data to send
     */
    void sendResponse(byte ident, const byte data[]) {
        byte chksm = getChecksum(&ident, data);
        for(int i=0; i<8; i++) ser->write(data[i]);
        ser->write(chksm);
        ser->flush();
    }

    /**
     * Calculates the checksum for a given frame.
     *
     * @param ident     The ID of the frame
     * @param frame     An 8-byte array of data
     * @return          The checksum for the frame
     */
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
        // invert result
        return (~sum);
    }
};

#endif //TOYOTALININTERCEPTOR_LINCONTROLLER_H
