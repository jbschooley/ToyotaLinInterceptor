/**
 * Handles reading, forwarding, and responding to messages from the car.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_CARHANDLER_H
#define TOYOTALININTERCEPTOR_CARHANDLER_H

#include "PanelHandler.h"
#include "Handler.h"

class CarHandler : public Handler {
private:
    Menu* menu;
    Toggle* toggle;
    OffButton* offButton;
    PresetController* pc;
public:
    PanelHandler* panelHandler{};

    /**
     * @param ds            data store
     * @param mod           modifier
     * @param menu          the Menu object
     * @param toggle        the Toggle object
     * @param offButton     the OffButton object
     * @param pc            PresetController
     * @param ser           serial port
     */
    explicit CarHandler(DataStore* ds, Modifier* mod, Menu* menu, Toggle* toggle, OffButton* offButton, PresetController* pc, HardwareSerial* ser)
            : Handler(ds, mod, ser, new Logger("Car", false)) {
        this->menu = menu;
        this->toggle = toggle;
        this->offButton = offButton;
        this->pc = pc;
    }

    /**
     * When an ID is received, check if it is a data or request.
     *
     * If data, go to next state.
     *
     * If request, send response and go back to idle.
     *
     * If neither (unrecognized/ignored ID), go back to idle.
     */
    void onReceiveID() override {
        if (DataStore::idIsData(currID)) {
            // if data, go to next state
            state = WAIT_BYTE_0;
        } else if (DataStore::idIsRequest(currID)) {
            // if request, send response and go back to idle
            // TODO modify buttons at send?
            lin->sendResponse(currID, ds->getFrame(currID));
            reset();
        } else {
            // if neither, go back to idle
            reset();
        }
        // Request button status from panel if car sent 0x78
        // 0x78 is 3 messages before 0x39 for buffer; seems to be fastest and most reliable
        if (currID == 0x78) panelHandler->sendMsg(0x39);
    }

    /**
     * If climate status is received, process buttons to modify display
     * and forward to panel. Also, ensure the
     */
    void onReceiveData() override {
        // if car sent climate status, forward to panel
        if (currID == 0xb1) {
            //l->log(
            //        "received data: "
            //        + String(currID, HEX)
            //        + " - "
            //        + DataStore::frameToString(currFrame)
            //        + " - "
            //        + String(calculatedChecksum, HEX)
            //);
            menu->run();
            toggle->run();
            offButton->run();
            panelHandler->sendMsg(currID);
            // TODO modify buttons after climate status received
            pc->presetDelay();
        }
    }

};

#endif //TOYOTALININTERCEPTOR_CARHANDLER_H
