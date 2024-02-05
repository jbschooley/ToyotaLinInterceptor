/**
 * Handles application of the correct preset. New presets can be added here.
 * Remember to increment maxPresets in Menu.h.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_PRESETCONTROLLER_H
#define TOYOTALININTERCEPTOR_PRESETCONTROLLER_H

#include "Preset.h"
#include "PresetTesting.h"
#include "PresetMaxHeatDefrost.h"
#include "PresetMaxCool.h"

class PresetController {
private:
    Preset* preset;
    Modifier* mod;
    DataStore* ds;
    Logger* l;

public:

    /**
     * @param ds    data store
     * @param mod   modifier
     */
    PresetController(DataStore* ds, Modifier* mod) {
        this->ds = ds;
        this->mod = mod;
        this->l = new Logger("PresetController");
        this->preset = nullptr;
        setPresetFromMode();
    }

    /**
     * Set the preset determined by the value in presetMode.\n
     * 1 = MaxHeatDefrost\n
     * 2 = MaxCool\n
     * 3 = Testing\n
     */
    void setPresetFromMode() {
        l->log("Preset " + String(ds->presetMode) + " selected");
        switch (ds->presetMode) {
            case 1:
                preset = new PresetMaxHeatDefrost(mod);
                break;
            case 2:
                preset = new PresetMaxCool(mod);
                break;
            case 3:
                preset = new PresetTesting(mod);
                break;
            default:
                break;
        }
    }

    /**
     * Only activate the preset if it is enabled, and introduce a delay
     * to prevent the car from freaking out and disabling HVAC for a few
     * seconds (still working on that)
     */
    void presetDelay() {
        // TODO check for remote start here
        if (millis() > 3000 && ds->presetEnabled) {
            activatePreset();
        }
    }

    /**
     * Activate the preset if a preset has been set.
     */
    void activatePreset() {
        // TODO combine with presetDelay
        if (preset != nullptr) {
            preset->activate();
        }
    }

    /**
     * Cancel activation of the preset if it is in progress.
     */
    void cancelPreset() {
        if (preset != nullptr) {
            preset->cancel();
        }
    }
};

#endif //TOYOTALININTERCEPTOR_PRESETCONTROLLER_H
