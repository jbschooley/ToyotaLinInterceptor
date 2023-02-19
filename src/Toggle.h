//
// Created by Jacob on 1/31/2023.
//

#ifndef TOYOTALININTERCEPTOR_TOGGLE_H
#define TOYOTALININTERCEPTOR_TOGGLE_H

class Toggle : public Button {
private:
    DataStore* ds;
    PresetController* pc;

public:
    Toggle(DataStore* ds, Modifier* mod, PresetController* pc)
            : Button(mod, mod->BUTTON_ECO, new Logger("Toggle", false)) {
        this->ds = ds;
        this->pc = pc;
    }

    void on() override {
        if (ds->presetMode == 0) ds->presetMode = 1;
        else ds->presetMode = 0;
    }

    void hold() override {
        ds->xB1[4] = displayPreset();
    }

    void off() override {
        // save preset to eeprom
        pc->setPresetFromMode();
        ds->savePresetToEEPROM();
    }

    uint8_t displayPreset() {
        if (ds->presetMode == 0) {
            return 0x39;
        }
        return ds->presetMode + 0x3f;
    }

};

#endif //TOYOTALININTERCEPTOR_TOGGLE_H
