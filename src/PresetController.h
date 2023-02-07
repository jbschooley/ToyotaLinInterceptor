//
// Created by Jacob on 2/6/2023.
//

#ifndef TOYOTALININTERCEPTOR_PRESETCONTROLLER_H
#define TOYOTALININTERCEPTOR_PRESETCONTROLLER_H

#include "Preset.h"
#include "PresetTesting.h"
#include "PresetMaxDefrost.h"

class PresetController {
private:
    Preset* preset;
    Modifier* mod;
    DataStore* ds;
    Logger* l;

public:
    PresetController(DataStore* ds, Modifier* mod) {
        this->ds = ds;
        this->mod = mod;
        this->l = new Logger("PresetController");
        setPresetFromMode();
    }

    void setPresetFromMode() {
        l->log("Preset " + String(ds->presetMode) + " selected");
        switch (ds->presetMode) {
            case 1:
                preset = new PresetMaxDefrost(mod);
                break;
            case 2:
                preset = new PresetTesting(mod);
                break;
            default:
                break;
        }
    }

    void presetAfter1s() {
        // TODO check for remote start here
        if (millis() > 1500) {
            activatePreset();
        }
    }

    void activatePreset() {
        if (preset != nullptr) {
            preset->activate();
        }
    }
};

#endif //TOYOTALININTERCEPTOR_PRESETCONTROLLER_H
