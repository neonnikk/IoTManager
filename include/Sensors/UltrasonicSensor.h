#pragma once

#include <Arduino.h>

#include "Base/Sensor.h"
#include "Base/ValueMap.h"

class UltrasonicSensor : public Sensor,
                         public PinAssigned {
   public:
    UltrasonicSensor(const String& name, const String& assign) : Sensor{name, assign, VT_INT},
                                                                 PinAssigned{this} {
        _trig = getPin(1);
        _echo = getPin(2);
        pinMode(_trig, OUTPUT);
        pinMode(_echo, INPUT);
    }

    bool sensorReady() override {
        return true;
    }

    float readSensor() override {
        digitalWrite(_trig, LOW);
        delayMicroseconds(2);
        digitalWrite(_trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(_trig, LOW);

        // 3000 µs = 50cm // 30000 µs = 5 m
        return pulseIn(_echo, HIGH, 30000) / 29 / 2;
    }

   private:
    uint8_t _trig, _echo;
};
