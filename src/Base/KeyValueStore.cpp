#include "Base/KeyValueStore.h"

KeyValueStore::KeyValueStore() {
    _items.reserve(8);
};

KeyValueStore::KeyValueStore(const String& jsonStr) {
    fromJson(jsonStr);
}

KeyValueStore::~KeyValueStore() {
    clear();
}

KeyValue* KeyValueStore::find(const char* key) const {
    KeyValue* res{NULL};
    for (auto* item : _items) {
        if (strcmp(key, item->getKey()) == 0) {
            res = item;
            break;
        }
    }
    return res;
}

bool KeyValueStore::write(const char* key, const char* value, ValueType_t valueType, KeyType_t keyType) {
    auto* item = find(key);
    if (!item) {
        item = new KeyValue{key, value, valueType, keyType};
        _items.push_back(item);
        onAdd(item);
        return true;
    } else {
        item->setValue(value, valueType);
        onUpdate(item);
        return false;
    }
}

bool KeyValueStore::read(const char* key, String& value) const {
    bool res = false;
    auto item = find(key);
    if (item) {
        value = item->getValue();
        res = true;
    }
    return res;
}

bool KeyValueStore::read(const char* key, String& value, ValueType_t& type) const {
    bool res = false;
    auto item = find(key);
    if (item) {
        value = item->getValue();
        type = item->getType();
        res = true;
    }
    return res;
}

bool KeyValueStore::erase(const char* key) {
    for (size_t i = 0; i < _items.size(); i++) {
        if (strcmp(key, _items.at(i)->getKey()) == 0) {
            onErase(_items.at(i));
            _items.erase(_items.begin() + i);
            return true;
        }
    }
    return false;
}

void KeyValueStore::clear() {
    for (auto* item : _items) {
        delete item;
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

void KeyValueStore::fromJson(const String& jsonStr) {
    DynamicJsonBuffer buf;
    JsonObject& obj = buf.parse(jsonStr.c_str());
    load(obj);
}

String& KeyValueStore::toJson(String& jsonStr) const {
    DynamicJsonBuffer json;
    JsonObject& obj = json.createObject();
    save(obj);
    obj.printTo(jsonStr);
    return jsonStr;
}

void KeyValueStore::forEach(KeyValueHandler func) {
    for (auto item : _items) {
        func(item);
    }
}
