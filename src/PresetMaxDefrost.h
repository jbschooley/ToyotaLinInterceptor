//
// Created by Jacob on 2/6/2023.
//

#ifndef TOYOTALININTERCEPTOR_PRESETMAXDEFROST_H
#define TOYOTALININTERCEPTOR_PRESETMAXDEFROST_H

#include "Preset.h"

class PresetMaxDefrost : public Preset {
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
        l.log("Setting temp and fan");
        bool settingsChangedThisRound = false;

        // temp
        settingsChangedThisRound |= mod->setTemp(mod->ZONE_DRIVER, mod->TEMP_HI);

        // fan speed
        settingsChangedThisRound |= mod->setFan(7);

        return settingsChangedThisRound;
    }

public:
    explicit PresetMaxDefrost(Modifier* mod) : Preset(mod) {}
};

#endif //TOYOTALININTERCEPTOR_PRESETMAXDEFROST_H
