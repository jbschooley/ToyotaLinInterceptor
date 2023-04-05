/**
 * Stores data received from the car and the panel and handles
 * reading and writing to EEPROM.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_DATASTORE_H
#define TOYOTALININTERCEPTOR_DATASTORE_H

#include <EEPROM.h>
#include "Logger.h"

class DataStore {
private:
    Logger* l;

public:
    // data
    uint8_t xB1[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // status
    //uint8_t xB1[8] = {0x00, 0x06, 0x14, 0x00, 0x34, 0x37, 0x00, 0xc1}; // status
    uint8_t x32[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x10};
    uint8_t xF5[8] = {0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00};
    // requests
    uint8_t x39[8] = {0x40, 0x00, 0x00, 0x00, 0x10, 0x90, 0x00, 0x00}; // buttons
    uint8_t xBA[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
    uint8_t x76[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // unused as nothing responds to this
    uint8_t x78[8] = {0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    //bool doModifyButtons = false;
    uint8_t x39Mod[8] = {0x40, 0x00, 0x00, 0x00, 0x10, 0x90, 0x00, 0x00};

    bool buttonsModifiedSinceLastSend = false;

    // preset settings
    const int presetEnabledAddress = 0;
    const int presetModeAddress = 1;
    bool presetEnabled = true;
    uint8_t presetMode = 0;

    DataStore() {
        this->l = new Logger("DataStore", false);
        readPresetFromEEPROM();
    }

    /**
     * Save the provided frame to a data array.
     * @param id     ID of frame
     * @param frame  frame data
     */
    void saveFrame(uint8_t id, uint8_t* frame) {
        // write to data store
        switch (id) {
            case 0xB1:
                memcpy(this->xB1, frame, 8);
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

    /**
     * Get a frame from the data store. If buttons have been modified, return
     * the modified buttons frame only once.
     * @param id  ID of frame
     * @return    frame data
     */
    uint8_t* getFrame(uint8_t id) {
        // read from data store
        switch (id) {
            case 0xB1:
                return this->xB1;
            case 0x32:
                return this->x32;
            case 0xF5:
                return this->xF5;
            case 0x39:
                // only return modified buttons once
                if (buttonsModifiedSinceLastSend) {
                    buttonsModifiedSinceLastSend = false;
                    return this->x39Mod;
                } else {
                    return this->x39;
                }
            case 0xBA:
                return this->xBA;
            case 0x76:
                return this->x76;
            case 0x78:
                return this->x78;
            default:
                return nullptr;
        }
    }

    /**
     * Save the selected preset mode to EEPROM.
     */
    void savePresetToEEPROM() const {
        EEPROM.update(presetEnabledAddress, presetEnabled);
        EEPROM.update(presetModeAddress, presetMode);
    }

    /**
     * Read the selected preset mode from EEPROM.
     */
    void readPresetFromEEPROM() {
        presetEnabled = EEPROM.read(presetEnabledAddress);
        presetMode = EEPROM.read(presetModeAddress);
    }

    /**
     * @param id
     * @return true if the provided ID is a data frame
     */
    static bool idIsData(uint8_t id) {
        return id == 0xb1 || id == 0x32 || id == 0xf5;
    }

    /**
     * @param id
     * @return true if the provided ID is a request frame
     */
    static bool idIsRequest(uint8_t id) {
        return id == 0x39 || id == 0xba; // || id == 0x78;
    }

    /**
     * @param id
     * @return true if the provided ID is a data frame containing display data
     * to be sent to the panel
     */
    static bool idIsDataPanel(uint8_t id) {
        return id == 0xb1;
    }

    /**
     * @param id
     * @return true if the provided ID is a request frame to retrieve button
     * data from the panel
     */
    static bool idIsRequestPanel(uint8_t id) {
        return id == 0x39;
    }

    /**
     * Build a string representation of a frame.
     * @param frame
     * @return string containing all bytes in the frame
     */
    static String frameToString(const uint8_t* frame) {
        String s = "";
        for (int i = 0; i < 8; i++) {
            s += String(frame[i], HEX);
            if (i < 7) s += " ";
        }
        return s;
    }
};

#endif //TOYOTALININTERCEPTOR_DATASTORE_H
