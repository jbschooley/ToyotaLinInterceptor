//
// Created by Jacob on 1/31/2023.
//

#ifndef TOYOTALININTERCEPTOR_OFFBUTTON_H
#define TOYOTALININTERCEPTOR_OFFBUTTON_H

class OffButton : public Button {
private:
    DataStore* ds;
    PresetController* pc;

public:
    OffButton(DataStore* ds, Modifier* mod, PresetController* pc)
            : Button(mod, mod->BUTTON_OFF, new Logger("OffButton", false)) {
        this->ds = ds;
        this->pc = pc;
    }

    void press() override {
        // cancel preset
        pc->cancelPreset();
        // turn off rear defrost
        if (mod->statusRearDefrost()) {
            mod->pressButton(mod->BUTTON_REAR_DEFROST);
        }
    }
};

#endif //TOYOTALININTERCEPTOR_OFFBUTTON_H
