#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>

#include "KeyValue.h"

class KeyValueStore {
   protected:
    std::vector<KeyValue*> _items;

   public:
    KeyValueStore();
    KeyValueStore(const String& jsonStr);
    virtual ~KeyValueStore();

    void forEach(KeyValueHandler func);
    void fromJson(const String& jsonStr);
    String& toJson(String& jsonStr) const;

   protected:
    KeyValue* find(const char* key) const;
    bool write(const char* key, const char* value, ValueType_t valueType = VT_STRING, KeyType_t keyType = KT_STATE);
    bool erase(const char* key);
    void clear();

    bool read(const char* key, String& value) const;
    bool read(const char* key, String& value, ValueType_t& type) const;

    void load(JsonObject& obj);

    void save(JsonObject& obj) const;

   protected:
    virtual void onUpdate(KeyValue*){};
    virtual void onAdd(KeyValue*){};
    virtual void onErase(KeyValue*){};
};
