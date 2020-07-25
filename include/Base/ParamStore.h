#pragma once

#include "KeyValueStore.h"

#include "IPAddress.h"

class ParamStore : public KeyValueStore {
   public:
    ParamStore();
    ParamStore(const char* name);

    const String read(const String& key) const;
    const String read(const String& key, const char* defaults) const;
    int readInt(const String& key, int defaults = 0) const;
    float readFloat(const String& key) const;

    void write(const String& key, const String& value, const ValueType_t type);
    void write(const String& key, const String& value);
    
    void write(const String& key, int value);
    void write(const String& key, float value);
    void write(const String& key, IPAddress ip);

    void writeAsInt(const String& key, const String& value);
    void writeAsFloat(const String& key, const String& value);

    void erase(const String& key);

    const String asJson() const;

   protected:
    virtual void onUpdate(KeyValue*){};
    virtual void onAdd(KeyValue*){};
};