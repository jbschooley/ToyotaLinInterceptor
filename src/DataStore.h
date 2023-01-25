//
// Created by Jacob on 1/23/2023.
//

#ifndef TOYOTALININTERCEPTOR_DATASTORE_H
#define TOYOTALININTERCEPTOR_DATASTORE_H

#include "Logger.h"

class DataStore {
private:
    Logger* l;
public:
    // data
//    uint8_t xB1[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // status
    uint8_t xB1[8] = {0x00, 0x06, 0x14, 0x00, 0x34, 0x37, 0x00, 0xc1}; // status
    uint8_t x32[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x10};
    uint8_t xF5[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00};
    // requests
    uint8_t x39[8] = {0x40, 0x00, 0x00, 0x00, 0x10, 0x90, 0x00, 0x00}; // buttons
    uint8_t xBA[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
    uint8_t x76[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // unused as nothing responds to this
    uint8_t x78[8] = {0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    DataStore() {
        this->l = new Logger("DataStore", false);
    }

    void saveFrame(uint8_t id, uint8_t* frame) {
        // write to data store
        switch (id) {
            case 0xB1:
//                l->log("SAVING B1 " + DataStore::frameToString(frame) + " to memloc " + String((int)this->getFrame(id), HEX));
                memcpy(this->xB1, frame, 8);
//                l->log("SAVED B1  " + DataStore::frameToString(this->xB1) + " to memloc " + String((int)this->xB1, HEX));
                break;
            case 0x32:
                memcpy(this->x32, frame, 8);
                break;
            case 0xF5:
                memcpy(this->xF5, frame, 8);
                break;
            case 0x39:
                memcpy(this->x39, frame, 8);
                break;
            case 0xBA:
                memcpy(this->xBA, frame, 8);
                break;
            case 0x76:
                memcpy(this->x76, frame, 8);
                break;
            case 0x78:
                memcpy(this->x78, frame, 8);
                break;
            default:
                break;
        }
    }

    uint8_t* getFrame(uint8_t id) {
        // read from data store
//        l->log("get datastore memloc " + String((int)this, HEX));
        uint8_t* frame = nullptr;
//        switch (id) {
//            case 0xB1:
//                return this->xB1;
//            case 0x32:
//                return this->x32;
//            case 0xF5:
//                return this->xF5;
//            case 0x39:
//                return this->x39;
//            case 0xBA:
//                return this->xBA;
//            case 0x76:
//                return this->x76;
//            case 0x78:
//                return this->x78;
//            default:
//                return nullptr;
//        }
        switch (id) {
            case 0xB1:
                frame = this->xB1;
                break;
            case 0x32:
                frame = this->x32;
                break;
            case 0xF5:
                frame = this->xF5;
                break;
            case 0x39:
                frame = this->x39;
                break;
            case 0xBA:
                frame = this->xBA;
                break;
            case 0x76:
                frame = this->x76;
                break;
            case 0x78:
                frame = this->x78;
                break;
        }
//        l->log("Returning " + String(id, HEX) + " from memloc " + String((int)frame, HEX));
        return frame;
    }

    static bool idIsData(uint8_t id) {
        return id == 0xb1 || id == 0x32 || id == 0xf5;
    }

    static bool idIsRequest(uint8_t id) {
        return id == 0x39 || id == 0xba || id == 0x78;
    }

    static String frameToString(uint8_t* frame) {
        String s = "";
        for (int i = 0; i < 8; i++) {
            s += String(frame[i], HEX);
            if (i < 7) s += " ";
        }
        return s;
    }
};

#endif //TOYOTALININTERCEPTOR_DATASTORE_H