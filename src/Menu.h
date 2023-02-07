//
// Created by Jacob on 1/31/2023.
//

#ifndef TOYOTALININTERCEPTOR_MENU_H
#define TOYOTALININTERCEPTOR_MENU_H

class Menu {
private:
    Modifier* mod;
    DataStore* ds;
    PresetController* pc;
    Logger* l;

    enum TriggerState {
        TRIGGER_STATE_IDLE,
        TRIGGER_STATE_PRESSING,
        TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON,
        TRIGGER_STATE_ACTIVATED
    };

    TriggerState triggerState = TRIGGER_STATE_IDLE;
    unsigned long startedPressingTrigger = 0;

public:
    Menu(DataStore* ds, Modifier* mod, PresetController* pc) {
        this->ds = ds;
        this->mod = mod;
        this->pc = pc;
        this->l = new Logger("Menu", false);
    }

    // use eco button and driver temp dial to turn preset on/off
    void modifyPanel() {
        switch (triggerState) {
            case TRIGGER_STATE_IDLE:
                //l->log("TRIGGER_STATE_IDLE");
                if (mod->isButtonPressed(mod->BUTTON_ECO)) {
                    triggerState = TRIGGER_STATE_PRESSING;
                    startedPressingTrigger = millis();
                }
                break;
            case TRIGGER_STATE_PRESSING:
                //l->log("TRIGGER_STATE_PRESSING");
                if (mod->isButtonPressed(mod->BUTTON_ECO)) {
                    if (millis() - startedPressingTrigger > 1000) {
                        triggerState = TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON;
                        // TODO trigger on
                    } else {
                        break;
                    }
                } else {
                    triggerState = TRIGGER_STATE_IDLE;
                    break;
                }
            case TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON:
                //l->log("TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON");
                triggerState = TRIGGER_STATE_ACTIVATED;
                // release button for one frame to turn eco mode back off
                mod->releaseButton(mod->BUTTON_ECO);
            case TRIGGER_STATE_ACTIVATED:
                //l->log("TRIGGER_STATE_ACTIVATED");
                if (mod->isButtonPressed(mod->BUTTON_ECO)) {
                    // TODO trigger continue
                    // use temp dial to change preset
                    if (ds->x39[4] > 0x10) {
                        ds->x39[4] = 0x10;
                        incrementPreset();
                    } else if (ds->x39[4] < 0x10) {
                        ds->x39[4] = 0x10;
                        decrementPreset();
                    }
                    // send preset to display
                    ds->xB1[4] = displayPreset();
                } else {
                    triggerState = TRIGGER_STATE_IDLE;
                    // TODO trigger off
                    // save preset to eeprom
                    pc->setPresetFromMode();
                    ds->savePresetToEEPROM();
                }
        }
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
