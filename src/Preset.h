//
// Created by Jacob on 2/6/2023.
//

#ifndef TOYOTALININTERCEPTOR_PRESET_H
#define TOYOTALININTERCEPTOR_PRESET_H

#include "Modifier.h"

class Preset {
private:
    bool presetApplied = false;
    bool oneTimeButtonsPressed = false;

protected:
    Logger l = Logger("Preset", false);
    Modifier* mod;

public:
    explicit Preset(Modifier* mod) {
        this->mod = mod;
    }

    void activate() {
        l.log("Activating preset");
        //this->mod = mod;
        if (!presetApplied) {
            if (!oneTimeButtonsPressed) {
                l.log("Setting buttons");
                setButtons();
                oneTimeButtonsPressed = true;
            }

            // if temp and fan set, finish setting preset
            if (!setTempAndFan()) {
                presetApplied = true;
            }
        }
    }

    void cancel() {
        l.log("Cancelling preset");
        oneTimeButtonsPressed = true;
        presetApplied = true;
    }

    virtual void setButtons() {}
    virtual bool setTempAndFan() {
        return false;
    }
};

#endif //TOYOTALININTERCEPTOR_PRESET_H
