#include "Collection/Sensors.h"

#include "Runtime.h"

static const char* MODULE = "Sensors";

namespace Sensors {
std::vector<Sensor*> _items;

Sensor* last() {
    return _items.at(_items.size() - 1);
}

Sensor* add(SensorType_t type, const String& name, const String& assign) {
    Sensor* item = NULL;
    switch (type) {
        case SensorType_t::ADC:
            item = new ADCSensor{name, assign};
            break;
        case SensorType_t::DALLAS:
            if (!onewire.attached()) {
                pm.error("attach bus first");
                break;
            }
            item = new DallasSensor{name, assign};
            break;
        default:
            break;
    }
    if (item) {
        _items.push_back(item);
        return last();
    }
    return NULL;
}

void update() {
    for (Sensor* item : _items) {
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