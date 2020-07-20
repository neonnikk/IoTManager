#include "Objects/OneWireScanner.h"

#include "Objects/OneWireBus.h"

#include "StringConsts.h"
#include "PrintMessage.h"

const char* MODULE = "OneWireScanner";

OneWireScanner::OneWireScanner() : BusScanner(TAG_ONE_WIRE) {}

OneWireScanner::~OneWireScanner(){};

bool OneWireScanner::onInit() {
    return onewire.attached();
}

bool OneWireScanner::onScan() {
    if (!onewire.get()->search(_addr)) {
        onewire.get()->reset_search();
        return true;
    }
    onewire.addItem(_addr);
    return false;
}
