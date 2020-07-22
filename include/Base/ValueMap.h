#pragma once

#include "Value.h"
#include "Mapper.h"

class ValueMap {
   public:
    ValueMap(Value* obj) : _obj{obj}, _mapper{NULL} {};

    ~ValueMap() {
        if (_mapper) {
            delete _mapper;
        }
    }

    void setMap(long in_min, long in_max, long out_min, long out_max) {
        if (_mapper) {
            delete _mapper;
        }
        _mapper = new Mapper{in_min, in_max, out_min, out_max};
    }

    String mapValue(const String& value) {
        return String(mapValue(value.toInt()), DEC);
    }

    int mapValue(const int value) {
        if (_mapper) {
            return _mapper->mapValue(value);
        } else {
            return value;
        }
    }

   private:
    Value* _obj;
    Mapper* _mapper;
};