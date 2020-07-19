#include "Collection/Servos.h"

#include "Base/Value.h"
#ifdef ESP8266
#include <Servo.h>
#else
#include <ESP32Servo.h>
#endif

Servos servos;

Servos::Servos(){};

BasicServo* Servos::add(const String& name, const String& pin, const String& value,
                       const String& in_min, const String& in_max, const String& out_min, const String& out_max) {
    BasicServo item{name, pin};
    item.setMap(in_min.toInt(), in_max.toInt(), out_min.toInt(), out_max.toInt());
    item.setValue(value);

    return last();
}

BasicServo* Servos::last() {
    return &_items.at(_items.size() - 1);
}

BasicServo* Servos::get(const String& name) {
    for (size_t i = 0; i < _items.size(); i++) {
        auto* item = &_items.at(i);
        if (name.equals(name)) {
            return item;
        }
    }
    return nullptr;
}

size_t Servos::count() {
    return _items.size();
}
