#include "Value.h"
#include "Filter.h"

class ValueFilter : public Value {
   public:
    ValueFilter(ValueType_t type) : Value(type), _filter{NULL} {};

    ~ValueFilter() {
        if (_filter) {
            delete _filter;
        }
    }

    void setFilter(Filter* filter) {
        _filter = filter;
    }

   protected:
    virtual const String onGetValue() override {
        String value = Value::onGetValue();
        if (_filter) {
            _filter->process(value.toFloat());
            return String(_filter->get(), 2);
        }
        return value;
    }

   private:
    Filter* _filter;
};