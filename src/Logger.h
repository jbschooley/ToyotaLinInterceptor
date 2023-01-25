//
// Created by Jacob on 1/24/2023.
//

#ifndef TOYOTALININTERCEPTOR_LOGGER_H
#define TOYOTALININTERCEPTOR_LOGGER_H

class Logger {
private:
    HardwareSerial* ser;
    String name;
    bool enabled;

public:
    explicit Logger(const String& name, bool enabled=true) {
        this->ser = &Serial;
        this->name = name;
        this->enabled = enabled;
    }

    void log(const String& msg) {
        if (enabled) {
            ser->print(millis());
            ser->print(" | ");
            ser->print(name);
            ser->print(" | ");
            ser->print(msg);
            ser->println();
        }
    }
};

#endif //TOYOTALININTERCEPTOR_LOGGER_H
