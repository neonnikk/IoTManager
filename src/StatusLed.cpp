#include "StatusLed.h"

#include <Arduino.h>

#include "Config.h"

StatusLed led;

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

StatusLed::StatusLed() : _pin{LED_BUILTIN},
                         _initialized{false},
                         _lastStatus{LedStatus::OFF} {}

void StatusLed::init() {
    pinMode(_pin, OUTPUT);
    apply(_lastStatus);
}

void StatusLed::apply(LedStatus status) {
    switch (status) {
        case LedStatus::OFF:
#ifdef ESP8266
            noTone(_pin);
#endif
            digitalWrite(_pin, HIGH);
            break;
        case LedStatus::ON:
#ifdef ESP8266
            noTone(_pin);
#endif
            digitalWrite(_pin, LOW);
            break;
        case LedStatus::SLOW:
#ifdef ESP8266
            tone(_pin, 1);
#endif
            break;
        case LedStatus::FAST:
#ifdef ESP8266
            tone(_pin, 20);
#endif
            break;
        default:
            break;
    }
}

void StatusLed::set(LedStatus value) {
    if (!config.general()->isLedEnabled()) {
        return;
    }
    if (_initialized) {
        init();
        _initialized = true;
    }
    if (_lastStatus == value) {
        return;
    }
    apply(value);
    _lastStatus = value;
}