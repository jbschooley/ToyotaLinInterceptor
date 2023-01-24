//
// Created by Jacob on 1/23/2023.
//

#ifndef TOYOTALININTERCEPTOR_DATASTORE_H
#define TOYOTALININTERCEPTOR_DATASTORE_H

class DataStore {
public:
    // data
    uint8_t xB1[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // status
    uint8_t x32[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x10};
    uint8_t xF5[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00};
    // requests
    uint8_t x39[8] = {0x40, 0x00, 0x00, 0x00, 0x10, 0x90, 0x00, 0x00}; // buttons
    uint8_t xBA[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
    uint8_t x76[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // unused as nothing responds to this
    uint8_t x78[8] = {0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    DataStore() {}

    void saveFrame(uint8_t id, uint8_t* frame) {
        // write to data store
        switch (id) {
            case 0xB1:
                memcpy(frame, this->xB1, 8);
                // TODO modify buttons depending on status
                break;
            case 0x32:
                memcpy(frame, this->x32, 8);
                break;
            case 0xF5:
                memcpy(frame, this->xF5, 8);
                break;
            case 0x39:
                memcpy(frame, this->x39, 8);
                // TODO monitor button presses for defrost enable
                // if button pressed and is disabled in status, store enable in eeprom
                // if button pressed and is enabled in status, store disable in eeprom
                break;
            case 0xBA:
                memcpy(frame, this->xBA, 8);
                break;
            case 0x76:
                memcpy(frame, this->x76, 8);
                break;
            case 0x78:
                memcpy(frame, this->x78, 8);
                break;
            default:
                break;
        }
    }

    static bool idIsData(uint8_t id) {
        return id == 0xb1 || id == 0x32 || id == 0xf5;
    }

    static bool idIsRequest(uint8_t id) {
        return id == 0x39 || id == 0xba || id == 0x76;
    }

    static String frameToString(uint8_t* frame) {
        String s = "";
        for (int i = 0; i < 8; i++) {
            s += String(frame[i], HEX);
            s += " ";
        }
        return s;
    }
};

#endif //TOYOTALININTERCEPTOR_DATASTORE_H
