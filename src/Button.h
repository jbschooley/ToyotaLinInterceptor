/**
 * Button is a base class for all buttons that can be pressed on the car's
 * climate control panel. It handles the logic for detecting when a button
 * is pressed, long-pressed, held, and released, and triggers the
 * appropriate actions.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_BUTTON_H
#define TOYOTALININTERCEPTOR_BUTTON_H

class Button {
private:
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

protected:
    Modifier* mod;
public:
    /**
     * @param mod   the modifier that will be used to press the button and check status
     * @param btn   the button to press
     * @param l     the logger to use
     */
    Button(Modifier* mod, const uint8_t* btn, Logger* l) {
        this->mod = mod;
        this->btn = btn;
        this->l = l;
    }

    virtual void press() {} // immediately when button is pressed
    virtual void on() {}    // long hold trigger
    virtual void hold() {}  // while button is held
    virtual void off() {}   // when button is released

    /**
     * Run the button logic. This should be called every time a button frame
     * is received from the panel.
     */
    void run() {
        switch (triggerState) {
            case TRIGGER_STATE_IDLE:
                l->log("TRIGGER_STATE_IDLE");
                if (mod->isButtonPressed(btn)) {
                    triggerState = TRIGGER_STATE_PRESSING;
                    startedPressingTrigger = millis();
                    press();
                }
                break;
            case TRIGGER_STATE_PRESSING:
                l->log("TRIGGER_STATE_PRESSING");
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
                l->log("TRIGGER_STATE_ACTIVATED_RELEASE_BUTTON");
                triggerState = TRIGGER_STATE_ACTIVATED;
                // release button for one frame to turn eco mode back off
                // TODO sometimes doesn't work
                mod->releaseButton(btn);
            case TRIGGER_STATE_ACTIVATED:
                l->log("TRIGGER_STATE_ACTIVATED");
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
