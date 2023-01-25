//
// Created by Jacob on 1/25/2023.
//

#ifndef TOYOTALININTERCEPTOR_MODIFIER_H
#define TOYOTALININTERCEPTOR_MODIFIER_H

class Modifier {
private:
    Logger* l;
    DataStore* ds;

    enum Button {
        OFF = 0x0200000000000000,
        AUTO = 0x0800000000000000,
        DRIVER_TEMP = 0x01000000,
        PASSENGER_TEMP = 0x010000,
        SYNC = 0x2000000000,
        AC = 0x80000000000000,
        FRONT_DEFROST = 0x8000000000,
        REAR_DEFROST = 0x4000000000,
        ECO = 0x40000000000000,
        FAN_DOWN = 0x3d000000000000,
        FAN_UP = 0x3c000000000000,
        MODE = 0x1c000000000000,
        RECYCLE = 0xc000,
        S_MODE = 0x80000000000000,
    };

public:
    explicit Modifier(DataStore* ds) {
        this->l = new Logger("Modifier", true);
        this->ds = ds;
    }

    void pressButton(Button button) {
        uint8_t* frame = ds->x39;
        frame[0] |= button;
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
            pressButton(Button::FRONT_DEFROST);
            l->log("after:  " + DataStore::frameToString(ds->x39));
        }
    }

    void testIncreaseTemp() {
        if (millis() > 6000)
            pressButton(Button::DRIVER_TEMP);
    }
};

#endif //TOYOTALININTERCEPTOR_MODIFIER_H
