#include "StatusLed.h"

#include "Config.h"

StatusLed led;

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
            noTone(_pin);
            digitalWrite(_pin, HIGH);
            break;
        case LedStatus::ON:
            noTone(_pin);
            digitalWrite(_pin, LOW);
            break;
        case LedStatus::SLOW:
            tone(_pin, 1);
            break;
        case LedStatus::FAST:
            tone(_pin, 20);
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