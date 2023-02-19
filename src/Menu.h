//
// Created by Jacob on 1/31/2023.
//

#ifndef TOYOTALININTERCEPTOR_MENU_H
#define TOYOTALININTERCEPTOR_MENU_H

class Menu : public Button {
private:
    DataStore* ds;
    PresetController* pc;

public:
    Menu(DataStore* ds, Modifier* mod, PresetController* pc)
            : Button(mod, mod->BUTTON_S_MODE, new Logger("Menu", false)) {
        this->ds = ds;
        this->pc = pc;
    }

    void hold() override {
        // send preset to display
        ds->xB1[4] = displayPreset();
        // use temp dial to change preset
        if (ds->x39[4] > 0x10) {
            ds->x39[4] = 0x10;
            incrementPreset();
        } else if (ds->x39[4] < 0x10) {
            ds->x39[4] = 0x10;
            decrementPreset();
        }
    }

    void off() override {
        // save preset to eeprom
        pc->setPresetFromMode();
        ds->savePresetToEEPROM();
    }

    const uint8_t numPresets = 2;
    void incrementPreset() {
        if (ds->presetMode < numPresets) {
            ds->presetMode += 1;
        } else {
            ds->presetMode = 0;
        }
    }

    void decrementPreset() {
        if (ds->presetMode-1 < 0) {
            ds->presetMode = numPresets;
        } else {
            ds->presetMode -= 1;
        }
    }

    uint8_t displayPreset() {
        if (ds->presetMode == 0) {
            return 0x39;
        }
        return ds->presetMode + 0x3f;
    }
};

#endif //TOYOTALININTERCEPTOR_MENU_H
