#pragma once

#include "Value.h"
#include "Mapper.h"

class ValueMap : public Value {
   public:
    ValueMap(ValueType_t type) : Value(type) {
        _mapper = NULL;
    }

    ~ValueMap() {
        if (_mapper) {
            delete _mapper;
        }
    }

    void setMap(Mapper* mapper) {
        if (_mapper) {
            delete _mapper;
        };
        _mapper = mapper;
    }

   protected:
    virtual const String onGetValue() override {
        String value = Value::onGetValue();
        if (_mapper) {
            return String(_mapper->mapValue(value.toInt()), DEC);
        }
        return value;
    }

   private:
    Mapper* _mapper;
};