#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>

#include "PrintMessage.h"
#include "Base/KeyValue.h"
#include "Utils/FileUtils.h"

class KeyValueStore {
   protected:
    std::vector<KeyValue*> _items;

   protected:
    virtual void onUpdate(KeyValue*){};
    virtual void onAdd(KeyValue*){};

   public:
    KeyValueStore();

    KeyValueStore(const char* content);

    virtual ~KeyValueStore();

    void erase(const String key);

    void clear();

    KeyValue* find(const String key) const;

    bool get(const String& key, String& value, ValueType_t& type) const;

    void writeAsInt(const String& key, const String& value);

    void writeAsFloat(const String& key, const String& value);

    void write(const String& key, int value);

    void write(const String& key, const String& value, ValueType_t type = VT_STRING, KeyType_t key_type = MT_MQTT_EVENT);

    const String read(const String& key) const;

    const String read(const String& key, const char* default_value) const;

    int readInt(const String& key) const;

    float readFloat(const String& key) const;

    void loadString(const char* content);

    void forEach(KeyValueHandler func);

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