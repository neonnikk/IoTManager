#include "Collection/Sensors.h"

#include "Runtime.h"

namespace Sensors {

std::vector<BaseSensor*> _items;

BaseSensor* last() {
    return _items.at(_items.size() - 1);
}

BaseSensor* add(SensorType_t type, const String& name, const String& assign) {
    BaseSensor* item;
    switch (type) {
        case SENSOR_ADC:
            item = new AnalogSensor{name, assign};
            break;
        case SENSOR_DALLAS:
            item = new DallasSensor{name, assign};
            break;
        default:
            break;
    }

    _items.push_back(item);
    return last();
}

void update() {
    for (BaseSensor* item : _items) {
        if (item->hasValue()) {
            switch (item->getValueType()) {
                case VT_INT:
                    runtime.writeAsInt(item->getName(), item->getValue());
                    break;
                case VT_FLOAT:
                    runtime.writeAsFloat(item->getName(), item->getValue());
                    break;
                case VT_STRING:
                    runtime.write(item->getName(), item->getValue());
                    break;
            }
        }
    }
}

}  // namespace Sensors