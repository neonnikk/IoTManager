#pragma once

#include <Arduino.h>
#include <IPAddress.h>

struct DeviceItem {
   private:
    char _id[32];
    char _name[32];
    IPAddress _ip;
    unsigned long _timestamp;

   public:
    DeviceItem(const String& id, const String& name, const String& ip) {
        strncpy(_id, id.c_str(), sizeof(_id));
        update(name, ip);
    }

    void update(const String& name, const String& ip) {
        strncpy(_name, name.c_str(), sizeof(_name));
        _ip.fromString(ip);
        _timestamp = millis();
    }

    const char* id() {
        return _id;
    }

    const char* name() {
        return _name;
    }

    IPAddress ip() {
        return _ip;
    }

    unsigned long timestamp() {
        return _timestamp;
    }

    unsigned long seenago() {
        unsigned long now = millis();
        return _timestamp > now ? 0 : (now - _timestamp) / 1000;
    }

    const String asString() {
        char buf[256];
        sprintf(buf, "%s;%s;%s;%lu", _id, _name, _ip.toString().c_str(), _timestamp);
        return buf;
    }
};
