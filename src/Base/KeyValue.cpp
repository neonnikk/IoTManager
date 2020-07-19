#include "Base/KeyValue.h"

KeyValue::KeyValue(const char* key, const char* value,
                   const ValueType_t type, const KeyType_t key_type) : _type{type}, _key_type{key_type} {
    _key = strdup(key);
    _value = strdup(value);
}

KeyValue::~KeyValue() {
    free(_key);
    free(_value);
}

ValueType_t KeyValue::getType() const {
    return _type;
}

// TODO bitmask
bool KeyValue::isEvent() const {
    return _key_type == KT_EVENT || _key_type == MT_MQTT_EVENT;
}

bool KeyValue::isPublished() const {
    return _key_type == KT_MQTT || _key_type == MT_MQTT_EVENT;
}

const char* KeyValue::getKey() const {
    return _key;
}

const char* KeyValue::getValue() const {
    return _value;
}

void KeyValue::setValue(const char* value, const ValueType_t type) {
    _type = type;

    size_t new_len = strlen(value) + 1;
    if (strlen(_value) < new_len) {
        free(_value);
        _value = (char*)malloc(new_len);
    }
    strcpy(_value, value);
};

void KeyValue::setValueInt(int value) {
    char buf[16];
    utoa(value, buf, DEC);
    setValue(buf, VT_INT);
};

void KeyValue::setValueFloat(float value) {
    char buf[16];
    setValue(dtostrf(value, 4, 2, buf), VT_FLOAT);
};

const String KeyValue::asJson() const {
    String res = "{\"status\":";
    if (_type == VT_STRING) {
        res += "\"";
    }
    res += _value;
    if (_type == VT_STRING) {
        res += "\"";
    }
    res += "}";
    return res;
}