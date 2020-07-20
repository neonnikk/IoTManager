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

    virtual bool sensorReady() = 0;

    virtual float readSensor() = 0;

    const bool hasValue() override {
        if (sensorReady()) {
            auto readed = readSensor();
            return filterValue(readed);
        }
        return false;
    }

   protected:
    const String onGetValue() override {
        auto filtered = getFilteredValue();
        auto mapped = mapValue(filtered);
        return String(mapped, 2);
    }
};
