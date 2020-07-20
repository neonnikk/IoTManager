#pragma once

#include <Arduino.h>

#include "Base/Sensor.h"
#include "Base/ValueMap.h"

class ADCSensor : public Sensor,
                  public PinAssigned {
   public:
    ADCSensor(const String& name, const String& assign) : Sensor{name, assign, VT_INT},
                                                          PinAssigned{this} {
        pinMode(getPin(), INPUT);
    }

    bool sensorReady() override {
        return true;
    }

    float readSensor() override {
        return analogRead(getPin());
    }
};
