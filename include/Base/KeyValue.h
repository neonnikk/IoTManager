#pragma once

#include "Value.h"

class KeyValue;
typedef std::function<void(KeyValue*)> KeyValueHandler;

class KeyValue {
   private:
    char* _key;
    char* _value;
    ValueType_t _type;
    KeyType_t _key_type;

   public:
    KeyValue(const char* key, const char* value, const ValueType_t type, const KeyType_t key_type = MT_MQTT_EVENT);

    ~KeyValue();

    const char* getKey() const;

    const char* getValue() const;

    ValueType_t getType() const;

    bool isEvent() const;

    bool isPublished() const;

    void setValue(const char* value, const ValueType_t type = VT_STRING);

    void setValue(const char* value);

    void setValueInt(int value);

    void setValueFloat(float value);

    const String asJson() const;
};
