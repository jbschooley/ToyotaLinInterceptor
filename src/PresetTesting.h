/**
 * This preset is just used for testing, because otherwise I get too hot
 * when sitting in the car playing with this thing.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_PRESETTESTING_H
#define TOYOTALININTERCEPTOR_PRESETTESTING_H

#include "Preset.h"

class PresetTesting : public Preset {
private:

    void setButtons() override {
        // defrost
        if (!mod->statusFrontDefrost()) { mod->pressButton(mod->BUTTON_FRONT_DEFROST); }
        if (!mod->statusRearDefrost()) { mod->pressButton(mod->BUTTON_REAR_DEFROST); }

        // sync on
        if (!mod->statusSync()) { mod->pressButton(mod->BUTTON_SYNC); }

        // a/c on
        if (!mod->statusAC()) { mod->pressButton(mod->BUTTON_AC); }

        // s-mode off
        if (mod->statusSMode()) { mod->pressButton(mod->BUTTON_S_MODE); }

        // eco off
        if (mod->statusEco()) { mod->pressButton(mod->BUTTON_ECO); }
    }

    bool setTempAndFan() override {
        bool settingsChangedThisRound = false;

        // temp
        settingsChangedThisRound |= mod->setTemp(mod->ZONE_DRIVER, 80);

        // fan speed
        settingsChangedThisRound |= mod->setFan(2);

        return settingsChangedThisRound;
    }

public:
    explicit PresetTesting(Modifier* mod) : Preset(mod) {}
};

#endif //TOYOTALININTERCEPTOR_PRESETTESTING_H
