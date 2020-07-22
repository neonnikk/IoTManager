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

    void setMap(Mapper* mapper) {
        if (_mapper) {
            delete _mapper;
        };
        _mapper = mapper;
    }

    String mapValue(const String& value) {
        return String(mapValue(value.toInt()), DEC);
    }

    int mapValue(const int value) {
        int res = value;
        if (_mapper) {
            res = _mapper->mapValue(value);
        }
        return res;
    }

   private:
    Value* _obj;
    Mapper* _mapper;
};