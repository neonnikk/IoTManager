#include "Objects/OneWireBus.h"

OneWireBus oneWire;

OneWireBus::OneWireBus() : _bus{NULL} {};

OneWireBus::~OneWireBus() {
    for (size_t i = 0; i < _items.size(); i++) {
        delete _items[i];
    }
    _items.clear();
    if (_bus) delete _bus;
}

void OneWireBus::addItem(uint8_t addr[8]) {
    for (size_t i = 0; i < _items.size(); i++) {
        OneBusItem* item = _items.at(i);
        if (item->equals(addr)) {
            return;
        }
    }
    _items.push_back(new OneBusItem(&addr[0], getFamily(addr[0])));
}

const String OneWireBus::asJson() {
    String res = "{\"onewire\":[";
    for (size_t i = 0; i < _items.size(); i++) {
        OneBusItem* item = _items.at(i);
        res += "{\"num\":";
        res += i + 1;
        res += ",\"url\":\"";
        res += item->getAddUrl();
        res += "\"";
        res += ",\"family\":\"";
        res += getFamily(item->getAddress()->at(0));
        res += "\"}";
        if (i < _items.size() - 1) {
            res += ",";
        }
    }
    res += "]}";
    return res;
};

OneWire* OneWireBus::get() {
    return _bus;
}

bool OneWireBus::attached() {
    return _bus != NULL;
}

void OneWireBus::attach(uint8_t pin) {
    if (_bus) {
        delete _bus;
    }
    _bus = new OneWire(pin);
}

const String getFamily(uint8_t family) {
    String res;
    switch (family) {
        case 0x10:
            res = F("DS1820(S)");
            break;
        case 0x28:
            res = F("DS18B20");
            break;
        case 0x22:
            res = F("DS1822");
            break;
        case 0x3B:
            res = F("DS1825");
            break;
        case 0x42:
            res = F("DS28EA00");
            break;
        default:
            res = F("-");
    }
    return res;
};
