#pragma once

#include <Arduino.h>

enum class LedStatus {
    OFF,
    ON,
    SLOW,
    FAST
};

class StatusLed {
   public:
    StatusLed();

    void init();
    void set(LedStatus status);

   private:
    void apply(LedStatus status);

   private:
    uint8_t _pin;
    bool _initialized;
    LedStatus _lastStatus;
};

extern StatusLed led;