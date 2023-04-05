/**
 * Serves as the template for a preset and defines how the preset is applied.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_PRESET_H
#define TOYOTALININTERCEPTOR_PRESET_H

#include "Modifier.h"

class Preset {
private:
    /**
     * Whether the preset has been successfully applied
     */
    bool presetApplied = false;
    /**
     * Whether the buttons that only need to be pressed once during the
     * activation process have been pressed
     */
    bool oneTimeButtonsPressed = false;

protected:
    Logger l = Logger("Preset", false);
    Modifier* mod;

public:
    explicit Preset(Modifier* mod) {
        this->mod = mod;
    }

    /**
     * Activate the preset. If it has already been activated, do nothing.
     * To activate, the buttons that should only be pressed once are set to
     * pressed for one frame. The temperature and fan speed bits are modified
     * for each subsequent frame until the target temperature and fan speed
     * are reached.
     */
    void activate() {
        l.log("Activating preset");
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

    /**
     * Cancel activation of the preset if it is in progress.
     */
    void cancel() {
        l.log("Cancelling preset");
        oneTimeButtonsPressed = true;
        presetApplied = true;
    }

    /**
     * Run once during preset application.
     */
    virtual void setButtons() {}

    /**
     * Run repeatedly until preset application has completed.
     * @return
     */
    virtual bool setTempAndFan() {
        return false;
    }
};

#endif //TOYOTALININTERCEPTOR_PRESET_H
