#pragma once

#include "Item.h"
#include "Value.h"
#include "ValueMap.h"
#include "ValueFilter.h"

enum class SensorType {
    TEMP
};

class Sensor : public Item,
               public ValueMap,
               public ValueFilter {
   public:
    Sensor(const String& name, const String& assign, const ValueType_t type) : Item{name, assign},
                                                                               ValueMap{this},
                                                                               ValueFilter{type} {}
    virtual ~Sensor() = default;

    virtual bool sensorReady() = 0;

    virtual float readSensor() = 0;

    const bool hasValue() override {
        if (sensorReady()) {
            auto readed = readSensor();
            setValue(readed);
            return true;
        }
        return false;
    }

   protected:
    const String onGetValue() override {
        return mapValue(_lastValue);
    }
};
