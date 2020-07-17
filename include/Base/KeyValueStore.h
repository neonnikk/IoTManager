#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>

#include <PrintMessage.h>
#include <Utils/FileUtils.h>
#include "Value.h"

class KeyValue {
   private:
    char* _key;
    char* _value;
    ValueType_t _type;

   public:
    KeyValue(const char* key, const char* value, ValueType_t type);

    ~KeyValue();
    ValueType_t getType() const;
    const char* getKey() const;
    const char* getValue() const;

    void setValue(const char* value, const ValueType_t type);
    void setValue(const char* value);
    void setValueInt(int value);
    void setValueFloat(float value);
};

typedef std::function<void(KeyValue*)> KeyValueHandler;

class KeyValueStore {
   protected:
    std::vector<KeyValue*> _items;

   public:
    KeyValueStore();
    KeyValueStore(const char* content);
    ~KeyValueStore();

    void loadString(const char* content);

    void erase(const String key);

    KeyValue* findKey(const String key);

    void writeInt(const String& key, const String& value);
    void write(const String& key, int value);

    void write(const String& key, const String& value, ValueType_t type = VT_STRING);

    const String read(const String& key);

    bool read(const String& key, String& value, ValueType_t& type);

    int readInt(const String& key);

    float readFloat(const String& key);

    void forEach(KeyValueHandler func);
    void clear();
    const String asJson();
};

class KeyValueFile : public KeyValueStore {
   private:
    char* _filename;

   public:
    KeyValueFile(const char* filename);
    ~KeyValueFile();
    const char* getFilename();
    bool save();
    bool load();
};