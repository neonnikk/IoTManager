#pragma once

#include "Utils/TimeUtils.h"

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
                                                                               ValueFilter{type},
                                                                               _lastRead{0},
                                                                               _readInterval{0} {}
    virtual ~Sensor() = default;

    virtual bool sensorReady() = 0;

    virtual float readSensor() = 0;

    const bool hasValue() override {
        if (!millis_since(_lastRead) > _readInterval) {
            return false;
        }
        _lastRead = millis();
        if (sensorReady()) {
            auto readed = readSensor();
            setValue(readed);
            return true;
        }
        return false;
    }

    void setReadInterval(unsigned long time_ms) {
        _readInterval = time_ms;
    }

   protected:
    const String onGetValue() override {
        return mapValue(_lastValue);
    }

   private:
    unsigned long _lastRead, _readInterval;
};
