//
// Created by Jacob on 2/19/2023.
//

#ifndef TOYOTALININTERCEPTOR_BUTTON_H
#define TOYOTALININTERCEPTOR_BUTTON_H

class Button {
private:
    Modifier* mod;
    Logger* l;
    const uint8_t* btn;

    enum TriggerState {
        TRIGGER_STATE_IDLE,
        TRIGGER_STATE_PRESSING,
        TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON,
        TRIGGER_STATE_ACTIVATED
    };

    TriggerState triggerState = TRIGGER_STATE_IDLE;
    unsigned long startedPressingTrigger = 0;

public:
    Button(Modifier* mod, const uint8_t* btn, Logger* l) {
        this->mod = mod;
        this->btn = btn;
        this->l = l;
    }

    virtual void on() {}
    virtual void hold() {}
    virtual void off() {}

    void run() {
        switch (triggerState) {
            case TRIGGER_STATE_IDLE:
                //l->log("TRIGGER_STATE_IDLE");
                if (mod->isButtonPressed(btn)) {
                    triggerState = TRIGGER_STATE_PRESSING;
                    startedPressingTrigger = millis();
                }
                break;
            case TRIGGER_STATE_PRESSING:
                //l->log("TRIGGER_STATE_PRESSING");
                if (mod->isButtonPressed(btn)) {
                    if (millis() - startedPressingTrigger > 400) {
                        triggerState = TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON;
                        on();
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
                mod->releaseButton(btn);
            case TRIGGER_STATE_ACTIVATED:
                //l->log("TRIGGER_STATE_ACTIVATED");
                if (mod->isButtonPressed(btn)) {
                    hold();
                } else {
                    triggerState = TRIGGER_STATE_IDLE;
                    off();
                }
        }
    }
};

#endif //TOYOTALININTERCEPTOR_BUTTON_H
