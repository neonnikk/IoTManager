#include "Base/KeyValueStore.h"

#include <PrintMessage.h>
#include <Utils/FileUtils.h>

static const char* MODULE = "KeyValueStore";

KeyValue::KeyValue(const char* key, const char* value, ValueType_t type) : _type{type} {
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

const char* KeyValue::getKey() const {
    return _key;
}

const char* KeyValue::getValue() const {
    return _value;
}

void KeyValue::setValue(const char* value, const ValueType_t type) {
    _type = type;
    size_t new_len = strlen(value);
    if (strlen(_value) < new_len) {
        _value = (char*)realloc(_value, new_len);
    }
    strcpy(_value, value);
};

void KeyValue::setValue(const char* value) {
    setValue(value, VT_STRING);
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

KeyValue* KeyValueStore::findKey(const String key) {
    for (size_t i = 0; i < _items.size(); i++) {
        if (key.equals(_items.at(i)->getKey())) {
            return _items.at(i);
        }
    }
    return NULL;
}

void KeyValueStore::write(const String& key, int value) {
    write(key, String(value, DEC), VT_INT);
}

void KeyValueStore::writeInt(const String& key, const String& value) {
    write(key, value, VT_INT);
}

void KeyValueStore::write(const String& key, const String& value, ValueType_t type) {
    auto item = findKey(key);
    if (item) {
        item->setValue(value.c_str(), type);
    } else {
        _items.push_back(new KeyValue{key.c_str(), value.c_str(), type});
    }
}

const String KeyValueStore::read(const String& key) {
    auto item = findKey(key);
    return item ? item->getValue() : "";
}

bool KeyValueStore::read(const String& key, String& value, ValueType_t& type) {
    auto item = findKey(key);
    if (!item) {
        pm.error("not found:" + key);
        return false;
    }
    value = item->getValue();
    type = item->getType();
    return true;
}

int KeyValueStore::readInt(const String& key) {
    String buf = read(key);
    return buf.toInt();
}

float KeyValueStore::readFloat(const String& key) {
    String buf = read(key);
    return buf.toFloat();
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