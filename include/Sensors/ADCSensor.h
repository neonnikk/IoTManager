#pragma once

#include <Arduino.h>

#include "Base/Sensor.h"

class ADCSensor : public Sensor,
                  public PinAssigned,
                  public ValueMap {
   public:
    ADCSensor(const String& name, const String& assign) : Sensor{name, assign, VT_INT},
                                                          PinAssigned{this},
                                                          ValueMap{this} {
        pinMode(getPin(), INPUT);
    }

    const bool hasValue() override {
        return true;
    }

    const String onGetValue() override {
        int raw = onReadSensor().toInt();
        int mapped = mapValue(raw);
        return String(mapped, DEC);
    }

    const String onReadSensor() override {
        return String(analogRead(getPin()), DEC);
    }
};
