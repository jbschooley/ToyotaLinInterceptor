/**
 * Used to modify intercepted data before it's forwarded. This includes
 * modifying temperature, fan speed, and buttons pressed.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_MODIFIER_H
#define TOYOTALININTERCEPTOR_MODIFIER_H

class Modifier {
private:
    Logger* l;
    DataStore* ds;

    /**
     * Copy the current button state to the modified button state and set the
     * modified flag. This should be called before modifying the button state.
     */
    void setButtonsModified() {
        if (!ds->buttonsModifiedSinceLastSend) {
            ds->buttonsModifiedSinceLastSend = true;
            memcpy(ds->x39Mod, ds->x39, 8);
        }
    }

    /**
     * Calculate the temperature from the hex value provided in the 0xb1 frame.
     * @param hexTemp
     * @return
     */
    static uint8_t calcTemp(uint8_t hexTemp) {
        if (hexTemp == 0) {
            return TEMP_LO;
        } else if (hexTemp >= 0x66) {
            return hexTemp - 0x2a;
        } else {
            return hexTemp + 0x1f;
        }
    }

    /**
     * Calculate the delta between the current temp and the desired temp.
     * @param currTemp
     * @param newTemp
     * @return delta between the current temp and the desired temp, not exceeding |15|
     */
    static int getTempDelta(const int currTemp, const int newTemp) {
        return bounds(newTemp - currTemp, -15, 15);
    }

    /**
     * Calculate the delta between the current and desired fan speed.
     * @param currFan
     * @param newFan
     * @return  -1 if new speed is lower than current speed \n
     *          1 if new speed is higher than current speed \n
     *          0 if they match
     */
    static int getFanDelta(const int currFan, const int newFan) {
        return bounds(newFan - currFan, -1, 1);
    }

    /**
     * Ensure that values do not exceed a min and max.
     * @param val
     * @param min
     * @param max
     * @return  val if min <= val <= max \n
     *          min if val < min \n
     *          max if val > max
     */
    static int bounds(int val, int min, int max) {
        if (val < min) val = min;
        else if (val > max) val = max;
        return val;
    }

public:

    /**
     * @param ds data store
     */
    explicit Modifier(DataStore* ds) {
        this->l = new Logger("Modifier", true);
        this->ds = ds;
    }

    // buttons {byte, mask}
    const uint8_t BUTTON_OFF[2] =           {0, 0x02};
    const uint8_t BUTTON_AUTO[2] =          {0, 0x08};
    const uint8_t BUTTON_SYNC[2] =          {3, 0x20};
    const uint8_t BUTTON_AC[2] =            {1, 0x80};
    const uint8_t BUTTON_FRONT_DEFROST[2] = {3, 0x80};
    const uint8_t BUTTON_REAR_DEFROST[2] =  {3, 0x40};
    const uint8_t BUTTON_ECO[2] =           {1, 0x40};
    const uint8_t BUTTON_FAN_DOWN[2] =      {1, 0x3d};
    const uint8_t BUTTON_FAN_UP[2] =        {1, 0x3c};
    const uint8_t BUTTON_MODE[2] =          {2, 0x1c};
    const uint8_t BUTTON_RECYCLE[2] =       {6, 0xc0};
    const uint8_t BUTTON_S_MODE[2] =        {2, 0x80};
    const uint8_t ZONE_DRIVER[2] =          {4, 0x10};
    const uint8_t ZONE_PASSENGER[2] =       {5, 0x90};

    /**
     * Determine whether a button is pressed as of the panel's most recent
     * response to the 0x39 request
     * @param button
     * @return  true if button is pressed
     *          false if button not pressed
     */
    bool isButtonPressed(const uint8_t* button) {
        return bool(ds->x39[button[0]] & button[1]);
    }

    // car status
    bool statusFrontDefrost() { return bool(ds->xB1[2] & 0x08); }
    bool statusRearDefrost() { return bool(ds->xB1[3] & 0x40); }
    bool statusSync() { return bool(ds->xB1[3] & 0x20); }
    bool statusAC() { return bool(ds->xB1[7] & 0x01); }
    bool statusEco() { return bool(ds->xB1[0] & 0x08); }
    bool statusSMode() { return bool(ds->xB1[0] & 0x80); }
    int statusFanSpeed() { return ds->xB1[1] & 7; }
    int statusTemp(const uint8_t* zone) { return calcTemp(ds->xB1[zone[0]]); }

    // max and min temps
    enum Temp {
        TEMP_LO = 59,
        TEMP_HI = 86
    };

    /**
     * Change a button's status to pressed (if it is not already pressed)
     * @param button the button to press
     */
    void pressButton(const uint8_t* button) {
        setButtonsModified();
        ds->x39Mod[button[0]] |= button[1];
    }

    /**
     * Change a button's status to unpressed if it is being pressed
     * @param button the button to release
     */
    void releaseButton(const uint8_t* button) {
        setButtonsModified();
        ds->x39Mod[button[0]] &= ~button[1];
    }

    /**
     * Modify the temperature knob data to set the specified temperature. Because
     * the temperature can only be changed ±15 degrees per message, this may
     * need to be called multiple times to reach the desired temperature.
     * @param zone ZONE_DRIVER or ZONE_PASSENGER
     * @param newTemp the target temperature
     * @return  true if temperature matches target
     *          false if temperature has not reached the target
     */
    bool setTemp(const uint8_t* zone, const uint8_t newTemp) {
        int delta = getTempDelta(statusTemp(zone), newTemp);
        if (delta != 0) {
            setButtonsModified();
            ds->x39Mod[zone[0]] = zone[1] + delta;
            return true;
        }
        return false;
    }

    /**
     * Press the fan increase or decrease button to reach the specified
     * fan speed. If the desired speed is lower than the current speed,
     * the decrease button is pressed. If the desired speed is higher than
     * the current speed, the increase button is pressed. Because the fan
     * speed can only be increased or decreased one level per message, this
     * may need to be called multiple times to reach the desired speed.
     * @param newFanSpeed the target fan speed
     * @return  true if fan speed matches target
     *          false if fan speed has not reached the target
     */
    bool setFan(const uint8_t newFanSpeed) {
        int delta = getFanDelta(statusFanSpeed(), newFanSpeed);
        if (delta != 0) {
            setButtonsModified();
            delta > 0 ? pressButton(BUTTON_FAN_UP) : pressButton(BUTTON_FAN_DOWN);
            return true;
        }
        return false;
    }
};

#endif //TOYOTALININTERCEPTOR_MODIFIER_H
