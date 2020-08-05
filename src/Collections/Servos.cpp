#include "Collection/Servos.h"

Collection<BasicServo> servos;

template <>
BasicServo* Collection<BasicServo>::add(const String& name, const String& assign) {
    _list.push_back(new BasicServo{name, assign});
    return last();
}
