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
    const uint8_t BUTTON_OFF[8] =           {0x02,    0,    0,    0,    0,    0,    0,    0};
    const uint8_t BUTTON_AUTO[8] =          {0x08,    0,    0,    0,    0,    0,    0,    0};
    const uint8_t BUTTON_SYNC[8] =          {   0,    0,    0, 0x20,    0,    0,    0,    0};
    const uint8_t BUTTON_AC[8] =            {0x80,    0,    0,    0,    0,    0,    0,    0};
    const uint8_t BUTTON_FRONT_DEFROST[8] = {   0,    0,    0, 0x80,    0,    0,    0,    0};
    const uint8_t BUTTON_REAR_DEFROST[8] =  {   0,    0,    0, 0x40,    0,    0,    0,    0};
    const uint8_t BUTTON_ECO[8] =           {   0, 0x40,    0,    0,    0,    0,    0,    0};
    const uint8_t BUTTON_FAN_DOWN[8] =      {   0, 0x3d,    0,    0,    0,    0,    0,    0};
    const uint8_t BUTTON_FAN_UP[8] =        {   0, 0x3c,    0,    0,    0,    0,    0,    0};
    const uint8_t BUTTON_MODE[8] =          {   0,    0, 0x1c,    0,    0,    0,    0,    0};
    const uint8_t BUTTON_RECYCLE[8] =       {   0,    0,    0,    0,    0,    0, 0xc0,    0};
    const uint8_t BUTTON_S_MODE[8] =        {   0, 0x80,    0,    0,    0,    0,    0,    0};

    const uint8_t TEMP_DRIVER[8] =          {   0,    0,    0,    0, 0x01,    0,    0,    0};
    const uint8_t TEMP_PASSENGER[8] =       {   0,    0,    0,    0,    0, 0x01,    0,    0};

    explicit Modifier(DataStore* ds) {
        this->l = new Logger("Modifier", true);
        this->ds = ds;
    }

    void pressButton(const uint8_t* button) {
        uint8_t* frame = ds->x39;
        for (int i = 0; i < 8; i++) {
            frame[i] |= button[i];
        }
    }

    void changeTemp(const uint8_t* zone, uint8_t delta) {
        uint8_t* frame = ds->x39;
        frame[4] += zone[4] * delta;
        frame[5] += zone[5] * delta;
    }

    void changeTempLog(const uint8_t* zone, uint8_t delta) {
        l->log("before: " + DataStore::frameToString(ds->x39));
        changeTemp(zone, delta);
        l->log("after:  " + DataStore::frameToString(ds->x39));
    }

    // TEST STUFF

    void testButtons() {
        testDefrostAfter3s();
        testIncreaseTemp();
    }

    bool testDefrostChanged = false;
    void testDefrostAfter3s() {
        if (!testDefrostChanged && millis() > 3000) {
            testDefrostChanged = true;
            l->log("testDefrostAfter3s");
            l->log("before: " + DataStore::frameToString(ds->x39));
            pressButton(BUTTON_FRONT_DEFROST);
            l->log("after:  " + DataStore::frameToString(ds->x39));
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
        if (millis() > 6000) {
            l->log("testIncreaseTemp");
            l->log("before: " + DataStore::frameToString(ds->x39));
            changeTemp(TEMP_DRIVER, 1);
            l->log("after:  " + DataStore::frameToString(ds->x39));
        }
    }
};

#endif //TOYOTALININTERCEPTOR_MODIFIER_H
