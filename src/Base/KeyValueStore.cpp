#include "Base/KeyValueStore.h"

KeyValueStore::KeyValueStore() {
    _items.reserve(8);
};

KeyValueStore::KeyValueStore(const char* jsonStr) : KeyValueStore() {
    fromJson(jsonStr);
}

KeyValueStore::~KeyValueStore() {
    clear();
}

KeyValue* KeyValueStore::find(const char* key) const {
    KeyValue* item = NULL;
    find(key, &item);
    if (item) {
        Serial.printf("find: %s ", key);
        Serial.printf("%s", item->getValue());
        Serial.println();
    }
    return item;
}

bool KeyValueStore::find(const char* key, KeyValue** item) const {
    size_t num;
    return find(key, item, num);
}

bool KeyValueStore::find(const char* key, KeyValue** item, size_t& num) const {
    for (size_t i = 0; i < _items.size(); i++) {
        if (strcmp(key, _items.at(i)->getKey()) == 0) {
            *(item) = _items.at(i);
            num = i;
            return true;
        }
    }
    return false;
}

void KeyValueStore::write(const char* key, const char* value, ValueType_t valueType, KeyType_t keyType) {
    KeyValue* item = find(key);
    if (!item) {
        _items.push_back(new KeyValue{key, value, valueType, keyType});
        item = _items.at(_items.size() - 1);
    }
    item->setValue(value, valueType);
}

bool KeyValueStore::read(const char* key, String& value) const {
    ValueType_t type = VT_STRING;
    return read(key, value, type);
}

bool KeyValueStore::read(const char* key, String& value, ValueType_t& type) const {
    KeyValue* item = find(key);
    if (item) {
        value = String(item->getValue());
        type = item->getType();
        return true;
    }
    return false;
}

bool KeyValueStore::erase(const char* key) {
    KeyValue* item = NULL;
    size_t num = 0;
    if (find(key, &item, num)) {
        _items.erase(_items.begin() + num);
        delete item;
        return true;
    }
    return false;
}

void KeyValueStore::clear() {
    for (size_t i = 0; i < _items.size(); i++) {
        delete _items.at(i);
    }
    _items.clear();
}

void KeyValueStore::save(JsonObject& obj) const {
    for (auto item : _items) {
        switch (item->getType()) {
            case VT_STRING:
                obj[item->getKey()] = item->getValue();
                break;
            case VT_FLOAT:
                obj[item->getKey()] = atof(item->getValue());
                break;
            case VT_INT:
                obj[item->getKey()] = atoi(item->getValue());
            default:
                break;
        }
    }
}

void KeyValueStore::load(JsonObject& obj) {
    for (JsonPair& p : obj) {
        String key = p.key;
        String value{p.value.as<String>()};
        _items.push_back(new KeyValue{key.c_str(), value.c_str(), VT_STRING});
    }
}

void KeyValueStore::fromJson(const char* inStr) {
    DynamicJsonBuffer buf;
    JsonObject& obj = buf.parse(inStr);
    load(obj);
}

String& KeyValueStore::toJson(String& outStr) const {
    DynamicJsonBuffer json;
    JsonObject& obj = json.createObject();
    save(obj);
    obj.printTo(outStr);
    return outStr;
}

void KeyValueStore::forEach(KeyValueHandler func) {
    for (auto item : _items) {
        func(item);
    }
}
