#include "Objects/OneWireScanner.h"

#include "Objects/OneWireBus.h"

#include "StringConsts.h"

OneWireScanner::OneWireScanner() : BusScanner(TAG_ONE_WIRE) {}

OneWireScanner::~OneWireScanner(){};

bool OneWireScanner::onInit() {
    return oneWire.attached();
}

bool OneWireScanner::onScan() {
    if (!oneWire.get()->search(_addr)) {
        oneWire.get()->reset_search();
        return true;
    }
    oneWire.addItem(_addr);
    return false;
}
