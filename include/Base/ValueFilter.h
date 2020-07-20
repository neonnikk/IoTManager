#include "Value.h"
#include "Filter.h"

class ValueFilter {
   public:
    ValueFilter(Value* obj) : _obj{obj},
                              _filter{NULL} {};

    ~ValueFilter() {
        if (_filter) {
            delete _filter;
        }
    }

    bool filterValue(const float value) {
        return _filter ? _filter->process(value) : false;
    }

    float getFilteredValue() {
        return _filter ? _filter->get() : 0;
    }

    void setFilter(Filter* filter) {
        if (_filter) {
            delete _filter;
        }
        _filter = filter;
    }

   private:
    Value* _obj;
    Filter* _filter;
};