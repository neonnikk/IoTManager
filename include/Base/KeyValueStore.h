#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include <functional>

#include "KeyValue.h"

class KeyValueStore {
   public:
    KeyValueStore();
    KeyValueStore(const char* jsonStr);
    virtual ~KeyValueStore();

    void fromJson(const char* jsonStr);
    void forEach(KeyValueHandler func);
    String& toJson(String& jsonStr) const;

   protected:
    void write(const char* key, const char* value, ValueType_t valueType = VT_STRING, KeyType_t keyType = KT_STATE);
    bool erase(const char* key);
    void clear();

    bool read(const char* key, String& value) const;
    bool read(const char* key, String& value, ValueType_t& type) const;

    void load(JsonObject& obj);

    void save(JsonObject& obj) const;

   private:
    KeyValue* find(const char* key) const;
    bool find(const char* key, KeyValue** item) const;
    bool find(const char* key, KeyValue** item, size_t& num) const;

   private:
    std::vector<KeyValue*> _items;
};
