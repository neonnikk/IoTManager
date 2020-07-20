#pragma once

#include "Item.h"
#include "Value.h"
#include "ValueMap.h"
#include "ValueFilter.h"

enum class SensorType {
    TEMP
};

class Sensor : public Item,
               public Value,
               public ValueMap,
               public ValueFilter {
   public:
    Sensor(const String& name, const String& assign, const ValueType_t type) : Item{name, assign},
                                                                               Value{type},
                                                                               ValueMap{this},
                                                                               ValueFilter{this} {}
    virtual ~Sensor() = default;

    virtual const String onReadSensor() = 0;

   protected:
    const String onGetValue() override {
        return onReadSensor();
    }
};
