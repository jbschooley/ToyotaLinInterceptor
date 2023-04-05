/**
 * Sends data and requests and handles responses from the climate control panel.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_PANELHANDLER_H
#define TOYOTALININTERCEPTOR_PANELHANDLER_H

#include "Handler.h"

class PanelHandler : public Handler {
public:

    /**
     * @param ds    data store
     * @param mod   modifier
     * @param ser   serial port
     */
    explicit PanelHandler(DataStore* ds, Modifier* mod, HardwareSerial* ser)
            : Handler(ds, mod, ser, new Logger("Panel", false)) {}

    /**
     * Sends a message to the panel. Used by CarHandler to trigger a display
     * update or request button status.
     * @param id ID of message to send. If data, sends data from DataStore.
     *           If request, sends ID.
     */
    void sendMsg(uint8_t id) {
        // send message
        if (DataStore::idIsDataPanel(id)) {
            // if data, send data
            l->log(
                    "send data: "
                    + String(id, HEX)
                    + " - "
                    + DataStore::frameToString(ds->getFrame(id))
            );
            lin->sendFrame(id, ds->getFrame(id));
        } else if (DataStore::idIsRequestPanel(id)) {
            // if request, send id
            l->log(
                    "send request: "
                    + String(id, HEX)
            );
            lin->sendRequest(id);
        }
    }

    /**
     * When an ID is received, check if it is a known request. If so, begin
     * listening for response data. If not, go back to idle.
     */
    void onReceiveID() override {
        if (DataStore::idIsRequest(currID)) {
            // if expecting response, go to next state
            state = WAIT_BYTE_0;
        } else {
            // otherwise, go back to idle
            reset();
        }
    }

    void onReceiveData() override {
        // TODO modify buttons at receive?
    }

};

#endif //TOYOTALININTERCEPTOR_PANELHANDLER_H
