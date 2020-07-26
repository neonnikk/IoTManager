#pragma once

#include "ValueMap.h"
#include "Filter.h"

class ValueFilter : public ValueMap {
   public:
    ValueFilter(ValueType_t type) : ValueMap{type} {
        _filter = NULL;
    }

    ~ValueFilter() {
        if (_filter) {
            delete _filter;
        }
    }

    void setFilter(Filter* filter) {
        if (_filter) {
            delete _filter;
        }
        _filter = filter;
    }

   protected:
    virtual const String onGetValue() override {
        String value = ValueMap::onGetValue();
        if (_filter) {
            if (_filter->process(value.toFloat())) {
                value = String(_filter->get(), 2);
            }
        }
        return value;
    }

   private:
    Filter* _filter;
};