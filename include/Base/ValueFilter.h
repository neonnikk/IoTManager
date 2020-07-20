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

    void setFilter(Filter* filter) {
        if (_filter) {
            delete _filter;
        }
        _filter = filter;
    }

    float filterValue(const float value) {
        if (_filter) {
            return _filter->process(value);
        } else {
            return value;
        }
    }

   private:
    Value* _obj;
    Filter* _filter;
};