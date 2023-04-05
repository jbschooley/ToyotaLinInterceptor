/**
 * A timer that can be used to measure how long it takes to process a message.
 * It's not used in the final code, but it's useful for debugging.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_TIMER_H
#define TOYOTALININTERCEPTOR_TIMER_H

class Timer {
private:
    unsigned long startMicros = 0;

public:
    Timer() {
        startMicros = micros();
    }

    /**
     * @return microseconds since timer started
     */
    unsigned long getMicros() const {
        return micros() - startMicros;
    }

    /**
     * @return milliseconds since timer started
     */
    unsigned long getMillis() const {
        return millis() - startMicros / 1000;
    }

    /**
     * reset timer to 0 microseconds
     */
    void reset() {
        startMicros = micros();
    }
};

#endif //TOYOTALININTERCEPTOR_TIMER_H
