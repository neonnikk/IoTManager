#include "Collection/Servos.h"

#include "Base/Value.h"
#ifdef ESP8266
#include <Servo.h>
#else
#include <ESP32Servo.h>
#endif

Servos servos;

Servos::Servos(){};

BasicServo* Servos::add(const String& name, const String& assign) {
    _items.push_back(new BasicServo{name, assign});

    return last();
}

BasicServo* Servos::last() {
    return _items.at(_items.size() - 1);
}

BasicServo* Servos::get(const String& name) {
    for (size_t i = 0; i < _items.size(); i++) {
        auto item = _items.at(i);
        if (name.equals(name)) {
            return item;
        }
    }
    return NULL;
}

size_t Servos::count() {
    return _items.size();
}
