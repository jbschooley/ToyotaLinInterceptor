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

public:
    const uint8_t BUTTON_OFF[2] =           {0, 0x02};
    const uint8_t BUTTON_AUTO[2] =          {0, 0x08};
    const uint8_t BUTTON_SYNC[2] =          {3, 0x20};
    const uint8_t BUTTON_AC[2] =            {0, 0x80};
    const uint8_t BUTTON_FRONT_DEFROST[2] = {3, 0x80};
    const uint8_t BUTTON_REAR_DEFROST[2] =  {3, 0x40};
    const uint8_t BUTTON_ECO[2] =           {1, 0x40};
    const uint8_t BUTTON_FAN_DOWN[2] =      {1, 0x3d};
    const uint8_t BUTTON_FAN_UP[2] =        {1, 0x3c};
    const uint8_t BUTTON_MODE[2] =          {2, 0x1c};
    const uint8_t BUTTON_RECYCLE[2] =       {6, 0xc0};
    const uint8_t BUTTON_S_MODE[2] =        {1, 0x80};

    const uint8_t TEMP_DRIVER[2] =          {4, 0x01};
    const uint8_t TEMP_PASSENGER[2] =       {5, 0x01};

    explicit Modifier(DataStore* ds) {
        this->l = new Logger("Modifier", false);
        this->ds = ds;
    }

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

    void changeTemp(const uint8_t* zone, uint8_t delta) {
        setButtonsModified();
        ds->x39Mod[zone[0]] += zone[1] * delta;
    }

    void changeTempLog(const uint8_t* zone, uint8_t delta) {
        l->log("before: " + DataStore::frameToString(ds->x39));
        changeTemp(zone, delta);
        l->log("after:  " + DataStore::frameToString(ds->x39));
    }

    // TEST STUFF

    void testButtons() {
//        testDefrostAfter3s();
        testIncreaseTemp();
    }

    bool testDefrostChanged = false;
    void testDefrostAfter3s() {
        if (!testDefrostChanged && millis() > 3000) {
            testDefrostChanged = true;
            l->log("testDefrostAfter3s");
            l->log("before: " + DataStore::frameToString(ds->x39));
            pressButton(BUTTON_FRONT_DEFROST);
            l->log("after:  " + DataStore::frameToString(ds->x39Mod));
            l->log("checksum: " + String(LINUtils::getChecksum(reinterpret_cast<const uint8_t *>(0x39), ds->x39), HEX));
        }
    }

//    void testDefrost() {
//        l->log("before: " + DataStore::frameToString(ds->x39));
//        pressButton(BUTTON_FRONT_DEFROST);
//        l->log("after:  " + DataStore::frameToString(ds->x39));
//    }
//
//    void testTemp() {
//        changeTempLog(TEMP_DRIVER, -15);
//    }

    void testIncreaseTemp() {
        if (millis() > 2000) {
            l->log("testIncreaseTemp");
            l->log("before: " + DataStore::frameToString(ds->x39));
            changeTemp(TEMP_DRIVER, -15);
            l->log("after:  " + DataStore::frameToString(ds->x39));
        }
    }
};

#endif //TOYOTALININTERCEPTOR_MODIFIER_H
