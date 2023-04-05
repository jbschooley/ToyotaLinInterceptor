/**
 * Holding the Eco button will toggle the preset on and off. The display
 * will toggle between the preset number and `OFF` when the button is held.
 *
 * I don't want it always cranking the heat to high when I'm running
 * errands, so I turn it off for that and turn it back on when I park for
 * the night or know it'll be snowing. I might be able to eliminate this if
 * I can figure out a way to detect when the remote starter is used to start
 * the car.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_TOGGLE_H
#define TOYOTALININTERCEPTOR_TOGGLE_H

class Toggle : public Button {
private:
    DataStore* ds;
    PresetController* pc;

public:

    /**
     * @param ds    data store
     * @param mod   modifier
     * @param pc    preset controller
     */
    Toggle(DataStore* ds, Modifier* mod, PresetController* pc)
            : Button(mod, mod->BUTTON_ECO, new Logger("Toggle", false)) {
        this->ds = ds;
        this->pc = pc;
    }

    void on() override {
        ds->presetEnabled = !ds->presetEnabled;
    }

    void hold() override {
        ds->xB1[4] = displayEnabled();
    }

    void off() override {
        // save preset to eeprom
        pc->setPresetFromMode();
        ds->savePresetToEEPROM();
    }

    uint8_t displayEnabled() {
        if (!ds->presetEnabled) {
            return 0x39;
        }
        return ds->presetMode + 0x3f;
    }

};

#endif //TOYOTALININTERCEPTOR_TOGGLE_H
