#pragma once

#include <Arduino.h>

#include "Item.h"
#include "Mapper.h"

enum ValueType_t {
    VT_STRING,
    VT_FLOAT,
    VT_INT
};

class Value {
   public:
    Value(ValueType_t type) : _type{type},
                              _lastValue{"0"},
                              _lastTime{0} {}

    void setValue(int value) {
        setValue(String{value, DEC});
    }

    void setValue(const String value) {
        if (!value.equals(_lastValue)) {
            onValueChange(_lastValue, value);
            _lastValue = value;
        }
        onValueUpdate(value);
        _lastTime = millis();
    }

    const String getValue() {
        return onGetValue();
    }

    const ValueType_t getValueType() {
        return _type;
    }

    virtual const bool hasValue() {
        return false;
    }

   protected:
    virtual void onValueUpdate(const String& value) {}

    virtual void onValueChange(const String& prev, const String& value) {}

    virtual const String onGetValue() {
        return _lastValue;
    }

   private:
    ValueType_t _type;
    String _lastValue;
    unsigned long _lastTime;
};
