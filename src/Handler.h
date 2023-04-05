/**
 * Handles state for reading and writing messages on the LIN bus and
 * ensures received messages are validated before being processed.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_HANDLER_H
#define TOYOTALININTERCEPTOR_HANDLER_H

#include <Arduino.h>

class Handler {
protected:
    enum State {
        IDLE,
        WAIT_ID,
        WAIT_BYTE_0,
        WAIT_BYTE_1,
        WAIT_BYTE_2,
        WAIT_BYTE_3,
        WAIT_BYTE_4,
        WAIT_BYTE_5,
        WAIT_BYTE_6,
        WAIT_BYTE_7,
        WAIT_CHECKSUM,
    };

    Logger* l;
    DataStore* ds;
    Modifier* mod;
    LINController* lin;
    State state = IDLE;

    // current frame to store received data
    uint8_t currID = 0;
    uint8_t currFrame[8]{};

public:

    /**
     * @param ds    data store
     * @param mod   modifier
     * @param ser   serial port
     * @param l     logger
     */
    explicit Handler(DataStore* ds, Modifier* mod, HardwareSerial* ser, Logger* l) {
        this->l = l;
        this->ds = ds;
        this->mod = mod;
        this->lin = new LINController(ser);
    }

    /**
     * Set state back to IDLE at the end of a frame or if corrupt data
     * is received.
     */
    void reset() {
        state = IDLE;
    }

    /**
     * Reads available bytes from the input stream and handles them.
     */
    void handleRead() {
        while (lin->available()) {
            uint8_t b = lin->read();
            handleByte(&b);
        }
    }

    /**
     * Handles a single byte from the input stream. If it is part of a frame,
     * it will be stored in currFrame.
     *
     * A frame follows the format:
     * 0x55 ID BYTE_0 BYTE_1 BYTE_2 BYTE_3 BYTE_4 BYTE_5 BYTE_6 BYTE_7 CHECKSUM
     *
     * Receipt of the 0x55 sync byte starts the frame. When the ID is received,
     * the onReceiveID() method is called. This is used to determine if the
     * message is a request or data. If it is a request, the response is sent
     * and the state is reset. If it is data, the state is changed to
     * WAIT_BYTE_0 and the data is stored in currFrame.
     *
     * When all bytes of a data frame have been received, the checksum is
     * validated. If it is valid, onReceiveData() is called to process the data.
     * If it is invalid, the frame is ignored. The state is reset to IDLE.
     *
     * The same code can be used for reading messages as both the Commander and
     * Responder. This is because when the Commander sends an ID to request
     * data, that ID is fed back into the input stream as the first byte of
     * the response.
     *
     * @param b the byte to handle
     */
    void handleByte(const uint8_t* b) {
        switch (state) {
            case IDLE:
                if (*b == 0x55) state = WAIT_ID;
                break;
            case WAIT_ID:
                currID = *b;
                onReceiveID();
                break;
            case WAIT_BYTE_0:
                currFrame[0] = *b;
                state = WAIT_BYTE_1;
                break;
            case WAIT_BYTE_1:
                currFrame[1] = *b;
                state = WAIT_BYTE_2;
                break;
            case WAIT_BYTE_2:
                currFrame[2] = *b;
                state = WAIT_BYTE_3;
                break;
            case WAIT_BYTE_3:
                currFrame[3] = *b;
                state = WAIT_BYTE_4;
                break;
            case WAIT_BYTE_4:
                currFrame[4] = *b;
                state = WAIT_BYTE_5;
                break;
            case WAIT_BYTE_5:
                currFrame[5] = *b;
                state = WAIT_BYTE_6;
                break;
            case WAIT_BYTE_6:
                currFrame[6] = *b;
                state = WAIT_BYTE_7;
                break;
            case WAIT_BYTE_7:
                currFrame[7] = *b;
                state = WAIT_CHECKSUM;
                break;
            case WAIT_CHECKSUM:
                uint8_t calculatedChecksum = LINController::getChecksum(&currID, currFrame);
                // if checksum is good, save frame to data store
                if (calculatedChecksum == *b) {
                    // this is reached after frame has been received and verified
                    // print frame to serial
                    //l->log(
                    //       "received data: "
                    //       + String(currID, HEX)
                    //       + " - "
                    //       + DataStore::frameToString(currFrame)
                    //       + " - "
                    //       + String(calculatedChecksum, HEX)
                    //);
                    ds->saveFrame(currID, currFrame);
                    onReceiveData();
                    //l->log(
                    //        "saved data: "
                    //        + String(currID, HEX)
                    //        + " - "
                    //        + DataStore::frameToString(ds->getFrame(currID))
                    //        + " to "
                    //        + String((int)ds->getFrame(currID), HEX)
                    //);
                } else {
                    // if checksum is bad, log error
                    l->log(
                            "Checksum error ID "
                            + String(currID, HEX)
                            + " - "
                            + DataStore::frameToString(currFrame)
                            + " - "
                            + String(calculatedChecksum, HEX)
                            + " - "
                            + String(*b, HEX)
                    );
                }
                reset();
                break;
        }
    }

    /**
     * Used to determine if the message is a request or data and to
     * send a response if it is a request.
     */
    virtual void onReceiveID() {}

    /**
     * Used to process data after it has been received and validated.
     */
    virtual void onReceiveData() {}

};


#endif //TOYOTALININTERCEPTOR_HANDLER_H
