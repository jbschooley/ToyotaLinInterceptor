//
// Created by Jacob on 1/28/2023.
//

#ifndef TOYOTALININTERCEPTOR_TIMER_H
#define TOYOTALININTERCEPTOR_TIMER_H

class Timer {
private:
    unsigned long startMicros = 0;

public:
    Timer() {
        startMicros = micros();
    }

    unsigned long getMicros() const {
        return micros() - startMicros;
    }

    unsigned long getMillis() const {
        return millis() - startMicros / 1000;
    }

    void reset() {
        startMicros = micros();
    }
};

#endif //TOYOTALININTERCEPTOR_TIMER_H
