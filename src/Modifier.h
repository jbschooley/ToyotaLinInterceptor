//
// Created by Jacob on 1/25/2023.
//

#ifndef TOYOTALININTERCEPTOR_MODIFIER_H
#define TOYOTALININTERCEPTOR_MODIFIER_H

//#define BUTTON_OFF (uint8_t[8]){0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

class Modifier {
private:
    Logger* l;
    DataStore* ds;

    // buttons {byte, mask}
    const uint8_t BUTTON_OFF[2] =           {0, 0x02};
    const uint8_t BUTTON_AUTO[2] =          {0, 0x08};
    const uint8_t BUTTON_SYNC[2] =          {3, 0x20};
    const uint8_t BUTTON_AC[2] =            {1, 0x80};
    const uint8_t BUTTON_FRONT_DEFROST[2] = {3, 0x80};
    const uint8_t BUTTON_REAR_DEFROST[2] =  {3, 0x40};
    const uint8_t BUTTON_ECO[2] =           {1, 0x40};
    const uint8_t BUTTON_FAN_DOWN[2] =      {1, 0x3d};
    const uint8_t BUTTON_FAN_UP[2] =        {1, 0x3c};
    const uint8_t BUTTON_MODE[2] =          {2, 0x1c};
    const uint8_t BUTTON_RECYCLE[2] =       {6, 0xc0};
    const uint8_t BUTTON_S_MODE[2] =        {2, 0x80};
    const uint8_t ZONE_DRIVER[2] =          {4, 0x01};
    const uint8_t ZONE_PASSENGER[2] =       {5, 0x01};

    bool isButtonPressed(const uint8_t* button) {
        return bool(ds->x39[button[0]] & button[1]);
    }

    // car status
    bool statusFrontDefrost() { return bool(ds->xB1[2] & 0x08); }
    bool statusRearDefrost() { return bool(ds->xB1[3] & 0x40); }
    bool statusSync() { return bool(ds->xB1[3] & 0x20); }
    bool statusAC() { return bool(ds->xB1[7] & 0x01); }
    bool statusEco() { return bool(ds->xB1[0] & 0x08); }
    bool statusSMode() { return bool(ds->xB1[0] & 0x80); }
    int statusFanSpeed() { return ds->xB1[1] & 7; }
    int statusTemp(const uint8_t* zone) { return calcTemp(ds->xB1[zone[0]]); }

    // max and min temps
    enum Temp {
        TEMP_LO = 59,
        TEMP_HI = 86
    };

    void setButtonsModified() {
        if (!ds->buttonsModifiedSinceLastSend) {
            ds->buttonsModifiedSinceLastSend = true;
            memcpy(ds->x39Mod, ds->x39, 8);
        }
    }

    void pressButton(const uint8_t* button) {
        setButtonsModified();
        ds->x39Mod[button[0]] |= button[1];
    }

    void releaseButton(const uint8_t* button) {
        setButtonsModified();
        ds->x39Mod[button[0]] &= ~button[1];
    }

    void changeTemp(const uint8_t* zone, uint8_t delta) {
        setButtonsModified();
        ds->x39Mod[zone[0]] += zone[1] * delta;
    }

    bool setTemp(const uint8_t* zone, const uint8_t newTemp) {
        int delta = getTempDelta(statusTemp(zone), newTemp);
        if (delta != 0) {
            setButtonsModified();
            ds->x39Mod[zone[0]] += zone[1] * delta;
            return true;
        }
        return false;
    }

    bool setFan(const uint8_t newFanSpeed) {
        int delta = getFanDelta(statusFanSpeed(), newFanSpeed);
        if (delta != 0) {
            setButtonsModified();
            delta > 0 ? pressButton(BUTTON_FAN_UP) : pressButton(BUTTON_FAN_DOWN);
            return true;
        }
        return false;
    }

    void changeTempLog(const uint8_t* zone, uint8_t delta) {
        l->log("before: " + DataStore::frameToString(ds->x39));
        changeTemp(zone, delta);
        l->log("after:  " + DataStore::frameToString(ds->x39));
    }

    static uint8_t calcTemp(uint8_t hexTemp) {
        if (hexTemp == 0) {
            return TEMP_LO;
        } else if (hexTemp >= 0x66) {
            return hexTemp - 0x2a;
        } else {
            return hexTemp + 0x1f;
        }
    }

    static int getTempDelta(const int currTemp, const int newTemp) {
        return bounds(newTemp - currTemp, -15, 15);
    }

    static int getFanDelta(const int currFan, const int newFan) {
        return bounds(newFan - currFan, -1, 1);
    }

    static int bounds(int val, int min, int max) {
        if (val < min) val = min;
        else if (val > max) val = max;
        return val;
    }

public:
    explicit Modifier(DataStore* ds) {
        this->l = new Logger("Modifier", true);
        this->ds = ds;
    }

    void presetAfter1s() {
        // TODO check for remote start here
        if (millis() > 1500) {
            switch (ds->presetMode) {
                case 1:
                    presetMaxDefrost();
                    break;
                default:
                    break;
            }
            //presetTesting();
        }
    }

    bool presetApplied = false;
    bool oneTimeButtonsPressed = false;
    void presetMaxDefrost() {
        if (!presetApplied) {
            bool settingsChangedThisRound = false;

            if (!oneTimeButtonsPressed) {
                // defrost
                if (!statusFrontDefrost()) { pressButton(BUTTON_FRONT_DEFROST); }
                if (!statusRearDefrost()) { pressButton(BUTTON_REAR_DEFROST); }

                // sync on
                if (!statusSync()) { pressButton(BUTTON_SYNC); }

                // a/c on
                if (!statusAC()) { pressButton(BUTTON_AC); }

                // s-mode off
                if (statusSMode()) { pressButton(BUTTON_S_MODE); }

                // eco off
                if (statusEco()) { pressButton(BUTTON_ECO); }

                oneTimeButtonsPressed = true;
            }

            // temp
            settingsChangedThisRound |= setTemp(ZONE_DRIVER, TEMP_HI);

            // fan speed
            settingsChangedThisRound |= setFan(7);

            // keep changing if not complete
            if (!settingsChangedThisRound) {
                presetApplied = true;
            }
        }
    }

    // cause the engine keeps starting while I'm trying to test
    void presetTesting() {
        if (!presetApplied) {
            bool settingsChangedThisRound = false;

            if (!oneTimeButtonsPressed) {
                // defrost
                if (!statusFrontDefrost()) { pressButton(BUTTON_FRONT_DEFROST); }
                if (!statusRearDefrost()) { pressButton(BUTTON_REAR_DEFROST); }

                // sync on
                if (!statusSync()) { pressButton(BUTTON_SYNC); }

                // a/c on
                if (!statusAC()) { pressButton(BUTTON_AC); }

                // s-mode off
                if (statusSMode()) { pressButton(BUTTON_S_MODE); }

                // eco off
                if (statusEco()) { pressButton(BUTTON_ECO); }

                oneTimeButtonsPressed = true;
            }

            // temp
            settingsChangedThisRound |= setTemp(ZONE_DRIVER, 80);

            // fan speed
            settingsChangedThisRound |= setFan(2);

            // keep changing if not complete
            if (!settingsChangedThisRound) {
                presetApplied = true;
            }
        }
    }

    // use eco button and driver temp dial to turn preset on/off
    enum TriggerState {
        TRIGGER_STATE_IDLE,
        TRIGGER_STATE_PRESSING,
        TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON,
        TRIGGER_STATE_ACTIVATED
    };
    TriggerState triggerState = TRIGGER_STATE_IDLE;
    unsigned long startedPressingTrigger = 0;
    void modifyPanel() {
        switch (triggerState) {
            case TRIGGER_STATE_IDLE:
                if (isButtonPressed(BUTTON_ECO)) {
                    triggerState = TRIGGER_STATE_PRESSING;
                    startedPressingTrigger = millis();
                }
                break;
            case TRIGGER_STATE_PRESSING:
                if (isButtonPressed(BUTTON_ECO)) {
                    if (millis() - startedPressingTrigger > 1000) {
                        triggerState = TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON;
                        // TODO trigger on
                        //incrementPreset();
                    } else {
                        break;
                    }
                } else {
                    triggerState = TRIGGER_STATE_IDLE;
                    break;
                }
            case TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON:
                triggerState = TRIGGER_STATE_ACTIVATED;
                releaseButton(BUTTON_ECO);
            case TRIGGER_STATE_ACTIVATED:
                if (isButtonPressed(BUTTON_ECO)) {
                    // TODO trigger continue
                    // use temp dial to change preset
                    if (ds->x39[4] > 0x10) {
                        ds->x39[4] = 0x10;
                        incrementPreset();
                    } else if (ds->x39[4] < 0x10) {
                        ds->x39[4] = 0x10;
                        decrementPreset();
                    }
                    // send preset to display
                    ds->xB1[4] = getPresetDisplay();
                } else {
                    triggerState = TRIGGER_STATE_IDLE;
                    // TODO trigger off
                    // save preset to eeprom
                    ds->savePresetToEEPROM();
                }
        }
    }

    const uint8_t numPresets = 2;
    void incrementPreset() {
        if (ds->presetMode+1 < numPresets) {
            ds->presetMode += 1;
        } else {
            ds->presetMode = 0;
        }
    }

    void decrementPreset() {
        if (ds->presetMode-1 < 0) {
            ds->presetMode = numPresets-1;
        } else {
            ds->presetMode -= 1;
        }
    }

    uint8_t getPresetDisplay() {
        if (ds->presetMode == 0) {
            return 0x39;
        }
        return ds->presetMode + 0x3f;
    }
};

#endif //TOYOTALININTERCEPTOR_MODIFIER_H
