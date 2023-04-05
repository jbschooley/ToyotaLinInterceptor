/**
 * The menu used to change presets.
 *
 * Holding the S-Mode button will replace the driver's temperature display
 * with a preset menu. Turning the driver's temperature knob will cycle
 * between presets, and the preset will be saved when the button is
 * released.
 *
 * Presets are displayed as L1, L2, etc. on the display. (Why L#? Because I
 * had to pick from the text the panel was configured to display, and I
 * figured those were the least likely to be seen during normal operation. I
 * think the car can display up to L32.)
 *
 * If new presets are added to PresetController.h, increment the
 * `numPresets` constant.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_MENU_H
#define TOYOTALININTERCEPTOR_MENU_H

class Menu : public Button {
private:
    DataStore* ds;
    PresetController* pc;

public:

    /**
     * @param ds    data store
     * @param mod   modifier
     * @param pc    preset controller
     */
    Menu(DataStore* ds, Modifier* mod, PresetController* pc)
            : Button(mod, mod->BUTTON_S_MODE, new Logger("Menu", false)) {
        this->ds = ds;
        this->pc = pc;
    }

    /**
     * When the button is held, display the current preset and allow the
     * driver's temperature knob to change the preset.
     */
    void hold() override {
        // send preset to display
        ds->xB1[4] = displayPreset();
        // use temp dial to change preset
        if (ds->x39[4] > 0x10) {
            ds->x39[4] = 0x10; // don't change temp
            incrementPreset();
        } else if (ds->x39[4] < 0x10) {
            ds->x39[4] = 0x10; // don't change temp
            decrementPreset();
        }
    }

    /**
     * When the button is released, activate the selected preset and save
     * it to EEPROM.
     */
    void off() override {
        // save preset to eeprom
        pc->setPresetFromMode();
        ds->savePresetToEEPROM();
    }

    /**
     * Change this if more presets are added to PresetController.h
     */
    const uint8_t numPresets = 2;

    /**
     * Increment the preset mode. If the current mode is the last preset,
     * go back to the first preset.
     */
    void incrementPreset() {
        if (ds->presetMode > 0 && ds->presetMode < numPresets) {
            ds->presetMode += 1;
        } else {
            ds->presetMode = 1;
        }
    }

    /**
     * Decrement the preset mode. If the current mode is the first preset,
     * go back to the last preset.
     */
    void decrementPreset() {
        if (ds->presetMode <= 1) {
            ds->presetMode = numPresets;
        } else {
            ds->presetMode -= 1;
        }
    }

    /**
     * Convert the current preset to a displayable value. `0x39`
     * displays "OFF". `0x40` displays "L1", `0x41` displays "L2", etc.
     *
     * @return The value to display on the panel.
     */
    uint8_t displayPreset() {
        if (ds->presetMode == 0) {
            return 0x39;
        }
        return ds->presetMode + 0x3f;
    }
};

#endif //TOYOTALININTERCEPTOR_MENU_H
