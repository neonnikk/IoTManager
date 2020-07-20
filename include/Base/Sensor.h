#pragma once

#include "Item.h"

enum class SensorType {
    TEMP
};

class Sensor : public Item,
               public Value {
   public:
    Sensor(const String& name, const String& assign, const ValueType_t type) : Item{name, assign},
                                                                               Value{type} {}

    virtual const String onReadSensor() = 0;

   protected:
    const String onGetValue() override {
        return onReadSensor();
    }
};
