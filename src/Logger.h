/**
 * A debug logger that can be used to log messages to the USB serial port. Loggers are disabled unless necessary for debugging because log output is incredibly slow and causes the Arduino to miss messages on the LIN bus.
 *
 * @author Jacob Schooley
 */

#ifndef TOYOTALININTERCEPTOR_LOGGER_H
#define TOYOTALININTERCEPTOR_LOGGER_H

class Logger {
private:
    HardwareSerial* ser;
    String name;
    bool enabled;

public:
    /**
     * Create a new logger with the given name.
     * @param ser serial port to log to
     * @param name name of the logger
     * @param enabled whether or not the logger is enabled
     */
    explicit Logger(const String& name, bool enabled=true) {
        this->ser = &Serial;
        this->name = name;
        this->enabled = enabled;
    }

    /**
     * Log a message to the serial port. \n\n
     * Log format: `millis() | name | msg`
     * @param msg message to log, as an Arduino String
     */
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
