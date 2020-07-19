#include "Base/KeyValueStore.h"

#include <PrintMessage.h>

#include <Utils/FileUtils.h>

static const char* MODULE = "KeyValueStore";

KeyValueStore::KeyValueStore(){};

KeyValueStore::KeyValueStore(const char* content) {
    _items.reserve(8);
    loadString(content);
}

KeyValueStore::~KeyValueStore() {
    _items.clear();
}

void KeyValueStore::loadString(const char* jsonStr) {
    DynamicJsonBuffer buf;
    JsonObject& root = buf.parse(jsonStr);
    for (JsonPair& p : root) {
        String key = p.key;
        String value{p.value.as<String>()};
        _items.push_back(new KeyValue{key.c_str(), value.c_str(), VT_STRING});
    }
}

void KeyValueStore::clear() {
    _items.clear();
}

void KeyValueStore::forEach(KeyValueHandler func) {
    for (auto item : _items) {
        func(item);
    }
}

const String KeyValueStore::asJson() {
    DynamicJsonBuffer json;
    JsonObject& root = json.createObject();
    for (auto item : _items) {
        switch (item->getType()) {
            case VT_STRING:
                root[item->getKey()] = item->getValue();
                break;
            case VT_FLOAT:
                root[item->getKey()] = atof(item->getValue());
                break;
            case VT_INT:
                root[item->getKey()] = atoi(item->getValue());
            default:
                break;
        }
    }
    String buf;
    root.printTo(buf);
    return buf;
}

void KeyValueStore::erase(const String key) {
    for (size_t i = 0; i < _items.size(); i++) {
        if (key.equals(_items.at(i)->getKey())) {
            _items.erase(_items.begin() + i);
            break;
        }
    }
}

KeyValue* KeyValueStore::find(const String key) const {
    for (size_t i = 0; i < _items.size(); i++) {
        if (key.equals(_items.at(i)->getKey())) {
            return _items.at(i);
        }
    }
    return NULL;
}

void KeyValueStore::write(const String& key, const String& value, ValueType_t type, KeyType_t key_type) {
    bool new_flag = false;
    auto item = find(key);
    if (!item) {
        new_flag = true;
        item = new KeyValue(key.c_str(), value.c_str(), type, key_type);
        _items.push_back(item);
    };
    item->setValue(value.c_str(), type);
    if (new_flag) {
        onAdd(item);
    } else {
        onUpdate(item);
    }
}

void KeyValueStore::write(const String& key, int value) {
    write(key, String(value, DEC), VT_INT);
}

void KeyValueStore::writeAsInt(const String& key, const String& value) {
    write(key, value, VT_INT);
}

void KeyValueStore::writeAsFloat(const String& key, const String& value) {
    write(key, value, VT_FLOAT);
}

const String KeyValueStore::read(const String& key) const {
    return read(key, "");
}

const String KeyValueStore::read(const String& key, const char* default_value) const {
    auto item = find(key);
    String value;
    if (item) {
        value = item->getValue();
    } else {
        pm.error("not found: " + key + ", default: " + default_value);
        value = default_value;
    }
    return value;
}

int KeyValueStore::readInt(const String& key) const {
    return read(key).toInt();
}

float KeyValueStore::readFloat(const String& key) const {
    return read(key).toFloat();
}

bool KeyValueStore::get(const String& key, String& value, ValueType_t& type) const {
    auto item = find(key);
    if (!item) {
        return false;
    }
    value = item->getValue();
    type = item->getType();
    return true;
}

KeyValueFile::KeyValueFile(const char* filename) {
    _filename = strdup(filename);
}

KeyValueFile::~KeyValueFile() {
    free(_filename);
}

const char* KeyValueFile::getFilename() {
    return _filename;
}

bool KeyValueFile::save() {
    bool res = false;
    String buf = asJson();
    auto file = LittleFS.open(_filename, "w");
    if (file) {
        file.print(buf);
        res = true;
    }
    file.close();
    return res;
}

bool KeyValueFile::load() {
    bool res = false;
    DynamicJsonBuffer buf;
    auto file = LittleFS.open(_filename, "r");
    if (file) {
        JsonObject& root = buf.parse(file);
        for (JsonPair& p : root) {
            String key = p.key;
            String value{p.value.as<String>()};
            _items.push_back(new KeyValue{key.c_str(), value.c_str(), VT_STRING});
        }
        res = true;
    }
    file.close();
    return res;
};